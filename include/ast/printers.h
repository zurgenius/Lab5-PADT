#pragma once

#include "ast/nodes.h"

#include <string>

namespace ra {

class AstTreePrinter final : public Visitor {
public:
  std::string print(const ExprPtr &expr);

  void visit(const TableExpr &expr) override;
  void visit(const SelectExpr &expr) override;
  void visit(const ProjectExpr &expr) override;
  void visit(const JoinExpr &expr) override;
  void visit(const RenameExpr &expr) override;

private:
  void appendLine(const std::string &text);
  void printChild(const ExprPtr &expr);

  std::string result_;
  int depth_ = 0;
};

class RelationalAlgebraPrinter final : public Visitor {
public:
  std::string print(const ExprPtr &expr);

  void visit(const TableExpr &expr) override;
  void visit(const SelectExpr &expr) override;
  void visit(const ProjectExpr &expr) override;
  void visit(const JoinExpr &expr) override;
  void visit(const RenameExpr &expr) override;

private:
  std::string printNested(const ExprPtr &expr);

  std::string result_;
};

} // namespace ra
