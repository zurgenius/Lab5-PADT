#include "ops.h"

#include <utility>

namespace ra {

ScanOp::ScanOp(std::shared_ptr<TableSource> source) : source_(std::move(source)) {}

const Schema& ScanOp::getSchema() const {
    return source_->getSchema();
}

void ScanOp::open() {
    if (source_) {
        source_->open();
    }
}

bool ScanOp::next(Row& row) {
    if (!source_) {
        return false;
    }

    return source_->next(row);
}

void ScanOp::close() {
    if (source_) {
        source_->close();
    }
}

void ScanOp::accept(Visitor& visitor) {
    visitor.visit(*this);
}

const std::shared_ptr<TableSource>& ScanOp::source() const {
    return source_;
}

SelectOp::SelectOp(PredicatePtr predicate, OpPtr child) : predicate_(std::move(predicate)), child_(std::move(child)) {}

const Schema& SelectOp::getSchema() const {
    return child_->getSchema();
}

void SelectOp::open() {
    if (child_) {
        child_->open();
    }
}

bool SelectOp::next(Row& row) {
    if (!child_) {
        return false;
    }

    Row candidate;
    const Schema& schema = child_->getSchema();

    while (child_->next(candidate)) {
        if (predicate_ == nullptr || predicate_->evaluate(candidate, schema)) {
            row = candidate;
            return true;
        }
    }

    return false;
}

void SelectOp::close() {
    if (child_) {
        child_->close();
    }
}

void SelectOp::accept(Visitor& visitor) {
    visitor.visit(*this);
}

const OpPtr& SelectOp::child() const {
    return child_;
}

void SelectOp::setChild(OpPtr child) {
    child_ = std::move(child);
}

const PredicatePtr& SelectOp::predicate() const {
    return predicate_;
}

ProjectOp::ProjectOp(TextList columns, OpPtr child) : columns_(std::move(columns)), child_(std::move(child)) {}

const Schema& ProjectOp::getSchema() const {
    return columns_;
}

void ProjectOp::open() {
    column_indexes_.clear();
    if (child_) {
        const Schema& child_schema = child_->getSchema();
        for (std::size_t index = 0; index < columns_.size(); ++index) {
            column_indexes_.pushBack(findColumnIndex(child_schema, columns_[index]));
        }
    }

    if (child_) {
        child_->open();
    }
}

bool ProjectOp::next(Row& row) {
    if (!child_) {
        return false;
    }

    Row source_row;
    if (!child_->next(source_row)) {
        return false;
    }

    row.clear();
    for (std::size_t index = 0; index < column_indexes_.size(); ++index) {
        const int source_index = column_indexes_[index];
        if (source_index < 0 || static_cast<std::size_t>(source_index) >= source_row.size()) {
            continue;
        }
        row.pushBack(source_row[static_cast<std::size_t>(source_index)]);
    }

    return true;
}

void ProjectOp::close() {
    if (child_) {
        child_->close();
    }
}

void ProjectOp::accept(Visitor& visitor) {
    visitor.visit(*this);
}

const OpPtr& ProjectOp::child() const {
    return child_;
}

void ProjectOp::setChild(OpPtr child) {
    child_ = std::move(child);
}

const TextList& ProjectOp::columns() const {
    return columns_;
}

JoinOp::JoinOp(OpPtr left, OpPtr right, std::string left_column, std::string right_column)
    : left_(std::move(left)),
      right_(std::move(right)),
      left_column_(std::move(left_column)),
      right_column_(std::move(right_column)) {
    rebuildSchema();
}

const Schema& JoinOp::getSchema() const {
    return schema_;
}

void JoinOp::open() {
    rebuildSchema();
    has_current_left_row_ = false;
    if (left_) {
        left_->open();
        left_index_ = findColumnIndex(left_->getSchema(), left_column_);
    }
    if (right_) {
        right_->open();
        right_index_ = findColumnIndex(right_->getSchema(), right_column_);
    }
}

bool JoinOp::next(Row& row) {
    if (!left_ || !right_) {
        return false;
    }

    Row right_row;

    while (true) {
        if (!has_current_left_row_) {
            if (!left_->next(current_left_row_)) {
                return false;
            }

            has_current_left_row_ = true;
            right_->close();
            right_->open();
        }

        while (right_->next(right_row)) {
            if (!rowsMatch(current_left_row_, right_row)) {
                continue;
            }

            row = current_left_row_;
            row.append(right_row);
            return true;
        }

        has_current_left_row_ = false;
    }
}

void JoinOp::close() {
    has_current_left_row_ = false;
    if (left_) {
        left_->close();
    }
    if (right_) {
        right_->close();
    }
}

void JoinOp::accept(Visitor& visitor) {
    visitor.visit(*this);
}

const OpPtr& JoinOp::left() const {
    return left_;
}

const OpPtr& JoinOp::right() const {
    return right_;
}

void JoinOp::setLeft(OpPtr left) {
    left_ = std::move(left);
    rebuildSchema();
}

void JoinOp::setRight(OpPtr right) {
    right_ = std::move(right);
    rebuildSchema();
}

const std::string& JoinOp::leftColumn() const {
    return left_column_;
}

const std::string& JoinOp::rightColumn() const {
    return right_column_;
}

void JoinOp::rebuildSchema() {
    schema_.clear();
    if (left_) {
        schema_.append(left_->getSchema());
    }
    if (right_) {
        schema_.append(right_->getSchema());
    }
}

bool JoinOp::rowsMatch(const Row& left_row, const Row& right_row) const {
    if (left_index_ < 0 || right_index_ < 0) {
        return false;
    }

    if (static_cast<std::size_t>(left_index_) >= left_row.size()) {
        return false;
    }
    if (static_cast<std::size_t>(right_index_) >= right_row.size()) {
        return false;
    }

    return compareValues(left_row[static_cast<std::size_t>(left_index_)], right_row[static_cast<std::size_t>(right_index_)]) == 0;
}

}  // namespace ra
