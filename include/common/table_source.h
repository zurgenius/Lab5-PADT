#pragma once

#include "common/string_sequence.h"

#include <string>

namespace ra {

class TableSource {
  public:
    virtual ~TableSource() = default;

    virtual const std::string &name() const = 0;
    virtual const StringSequence &schema() const = 0;
};

class CsvTableSource final : public TableSource {
  public:
    CsvTableSource(const std::string &name, const std::string &file_path,
                   const Sequence<std::string> &schema);

    const std::string &name() const override;
    const StringSequence &schema() const override;
    const std::string &filePath() const;

  private:
    std::string name_;
    std::string file_path_;
    StringSequence schema_;
};

class SQLiteTableSource final : public TableSource {
  public:
    SQLiteTableSource(const std::string &name, const std::string &database_path,
                      const Sequence<std::string> &schema);

    const std::string &name() const override;
    const StringSequence &schema() const override;
    const std::string &databasePath() const;

  private:
    std::string name_;
    std::string database_path_;
    StringSequence schema_;
};

}  // namespace ra

#include "common/detail/table_source.tpp"
