#pragma once

namespace ra {

inline std::string exprKindToString(ExprKind kind) {
    switch (kind) {
        case ExprKind::Table:
            return "Table";
        case ExprKind::Select:
            return "Select";
        case ExprKind::Project:
            return "Project";
        case ExprKind::Join:
            return "Join";
        case ExprKind::Rename:
            return "Rename";
    }
    return "Unknown";
}

}  // namespace ra
