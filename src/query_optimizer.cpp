#include "query_optimizer.h"

namespace ra {

OpPtr QueryOptimizer::optimize(OpPtr root) {
    notes_.clear();
    notes_.pushBack("Optimizer skeleton is ready.");

    if (root) {
        root->accept(*this);
    }

    return root;
}

const TextList& QueryOptimizer::notes() const {
    return notes_;
}

void QueryOptimizer::visit(ScanOp&) {
    notes_.pushBack("Visited ScanOp skeleton.");
}

void QueryOptimizer::visit(SelectOp& op) {
    notes_.pushBack("Visited SelectOp skeleton.");
    if (op.child()) {
        op.child()->accept(*this);
    }
}

void QueryOptimizer::visit(ProjectOp& op) {
    notes_.pushBack("Visited ProjectOp skeleton.");
    if (op.child()) {
        op.child()->accept(*this);
    }
}

void QueryOptimizer::visit(JoinOp& op) {
    notes_.pushBack("Visited JoinOp skeleton.");
    if (op.left()) {
        op.left()->accept(*this);
    }
    if (op.right()) {
        op.right()->accept(*this);
    }
}

}  // namespace ra
