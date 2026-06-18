#include "query_optimizer/query_optimizer.h"
#include "ast/printers.h"
#include "dsl/query.h"

#include <gtest/gtest.h>
#include <memory>
#include <string>

namespace {

bool hasNote(const Sequence<std::string> &notes, const std::string &needle) {
    for (int index = 0; index < notes.get_count(); index++) {
        if (notes.get(index).find(needle) != std::string::npos) {
            return true;
        }
    }
    return false;
}

ra::ExprPtr buildQualifiedJoin() {
    ra::ExprPtr users = ra::Table("users", {"users.id", "users.name", "users.age"});
    ra::ExprPtr orders = ra::Table("orders", {"orders.id", "orders.user_id", "orders.total"});
    return ra::Join(users, orders, "users.id", "orders.user_id");
}

}  // namespace

TEST(QueryOptimizerTest, CollapsesNestedProjects) {
    ra::ExprPtr query =
        ra::Pi({"users.name"}, ra::Pi({"users.id", "users.name"},
                                      ra::Table("users", {"users.id", "users.name"})));
    ra::QueryOptimizer optimizer;

    ra::OptimizationResult result = optimizer.optimize(query);
    ra::AstTreePrinter printer;

    EXPECT_EQ(printer.print(result.optimized()),
              "Project(users.name)\n"
              "  Table(users) schema=[users.id, users.name]\n");
    EXPECT_TRUE(hasNote(result.report().notes(), "Project(Project(x))"));
}

TEST(QueryOptimizerTest, PushesSelectIntoLeftJoinBranch) {
    ra::ExprPtr query = ra::Sigma(ra::Gt("users.age", ra::Literal(20)), buildQualifiedJoin());
    ra::QueryOptimizer optimizer;

    ra::OptimizationResult result = optimizer.optimize(query);
    ra::AstTreePrinter printer;

    EXPECT_EQ(printer.print(result.optimized()),
              "Join(users.id = orders.user_id)\n"
              "  Select(users.age > '20')\n"
              "    Table(users) schema=[users.id, users.name, users.age]\n"
              "  Table(orders) schema=[orders.id, orders.user_id, orders.total]\n");
    EXPECT_TRUE(hasNote(result.report().notes(), "Join(Select(left), right)"));
}

TEST(QueryOptimizerTest, KeepsRawSelectAboveJoin) {
    ra::ExprPtr query = ra::Sigma("users.age > 20", buildQualifiedJoin());
    ra::QueryOptimizer optimizer;

    ra::OptimizationResult result = optimizer.optimize(query);
    ra::AstTreePrinter printer;

    EXPECT_EQ(printer.print(result.optimized()),
              "Select(users.age > 20)\n"
              "  Join(users.id = orders.user_id)\n"
              "    Table(users) schema=[users.id, users.name, users.age]\n"
              "    Table(orders) schema=[orders.id, orders.user_id, orders.total]\n");
    EXPECT_TRUE(hasNote(result.report().notes(), "Raw-условие не анализируется"));
}

TEST(QueryOptimizerTest, PrunesJoinInputsForProject) {
    ra::ExprPtr query = ra::Pi({"users.name"}, buildQualifiedJoin());
    ra::QueryOptimizer optimizer;

    ra::OptimizationResult result = optimizer.optimize(query);
    ra::AstTreePrinter printer;

    EXPECT_EQ(printer.print(result.optimized()),
              "Project(users.name)\n"
              "  Join(users.id = orders.user_id)\n"
              "    Project(users.name, users.id)\n"
              "      Table(users) schema=[users.id, users.name, users.age]\n"
              "    Project(orders.user_id)\n"
              "      Table(orders) schema=[orders.id, orders.user_id, orders.total]\n");
    EXPECT_TRUE(hasNote(result.report().notes(), "Projection pruning"));
}

TEST(QueryOptimizerTest, DoesNotMutateOriginalTree) {
    ra::ExprPtr query = ra::Pi({"users.name"}, buildQualifiedJoin());
    ra::AstTreePrinter printer;
    const std::string before = printer.print(query);
    ra::QueryOptimizer optimizer;

    ra::OptimizationResult result = optimizer.optimize(query);

    EXPECT_EQ(printer.print(query), before);
    EXPECT_NE(result.optimized().get(), query.get());
}
