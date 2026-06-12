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

bool SelectOp::next(Row&) {
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
    if (child_) {
        child_->open();
    }
}

bool ProjectOp::next(Row&) {
    return false;
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
    has_current_left_row_ = false;
    if (left_) {
        left_->open();
    }
    if (right_) {
        right_->open();
    }
}

bool JoinOp::next(Row&) {
    return false;
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

bool JoinOp::rowsMatch(const Row&, const Row&) const {
    return false;
}

}  // namespace ra
