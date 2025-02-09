#pragma once
#ifndef ERROR_HPP
#define ERROR_HPP

#include <git2.h>
#include <stdexcept>

namespace Git {

class Exception : public std::runtime_error {
public:
  Exception();
};

} // namespace Git

#endif // ERROR_HPP
