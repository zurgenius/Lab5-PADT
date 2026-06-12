#pragma once

#include <cstddef>
#include <string>

#include "io_streams.h"
#include "simple_array.h"

namespace ra {

enum class ValueType {
    Integer,
    Text,
    Empty,
};

struct Value {
    ValueType type;
    int int_value;
    std::string text_value;

    Value();
    explicit Value(int value);
    explicit Value(const std::string& value);
    explicit Value(const char* value);
};

using Row = SimpleArray<Value>;
using Schema = SimpleArray<std::string>;
using TextList = SimpleArray<std::string>;

enum class CompareOp {
    Equal,
    NotEqual,
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual,
};

std::string valueToString(const Value& value);
int findColumnIndex(const Schema& schema, const std::string& column_name);
int compareValues(const Value& lhs, const Value& rhs);
void printRow(const Row& row, OutputStream& out);

}  // namespace ra
