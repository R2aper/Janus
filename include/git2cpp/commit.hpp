#pragma once
#ifndef COMMIT_HPP
#define COMMIT_HPP

#include <git2.h>

#include "git2cpp/repository.hpp"

namespace Git {

class Repository;

// Wrapper for git_commit
class Commit {
private:
  git_commit *_commit;

public:
  /* Basic constructor.
   *
   * Assign commit to nullptr
   */
  Commit();

  /* Constructor
   *
   * @param commit Existing commit
   */
  Commit(git_commit *commit);

  /* Destructor
   *
   * Free commit if it is not nullptr
   */
  ~Commit();

  // @return commit id
  git_oid id() const noexcept;

  /*
   * Lookup a commit object from a repository.
   *
   * @param repo The repository to use when locating the Commit
   * @param ref Direct reference
   * @throw Git::Exception if lookup fails
   */
  void Lookup(const Repository &repo, git_reference *ref);

  git_commit *ptr() const;

  Commit(const Commit &) = delete;
  Commit &operator=(const Commit &) = delete;

  Commit(Commit &&other) noexcept;
  Commit &operator=(Commit &&other) noexcept;
};

}; // namespace Git

#endif // COMMIT_HPP
