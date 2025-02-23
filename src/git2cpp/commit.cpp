#include <git2.h>

#include "git2cpp/abstract.hpp"
#include "git2cpp/commit.hpp"
#include "git2cpp/error.hpp"

namespace Git {

Commit::Commit() : AbstractClass(nullptr) {}
Commit::Commit(git_commit *commit) : AbstractClass(commit) {}

git_oid Commit::id() const noexcept { return *git_commit_id(_handle); }

void Commit::Lookup(const Repository &repo, git_reference *ref) {
  if (git_commit_lookup(&_handle, repo.ptr(), git_reference_target(ref)) != 0)
    throw Exception();
}

}; // namespace Git
