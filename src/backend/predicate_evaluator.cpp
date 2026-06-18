#include "backend/predicate_evaluator.h"

#include <cerrno>
#include <cstdlib>
#include <stdexcept>

namespace ra {
namespace {

bool parseNumber(const std::string &text, double &value) {
  errno = 0;
  char *end = nullptr;
  value = std::strtod(text.c_str(), &end);
  return errno == 0 && end != text.c_str() && *end == '\0';
}

int compareValues(const std::string &left, const std::string &right) {
  double left_number = 0.0;
  double right_number = 0.0;
  if (parseNumber(left, left_number) && parseNumber(right, right_number)) {
    if (left_number < right_number) {
      return -1;
    }
    if (left_number > right_number) {
      return 1;
    }
    return 0;
  }
  if (left < right) {
    return -1;
  }
  if (left > right) {
    return 1;
  }
  return 0;
}

} // namespace

bool PredicateEvaluator::evaluate(const Condition &condition,
                                  const Sequence<std::string> &schema,
                                  const Row &row) const {
  if (!condition.rawText().empty()) {
    throw std::invalid_argument("Raw condition cannot be executed: " +
                                condition.rawText());
  }

  const Option<std::string> left = row.value(schema, condition.leftColumn());
  if (!left.has_value()) {
    return false;
  }

  std::string right;
  if (condition.rightOperand().kind() == OperandKind::Column) {
    const Option<std::string> column =
        row.value(schema, condition.rightOperand().text());
    if (!column.has_value()) {
      return false;
    }
    right = column.get_value();
  } else {
    right = condition.rightOperand().text();
  }

  const int comparison = compareValues(left.get_value(), right);
  switch (condition.compareOp()) {
  case CompareOp::Equal:
    return comparison == 0;
  case CompareOp::NotEqual:
    return comparison != 0;
  case CompareOp::Less:
    return comparison < 0;
  case CompareOp::LessOrEqual:
    return comparison <= 0;
  case CompareOp::Greater:
    return comparison > 0;
  case CompareOp::GreaterOrEqual:
    return comparison >= 0;
  }
  return false;
}

} // namespace ra
