#pragma once
#ifndef COMMIT_HPP
#define COMMIT_HPP

#include <git2.h>

#include "git2cpp/abstract.hpp"
#include "git2cpp/repository.hpp"

namespace Git {

class Repository;

// Wrapper for git_commit
class Commit : public AbstractClass<Commit, git_commit, git_commit_free> {
public:
  /* Basic constructor.
   *
   * Assign commit to nullptr
   */
  explicit Commit();

  /* Constructor
   *
   * @param commit Existing commit
   */
  explicit Commit(git_commit *commit);

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
};

}; // namespace Git

#endif // COMMIT_HPP
