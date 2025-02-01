#ifndef ERROR_HPP
#define ERROR_HPP

#include <git2.h>
#include <iostream>
#include <string>

namespace Git {

class Error : public std::runtime_error {
  const git_error *error = git_error_last();

public:
  Error(const std::string &message);
  const char *what() const noexcept override;
};

} // namespace Git

#endif // ERROR_HPP