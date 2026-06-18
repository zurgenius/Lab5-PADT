#include "backend/query_result.h"

#include <memory>
#include <stdexcept>

namespace ra {
namespace {

std::unique_ptr<MutableArraySequence<Cell>>
cloneCells(const Sequence<Cell> &cells) {
  auto result = std::make_unique<MutableArraySequence<Cell>>();
  for (int index = 0; index < cells.get_count(); index++) {
    result->append(cells.get(index));
  }
  return result;
}

} // namespace

Cell::Cell() : null_(true), value_() {}

Cell::Cell(const std::string &value) : null_(false), value_(value) {}

Cell Cell::null() { return Cell(); }

bool Cell::isNull() const { return null_; }

const std::string &Cell::value() const {
  if (null_) {
    throw std::logic_error("NULL cell has no string value");
  }
  return value_;
}

std::string Cell::display() const { return null_ ? "NULL" : value_; }

Row::Row() : cells_(std::make_unique<MutableArraySequence<Cell>>()) {}

Row::Row(const Sequence<Cell> &cells) : cells_(cloneCells(cells)) {}

Row::Row(const Row &other) : Row(other.cells()) {}

Row &Row::operator=(const Row &other) {
  if (this != &other) {
    cells_ = cloneCells(other.cells());
  }
  return *this;
}

void Row::append(const Cell &cell) { cells_->append(cell); }

const Sequence<Cell> &Row::cells() const { return *cells_; }

const Cell &Row::cell(int index) const { return cells_->get(index); }

int Row::size() const { return cells_->get_count(); }

Option<std::string> Row::value(const Sequence<std::string> &schema,
                               const std::string &column) const {
  for (int index = 0; index < schema.get_count(); index++) {
    if (schema.get(index) == column) {
      if (cell(index).isNull()) {
        return Option<std::string>::None();
      }
      return Option<std::string>::Some(cell(index).value());
    }
  }
  return Option<std::string>::None();
}

QueryResult::QueryResult() = default;

QueryResult::QueryResult(const Sequence<std::string> &schema)
    : schema_(makeSequence(schema)) {}

const Sequence<std::string> &QueryResult::schema() const { return schema_; }

const Sequence<Row> &QueryResult::rows() const { return rows_; }

void QueryResult::append(const Row &row) {
  if (row.size() != schema_.get_count()) {
    throw std::invalid_argument("Row size does not match result schema");
  }
  rows_.append(row);
}

int QueryResult::rowCount() const { return rows_.get_count(); }

} // namespace ra
