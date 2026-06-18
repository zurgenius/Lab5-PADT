#pragma once

#include "ast/relational_expr.h"
#include "dsl/predicate.h"
#include "visitor/visitor.h"

#include <string>

namespace ra {

class TableExpr final : public RelationalExpr {
public:
  TableExpr(const std::string &name, const Sequence<std::string> &schema);

  ExprKind kind() const override;
  const Sequence<std::string> &schema() const override;
  void accept(Visitor &visitor) const override;
  const std::string &name() const;

private:
  std::string name_;
  MutableArraySequence<std::string> schema_;
};

class SelectExpr final : public RelationalExpr {
public:
  SelectExpr(const Condition &condition, ExprPtr child);

  ExprKind kind() const override;
  const Sequence<std::string> &schema() const override;
  void accept(Visitor &visitor) const override;
  const Condition &condition() const;
  const ExprPtr &child() const;

private:
  Condition condition_;
  ExprPtr child_;
  MutableArraySequence<std::string> schema_;
};

class ProjectExpr final : public RelationalExpr {
public:
  ProjectExpr(const Sequence<std::string> &columns, ExprPtr child);

  ExprKind kind() const override;
  const Sequence<std::string> &schema() const override;
  void accept(Visitor &visitor) const override;
  const Sequence<std::string> &columns() const;
  const ExprPtr &child() const;

private:
  MutableArraySequence<std::string> columns_;
  ExprPtr child_;
  MutableArraySequence<std::string> schema_;
};

class JoinExpr final : public RelationalExpr {
public:
  JoinExpr(ExprPtr left, ExprPtr right, const std::string &left_column,
           const std::string &right_column);

  ExprKind kind() const override;
  const Sequence<std::string> &schema() const override;
  void accept(Visitor &visitor) const override;
  const ExprPtr &left() const;
  const ExprPtr &right() const;
  const std::string &leftColumn() const;
  const std::string &rightColumn() const;

private:
  ExprPtr left_;
  ExprPtr right_;
  std::string left_column_;
  std::string right_column_;
  MutableArraySequence<std::string> schema_;
};

class RenameExpr final : public RelationalExpr {
public:
  RenameExpr(const std::string &alias, ExprPtr child);

  ExprKind kind() const override;
  const Sequence<std::string> &schema() const override;
  void accept(Visitor &visitor) const override;
  const std::string &alias() const;
  const ExprPtr &child() const;

private:
  std::string alias_;
  ExprPtr child_;
  MutableArraySequence<std::string> schema_;
};

} // namespace ra
