#pragma once

#include <stdexcept>
#include <utility>

namespace ra {

namespace ast_detail {

inline void requireExpr(const ExprPtr &expr, const std::string &message) {
    if (!expr) {
        throw std::invalid_argument(message);
    }
}

inline void requireColumnsInSchema(const Sequence<std::string> &columns,
                                   const Sequence<std::string> &schema) {
    for (int index = 0; index < columns.get_count(); index++) {
        if (!containsString(schema, columns.get(index))) {
            throw std::invalid_argument("Project references unknown column: " + columns.get(index));
        }
    }
}

inline void copyStringsTo(StringSequence &target, const Sequence<std::string> &source) {
    for (int index = 0; index < source.get_count(); index++) {
        target.append(source.get(index));
    }
}

inline void appendStringsTo(StringSequence &target, const Sequence<std::string> &source) {
    for (int index = 0; index < source.get_count(); index++) {
        target.append(source.get(index));
    }
}

}  // namespace ast_detail

inline TableExpr::TableExpr(const std::string &name, const Sequence<std::string> &schema)
    : name_(name), schema_(makeStringSequence(schema)) {}

inline ExprKind TableExpr::kind() const { return ExprKind::Table; }

inline const StringSequence &TableExpr::schema() const { return schema_; }

inline void TableExpr::accept(Visitor &visitor) const { visitor.visit(*this); }

inline const std::string &TableExpr::name() const { return name_; }

inline SelectExpr::SelectExpr(const Condition &condition, ExprPtr child)
    : condition_(condition), child_(std::move(child)), schema_() {
    ast_detail::requireExpr(child_, "Select requires a child expression");
    ast_detail::copyStringsTo(schema_, child_->schema());
}

inline ExprKind SelectExpr::kind() const { return ExprKind::Select; }

inline const StringSequence &SelectExpr::schema() const { return schema_; }

inline void SelectExpr::accept(Visitor &visitor) const { visitor.visit(*this); }

inline const Condition &SelectExpr::condition() const { return condition_; }

inline const ExprPtr &SelectExpr::child() const { return child_; }

inline ProjectExpr::ProjectExpr(const Sequence<std::string> &columns, ExprPtr child)
    : columns_(makeStringSequence(columns)), child_(std::move(child)), schema_() {
    ast_detail::requireExpr(child_, "Project requires a child expression");
    ast_detail::requireColumnsInSchema(columns_, child_->schema());
    ast_detail::copyStringsTo(schema_, columns_);
}

inline ExprKind ProjectExpr::kind() const { return ExprKind::Project; }

inline const StringSequence &ProjectExpr::schema() const { return schema_; }

inline void ProjectExpr::accept(Visitor &visitor) const { visitor.visit(*this); }

inline const StringSequence &ProjectExpr::columns() const { return columns_; }

inline const ExprPtr &ProjectExpr::child() const { return child_; }

inline JoinExpr::JoinExpr(ExprPtr left, ExprPtr right, const std::string &left_column,
                          const std::string &right_column)
    : left_(std::move(left)),
      right_(std::move(right)),
      left_column_(left_column),
      right_column_(right_column),
      schema_() {
    ast_detail::requireExpr(left_, "Join requires a left expression");
    ast_detail::requireExpr(right_, "Join requires a right expression");
    ast_detail::copyStringsTo(schema_, left_->schema());
    ast_detail::appendStringsTo(schema_, right_->schema());
}

inline ExprKind JoinExpr::kind() const { return ExprKind::Join; }

inline const StringSequence &JoinExpr::schema() const { return schema_; }

inline void JoinExpr::accept(Visitor &visitor) const { visitor.visit(*this); }

inline const ExprPtr &JoinExpr::left() const { return left_; }

inline const ExprPtr &JoinExpr::right() const { return right_; }

inline const std::string &JoinExpr::leftColumn() const { return left_column_; }

inline const std::string &JoinExpr::rightColumn() const { return right_column_; }

inline RenameExpr::RenameExpr(const std::string &alias, ExprPtr child)
    : alias_(alias), child_(std::move(child)), schema_() {
    ast_detail::requireExpr(child_, "Rename requires a child expression");
    ast_detail::copyStringsTo(schema_, child_->schema());
}

inline ExprKind RenameExpr::kind() const { return ExprKind::Rename; }

inline const StringSequence &RenameExpr::schema() const { return schema_; }

inline void RenameExpr::accept(Visitor &visitor) const { visitor.visit(*this); }

inline const std::string &RenameExpr::alias() const { return alias_; }

inline const ExprPtr &RenameExpr::child() const { return child_; }

}  // namespace ra
