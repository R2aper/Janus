#include <git2.h>

#include "git2cpp/commit.hpp"
#include "git2cpp/error.hpp"

namespace Git {

Commit::Commit() : _commit(nullptr) {}
Commit::Commit(git_commit *commit) : _commit(commit) {}

Commit::~Commit() {
  if (_commit)
    git_commit_free(_commit);
}

git_oid Commit::id() const noexcept { return *git_commit_id(_commit); }

void Commit::Lookup(const Repository &repo, git_reference *ref) {
  if (git_commit_lookup(&_commit, repo.ptr(), git_reference_target(ref)) != 0)
    throw Exception();
}

git_commit *Commit::ptr() const { return _commit; }

Commit::Commit(Commit &&other) noexcept : _commit(other._commit) { other._commit = nullptr; }

Commit &Commit::operator=(Commit &&other) noexcept {
  if (this != &other) {
    if (_commit) {
      git_commit_free(_commit);
    }
    _commit = other._commit;
    other._commit = nullptr;
  }
  return *this;
}

}; // namespace Git
