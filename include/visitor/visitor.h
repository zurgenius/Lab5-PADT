#pragma once

namespace ra {

class TableExpr;
class SelectExpr;
class ProjectExpr;
class JoinExpr;
class RenameExpr;

class Visitor {
public:
  virtual ~Visitor() = default;

  virtual void visit(const TableExpr &expr) = 0;
  virtual void visit(const SelectExpr &expr) = 0;
  virtual void visit(const ProjectExpr &expr) = 0;
  virtual void visit(const JoinExpr &expr) = 0;
  virtual void visit(const RenameExpr &expr) = 0;
};

} // namespace ra
