#include "backend/sql/compiled_sql.h"

namespace ra {

SqlParameter::SqlParameter() : value_(), kind_(LiteralKind::Text) {}

SqlParameter::SqlParameter(const std::string &value, LiteralKind kind)
    : value_(value), kind_(kind) {}

const std::string &SqlParameter::value() const { return value_; }
LiteralKind SqlParameter::kind() const { return kind_; }

CompiledSql::CompiledSql() = default;

CompiledSql::CompiledSql(const std::string &text,
                         const Sequence<SqlParameter> &parameters,
                         const Sequence<std::string> &schema)
    : text_(text), schema_(makeSequence(schema)) {
  for (int index = 0; index < parameters.get_count(); index++) {
    parameters_.append(parameters.get(index));
  }
}

const std::string &CompiledSql::text() const { return text_; }
const Sequence<SqlParameter> &CompiledSql::parameters() const {
  return parameters_;
}
const Sequence<std::string> &CompiledSql::schema() const { return schema_; }

} // namespace ra
