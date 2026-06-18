#include "common/table_source.h"

namespace ra {

CsvTableSource::CsvTableSource(const std::string &name,
                               const std::string &file_path,
                               const Sequence<std::string> &schema)
    : name_(name), file_path_(file_path), schema_(makeSequence(schema)) {}

const std::string &CsvTableSource::name() const { return name_; }

const Sequence<std::string> &CsvTableSource::schema() const { return schema_; }

const std::string &CsvTableSource::filePath() const { return file_path_; }

SQLiteTableSource::SQLiteTableSource(const std::string &name,
                                     const std::string &database_path,
                                     const Sequence<std::string> &schema)
    : name_(name), database_path_(database_path),
      schema_(makeSequence(schema)) {}

const std::string &SQLiteTableSource::name() const { return name_; }

const Sequence<std::string> &SQLiteTableSource::schema() const {
  return schema_;
}

const std::string &SQLiteTableSource::databasePath() const {
  return database_path_;
}

} // namespace ra
