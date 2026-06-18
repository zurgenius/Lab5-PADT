#include "ast/printers.h"
#include "dsl/query.h"

#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>

TEST(RelationalAstTest, TableStoresNameAndSchema) {
    ra::ExprPtr table = ra::Table("users", {"id", "name", "age"});
    const auto *table_expr = dynamic_cast<const ra::TableExpr *>(table.get());

    ASSERT_NE(table_expr, nullptr);
    EXPECT_EQ(table_expr->name(), "users");
    ASSERT_EQ(table_expr->schema().get_count(), 3);
    EXPECT_EQ(table_expr->schema().get(1), "name");
}

TEST(RelationalAstTest, SelectStoresChildAndCondition) {
    ra::ExprPtr table = ra::Table("users", {"id", "age"});
    ra::ExprPtr select = ra::Sigma(ra::Gt("age", ra::Literal(20)), table);
    const auto *select_expr = dynamic_cast<const ra::SelectExpr *>(select.get());

    ASSERT_NE(select_expr, nullptr);
    EXPECT_EQ(select_expr->child(), table);
    EXPECT_EQ(select_expr->condition().toString(), "age > '20'");
    EXPECT_EQ(select_expr->schema().get_count(), table->schema().get_count());
}

TEST(RelationalAstTest, ProjectBuildsRequestedSchema) {
    ra::ExprPtr table = ra::Table("users", {"id", "name", "age"});
    ra::ExprPtr project = ra::Pi({"name", "age"}, table);

    ASSERT_EQ(project->schema().get_count(), 2);
    EXPECT_EQ(project->schema().get(0), "name");
    EXPECT_EQ(project->schema().get(1), "age");
}

TEST(RelationalAstTest, ProjectRejectsUnknownColumn) {
    ra::ExprPtr table = ra::Table("users", {"id", "name"});

    EXPECT_THROW(ra::Pi({"missing"}, table), std::invalid_argument);
}

TEST(RelationalAstTest, JoinCombinesSchemasAndChildren) {
    ra::ExprPtr users = ra::Table("users", {"id", "name"});
    ra::ExprPtr orders = ra::Table("orders", {"order_id", "user_id"});
    ra::ExprPtr join = ra::Join(users, orders, "id", "user_id");
    const auto *join_expr = dynamic_cast<const ra::JoinExpr *>(join.get());

    ASSERT_NE(join_expr, nullptr);
    EXPECT_EQ(join_expr->left(), users);
    EXPECT_EQ(join_expr->right(), orders);
    ASSERT_EQ(join_expr->schema().get_count(), 4);
    EXPECT_EQ(join_expr->schema().get(0), "id");
    EXPECT_EQ(join_expr->schema().get(3), "user_id");
}

TEST(RelationalDslTest, PipeSyntaxBuildsExpectedAst) {
    ra::ExprPtr query = ra::Table("users", {"id", "name", "age"}) |
                        ra::Sigma(ra::Gt("age", ra::Literal(20))) | ra::Pi({"name"});

    EXPECT_EQ(query->kind(), ra::ExprKind::Project);
    ASSERT_EQ(query->schema().get_count(), 1);
    EXPECT_EQ(query->schema().get(0), "name");
}

TEST(RelationalPrinterTest, AstTreePrinterIsStable) {
    ra::ExprPtr query = ra::Table("users", {"id", "name", "age"}) |
                        ra::Sigma("age > 20") | ra::Pi({"name", "age"});
    ra::AstTreePrinter printer;

    EXPECT_EQ(printer.print(query),
              "Project(name, age)\n"
              "  Select(age > 20)\n"
              "    Table(users) schema=[id, name, age]\n");
}

TEST(RelationalPrinterTest, AlgebraPrinterShowsOperations) {
    ra::ExprPtr query = ra::Table("users", {"id", "name", "age"}) |
                        ra::Sigma("age > 20") | ra::Pi({"name", "age"});
    ra::RelationalAlgebraPrinter printer;

    EXPECT_EQ(printer.print(query), "pi[name, age](sigma[age > 20](users))");
}
