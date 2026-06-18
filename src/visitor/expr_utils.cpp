#include "visitor/expr_utils.h"

#include <memory>

namespace ra {

ExprPtr cloneExpr(const ExprPtr &expr) {
  if (!expr) {
    return nullptr;
  }

  if (const auto *table = dynamic_cast<const TableExpr *>(expr.get())) {
    return std::make_shared<TableExpr>(table->name(), table->schema());
  }
  if (const auto *select = dynamic_cast<const SelectExpr *>(expr.get())) {
    return std::make_shared<SelectExpr>(select->condition(),
                                        cloneExpr(select->child()));
  }
  if (const auto *project = dynamic_cast<const ProjectExpr *>(expr.get())) {
    return std::make_shared<ProjectExpr>(project->columns(),
                                         cloneExpr(project->child()));
  }
  if (const auto *join = dynamic_cast<const JoinExpr *>(expr.get())) {
    return std::make_shared<JoinExpr>(cloneExpr(join->left()),
                                      cloneExpr(join->right()),
                                      join->leftColumn(), join->rightColumn());
  }
  if (const auto *rename = dynamic_cast<const RenameExpr *>(expr.get())) {
    return std::make_shared<RenameExpr>(rename->alias(),
                                        cloneExpr(rename->child()));
  }

  return nullptr;
}

bool sequenceContains(const Sequence<std::string> &sequence,
                      const std::string &value) {
  for (int index = 0; index < sequence.get_count(); index++) {
    if (sequence.get(index) == value) {
      return true;
    }
  }
  return false;
}

bool appendUnique(MutableArraySequence<std::string> &sequence,
                  const std::string &value) {
  if (sequenceContains(sequence, value)) {
    return false;
  }
  sequence.append(value);
  return true;
}

int countColumnMatches(const Sequence<std::string> &schema,
                       const std::string &column) {
  int matches = 0;
  for (int index = 0; index < schema.get_count(); index++) {
    if (schema.get(index) == column) {
      matches++;
    }
  }
  return matches;
}

SchemaSide locateColumn(const std::string &column,
                        const Sequence<std::string> &left_schema,
                        const Sequence<std::string> &right_schema) {
  const int left_matches = countColumnMatches(left_schema, column);
  const int right_matches = countColumnMatches(right_schema, column);

  if (left_matches == 0 && right_matches == 0) {
    return SchemaSide::None;
  }
  if (left_matches == 1 && right_matches == 0) {
    return SchemaSide::Left;
  }
  if (left_matches == 0 && right_matches == 1) {
    return SchemaSide::Right;
  }
  if (left_matches == 1 && right_matches == 1) {
    return SchemaSide::Both;
  }
  return SchemaSide::Unknown;
}

bool allColumnsIncluded(const Sequence<std::string> &required,
                        const Sequence<std::string> &available) {
  for (int index = 0; index < required.get_count(); index++) {
    if (!sequenceContains(available, required.get(index))) {
      return false;
    }
  }
  return true;
}

bool sameColumns(const Sequence<std::string> &left,
                 const Sequence<std::string> &right) {
  return left.get_count() == right.get_count() &&
         allColumnsIncluded(left, right) && allColumnsIncluded(right, left);
}

} // namespace ra
