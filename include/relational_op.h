#pragma once

#include <memory>

#include "common.h"
#include "visitor.h"

namespace ra {

class RelationalOp {
public:
    virtual ~RelationalOp() = default;

    virtual const Schema& getSchema() const = 0;
    virtual void open() = 0;
    virtual bool next(Row& row) = 0;
    virtual void close() = 0;
    virtual void accept(Visitor& visitor) = 0;
};

using OpPtr = std::shared_ptr<RelationalOp>;

}  // namespace ra
