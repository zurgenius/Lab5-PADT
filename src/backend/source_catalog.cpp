#include "backend/source_catalog.h"

#include <memory>

namespace ra {
namespace {

std::unique_ptr<MutableArraySequence<std::string>>
cloneSchema(const Sequence<std::string> &schema) {
  return std::make_unique<MutableArraySequence<std::string>>(
      makeSequence(schema));
}

} // namespace

SourceBinding::SourceBinding()
    : kind_(SourceKind::Csv), name_(), location_(), physical_name_(),
      schema_(std::make_unique<MutableArraySequence<std::string>>()) {}

SourceBinding::SourceBinding(SourceKind kind, const std::string &name,
                             const std::string &location,
                             const std::string &physical_name,
                             const Sequence<std::string> &schema)
    : kind_(kind), name_(name), location_(location),
      physical_name_(physical_name), schema_(cloneSchema(schema)) {}

SourceBinding::SourceBinding(const SourceBinding &other)
    : kind_(other.kind_), name_(other.name_), location_(other.location_),
      physical_name_(other.physical_name_),
      schema_(cloneSchema(other.schema())) {}

SourceBinding &SourceBinding::operator=(const SourceBinding &other) {
  if (this != &other) {
    kind_ = other.kind_;
    name_ = other.name_;
    location_ = other.location_;
    physical_name_ = other.physical_name_;
    schema_ = cloneSchema(other.schema());
  }
  return *this;
}

SourceKind SourceBinding::kind() const { return kind_; }
const std::string &SourceBinding::name() const { return name_; }
const std::string &SourceBinding::location() const { return location_; }
const std::string &SourceBinding::physicalName() const {
  return physical_name_;
}
const Sequence<std::string> &SourceBinding::schema() const { return *schema_; }

void SourceCatalog::addCsv(const std::string &name,
                           const std::string &file_path,
                           const Sequence<std::string> &schema) {
  bindings_.append(
      SourceBinding(SourceKind::Csv, name, file_path, name, schema));
}

void SourceCatalog::addSQLite(const std::string &name,
                              const std::string &database_path,
                              const std::string &table_name,
                              const Sequence<std::string> &schema) {
  bindings_.append(SourceBinding(SourceKind::SQLite, name, database_path,
                                 table_name, schema));
}

const SourceBinding *SourceCatalog::find(const std::string &name,
                                         SourceKind kind) const {
  for (int index = 0; index < bindings_.get_count(); index++) {
    const SourceBinding &binding = bindings_.get(index);
    if (binding.name() == name && binding.kind() == kind) {
      return &binding;
    }
  }
  return nullptr;
}

const Sequence<SourceBinding> &SourceCatalog::bindings() const {
  return bindings_;
}

} // namespace ra
