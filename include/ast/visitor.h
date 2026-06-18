#pragma once

namespace ra {

class JoinExpr;
class ProjectExpr;
class RenameExpr;
class SelectExpr;
class TableExpr;

class Visitor {
  public:
    virtual ~Visitor() = default;

    virtual void visit(const TableExpr &expr) = 0;
    virtual void visit(const SelectExpr &expr) = 0;
    virtual void visit(const ProjectExpr &expr) = 0;
    virtual void visit(const JoinExpr &expr) = 0;
    virtual void visit(const RenameExpr &expr) = 0;
};

}  // namespace ra
