#pragma once

namespace ra {

inline std::string AstTreePrinter::print(const ExprPtr &expr) {
    result_.clear();
    depth_ = 0;
    if (expr) {
        expr->accept(*this);
    }
    return result_;
}

inline void AstTreePrinter::visit(const TableExpr &expr) {
    appendLine("Table(" + expr.name() + ") schema=[" + joinStrings(expr.schema(), ", ") + "]");
}

inline void AstTreePrinter::visit(const SelectExpr &expr) {
    appendLine("Select(" + expr.condition().toString() + ")");
    printChild(expr.child());
}

inline void AstTreePrinter::visit(const ProjectExpr &expr) {
    appendLine("Project(" + joinStrings(expr.columns(), ", ") + ")");
    printChild(expr.child());
}

inline void AstTreePrinter::visit(const JoinExpr &expr) {
    appendLine("Join(" + expr.leftColumn() + " = " + expr.rightColumn() + ")");
    printChild(expr.left());
    printChild(expr.right());
}

inline void AstTreePrinter::visit(const RenameExpr &expr) {
    appendLine("Rename(" + expr.alias() + ")");
    printChild(expr.child());
}

inline void AstTreePrinter::appendLine(const std::string &text) {
    for (int index = 0; index < depth_; index++) {
        result_ += "  ";
    }
    result_ += text;
    result_ += "\n";
}

inline void AstTreePrinter::printChild(const ExprPtr &expr) {
    depth_++;
    if (expr) {
        expr->accept(*this);
    }
    depth_--;
}

inline std::string RelationalAlgebraPrinter::print(const ExprPtr &expr) {
    result_.clear();
    if (expr) {
        expr->accept(*this);
    }
    return result_;
}

inline void RelationalAlgebraPrinter::visit(const TableExpr &expr) { result_ = expr.name(); }

inline void RelationalAlgebraPrinter::visit(const SelectExpr &expr) {
    result_ = "sigma[" + expr.condition().toString() + "](" + printNested(expr.child()) + ")";
}

inline void RelationalAlgebraPrinter::visit(const ProjectExpr &expr) {
    result_ = "pi[" + joinStrings(expr.columns(), ", ") + "](" + printNested(expr.child()) + ")";
}

inline void RelationalAlgebraPrinter::visit(const JoinExpr &expr) {
    result_ = "(" + printNested(expr.left()) + ") join[" + expr.leftColumn() + " = " +
              expr.rightColumn() + "] (" + printNested(expr.right()) + ")";
}

inline void RelationalAlgebraPrinter::visit(const RenameExpr &expr) {
    result_ = "rho[" + expr.alias() + "](" + printNested(expr.child()) + ")";
}

inline std::string RelationalAlgebraPrinter::printNested(const ExprPtr &expr) {
    RelationalAlgebraPrinter printer;
    return printer.print(expr);
}

}  // namespace ra
