#pragma once

#include <stdexcept>

namespace Git {

class Exception : public std::runtime_error {
public:
  Exception();
};

} // namespace Git
