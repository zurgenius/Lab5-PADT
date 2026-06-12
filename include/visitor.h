#pragma once

namespace ra {

class JoinOp;
class ProjectOp;
class ScanOp;
class SelectOp;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(ScanOp& op) = 0;
    virtual void visit(SelectOp& op) = 0;
    virtual void visit(ProjectOp& op) = 0;
    virtual void visit(JoinOp& op) = 0;
};

}  // namespace ra

