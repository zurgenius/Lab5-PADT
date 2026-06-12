#include <memory>
#include <string>
#include "ops.h"
#include "query_optimizer.h"

namespace {

class GreaterThanPredicate : public ra::Predicate {
public:
    GreaterThanPredicate(const std::string& column_name, int threshold)
        : column_name_(column_name), threshold_(threshold), description_("age > 25") {}

    bool evaluate(const ra::Row& row, const ra::Schema& schema) const override {
        const int index = ra::findColumnIndex(schema, column_name_);
        if (index < 0) {
            return false;
        }

        const ra::Value& value = row[static_cast<std::size_t>(index)];
        if (value.type != ra::ValueType::Integer) {
            return false;
        }

        return value.int_value > threshold_;
    }

    const std::string& description() const override {
        return description_;
    }

private:
    std::string column_name_;
    int threshold_;
    std::string description_;
};

}  // namespace

int main() {
    using namespace ra;

    ConsoleOutputStream& out = console();

    const std::string users_path = std::string(PROJECT_SOURCE_DIR) + "/data/users.csv";
    const std::string departments_path = std::string(PROJECT_SOURCE_DIR) + "/data/departments.csv";

    OpPtr query_root = std::make_shared<ProjectOp>(
        TextList{"name", "department_name"},
        std::make_shared<JoinOp>(
            std::make_shared<SelectOp>(
                std::make_shared<GreaterThanPredicate>("age", 25),
                std::make_shared<ScanOp>(std::make_shared<CsvTableSource>(users_path))),
            std::make_shared<ScanOp>(std::make_shared<CsvTableSource>(departments_path)),
            "department_id",
            "id"));

    QueryOptimizer optimizer;
    OpPtr optimized_root = optimizer.optimize(query_root);
    Row row_buffer;

    out.writeText("Optimizer notes:\n");
    for (std::size_t index = 0; index < optimizer.notes().size(); ++index) {
        out.writeText(" - ");
        out.writeString(optimizer.notes()[index]);
        out.writeChar('\n');
    }

    out.writeText("\nPerson 3 demo result:\n");
    optimized_root->open();
    while (optimized_root->next(row_buffer)) {
        printRow(row_buffer, out);
    }
    optimized_root->close();
    return 0;
}
