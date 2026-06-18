#pragma once

#include "ast/nodes.h"

namespace ra {

enum class SchemaSide { None, Left, Right, Both, Unknown };

ExprPtr cloneExpr(const ExprPtr &expr);
bool sequenceContains(const Sequence<std::string> &sequence,
                      const std::string &value);
bool appendUnique(MutableArraySequence<std::string> &sequence,
                  const std::string &value);
int countColumnMatches(const Sequence<std::string> &schema,
                       const std::string &column);
SchemaSide locateColumn(const std::string &column,
                        const Sequence<std::string> &left_schema,
                        const Sequence<std::string> &right_schema);
bool allColumnsIncluded(const Sequence<std::string> &required,
                        const Sequence<std::string> &available);
bool sameColumns(const Sequence<std::string> &left,
                 const Sequence<std::string> &right);

} // namespace ra
