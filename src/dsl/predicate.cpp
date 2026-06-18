#include "dsl/predicate.h"

#include <sstream>

namespace ra {

Operand::Operand()
    : kind_(OperandKind::Literal), text_(), literal_kind_(LiteralKind::Text) {}

Operand::Operand(OperandKind kind, const std::string &text,
                 LiteralKind literal_kind)
    : kind_(kind), text_(text), literal_kind_(literal_kind) {}

OperandKind Operand::kind() const { return kind_; }

const std::string &Operand::text() const { return text_; }

LiteralKind Operand::literalKind() const { return literal_kind_; }

Condition::Condition()
    : left_column_(), compare_op_(CompareOp::Equal), right_operand_(),
      raw_text_() {}

Condition::Condition(const std::string &raw_text)
    : left_column_(), compare_op_(CompareOp::Equal), right_operand_(),
      raw_text_(raw_text) {}

Condition::Condition(const std::string &left_column, CompareOp compare_op,
                     const Operand &right_operand)
    : left_column_(left_column), compare_op_(compare_op),
      right_operand_(right_operand), raw_text_() {}

const std::string &Condition::leftColumn() const { return left_column_; }

CompareOp Condition::compareOp() const { return compare_op_; }

const Operand &Condition::rightOperand() const { return right_operand_; }

const std::string &Condition::rawText() const { return raw_text_; }

std::string Condition::toString() const {
  if (!raw_text_.empty()) {
    return raw_text_;
  }

  std::ostringstream output;
  output << left_column_ << " " << compareOpToString(compare_op_) << " ";
  if (right_operand_.kind() == OperandKind::Column) {
    output << right_operand_.text();
  } else {
    output << "'" << right_operand_.text() << "'";
  }
  return output.str();
}

std::string compareOpToString(CompareOp op) {
  switch (op) {
  case CompareOp::Equal:
    return "=";
  case CompareOp::NotEqual:
    return "!=";
  case CompareOp::Less:
    return "<";
  case CompareOp::LessOrEqual:
    return "<=";
  case CompareOp::Greater:
    return ">";
  case CompareOp::GreaterOrEqual:
    return ">=";
  }
  return "?";
}

Operand Column(const std::string &name) {
  return Operand(OperandKind::Column, name);
}

Operand Literal(const std::string &value) {
  return Operand(OperandKind::Literal, value);
}

Operand Literal(int value) {
  return Operand(OperandKind::Literal, std::to_string(value),
                 LiteralKind::Integer);
}

Operand Literal(double value) {
  std::ostringstream output;
  output << value;
  return Operand(OperandKind::Literal, output.str(), LiteralKind::Real);
}

Condition Eq(const std::string &left_column, const Operand &right_operand) {
  return Condition(left_column, CompareOp::Equal, right_operand);
}

Condition Ne(const std::string &left_column, const Operand &right_operand) {
  return Condition(left_column, CompareOp::NotEqual, right_operand);
}

Condition Lt(const std::string &left_column, const Operand &right_operand) {
  return Condition(left_column, CompareOp::Less, right_operand);
}

Condition Le(const std::string &left_column, const Operand &right_operand) {
  return Condition(left_column, CompareOp::LessOrEqual, right_operand);
}

Condition Gt(const std::string &left_column, const Operand &right_operand) {
  return Condition(left_column, CompareOp::Greater, right_operand);
}

Condition Ge(const std::string &left_column, const Operand &right_operand) {
  return Condition(left_column, CompareOp::GreaterOrEqual, right_operand);
}

} // namespace ra
