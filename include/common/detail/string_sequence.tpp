#pragma once

#include <sstream>

namespace ra {

inline StringSequence makeStringSequence(const std::string items[], int count) {
    StringSequence result;
    for (int index = 0; index < count; index++) {
        result.append(items[index]);
    }
    return result;
}

inline StringSequence makeStringSequence(const Sequence<std::string> &source) {
    StringSequence result;
    for (int index = 0; index < source.get_count(); index++) {
        result.append(source.get(index));
    }
    return result;
}

inline StringSequence makeStringSequence(std::initializer_list<std::string> items) {
    StringSequence result;
    for (const std::string &item : items) {
        result.append(item);
    }
    return result;
}

inline StringSequence concatStringSequences(const Sequence<std::string> &left,
                                            const Sequence<std::string> &right) {
    StringSequence result = makeStringSequence(left);
    for (int index = 0; index < right.get_count(); index++) {
        result.append(right.get(index));
    }
    return result;
}

inline bool containsString(const Sequence<std::string> &source, const std::string &value) {
    for (int index = 0; index < source.get_count(); index++) {
        if (source.get(index) == value) {
            return true;
        }
    }
    return false;
}

inline std::string joinStrings(const Sequence<std::string> &source,
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

}  // namespace ra
