#pragma once

#include <memory>
#include <utility>

namespace ra {

inline SelectPipe::SelectPipe(const Condition &condition) : condition_(condition) {}

inline const Condition &SelectPipe::condition() const { return condition_; }

inline ProjectPipe::ProjectPipe(const Sequence<std::string> &columns)
    : columns_(makeStringSequence(columns)) {}

inline const StringSequence &ProjectPipe::columns() const { return columns_; }

inline JoinPipe::JoinPipe(ExprPtr right, const std::string &left_column,
                          const std::string &right_column)
    : right_(std::move(right)), left_column_(left_column), right_column_(right_column) {}

inline const ExprPtr &JoinPipe::right() const { return right_; }

inline const std::string &JoinPipe::leftColumn() const { return left_column_; }

inline const std::string &JoinPipe::rightColumn() const { return right_column_; }

inline RenamePipe::RenamePipe(const std::string &alias) : alias_(alias) {}

inline const std::string &RenamePipe::alias() const { return alias_; }

inline ExprPtr Table(const std::string &name, const Sequence<std::string> &schema) {
    return std::make_shared<TableExpr>(name, schema);
}

inline ExprPtr Table(const std::string &name, std::initializer_list<std::string> schema) {
    StringSequence schema_sequence = makeStringSequence(schema);
    return Table(name, schema_sequence);
}

inline ExprPtr Sigma(const Condition &condition, ExprPtr child) {
    return std::make_shared<SelectExpr>(condition, std::move(child));
}

inline ExprPtr Sigma(const std::string &condition, ExprPtr child) {
    return Sigma(Condition(condition), std::move(child));
}

inline ExprPtr Pi(const Sequence<std::string> &columns, ExprPtr child) {
    return std::make_shared<ProjectExpr>(columns, std::move(child));
}

inline ExprPtr Pi(std::initializer_list<std::string> columns, ExprPtr child) {
    StringSequence column_sequence = makeStringSequence(columns);
    return Pi(column_sequence, std::move(child));
}

inline ExprPtr Join(ExprPtr left, ExprPtr right, const std::string &left_column,
                    const std::string &right_column) {
    return std::make_shared<JoinExpr>(std::move(left), std::move(right), left_column,
                                      right_column);
}

inline ExprPtr Rho(const std::string &alias, ExprPtr child) {
    return std::make_shared<RenameExpr>(alias, std::move(child));
}

inline SelectPipe Sigma(const Condition &condition) { return SelectPipe(condition); }

inline SelectPipe Sigma(const std::string &condition) { return SelectPipe(Condition(condition)); }

inline ProjectPipe Pi(const Sequence<std::string> &columns) { return ProjectPipe(columns); }

inline ProjectPipe Pi(std::initializer_list<std::string> columns) {
    StringSequence column_sequence = makeStringSequence(columns);
    return ProjectPipe(column_sequence);
}

inline JoinPipe Join(ExprPtr right, const std::string &left_column,
                     const std::string &right_column) {
    return JoinPipe(std::move(right), left_column, right_column);
}

inline RenamePipe Rho(const std::string &alias) { return RenamePipe(alias); }

inline ExprPtr operator|(ExprPtr left, const SelectPipe &pipe) {
    return Sigma(pipe.condition(), std::move(left));
}

inline ExprPtr operator|(ExprPtr left, const ProjectPipe &pipe) {
    return Pi(pipe.columns(), std::move(left));
}

inline ExprPtr operator|(ExprPtr left, const JoinPipe &pipe) {
    return Join(std::move(left), pipe.right(), pipe.leftColumn(), pipe.rightColumn());
}

inline ExprPtr operator|(ExprPtr left, const RenamePipe &pipe) {
    return Rho(pipe.alias(), std::move(left));
}

}  // namespace ra
