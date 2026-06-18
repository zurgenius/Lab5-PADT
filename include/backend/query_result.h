#pragma once

#include "common/sequence_utils.h"
#include "core/option.h"

#include <memory>
#include <string>

namespace ra {

class Cell {
public:
  Cell();
  explicit Cell(const std::string &value);

  static Cell null();
  bool isNull() const;
  const std::string &value() const;
  std::string display() const;

private:
  bool null_;
  std::string value_;
};

class Row {
public:
  Row();
  explicit Row(const Sequence<Cell> &cells);
  Row(const Row &other);
  Row &operator=(const Row &other);

  void append(const Cell &cell);
  const Sequence<Cell> &cells() const;
  const Cell &cell(int index) const;
  int size() const;
  Option<std::string> value(const Sequence<std::string> &schema,
                            const std::string &column) const;

private:
  std::unique_ptr<MutableArraySequence<Cell>> cells_;
};

class QueryResult {
public:
  QueryResult();
  explicit QueryResult(const Sequence<std::string> &schema);

  const Sequence<std::string> &schema() const;
  const Sequence<Row> &rows() const;
  void append(const Row &row);
  int rowCount() const;

private:
  MutableArraySequence<std::string> schema_;
  MutableArraySequence<Row> rows_;
};

} // namespace ra
