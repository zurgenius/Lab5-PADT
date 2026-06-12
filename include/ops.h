#pragma once

#include <memory>
#include <string>

#include "predicate.h"
#include "relational_op.h"
#include "table_source.h"

namespace ra {

class ScanOp : public RelationalOp {
public:
    explicit ScanOp(std::shared_ptr<TableSource> source);

    const Schema& getSchema() const override;
    void open() override;
    bool next(Row& row) override;
    void close() override;
    void accept(Visitor& visitor) override;

    const std::shared_ptr<TableSource>& source() const;

private:
    std::shared_ptr<TableSource> source_;
};

class SelectOp : public RelationalOp {
public:
    SelectOp(PredicatePtr predicate, OpPtr child);

    const Schema& getSchema() const override;
    void open() override;
    bool next(Row& row) override;
    void close() override;
    void accept(Visitor& visitor) override;

    const OpPtr& child() const;
    void setChild(OpPtr child);
    const PredicatePtr& predicate() const;

private:
    PredicatePtr predicate_;
    OpPtr child_;
};

class ProjectOp : public RelationalOp {
public:
    ProjectOp(TextList columns, OpPtr child);

    const Schema& getSchema() const override;
    void open() override;
    bool next(Row& row) override;
    void close() override;
    void accept(Visitor& visitor) override;

    const OpPtr& child() const;
    void setChild(OpPtr child);
    const TextList& columns() const;

private:
    TextList columns_;
    OpPtr child_;
    SimpleArray<int> column_indexes_;
};

class JoinOp : public RelationalOp {
public:
    JoinOp(OpPtr left, OpPtr right, std::string left_column, std::string right_column);

    const Schema& getSchema() const override;
    void open() override;
    bool next(Row& row) override;
    void close() override;
    void accept(Visitor& visitor) override;

    const OpPtr& left() const;
    const OpPtr& right() const;
    void setLeft(OpPtr left);
    void setRight(OpPtr right);
    const std::string& leftColumn() const;
    const std::string& rightColumn() const;

private:
    void rebuildSchema();
    bool rowsMatch(const Row& left_row, const Row& right_row) const;

    OpPtr left_;
    OpPtr right_;
    std::string left_column_;
    std::string right_column_;
    Schema schema_;
    Row current_left_row_;
    bool has_current_left_row_ = false;
    int left_index_ = -1;
    int right_index_ = -1;
};

}  // namespace ra
