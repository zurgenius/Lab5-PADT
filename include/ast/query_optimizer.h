#pragma once

#include "ast/nodes.h"

namespace ra {

class QueryOptimizer final : public Visitor {
  public:
    ExprPtr optimize(const ExprPtr &root);
    const StringSequence &notes() const;

    void visit(const TableExpr &expr) override;
    void visit(const SelectExpr &expr) override;
    void visit(const ProjectExpr &expr) override;
    void visit(const JoinExpr &expr) override;
    void visit(const RenameExpr &expr) override;

  private:
    StringSequence notes_;
};

}  // namespace ra

#include "ast/detail/query_optimizer.tpp"
