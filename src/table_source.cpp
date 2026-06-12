#include "table_source.h"

#include <cctype>
#include <stdexcept>

namespace ra {

namespace {

std::string trim(const std::string& value) {
    std::size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])) != 0) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) {
        --end;
    }

    return value.substr(start, end - start);
}

SimpleArray<std::string> splitCsvLine(const std::string& line) {
    SimpleArray<std::string> parts;
    std::string current_part;

    for (std::size_t index = 0; index < line.size(); ++index) {
        if (line[index] == ',') {
            parts.pushBack(trim(current_part));
            current_part.clear();
            continue;
        }

        current_part += line[index];
    }

    parts.pushBack(trim(current_part));

    return parts;
}

bool isInteger(const std::string& value) {
    if (value.empty()) {
        return false;
    }

    std::size_t index = 0;
    if (value[0] == '-') {
        if (value.size() == 1) {
            return false;
        }
        index = 1;
    }

    for (; index < value.size(); ++index) {
        if (!std::isdigit(static_cast<unsigned char>(value[index]))) {
            return false;
        }
    }

    return true;
}

Value parseValue(const std::string& cell) {
    if (isInteger(cell)) {
        return Value(std::stoi(cell));
    }

    return Value(cell);
}

}  // namespace

CsvTableSource::CsvTableSource(const std::string& file_path) : file_path_(file_path), input_(file_path) {
    ensureSchemaLoaded();
}

const Schema& CsvTableSource::getSchema() const {
    return schema_;
}

void CsvTableSource::open() {
    close();
    if (!input_.open()) {
        throw std::runtime_error("Failed to open CSV file: " + file_path_);
    }

    std::string header_line;
    input_.readLine(header_line);
}

bool CsvTableSource::next(Row& row) {
    if (!input_.isOpen()) {
        throw std::runtime_error("CSV source is not open: " + file_path_);
    }

    std::string line;
    while (input_.readLine(line)) {
        if (trim(line).empty()) {
            continue;
        }

        const SimpleArray<std::string> cells = splitCsvLine(line);
        row.clear();
        for (std::size_t index = 0; index < cells.size(); ++index) {
            row.pushBack(parseValue(cells[index]));
        }
        return true;
    }

    return false;
}

void CsvTableSource::close() {
    input_.close();
}

void CsvTableSource::ensureSchemaLoaded() {
    if (!schema_.empty()) {
        return;
    }

    FileInputStream schema_stream(file_path_);
    if (!schema_stream.open()) {
        throw std::runtime_error("Failed to inspect CSV file: " + file_path_);
    }

    std::string header_line;
    if (!schema_stream.readLine(header_line)) {
        schema_stream.close();
        throw std::runtime_error("CSV file is empty: " + file_path_);
    }
    schema_stream.close();

    const SimpleArray<std::string> header_parts = splitCsvLine(header_line);
    schema_.clear();
    for (std::size_t index = 0; index < header_parts.size(); ++index) {
        schema_.pushBack(header_parts[index]);
    }
}

SQLiteTableSource::SQLiteTableSource(const std::string& database_path, const std::string& table_name)
    : database_path_(database_path), table_name_(table_name) {}

const Schema& SQLiteTableSource::getSchema() const {
    return schema_;
}

void SQLiteTableSource::open() {
    throw std::runtime_error(
        "SQLite backend is reserved for the next Person 3 step. Database: " + database_path_ + ", table: " + table_name_);
}

bool SQLiteTableSource::next(Row&) {
    return false;
}

void SQLiteTableSource::close() {
    // The SQLite backend is not connected yet.
}

}  // namespace ra
