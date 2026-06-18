#include "common/sequence_utils.h"

#include <sstream>

namespace ra {

MutableArraySequence<std::string> makeSequence(const std::string items[],
                                               int count) {
  MutableArraySequence<std::string> result;
  for (int index = 0; index < count; index++) {
    result.append(items[index]);
  }
  return result;
}

MutableArraySequence<std::string>
makeSequence(const Sequence<std::string> &source) {
  MutableArraySequence<std::string> result;
  for (int index = 0; index < source.get_count(); index++) {
    result.append(source.get(index));
  }
  return result;
}

MutableArraySequence<std::string>
makeSequence(std::initializer_list<std::string> items) {
  MutableArraySequence<std::string> result;
  for (const std::string &item : items) {
    result.append(item);
  }
  return result;
}

bool containsString(const Sequence<std::string> &source,
                    const std::string &value) {
  for (int index = 0; index < source.get_count(); index++) {
    if (source.get(index) == value) {
      return true;
    }
  }
  return false;
}

std::string joinStrings(const Sequence<std::string> &source,
                        const std::string &separator) {
  std::ostringstream output;
  for (int index = 0; index < source.get_count(); index++) {
    if (index > 0) {
      output << separator;
    }
    output << source.get(index);
  }
  return output.str();
}

} // namespace ra
