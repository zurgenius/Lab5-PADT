#pragma once

#include "ast/nodes.h"
#include "backend/predicate_evaluator.h"
#include "backend/source_catalog.h"

namespace ra {

class CsvExecutionResult {
public:
  CsvExecutionResult(const QueryResult &result,
                     const Sequence<std::string> &plan);

  const QueryResult &result() const;
  const Sequence<std::string> &plan() const;

private:
  QueryResult result_;
  MutableArraySequence<std::string> plan_;
};

class CsvBackend {
public:
  explicit CsvBackend(const SourceCatalog &catalog);
  CsvExecutionResult execute(const ExprPtr &expr) const;

private:
  QueryResult executeExpr(const ExprPtr &expr,
                          MutableArraySequence<std::string> &plan) const;
  QueryResult executeTable(const TableExpr &expr,
                           MutableArraySequence<std::string> &plan) const;
  QueryResult executeSelect(const SelectExpr &expr,
                            MutableArraySequence<std::string> &plan) const;
  QueryResult executeProject(const ProjectExpr &expr,
                             MutableArraySequence<std::string> &plan) const;
  QueryResult executeJoin(const JoinExpr &expr,
                          MutableArraySequence<std::string> &plan) const;

  const SourceCatalog &catalog_;
  PredicateEvaluator predicate_evaluator_;
};

} // namespace ra
