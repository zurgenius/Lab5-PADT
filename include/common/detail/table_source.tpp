#pragma once

namespace ra {

inline CsvTableSource::CsvTableSource(const std::string &name, const std::string &file_path,
                                      const Sequence<std::string> &schema)
    : name_(name), file_path_(file_path), schema_(makeStringSequence(schema)) {}

inline const std::string &CsvTableSource::name() const { return name_; }

inline const StringSequence &CsvTableSource::schema() const { return schema_; }

inline const std::string &CsvTableSource::filePath() const { return file_path_; }

inline SQLiteTableSource::SQLiteTableSource(const std::string &name,
                                            const std::string &database_path,
                                            const Sequence<std::string> &schema)
    : name_(name), database_path_(database_path), schema_(makeStringSequence(schema)) {}

inline const std::string &SQLiteTableSource::name() const { return name_; }

inline const StringSequence &SQLiteTableSource::schema() const { return schema_; }

inline const std::string &SQLiteTableSource::databasePath() const { return database_path_; }

}  // namespace ra
