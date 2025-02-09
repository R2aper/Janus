#include <git2.h>

#include "git2cpp/error.hpp"

namespace Git {

Exception::Exception() : std::runtime_error(git_error_last() ? git_error_last()->message: "Unknown Git error!") {}

} // namespace Git
