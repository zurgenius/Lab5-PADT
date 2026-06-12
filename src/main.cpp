#include "ops.h"
#include "query_optimizer.h"

int main() {
    using namespace ra;

    ConsoleOutputStream& out = console();

    out.writeText("Shared skeleton for the laboratory project is ready.\n\n");

    out.writeText("Person 1 responsibilities:\n");
    out.writeText(" - design the AST hierarchy in more detail;\n");
    out.writeText(" - add a DSL or helper functions for query construction;\n");
    out.writeText(" - prepare predicates or condition objects for SelectOp.\n\n");

    out.writeText("Person 2 responsibilities:\n");
    out.writeText(" - implement QueryOptimizer rules;\n");
    out.writeText(" - traverse the tree through Visitor;\n");
    out.writeText(" - return a transformed root node.\n\n");

    out.writeText("Person 3 responsibilities:\n");
    out.writeText(" - implement open/next/close in the operators;\n");
    out.writeText(" - finish CSV and SQLite backends;\n");
    out.writeText(" - connect execution with the optimized tree.\n\n");

    OpPtr query_root;
    OpPtr optimized_root;
    QueryOptimizer optimizer;
    Row row_buffer;

    // Person 1 will build a query tree here.
    // query_root = ...;

    // Person 2 will optimize the tree here.
    // optimized_root = optimizer.optimize(query_root);

    // Person 3 will execute the optimized tree here.
    // if (optimized_root) {
    //     optimized_root->open();
    //     while (optimized_root->next(row_buffer)) {
    //         printRow(row_buffer);
    //     }
    //     optimized_root->close();
    // }

    out.writeText("Use this branch as the common starting point for all three participants.\n");
    return 0;
}
