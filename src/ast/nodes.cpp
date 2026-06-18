#include "ast/nodes.h"

#include <stdexcept>
#include <utility>

namespace ra {

namespace {

void requireExpr(const ExprPtr &expr, const std::string &message) {
  if (!expr) {
    throw std::invalid_argument(message);
  }
}

void requireColumnsInSchema(const Sequence<std::string> &columns,
                            const Sequence<std::string> &schema) {
  for (int index = 0; index < columns.get_count(); index++) {
    if (!containsString(schema, columns.get(index))) {
      throw std::invalid_argument("Project references unknown column: " +
                                  columns.get(index));
    }
  }
}

void copyStringsTo(MutableArraySequence<std::string> &target,
                   const Sequence<std::string> &source) {
  for (int index = 0; index < source.get_count(); index++) {
    target.append(source.get(index));
  }
}

void appendStringsTo(MutableArraySequence<std::string> &target,
                     const Sequence<std::string> &source) {
  for (int index = 0; index < source.get_count(); index++) {
    target.append(source.get(index));
  }
}

} // namespace

TableExpr::TableExpr(const std::string &name,
                     const Sequence<std::string> &schema)
    : name_(name), schema_(makeSequence(schema)) {}

ExprKind TableExpr::kind() const { return ExprKind::Table; }

const Sequence<std::string> &TableExpr::schema() const { return schema_; }

void TableExpr::accept(Visitor &visitor) const { visitor.visit(*this); }

const std::string &TableExpr::name() const { return name_; }

SelectExpr::SelectExpr(const Condition &condition, ExprPtr child)
    : condition_(condition), child_(std::move(child)), schema_() {
  requireExpr(child_, "Select requires a child expression");
  copyStringsTo(schema_, child_->schema());
}

ExprKind SelectExpr::kind() const { return ExprKind::Select; }

const Sequence<std::string> &SelectExpr::schema() const { return schema_; }

void SelectExpr::accept(Visitor &visitor) const { visitor.visit(*this); }

const Condition &SelectExpr::condition() const { return condition_; }

const ExprPtr &SelectExpr::child() const { return child_; }

ProjectExpr::ProjectExpr(const Sequence<std::string> &columns, ExprPtr child)
    : columns_(makeSequence(columns)), child_(std::move(child)), schema_() {
  requireExpr(child_, "Project requires a child expression");
  requireColumnsInSchema(columns_, child_->schema());
  copyStringsTo(schema_, columns_);
}

ExprKind ProjectExpr::kind() const { return ExprKind::Project; }

const Sequence<std::string> &ProjectExpr::schema() const { return schema_; }

void ProjectExpr::accept(Visitor &visitor) const { visitor.visit(*this); }

const Sequence<std::string> &ProjectExpr::columns() const { return columns_; }

const ExprPtr &ProjectExpr::child() const { return child_; }

JoinExpr::JoinExpr(ExprPtr left, ExprPtr right, const std::string &left_column,
                   const std::string &right_column)
    : left_(std::move(left)), right_(std::move(right)),
      left_column_(left_column), right_column_(right_column), schema_() {
  requireExpr(left_, "Join requires a left expression");
  requireExpr(right_, "Join requires a right expression");
  copyStringsTo(schema_, left_->schema());
  appendStringsTo(schema_, right_->schema());
}

ExprKind JoinExpr::kind() const { return ExprKind::Join; }

const Sequence<std::string> &JoinExpr::schema() const { return schema_; }

void JoinExpr::accept(Visitor &visitor) const { visitor.visit(*this); }

const ExprPtr &JoinExpr::left() const { return left_; }

const ExprPtr &JoinExpr::right() const { return right_; }

const std::string &JoinExpr::leftColumn() const { return left_column_; }

const std::string &JoinExpr::rightColumn() const { return right_column_; }

RenameExpr::RenameExpr(const std::string &alias, ExprPtr child)
    : alias_(alias), child_(std::move(child)), schema_() {
  requireExpr(child_, "Rename requires a child expression");
  copyStringsTo(schema_, child_->schema());
}

ExprKind RenameExpr::kind() const { return ExprKind::Rename; }

const Sequence<std::string> &RenameExpr::schema() const { return schema_; }

void RenameExpr::accept(Visitor &visitor) const { visitor.visit(*this); }

const std::string &RenameExpr::alias() const { return alias_; }

const ExprPtr &RenameExpr::child() const { return child_; }

} // namespace ra
