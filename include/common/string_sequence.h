#pragma once

#include "core/array_sequence.h"
#include "core/sequence.h"

#include <initializer_list>
#include <string>

namespace ra {

using StringSequence = MutableArraySequence<std::string>;

StringSequence makeStringSequence(const std::string items[], int count);
StringSequence makeStringSequence(const Sequence<std::string> &source);
StringSequence makeStringSequence(std::initializer_list<std::string> items);
StringSequence concatStringSequences(const Sequence<std::string> &left,
                                     const Sequence<std::string> &right);
bool containsString(const Sequence<std::string> &source, const std::string &value);
std::string joinStrings(const Sequence<std::string> &source, const std::string &separator);

}  // namespace ra

#include "common/detail/string_sequence.tpp"
