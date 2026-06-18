#pragma once

#include "ast/nodes.h"
#include "visitor/expr_utils.h"

namespace ra {

class OptimizationReport {
public:
  void addRule(const std::string &message);
  void addWarning(const std::string &message);

  const Sequence<std::string> &rules() const;
  const Sequence<std::string> &warnings() const;
  const Sequence<std::string> &notes() const;

private:
  MutableArraySequence<std::string> rules_;
  MutableArraySequence<std::string> warnings_;
  MutableArraySequence<std::string> notes_;
};

class OptimizationResult {
public:
  OptimizationResult(const ExprPtr &original, const ExprPtr &optimized,
                     const OptimizationReport &report);

  const ExprPtr &original() const;
  const ExprPtr &optimized() const;
  const OptimizationReport &report() const;

private:
  ExprPtr original_;
  ExprPtr optimized_;
  OptimizationReport report_;
};

class QueryOptimizer {
public:
  OptimizationResult optimize(const ExprPtr &root);
  const Sequence<std::string> &notes() const;

private:
  ExprPtr optimizeExpr(const ExprPtr &expr, OptimizationReport &report);
  ExprPtr optimizeSelect(const SelectExpr &expr, OptimizationReport &report);
  ExprPtr optimizeProject(const ProjectExpr &expr, OptimizationReport &report);
  ExprPtr optimizeJoin(const JoinExpr &expr, OptimizationReport &report);

  SchemaSide conditionSide(const Condition &condition,
                           const Sequence<std::string> &left_schema,
                           const Sequence<std::string> &right_schema,
                           OptimizationReport &report) const;
  bool conditionUsesOnlyProjectedColumns(const Condition &condition,
                                         const Sequence<std::string> &columns,
                                         OptimizationReport &report) const;
  bool
  splitProjectColumnsForJoin(const ProjectExpr &project, const JoinExpr &join,
                             MutableArraySequence<std::string> &left_columns,
                             MutableArraySequence<std::string> &right_columns,
                             OptimizationReport &report) const;
  ExprPtr projectIfNeeded(const ExprPtr &expr,
                          const Sequence<std::string> &columns) const;

  MutableArraySequence<std::string> last_notes_;
};

} // namespace ra
