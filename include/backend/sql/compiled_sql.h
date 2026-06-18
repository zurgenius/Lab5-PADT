#pragma once

#include "common/sequence_utils.h"
#include "dsl/predicate.h"

namespace ra {

class SqlParameter {
public:
  SqlParameter();
  SqlParameter(const std::string &value, LiteralKind kind);

  const std::string &value() const;
  LiteralKind kind() const;

private:
  std::string value_;
  LiteralKind kind_;
};

class CompiledSql {
public:
  CompiledSql();
  CompiledSql(const std::string &text, const Sequence<SqlParameter> &parameters,
              const Sequence<std::string> &schema);

  const std::string &text() const;
  const Sequence<SqlParameter> &parameters() const;
  const Sequence<std::string> &schema() const;

private:
  std::string text_;
  MutableArraySequence<SqlParameter> parameters_;
  MutableArraySequence<std::string> schema_;
};

} // namespace ra
