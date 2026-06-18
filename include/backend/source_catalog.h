#pragma once

#include "common/sequence_utils.h"

#include <memory>
#include <string>

namespace ra {

enum class SourceKind { Csv, SQLite };

class SourceBinding {
public:
  SourceBinding();
  SourceBinding(SourceKind kind, const std::string &name,
                const std::string &location, const std::string &physical_name,
                const Sequence<std::string> &schema);
  SourceBinding(const SourceBinding &other);
  SourceBinding &operator=(const SourceBinding &other);

  SourceKind kind() const;
  const std::string &name() const;
  const std::string &location() const;
  const std::string &physicalName() const;
  const Sequence<std::string> &schema() const;

private:
  SourceKind kind_;
  std::string name_;
  std::string location_;
  std::string physical_name_;
  std::unique_ptr<MutableArraySequence<std::string>> schema_;
};

class SourceCatalog {
public:
  void addCsv(const std::string &name, const std::string &file_path,
              const Sequence<std::string> &schema);
  void addSQLite(const std::string &name, const std::string &database_path,
                 const std::string &table_name,
                 const Sequence<std::string> &schema);

  const SourceBinding *find(const std::string &name, SourceKind kind) const;
  const Sequence<SourceBinding> &bindings() const;

private:
  MutableArraySequence<SourceBinding> bindings_;
};

} // namespace ra
