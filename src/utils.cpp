#include "core/utils.h"

namespace utils {

int square(const int &value) { return value * value; }

bool is_positive(const int &value) { return value > 0; }

bool is_even(const int &value) { return value % 2 == 0; }

int sum(const int &left, const int &right) { return left + right; }

bool is_zero(const int &value) { return value == 0; }

void read_int(int &value) { std::cin >> value; }

}  // namespace utils
