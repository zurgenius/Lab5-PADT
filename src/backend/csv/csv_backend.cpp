#include "backend/csv/csv_backend.h"

#include "backend/csv/csv_parser.h"

#include <stdexcept>

namespace ra {
namespace {

int columnIndex(const Sequence<std::string> &schema,
                const std::string &column) {
  for (int index = 0; index < schema.get_count(); index++) {
    if (schema.get(index) == column) {
      return index;
    }
  }
  throw std::invalid_argument("Column not found in schema: " + column);
}

} // namespace

CsvExecutionResult::CsvExecutionResult(const QueryResult &result,
                                       const Sequence<std::string> &plan)
    : result_(result), plan_(makeSequence(plan)) {}

const QueryResult &CsvExecutionResult::result() const { return result_; }
const Sequence<std::string> &CsvExecutionResult::plan() const { return plan_; }

CsvBackend::CsvBackend(const SourceCatalog &catalog) : catalog_(catalog) {}

CsvExecutionResult CsvBackend::execute(const ExprPtr &expr) const {
  MutableArraySequence<std::string> plan;
  QueryResult result = executeExpr(expr, plan);
  return CsvExecutionResult(result, plan);
}

QueryResult
CsvBackend::executeExpr(const ExprPtr &expr,
                        MutableArraySequence<std::string> &plan) const {
  if (!expr) {
    throw std::invalid_argument("Cannot execute empty AST");
  }
  if (const auto *table = dynamic_cast<const TableExpr *>(expr.get())) {
    return executeTable(*table, plan);
  }
  if (const auto *select = dynamic_cast<const SelectExpr *>(expr.get())) {
    return executeSelect(*select, plan);
  }
  if (const auto *project = dynamic_cast<const ProjectExpr *>(expr.get())) {
    return executeProject(*project, plan);
  }
  if (const auto *join = dynamic_cast<const JoinExpr *>(expr.get())) {
    return executeJoin(*join, plan);
  }
  if (const auto *rename = dynamic_cast<const RenameExpr *>(expr.get())) {
    plan.append("Rename: " + rename->alias());
    return executeExpr(rename->child(), plan);
  }
  throw std::invalid_argument("Unsupported AST node in CSV backend");
}

QueryResult
CsvBackend::executeTable(const TableExpr &expr,
                         MutableArraySequence<std::string> &plan) const {
  const SourceBinding *source = catalog_.find(expr.name(), SourceKind::Csv);
  if (source == nullptr) {
    throw std::invalid_argument("CSV source is not registered: " + expr.name());
  }
  plan.append("Read CSV: " + source->location());
  CsvParser parser;
  return parser.read(source->location(), expr.schema());
}

QueryResult
CsvBackend::executeSelect(const SelectExpr &expr,
                          MutableArraySequence<std::string> &plan) const {
  QueryResult input = executeExpr(expr.child(), plan);
  QueryResult output(expr.schema());
  for (int index = 0; index < input.rows().get_count(); index++) {
    const Row &row = input.rows().get(index);
    if (predicate_evaluator_.evaluate(expr.condition(), input.schema(), row)) {
      output.append(row);
    }
  }
  plan.append("Filter: " + expr.condition().toString());
  return output;
}

QueryResult
CsvBackend::executeProject(const ProjectExpr &expr,
                           MutableArraySequence<std::string> &plan) const {
  QueryResult input = executeExpr(expr.child(), plan);
  QueryResult output(expr.columns());
  for (int row_index = 0; row_index < input.rows().get_count(); row_index++) {
    Row row;
    for (int column = 0; column < expr.columns().get_count(); column++) {
      row.append(input.rows().get(row_index).cell(
          columnIndex(input.schema(), expr.columns().get(column))));
    }
    output.append(row);
  }
  plan.append("Project: " + joinStrings(expr.columns(), ", "));
  return output;
}

QueryResult
CsvBackend::executeJoin(const JoinExpr &expr,
                        MutableArraySequence<std::string> &plan) const {
  QueryResult left = executeExpr(expr.left(), plan);
  QueryResult right = executeExpr(expr.right(), plan);
  QueryResult output(expr.schema());
  const int left_key = columnIndex(left.schema(), expr.leftColumn());
  const int right_key = columnIndex(right.schema(), expr.rightColumn());

  for (int left_index = 0; left_index < left.rows().get_count(); left_index++) {
    const Row &left_row = left.rows().get(left_index);
    for (int right_index = 0; right_index < right.rows().get_count();
         right_index++) {
      const Row &right_row = right.rows().get(right_index);
      if (left_row.cell(left_key).isNull() ||
          right_row.cell(right_key).isNull() ||
          left_row.cell(left_key).value() !=
              right_row.cell(right_key).value()) {
        continue;
      }
      Row joined;
      for (int cell = 0; cell < left_row.size(); cell++) {
        joined.append(left_row.cell(cell));
      }
      for (int cell = 0; cell < right_row.size(); cell++) {
        joined.append(right_row.cell(cell));
      }
      output.append(joined);
    }
  }
  plan.append("Nested-loop join: " + expr.leftColumn() + " = " +
              expr.rightColumn());
  return output;
}

} // namespace ra
