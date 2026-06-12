#pragma once

#include <memory>
#include <string>

#include "common.h"

namespace ra {

class Predicate {
public:
    virtual ~Predicate() = default;

    virtual bool evaluate(const Row& row, const Schema& schema) const = 0;
    virtual const std::string& description() const = 0;
};

using PredicatePtr = std::shared_ptr<Predicate>;

}  // namespace ra
