#pragma once

#include "ast/nodes.h"
#include "backend/source_catalog.h"
#include "backend/sql/compiled_sql.h"

namespace ra {

class SqlCompiler {
public:
  explicit SqlCompiler(const SourceCatalog &catalog);
  CompiledSql compile(const ExprPtr &expr) const;

private:
  CompiledSql compileExpr(const ExprPtr &expr) const;
  CompiledSql compileTable(const TableExpr &expr) const;
  CompiledSql compileSelect(const SelectExpr &expr) const;
  CompiledSql compileProject(const ProjectExpr &expr) const;
  CompiledSql compileJoin(const JoinExpr &expr) const;
  CompiledSql compileRename(const RenameExpr &expr) const;

  const SourceCatalog &catalog_;
};

} // namespace ra
