#pragma once

#include "ast/relational_expr.h"
#include "ast/visitor.h"
#include "dsl/predicate.h"

#include <string>

namespace ra {

class TableExpr final : public RelationalExpr {
  public:
    TableExpr(const std::string &name, const Sequence<std::string> &schema);

    ExprKind kind() const override;
    const StringSequence &schema() const override;
    void accept(Visitor &visitor) const override;
    const std::string &name() const;

  private:
    std::string name_;
    StringSequence schema_;
};

class SelectExpr final : public RelationalExpr {
  public:
    SelectExpr(const Condition &condition, ExprPtr child);

    ExprKind kind() const override;
    const StringSequence &schema() const override;
    void accept(Visitor &visitor) const override;
    const Condition &condition() const;
    const ExprPtr &child() const;

  private:
    Condition condition_;
    ExprPtr child_;
    StringSequence schema_;
};

class ProjectExpr final : public RelationalExpr {
  public:
    ProjectExpr(const Sequence<std::string> &columns, ExprPtr child);

    ExprKind kind() const override;
    const StringSequence &schema() const override;
    void accept(Visitor &visitor) const override;
    const StringSequence &columns() const;
    const ExprPtr &child() const;

  private:
    StringSequence columns_;
    ExprPtr child_;
    StringSequence schema_;
};

class JoinExpr final : public RelationalExpr {
  public:
    JoinExpr(ExprPtr left, ExprPtr right, const std::string &left_column,
             const std::string &right_column);

    ExprKind kind() const override;
    const StringSequence &schema() const override;
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
    StringSequence schema_;
};

class RenameExpr final : public RelationalExpr {
  public:
    RenameExpr(const std::string &alias, ExprPtr child);

    ExprKind kind() const override;
    const StringSequence &schema() const override;
    void accept(Visitor &visitor) const override;
    const std::string &alias() const;
    const ExprPtr &child() const;

  private:
    std::string alias_;
    ExprPtr child_;
    StringSequence schema_;
};

}  // namespace ra

#include "ast/detail/nodes.tpp"
