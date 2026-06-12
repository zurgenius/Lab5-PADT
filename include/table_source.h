#pragma once

#include <string>

#include "common.h"
#include "io_streams.h"

namespace ra {

class TableSource {
public:
    virtual ~TableSource() = default;

    virtual const Schema& getSchema() const = 0;
    virtual void open() = 0;
    virtual bool next(Row& row) = 0;
    virtual void close() = 0;
};

class CsvTableSource : public TableSource {
public:
    explicit CsvTableSource(const std::string& file_path);

    const Schema& getSchema() const override;
    void open() override;
    bool next(Row& row) override;
    void close() override;

private:
    std::string file_path_;
    Schema schema_;
    FileInputStream input_;
};

class SQLiteTableSource : public TableSource {
public:
    SQLiteTableSource(const std::string& database_path, const std::string& table_name);

    const Schema& getSchema() const override;
    void open() override;
    bool next(Row& row) override;
    void close() override;

private:
    std::string database_path_;
    std::string table_name_;
    Schema schema_;
};

}  // namespace ra
