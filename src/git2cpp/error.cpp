#include "git2cpp/error.hpp"
#include <git2.h>

namespace Git {

Error::Error(const std::string &message) : std::runtime_error(message) {}
const char *Error::what() const noexcept { return error->message; }

} // namespace Git
