#pragma once

#include "backend/query_result.h"

#include <string>

namespace ra {

class CsvParser {
public:
  MutableArraySequence<std::string> parseLine(const std::string &line) const;
  QueryResult read(const std::string &file_path,
                   const Sequence<std::string> &expected_schema) const;
};

} // namespace ra
