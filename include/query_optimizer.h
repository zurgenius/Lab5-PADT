#pragma once

#include "ops.h"
#include "visitor.h"

namespace ra {

class QueryOptimizer : public Visitor {
public:
    OpPtr optimize(OpPtr root);
    const TextList& notes() const;

    void visit(ScanOp& op) override;
    void visit(SelectOp& op) override;
    void visit(ProjectOp& op) override;
    void visit(JoinOp& op) override;

private:
    TextList notes_;
};

}  // namespace ra
