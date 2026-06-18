#pragma once

#include <string>

namespace ra {

enum class CompareOp {
    Equal,
    NotEqual,
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual,
};

enum class OperandKind {
    Column,
    Literal,
};

class Operand {
  public:
    Operand();
    Operand(OperandKind kind, const std::string &text);

    OperandKind kind() const;
    const std::string &text() const;

  private:
    OperandKind kind_;
    std::string text_;
};

class Condition {
  public:
    Condition();
    explicit Condition(const std::string &raw_text);
    Condition(const std::string &left_column, CompareOp compare_op, const Operand &right_operand);

    const std::string &leftColumn() const;
    CompareOp compareOp() const;
    const Operand &rightOperand() const;
    const std::string &rawText() const;
    std::string toString() const;

  private:
    std::string left_column_;
    CompareOp compare_op_;
    Operand right_operand_;
    std::string raw_text_;
};

std::string compareOpToString(CompareOp op);
Operand Column(const std::string &name);
Operand Literal(const std::string &value);
Operand Literal(int value);
Condition Eq(const std::string &left_column, const Operand &right_operand);
Condition Ne(const std::string &left_column, const Operand &right_operand);
Condition Lt(const std::string &left_column, const Operand &right_operand);
Condition Le(const std::string &left_column, const Operand &right_operand);
Condition Gt(const std::string &left_column, const Operand &right_operand);
Condition Ge(const std::string &left_column, const Operand &right_operand);

}  // namespace ra

#include "dsl/detail/predicate.tpp"
