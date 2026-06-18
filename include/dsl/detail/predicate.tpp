#pragma once

#include <sstream>

namespace ra {

inline Operand::Operand() : kind_(OperandKind::Literal), text_() {}

inline Operand::Operand(OperandKind kind, const std::string &text) : kind_(kind), text_(text) {}

inline OperandKind Operand::kind() const { return kind_; }

inline const std::string &Operand::text() const { return text_; }

inline Condition::Condition()
    : left_column_(), compare_op_(CompareOp::Equal), right_operand_(), raw_text_() {}

inline Condition::Condition(const std::string &raw_text)
    : left_column_(), compare_op_(CompareOp::Equal), right_operand_(), raw_text_(raw_text) {}

inline Condition::Condition(const std::string &left_column, CompareOp compare_op,
                            const Operand &right_operand)
    : left_column_(left_column),
      compare_op_(compare_op),
      right_operand_(right_operand),
      raw_text_() {}

inline const std::string &Condition::leftColumn() const { return left_column_; }

inline CompareOp Condition::compareOp() const { return compare_op_; }

inline const Operand &Condition::rightOperand() const { return right_operand_; }

inline const std::string &Condition::rawText() const { return raw_text_; }

inline std::string Condition::toString() const {
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

inline std::string compareOpToString(CompareOp op) {
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

inline Operand Column(const std::string &name) { return Operand(OperandKind::Column, name); }

inline Operand Literal(const std::string &value) { return Operand(OperandKind::Literal, value); }

inline Operand Literal(int value) { return Operand(OperandKind::Literal, std::to_string(value)); }

inline Condition Eq(const std::string &left_column, const Operand &right_operand) {
    return Condition(left_column, CompareOp::Equal, right_operand);
}

inline Condition Ne(const std::string &left_column, const Operand &right_operand) {
    return Condition(left_column, CompareOp::NotEqual, right_operand);
}

inline Condition Lt(const std::string &left_column, const Operand &right_operand) {
    return Condition(left_column, CompareOp::Less, right_operand);
}

inline Condition Le(const std::string &left_column, const Operand &right_operand) {
    return Condition(left_column, CompareOp::LessOrEqual, right_operand);
}

inline Condition Gt(const std::string &left_column, const Operand &right_operand) {
    return Condition(left_column, CompareOp::Greater, right_operand);
}

inline Condition Ge(const std::string &left_column, const Operand &right_operand) {
    return Condition(left_column, CompareOp::GreaterOrEqual, right_operand);
}

}  // namespace ra
