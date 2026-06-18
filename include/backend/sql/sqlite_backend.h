#pragma once

#include "backend/query_result.h"
#include "backend/sql/sql_compiler.h"

namespace ra {

class SqlExecutionResult {
public:
  SqlExecutionResult(const CompiledSql &sql, const QueryResult &result);

  const CompiledSql &sql() const;
  const QueryResult &result() const;

private:
  CompiledSql sql_;
  QueryResult result_;
};

class SQLiteBackend {
public:
  explicit SQLiteBackend(const SourceCatalog &catalog);
  SqlExecutionResult execute(const ExprPtr &expr) const;

private:
  const SourceCatalog &catalog_;
};

} // namespace ra
