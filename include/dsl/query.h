#pragma once

#include "ast/nodes.h"

#include <initializer_list>
#include <string>

namespace ra {

class SelectPipe {
public:
  explicit SelectPipe(const Condition &condition);
  const Condition &condition() const;

private:
  Condition condition_;
};

class ProjectPipe {
public:
  explicit ProjectPipe(const Sequence<std::string> &columns);
  const Sequence<std::string> &columns() const;

private:
  MutableArraySequence<std::string> columns_;
};

class JoinPipe {
public:
  JoinPipe(ExprPtr right, const std::string &left_column,
           const std::string &right_column);
  const ExprPtr &right() const;
  const std::string &leftColumn() const;
  const std::string &rightColumn() const;

private:
  ExprPtr right_;
  std::string left_column_;
  std::string right_column_;
};

class RenamePipe {
public:
  explicit RenamePipe(const std::string &alias);
  const std::string &alias() const;

private:
  std::string alias_;
};

ExprPtr Table(const std::string &name, const Sequence<std::string> &schema);
ExprPtr Table(const std::string &name,
              std::initializer_list<std::string> schema);
ExprPtr Sigma(const Condition &condition, ExprPtr child);
ExprPtr Sigma(const std::string &condition, ExprPtr child);
ExprPtr Pi(const Sequence<std::string> &columns, ExprPtr child);
ExprPtr Pi(std::initializer_list<std::string> columns, ExprPtr child);
ExprPtr Join(ExprPtr left, ExprPtr right, const std::string &left_column,
             const std::string &right_column);
ExprPtr Rho(const std::string &alias, ExprPtr child);

SelectPipe Sigma(const Condition &condition);
SelectPipe Sigma(const std::string &condition);
ProjectPipe Pi(const Sequence<std::string> &columns);
ProjectPipe Pi(std::initializer_list<std::string> columns);
JoinPipe Join(ExprPtr right, const std::string &left_column,
              const std::string &right_column);
RenamePipe Rho(const std::string &alias);

ExprPtr operator|(ExprPtr left, const SelectPipe &pipe);
ExprPtr operator|(ExprPtr left, const ProjectPipe &pipe);
ExprPtr operator|(ExprPtr left, const JoinPipe &pipe);
ExprPtr operator|(ExprPtr left, const RenamePipe &pipe);

} // namespace ra
