#include "backend/csv/csv_backend.h"
#include "backend/csv/csv_parser.h"
#include "backend/sql/sqlite_backend.h"
#include "dsl/query.h"
#include "query_optimizer/query_optimizer.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <string>

namespace {

std::string dataPath(const std::string &name) {
  return std::string(RELALG_DATA_DIR) + "/" + name;
}

ra::SourceCatalog makeCatalog() {
  ra::SourceCatalog catalog;
  const auto users =
      ra::makeSequence({"users.id", "users.name", "users.age", "users.city"});
  const auto orders = ra::makeSequence(
      {"orders.id", "orders.user_id", "orders.total", "orders.status"});
  catalog.addCsv("users", dataPath("users.csv"), users);
  catalog.addCsv("orders", dataPath("orders.csv"), orders);
  catalog.addSQLite("users", dataPath("demo.sqlite3"), "users", users);
  catalog.addSQLite("orders", dataPath("demo.sqlite3"), "orders", orders);
  return catalog;
}

ra::ExprPtr buildQuery() {
  ra::ExprPtr users =
      ra::Table("users", {"users.id", "users.name", "users.age", "users.city"});
  ra::ExprPtr orders = ra::Table("orders", {"orders.id", "orders.user_id",
                                            "orders.total", "orders.status"});
  return users | ra::Join(orders, "users.id", "orders.user_id") |
         ra::Sigma(ra::Gt("users.age", ra::Literal(20))) |
         ra::Pi({"users.name", "orders.total"});
}

void expectSameResult(const ra::QueryResult &left,
                      const ra::QueryResult &right) {
  ASSERT_EQ(left.schema().get_count(), right.schema().get_count());
  ASSERT_EQ(left.rowCount(), right.rowCount());
  for (int row = 0; row < left.rowCount(); row++) {
    for (int column = 0; column < left.schema().get_count(); column++) {
      const std::string left_value =
          left.rows().get(row).cell(column).display();
      const std::string right_value =
          right.rows().get(row).cell(column).display();
      char *left_end = nullptr;
      char *right_end = nullptr;
      const double left_number = std::strtod(left_value.c_str(), &left_end);
      const double right_number = std::strtod(right_value.c_str(), &right_end);
      if (*left_end == '\0' && *right_end == '\0') {
        EXPECT_DOUBLE_EQ(left_number, right_number);
      } else {
        EXPECT_EQ(left_value, right_value);
      }
    }
  }
}

} // namespace

TEST(CsvParserTest, HandlesQuotedCommaAndEscapedQuote) {
  ra::CsvParser parser;
  const auto fields = parser.parseLine("1,\"Alice, A.\",\"say \"\"hi\"\"\"");

  ASSERT_EQ(fields.get_count(), 3);
  EXPECT_EQ(fields.get(1), "Alice, A.");
  EXPECT_EQ(fields.get(2), "say \"hi\"");
}

TEST(CsvBackendTest, ExecutesOptimizedJoinPipeline) {
  const ra::SourceCatalog catalog = makeCatalog();
  ra::QueryOptimizer optimizer;
  const ra::OptimizationResult optimized = optimizer.optimize(buildQuery());
  const ra::CsvExecutionResult execution =
      ra::CsvBackend(catalog).execute(optimized.optimized());

  ASSERT_EQ(execution.result().rowCount(), 4);
  EXPECT_EQ(execution.result().rows().get(0).cell(0).value(), "Alice");
  EXPECT_GT(execution.plan().get_count(), 3);
}

TEST(SqlCompilerTest, UsesBindParameterAndQuotedColumns) {
  const ra::SourceCatalog catalog = makeCatalog();
  const ra::CompiledSql sql = ra::SqlCompiler(catalog).compile(buildQuery());

  EXPECT_NE(sql.text().find("INNER JOIN"), std::string::npos);
  EXPECT_NE(sql.text().find("?"), std::string::npos);
  EXPECT_NE(sql.text().find("\"users.age\""), std::string::npos);
  ASSERT_EQ(sql.parameters().get_count(), 1);
  EXPECT_EQ(sql.parameters().get(0).kind(), ra::LiteralKind::Integer);
}

TEST(SQLiteBackendTest, MatchesCsvBackendResult) {
  const ra::SourceCatalog catalog = makeCatalog();
  ra::QueryOptimizer optimizer;
  const ra::ExprPtr optimized = optimizer.optimize(buildQuery()).optimized();

  const ra::CsvExecutionResult csv = ra::CsvBackend(catalog).execute(optimized);
  const ra::SqlExecutionResult sql =
      ra::SQLiteBackend(catalog).execute(optimized);

  expectSameResult(csv.result(), sql.result());
}

TEST(BackendTest, RejectsRawConditionExplicitly) {
  const ra::SourceCatalog catalog = makeCatalog();
  const ra::ExprPtr query = ra::Table("users", {"users.id", "users.age"}) |
                            ra::Sigma("users.age > 20");

  EXPECT_THROW(ra::CsvBackend(catalog).execute(query), std::invalid_argument);
  EXPECT_THROW(ra::SQLiteBackend(catalog).execute(query),
               std::invalid_argument);
}
