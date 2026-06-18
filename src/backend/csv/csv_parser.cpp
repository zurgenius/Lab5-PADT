#include "backend/csv/csv_parser.h"

#include "streams/file.h"

#include <stdexcept>

namespace ra {
namespace {

std::string physicalColumn(const std::string &column) {
  const std::string::size_type dot = column.rfind('.');
  return dot == std::string::npos ? column : column.substr(dot + 1);
}

int findHeader(const Sequence<std::string> &header,
               const std::string &expected) {
  const std::string physical = physicalColumn(expected);
  for (int index = 0; index < header.get_count(); index++) {
    if (header.get(index) == physical) {
      return index;
    }
  }
  return -1;
}

} // namespace

MutableArraySequence<std::string>
CsvParser::parseLine(const std::string &line) const {
  MutableArraySequence<std::string> fields;
  std::string field;
  bool quoted = false;

  for (std::string::size_type index = 0; index < line.size(); index++) {
    const char current = line[index];
    if (current == '"') {
      if (quoted && index + 1 < line.size() && line[index + 1] == '"') {
        field += '"';
        index++;
      } else {
        quoted = !quoted;
      }
    } else if (current == ',' && !quoted) {
      fields.append(field);
      field.clear();
    } else {
      field += current;
    }
  }
  if (quoted) {
    throw std::invalid_argument("Unclosed quote in CSV line");
  }
  fields.append(field);
  return fields;
}

QueryResult
CsvParser::read(const std::string &file_path,
                const Sequence<std::string> &expected_schema) const {
  FileLineReadOnlyStream stream(file_path);
  stream.open();
  const Option<std::string> header_line = stream.try_read();
  if (!header_line.has_value()) {
    stream.close();
    throw std::invalid_argument("CSV file is empty: " + file_path);
  }

  const MutableArraySequence<std::string> header =
      parseLine(header_line.get_value());
  MutableArraySequence<int> mapping;
  for (int index = 0; index < expected_schema.get_count(); index++) {
    const int source_index = findHeader(header, expected_schema.get(index));
    if (source_index < 0) {
      stream.close();
      throw std::invalid_argument("CSV column not found: " +
                                  expected_schema.get(index));
    }
    mapping.append(source_index);
  }

  QueryResult result(expected_schema);
  while (true) {
    const Option<std::string> line = stream.try_read();
    if (!line.has_value()) {
      break;
    }
    const MutableArraySequence<std::string> fields =
        parseLine(line.get_value());
    if (fields.get_count() != header.get_count()) {
      stream.close();
      throw std::invalid_argument("CSV row has unexpected field count");
    }
    Row row;
    for (int index = 0; index < mapping.get_count(); index++) {
      row.append(Cell(fields.get(mapping.get(index))));
    }
    result.append(row);
  }
  stream.close();
  return result;
}

} // namespace ra
