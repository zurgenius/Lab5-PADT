#include "dsl/query.h"

#include <memory>
#include <utility>

namespace ra {

SelectPipe::SelectPipe(const Condition &condition) : condition_(condition) {}

const Condition &SelectPipe::condition() const { return condition_; }

ProjectPipe::ProjectPipe(const Sequence<std::string> &columns)
    : columns_(makeSequence(columns)) {}

const Sequence<std::string> &ProjectPipe::columns() const { return columns_; }

JoinPipe::JoinPipe(ExprPtr right, const std::string &left_column,
                   const std::string &right_column)
    : right_(std::move(right)), left_column_(left_column),
      right_column_(right_column) {}

const ExprPtr &JoinPipe::right() const { return right_; }

const std::string &JoinPipe::leftColumn() const { return left_column_; }

const std::string &JoinPipe::rightColumn() const { return right_column_; }

RenamePipe::RenamePipe(const std::string &alias) : alias_(alias) {}

const std::string &RenamePipe::alias() const { return alias_; }

ExprPtr Table(const std::string &name, const Sequence<std::string> &schema) {
  return std::make_shared<TableExpr>(name, schema);
}

ExprPtr Table(const std::string &name,
              std::initializer_list<std::string> schema) {
  MutableArraySequence<std::string> schema_sequence = makeSequence(schema);
  return Table(name, schema_sequence);
}

ExprPtr Sigma(const Condition &condition, ExprPtr child) {
  return std::make_shared<SelectExpr>(condition, std::move(child));
}

ExprPtr Sigma(const std::string &condition, ExprPtr child) {
  return Sigma(Condition(condition), std::move(child));
}

ExprPtr Pi(const Sequence<std::string> &columns, ExprPtr child) {
  return std::make_shared<ProjectExpr>(columns, std::move(child));
}

ExprPtr Pi(std::initializer_list<std::string> columns, ExprPtr child) {
  MutableArraySequence<std::string> column_sequence = makeSequence(columns);
  return Pi(column_sequence, std::move(child));
}

ExprPtr Join(ExprPtr left, ExprPtr right, const std::string &left_column,
             const std::string &right_column) {
  return std::make_shared<JoinExpr>(std::move(left), std::move(right),
                                    left_column, right_column);
}

ExprPtr Rho(const std::string &alias, ExprPtr child) {
  return std::make_shared<RenameExpr>(alias, std::move(child));
}

SelectPipe Sigma(const Condition &condition) { return SelectPipe(condition); }

SelectPipe Sigma(const std::string &condition) {
  return SelectPipe(Condition(condition));
}

ProjectPipe Pi(const Sequence<std::string> &columns) {
  return ProjectPipe(columns);
}

ProjectPipe Pi(std::initializer_list<std::string> columns) {
  MutableArraySequence<std::string> column_sequence = makeSequence(columns);
  return ProjectPipe(column_sequence);
}

JoinPipe Join(ExprPtr right, const std::string &left_column,
              const std::string &right_column) {
  return JoinPipe(std::move(right), left_column, right_column);
}

RenamePipe Rho(const std::string &alias) { return RenamePipe(alias); }

ExprPtr operator|(ExprPtr left, const SelectPipe &pipe) {
  return Sigma(pipe.condition(), std::move(left));
}

ExprPtr operator|(ExprPtr left, const ProjectPipe &pipe) {
  return Pi(pipe.columns(), std::move(left));
}

ExprPtr operator|(ExprPtr left, const JoinPipe &pipe) {
  return Join(std::move(left), pipe.right(), pipe.leftColumn(),
              pipe.rightColumn());
}

ExprPtr operator|(ExprPtr left, const RenamePipe &pipe) {
  return Rho(pipe.alias(), std::move(left));
}

} // namespace ra
