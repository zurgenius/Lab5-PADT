#include "query_optimizer/query_optimizer.h"

#include <memory>

namespace ra {

void OptimizationReport::addRule(const std::string &message) {
  rules_.append(message);
  notes_.append("Правило: " + message);
}

void OptimizationReport::addWarning(const std::string &message) {
  warnings_.append(message);
  notes_.append("Предупреждение: " + message);
}

const Sequence<std::string> &OptimizationReport::rules() const {
  return rules_;
}

const Sequence<std::string> &OptimizationReport::warnings() const {
  return warnings_;
}

const Sequence<std::string> &OptimizationReport::notes() const {
  return notes_;
}

OptimizationResult::OptimizationResult(const ExprPtr &original,
                                       const ExprPtr &optimized,
                                       const OptimizationReport &report)
    : original_(original), optimized_(optimized), report_(report) {}

const ExprPtr &OptimizationResult::original() const { return original_; }

const ExprPtr &OptimizationResult::optimized() const { return optimized_; }

const OptimizationReport &OptimizationResult::report() const { return report_; }

OptimizationResult QueryOptimizer::optimize(const ExprPtr &root) {
  OptimizationReport report;
  ExprPtr optimized = optimizeExpr(root, report);

  for (int index = 0; index < report.notes().get_count(); index++) {
    last_notes_.append(report.notes().get(index));
  }

  return OptimizationResult(root, optimized, report);
}

const Sequence<std::string> &QueryOptimizer::notes() const {
  return last_notes_;
}

ExprPtr QueryOptimizer::optimizeExpr(const ExprPtr &expr,
                                     OptimizationReport &report) {
  if (!expr) {
    return nullptr;
  }

  if (const auto *table = dynamic_cast<const TableExpr *>(expr.get())) {
    return std::make_shared<TableExpr>(table->name(), table->schema());
  }
  if (const auto *select = dynamic_cast<const SelectExpr *>(expr.get())) {
    return optimizeSelect(*select, report);
  }
  if (const auto *project = dynamic_cast<const ProjectExpr *>(expr.get())) {
    return optimizeProject(*project, report);
  }
  if (const auto *join = dynamic_cast<const JoinExpr *>(expr.get())) {
    return optimizeJoin(*join, report);
  }
  if (const auto *rename = dynamic_cast<const RenameExpr *>(expr.get())) {
    return std::make_shared<RenameExpr>(rename->alias(),
                                        optimizeExpr(rename->child(), report));
  }

  return cloneExpr(expr);
}

ExprPtr QueryOptimizer::optimizeSelect(const SelectExpr &expr,
                                       OptimizationReport &report) {
  ExprPtr child = optimizeExpr(expr.child(), report);

  if (const auto *project = dynamic_cast<const ProjectExpr *>(child.get())) {
    if (conditionUsesOnlyProjectedColumns(expr.condition(), project->columns(),
                                          report)) {
      report.addRule("Select(Project(x)) -> Project(Select(x))");
      ExprPtr pushed_select =
          std::make_shared<SelectExpr>(expr.condition(), project->child());
      return std::make_shared<ProjectExpr>(project->columns(), pushed_select);
    }
  }

  if (const auto *join = dynamic_cast<const JoinExpr *>(child.get())) {
    const SchemaSide side =
        conditionSide(expr.condition(), join->left()->schema(),
                      join->right()->schema(), report);
    if (side == SchemaSide::Left) {
      report.addRule("Select(Join(left, right)) -> Join(Select(left), right)");
      ExprPtr pushed_left =
          std::make_shared<SelectExpr>(expr.condition(), join->left());
      return std::make_shared<JoinExpr>(
          pushed_left, join->right(), join->leftColumn(), join->rightColumn());
    }
    if (side == SchemaSide::Right) {
      report.addRule("Select(Join(left, right)) -> Join(left, Select(right))");
      ExprPtr pushed_right =
          std::make_shared<SelectExpr>(expr.condition(), join->right());
      return std::make_shared<JoinExpr>(
          join->left(), pushed_right, join->leftColumn(), join->rightColumn());
    }
  }

  return std::make_shared<SelectExpr>(expr.condition(), child);
}

ExprPtr QueryOptimizer::optimizeProject(const ProjectExpr &expr,
                                        OptimizationReport &report) {
  ExprPtr child = optimizeExpr(expr.child(), report);

  if (const auto *project = dynamic_cast<const ProjectExpr *>(child.get())) {
    report.addRule("Project(Project(x)) -> Project(x)");
    return std::make_shared<ProjectExpr>(expr.columns(), project->child());
  }

  if (const auto *join = dynamic_cast<const JoinExpr *>(child.get())) {
    MutableArraySequence<std::string> left_columns;
    MutableArraySequence<std::string> right_columns;
    if (splitProjectColumnsForJoin(expr, *join, left_columns, right_columns,
                                   report)) {
      report.addRule("Projection pruning: Project(Join(left, right)) -> "
                     "Project(Join(Project(left), Project(right)))");
      ExprPtr left = projectIfNeeded(join->left(), left_columns);
      ExprPtr right = projectIfNeeded(join->right(), right_columns);
      ExprPtr pruned_join = std::make_shared<JoinExpr>(
          left, right, join->leftColumn(), join->rightColumn());
      return std::make_shared<ProjectExpr>(expr.columns(), pruned_join);
    }
  }

  return std::make_shared<ProjectExpr>(expr.columns(), child);
}

ExprPtr QueryOptimizer::optimizeJoin(const JoinExpr &expr,
                                     OptimizationReport &report) {
  return std::make_shared<JoinExpr>(optimizeExpr(expr.left(), report),
                                    optimizeExpr(expr.right(), report),
                                    expr.leftColumn(), expr.rightColumn());
}

SchemaSide
QueryOptimizer::conditionSide(const Condition &condition,
                              const Sequence<std::string> &left_schema,
                              const Sequence<std::string> &right_schema,
                              OptimizationReport &report) const {
  if (!condition.rawText().empty()) {
    report.addWarning("Raw-условие не анализируется: " + condition.rawText());
    return SchemaSide::Unknown;
  }

  const SchemaSide left_column_side =
      locateColumn(condition.leftColumn(), left_schema, right_schema);
  SchemaSide result = left_column_side;

  if (condition.rightOperand().kind() == OperandKind::Column) {
    const SchemaSide right_operand_side = locateColumn(
        condition.rightOperand().text(), left_schema, right_schema);
    if (right_operand_side == SchemaSide::Both ||
        right_operand_side == SchemaSide::Unknown ||
        right_operand_side == SchemaSide::None) {
      return SchemaSide::Unknown;
    }
    if (result == SchemaSide::Both || result == SchemaSide::Unknown ||
        result == SchemaSide::None) {
      return SchemaSide::Unknown;
    }
    if (result != right_operand_side) {
      return SchemaSide::Both;
    }
    return result;
  }

  if (result == SchemaSide::Both || result == SchemaSide::Unknown ||
      result == SchemaSide::None) {
    return SchemaSide::Unknown;
  }
  return result;
}

bool QueryOptimizer::conditionUsesOnlyProjectedColumns(
    const Condition &condition, const Sequence<std::string> &columns,
    OptimizationReport &report) const {
  if (!condition.rawText().empty()) {
    report.addWarning("Raw-условие не анализируется: " + condition.rawText());
    return false;
  }

  if (!sequenceContains(columns, condition.leftColumn())) {
    return false;
  }
  if (condition.rightOperand().kind() == OperandKind::Column &&
      !sequenceContains(columns, condition.rightOperand().text())) {
    return false;
  }
  return true;
}

bool QueryOptimizer::splitProjectColumnsForJoin(
    const ProjectExpr &project, const JoinExpr &join,
    MutableArraySequence<std::string> &left_columns,
    MutableArraySequence<std::string> &right_columns,
    OptimizationReport &report) const {
  for (int index = 0; index < project.columns().get_count(); index++) {
    const std::string &column = project.columns().get(index);
    const SchemaSide side =
        locateColumn(column, join.left()->schema(), join.right()->schema());
    if (side == SchemaSide::Left) {
      appendUnique(left_columns, column);
    } else if (side == SchemaSide::Right) {
      appendUnique(right_columns, column);
    } else {
      report.addWarning(
          "Projection pruning пропущен из-за неоднозначной колонки: " + column);
      return false;
    }
  }

  if (locateColumn(join.leftColumn(), join.left()->schema(),
                   join.right()->schema()) != SchemaSide::Left) {
    report.addWarning(
        "Projection pruning пропущен: левый ключ join неоднозначен");
    return false;
  }
  if (locateColumn(join.rightColumn(), join.left()->schema(),
                   join.right()->schema()) != SchemaSide::Right) {
    report.addWarning(
        "Projection pruning пропущен: правый ключ join неоднозначен");
    return false;
  }

  appendUnique(left_columns, join.leftColumn());
  appendUnique(right_columns, join.rightColumn());
  return true;
}

ExprPtr
QueryOptimizer::projectIfNeeded(const ExprPtr &expr,
                                const Sequence<std::string> &columns) const {
  if (sameColumns(expr->schema(), columns)) {
    return expr;
  }
  return std::make_shared<ProjectExpr>(columns, expr);
}

} // namespace ra
