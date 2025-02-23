#include <git2.h>

#include "git2cpp/error.hpp"
#include "git2cpp/library.hpp"

namespace Git {

Git2Library::Git2Library() {
  if (git_libgit2_init() < 0)
    throw Exception();
}

Git2Library::~Git2Library() { git_libgit2_shutdown(); }

} // namespace Git
