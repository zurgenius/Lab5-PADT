#include "backend/sql/sql_compiler.h"

#include <stdexcept>

namespace ra {
namespace {

std::string quoteIdentifier(const std::string &identifier) {
  std::string result = "\"";
  for (char character : identifier) {
    if (character == '"') {
      result += "\"\"";
    } else {
      result += character;
    }
  }
  result += "\"";
  return result;
}

std::string physicalColumn(const std::string &column) {
  const std::string::size_type dot = column.rfind('.');
  return dot == std::string::npos ? column : column.substr(dot + 1);
}

std::string selectColumns(const Sequence<std::string> &schema,
                          const std::string &alias, bool physical) {
  std::string result;
  for (int index = 0; index < schema.get_count(); index++) {
    if (index > 0) {
      result += ", ";
    }
    const std::string source =
        physical ? physicalColumn(schema.get(index)) : schema.get(index);
    if (!alias.empty()) {
      result += alias + ".";
    }
    result += quoteIdentifier(source);
    if (source != schema.get(index) || !alias.empty()) {
      result += " AS " + quoteIdentifier(schema.get(index));
    }
  }
  return result;
}

void appendParameters(MutableArraySequence<SqlParameter> &target,
                      const Sequence<SqlParameter> &source) {
  for (int index = 0; index < source.get_count(); index++) {
    target.append(source.get(index));
  }
}

CompiledSql makeSql(const std::string &text,
                    const Sequence<SqlParameter> &parameters,
                    const Sequence<std::string> &schema) {
  return CompiledSql(text, parameters, schema);
}

} // namespace

SqlCompiler::SqlCompiler(const SourceCatalog &catalog) : catalog_(catalog) {}

CompiledSql SqlCompiler::compile(const ExprPtr &expr) const {
  if (!expr) {
    throw std::invalid_argument("Cannot compile empty AST");
  }
  return compileExpr(expr);
}

CompiledSql SqlCompiler::compileExpr(const ExprPtr &expr) const {
  if (const auto *table = dynamic_cast<const TableExpr *>(expr.get())) {
    return compileTable(*table);
  }
  if (const auto *select = dynamic_cast<const SelectExpr *>(expr.get())) {
    return compileSelect(*select);
  }
  if (const auto *project = dynamic_cast<const ProjectExpr *>(expr.get())) {
    return compileProject(*project);
  }
  if (const auto *join = dynamic_cast<const JoinExpr *>(expr.get())) {
    return compileJoin(*join);
  }
  if (const auto *rename = dynamic_cast<const RenameExpr *>(expr.get())) {
    return compileRename(*rename);
  }
  throw std::invalid_argument("Unsupported AST node in SQL compiler");
}

CompiledSql SqlCompiler::compileTable(const TableExpr &expr) const {
  const SourceBinding *source = catalog_.find(expr.name(), SourceKind::SQLite);
  if (source == nullptr) {
    throw std::invalid_argument("SQLite source is not registered: " +
                                expr.name());
  }
  MutableArraySequence<SqlParameter> parameters;
  const std::string text = "SELECT " + selectColumns(expr.schema(), "", true) +
                           " FROM " + quoteIdentifier(source->physicalName());
  return makeSql(text, parameters, expr.schema());
}

CompiledSql SqlCompiler::compileSelect(const SelectExpr &expr) const {
  if (!expr.condition().rawText().empty()) {
    throw std::invalid_argument("Raw condition cannot be compiled to SQL: " +
                                expr.condition().rawText());
  }
  const CompiledSql child = compileExpr(expr.child());
  MutableArraySequence<SqlParameter> parameters;
  appendParameters(parameters, child.parameters());

  std::string right;
  if (expr.condition().rightOperand().kind() == OperandKind::Column) {
    right = "q." + quoteIdentifier(expr.condition().rightOperand().text());
  } else {
    right = "?";
    parameters.append(
        SqlParameter(expr.condition().rightOperand().text(),
                     expr.condition().rightOperand().literalKind()));
  }
  const std::string text =
      "SELECT " + selectColumns(expr.schema(), "q", false) + " FROM (" +
      child.text() + ") AS q WHERE q." +
      quoteIdentifier(expr.condition().leftColumn()) + " " +
      compareOpToString(expr.condition().compareOp()) + " " + right;
  return makeSql(text, parameters, expr.schema());
}

CompiledSql SqlCompiler::compileProject(const ProjectExpr &expr) const {
  const CompiledSql child = compileExpr(expr.child());
  const std::string text = "SELECT " +
                           selectColumns(expr.columns(), "q", false) +
                           " FROM (" + child.text() + ") AS q";
  return makeSql(text, child.parameters(), expr.schema());
}

CompiledSql SqlCompiler::compileJoin(const JoinExpr &expr) const {
  const CompiledSql left = compileExpr(expr.left());
  const CompiledSql right = compileExpr(expr.right());
  MutableArraySequence<SqlParameter> parameters;
  appendParameters(parameters, left.parameters());
  appendParameters(parameters, right.parameters());

  std::string columns = selectColumns(expr.left()->schema(), "l", false);
  if (expr.right()->schema().get_count() > 0) {
    columns += ", " + selectColumns(expr.right()->schema(), "r", false);
  }
  const std::string text = "SELECT " + columns + " FROM (" + left.text() +
                           ") AS l INNER JOIN (" + right.text() +
                           ") AS r ON l." + quoteIdentifier(expr.leftColumn()) +
                           " = r." + quoteIdentifier(expr.rightColumn());
  return makeSql(text, parameters, expr.schema());
}

CompiledSql SqlCompiler::compileRename(const RenameExpr &expr) const {
  const CompiledSql child = compileExpr(expr.child());
  const std::string text = "SELECT " +
                           selectColumns(expr.schema(), "q", false) +
                           " FROM (" + child.text() + ") AS q";
  return makeSql(text, child.parameters(), expr.schema());
}

} // namespace ra
