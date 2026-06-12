#include "common.h"

namespace ra {

Value::Value() : type(ValueType::Empty), int_value(0) {}

Value::Value(int value) : type(ValueType::Integer), int_value(value) {}

Value::Value(const std::string& value) : type(ValueType::Text), int_value(0), text_value(value) {}

Value::Value(const char* value) : type(ValueType::Text), int_value(0), text_value(value) {}

std::string valueToString(const Value& value) {
    if (value.type == ValueType::Integer) {
        return std::to_string(value.int_value);
    }

    if (value.type == ValueType::Text) {
        return value.text_value;
    }

    return "";
}

int findColumnIndex(const Schema& schema, const std::string& column_name) {
    for (std::size_t index = 0; index < schema.size(); ++index) {
        if (schema[index] == column_name) {
            return static_cast<int>(index);
        }
    }

    return -1;
}

int compareValues(const Value& lhs, const Value& rhs) {
    if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
        if (lhs.int_value < rhs.int_value) {
            return -1;
        }
        if (lhs.int_value > rhs.int_value) {
            return 1;
        }
        return 0;
    }

    const std::string left = valueToString(lhs);
    const std::string right = valueToString(rhs);
    if (left < right) {
        return -1;
    }
    if (left > right) {
        return 1;
    }
    return 0;
}

void printRow(const Row& row, OutputStream& out) {
    for (std::size_t index = 0; index < row.size(); ++index) {
        out.writeString(valueToString(row[index]));
        if (index + 1 < row.size()) {
            out.writeText(" | ");
        }
    }
    out.writeChar('\n');
}

}  // namespace ra
