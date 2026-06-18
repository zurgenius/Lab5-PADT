#pragma once

#include "core/array_sequence.h"
#include "core/sequence.h"

#include <initializer_list>
#include <string>

namespace ra {

MutableArraySequence<std::string> makeSequence(const std::string items[],
                                               int count);
MutableArraySequence<std::string>
makeSequence(const Sequence<std::string> &source);
MutableArraySequence<std::string>
makeSequence(std::initializer_list<std::string> items);
bool containsString(const Sequence<std::string> &source,
                    const std::string &value);
std::string joinStrings(const Sequence<std::string> &source,
                        const std::string &separator);

} // namespace ra
