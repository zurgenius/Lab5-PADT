#include "table_source.h"

namespace ra {

CsvTableSource::CsvTableSource(const std::string& file_path) : file_path_(file_path) {}

const Schema& CsvTableSource::getSchema() const {
    return schema_;
}

void CsvTableSource::open() {
    // Person 3 will add CSV opening logic here.
}

bool CsvTableSource::next(Row&) {
    return false;
}

void CsvTableSource::close() {
    // Person 3 will add CSV cleanup logic here.
}

SQLiteTableSource::SQLiteTableSource(const std::string& database_path, const std::string& table_name)
    : database_path_(database_path), table_name_(table_name) {}

const Schema& SQLiteTableSource::getSchema() const {
    return schema_;
}

void SQLiteTableSource::open() {
    // Person 3 will add SQLite opening logic here.
}

bool SQLiteTableSource::next(Row&) {
    return false;
}

void SQLiteTableSource::close() {
    // Person 3 will add SQLite cleanup logic here.
}

}  // namespace ra
