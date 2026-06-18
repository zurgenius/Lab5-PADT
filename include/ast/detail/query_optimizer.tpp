#pragma once

namespace ra {

inline ExprPtr QueryOptimizer::optimize(const ExprPtr &root) {
    notes_.append("Оптимизатор подключен как visitor-заглушка для второй итерации.");
    if (root) {
        root->accept(*this);
    }
    return root;
}

inline const StringSequence &QueryOptimizer::notes() const { return notes_; }

inline void QueryOptimizer::visit(const TableExpr &expr) {
    notes_.append("Table: " + expr.name());
}

inline void QueryOptimizer::visit(const SelectExpr &expr) {
    notes_.append("Select: " + expr.condition().toString());
    expr.child()->accept(*this);
}

inline void QueryOptimizer::visit(const ProjectExpr &expr) {
    notes_.append("Project: " + joinStrings(expr.columns(), ", "));
    expr.child()->accept(*this);
}

inline void QueryOptimizer::visit(const JoinExpr &expr) {
    notes_.append("Join: " + expr.leftColumn() + " = " + expr.rightColumn());
    expr.left()->accept(*this);
    expr.right()->accept(*this);
}

inline void QueryOptimizer::visit(const RenameExpr &expr) {
    notes_.append("Rename: " + expr.alias());
    expr.child()->accept(*this);
}

}  // namespace ra
