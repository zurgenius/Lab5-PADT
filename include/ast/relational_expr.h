#pragma once

#include "common/string_sequence.h"

#include <memory>
#include <string>

namespace ra {

class Visitor;

enum class ExprKind {
    Table,
    Select,
    Project,
    Join,
    Rename,
};

class RelationalExpr {
  public:
    virtual ~RelationalExpr() = default;

    virtual ExprKind kind() const = 0;
    virtual const StringSequence &schema() const = 0;
    virtual void accept(Visitor &visitor) const = 0;
};

using ExprPtr = std::shared_ptr<RelationalExpr>;

std::string exprKindToString(ExprKind kind);

}  // namespace ra

#include "ast/detail/relational_expr.tpp"
