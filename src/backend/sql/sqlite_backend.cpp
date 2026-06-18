#include "backend/sql/sqlite_backend.h"

#include <sqlite3.h>

#include <memory>
#include <stdexcept>

namespace ra {
namespace {

using DatabaseHandle = std::unique_ptr<sqlite3, decltype(&sqlite3_close)>;
using StatementHandle =
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

std::string databasePath(const SourceCatalog &catalog) {
  std::string path;
  for (int index = 0; index < catalog.bindings().get_count(); index++) {
    const SourceBinding &binding = catalog.bindings().get(index);
    if (binding.kind() != SourceKind::SQLite) {
      continue;
    }
    if (path.empty()) {
      path = binding.location();
    } else if (path != binding.location()) {
      throw std::invalid_argument(
          "One SQL query cannot use multiple SQLite databases");
    }
  }
  if (path.empty()) {
    throw std::invalid_argument("SQLite database is not registered");
  }
  return path;
}

void checkSqlite(int code, sqlite3 *database, const std::string &context) {
  if (code != SQLITE_OK) {
    throw std::runtime_error(context + ": " + sqlite3_errmsg(database));
  }
}

void bindParameters(sqlite3_stmt *statement,
                    const Sequence<SqlParameter> &parameters,
                    sqlite3 *database) {
  for (int index = 0; index < parameters.get_count(); index++) {
    const SqlParameter &parameter = parameters.get(index);
    int code = SQLITE_ERROR;
    if (parameter.kind() == LiteralKind::Integer) {
      code = sqlite3_bind_int64(statement, index + 1,
                                std::stoll(parameter.value()));
    } else if (parameter.kind() == LiteralKind::Real) {
      code = sqlite3_bind_double(statement, index + 1,
                                 std::stod(parameter.value()));
    } else {
      code = sqlite3_bind_text(statement, index + 1, parameter.value().c_str(),
                               -1, SQLITE_TRANSIENT);
    }
    checkSqlite(code, database, "Cannot bind SQL parameter");
  }
}

} // namespace

SqlExecutionResult::SqlExecutionResult(const CompiledSql &sql,
                                       const QueryResult &result)
    : sql_(sql), result_(result) {}

const CompiledSql &SqlExecutionResult::sql() const { return sql_; }
const QueryResult &SqlExecutionResult::result() const { return result_; }

SQLiteBackend::SQLiteBackend(const SourceCatalog &catalog)
    : catalog_(catalog) {}

SqlExecutionResult SQLiteBackend::execute(const ExprPtr &expr) const {
  SqlCompiler compiler(catalog_);
  const CompiledSql sql = compiler.compile(expr);
  sqlite3 *raw_database = nullptr;
  const int open_code = sqlite3_open_v2(
      databasePath(catalog_).c_str(), &raw_database, SQLITE_OPEN_READONLY,
      nullptr);
  DatabaseHandle database(raw_database, &sqlite3_close);
  if (open_code != SQLITE_OK) {
    const std::string message =
        raw_database == nullptr ? "unknown SQLite error"
                                : sqlite3_errmsg(raw_database);
    throw std::runtime_error("Cannot open SQLite database: " + message);
  }

  sqlite3_stmt *raw_statement = nullptr;
  checkSqlite(sqlite3_prepare_v2(database.get(), sql.text().c_str(), -1,
                                 &raw_statement, nullptr),
              database.get(), "Cannot prepare SQL");
  StatementHandle statement(raw_statement, &sqlite3_finalize);
  bindParameters(statement.get(), sql.parameters(), database.get());

  QueryResult result(sql.schema());
  int step_code = SQLITE_ROW;
  while ((step_code = sqlite3_step(statement.get())) == SQLITE_ROW) {
    Row row;
    for (int column = 0; column < sqlite3_column_count(statement.get());
         column++) {
      if (sqlite3_column_type(statement.get(), column) == SQLITE_NULL) {
        row.append(Cell::null());
      } else {
        const unsigned char *text =
            sqlite3_column_text(statement.get(), column);
        row.append(Cell(reinterpret_cast<const char *>(text)));
      }
    }
    result.append(row);
  }
  if (step_code != SQLITE_DONE) {
    throw std::runtime_error("Cannot execute SQL: " +
                             std::string(sqlite3_errmsg(database.get())));
  }
  return SqlExecutionResult(sql, result);
}

} // namespace ra
