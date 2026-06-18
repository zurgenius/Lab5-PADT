#pragma once

#include "backend/query_result.h"
#include "dsl/predicate.h"

namespace ra {

class PredicateEvaluator {
public:
  bool evaluate(const Condition &condition, const Sequence<std::string> &schema,
                const Row &row) const;
};

} // namespace ra
