#pragma once

#include <git2.h>
#include <string>
#include <vector>

#include "git2cpp/abstract.hpp"
#include "git2cpp/commit.hpp"
#include "git2cpp/index.hpp"
#include "git2cpp/signature.hpp"
#include "git2cpp/tree.hpp"

namespace Git {

class Signature;
class Commit;

// Wrapper for git_repository
class Repository : public AbstractClass<Repository, git_repository, git_repository_free> {
private:
  bool initialized = false;
  bool opened = false;

public:
  /* Basic constructor.
   *
   * Initialized and open repository at current directory
   * @throw Git::Exception if repository cannot be opened or initialized
   */
  explicit Repository();

  /* Constructor
   *
   * Will open a given repository
   * @param rep The initialized git repository.
   * @throw Git::Exception if repository cannot be opened or initialized
   */
  explicit Repository(git_repository *repo);

  /* Constructor.
   *
   * Will open a repository
   *
   * @param path The path of initialized repository
   * @throw Git::Exception if repository cannot be opened
   */
  explicit Repository(const std::string &path);

  /*
   * Get the Index file for this repository.
   *
   * If a custom index has not been set, the default index for the repository will be returned
   *
   * @throw Git::Exception if the getting of index fails
   * @return index file
   */
  Index GetIndex();

  /*
   * Lookup a tree object from the repository.
   *
   * @param oid Identity of the tree to locate.
   * @throw Git::Exception if lookup fails
   * @return The tree
   */
  Tree GetTree(const git_oid &oid);

  /*
   * @return The path of the repository.
   */
  const std::string Path() noexcept;

  /*
   * Checks if the repository is open.
   *
   * @return True if the repository is open, false otherwise.
   */
  bool isOpen() const noexcept;

  /*
   * Checks if the repository is initialized.
   *
   * @return True if the repository is initialized, false otherwise.
   */
  bool isInit() const noexcept;

  /*
   * Checks if given branch exists
   *
   * @param name The name of the branch
   * @return True if the branch exists, false otherwise
   */
  bool BranchExists(const std::string &name) const noexcept;

  /*
   * Go through the commit history of a branch
   *
   * @param branch_name The name of the branch
   * @throw Git::Exception if branch does not exist
   * @return vector of commits
   */
  std::vector<Commit> GetCommitHistory(const std::string &branch_name) const;

  /*
   * List all branches of a repository
   *
   * FIX: Should return Git::Exception
   * @return vector of branch names
   */
  std::vector<std::string> GetBranches() const;

  /*
   * Opens an existing Git repository in given path.
   *
   * @throw Throw a Git::Exception if the repository cannot be opened.
   */
  void Open(const std::string &path);

  /*
   * Initialize a repository in the given path
   *
   * @param path The path where the repository will be initialized
   * @throw Throw a Git::Exception if the repository cannot be initialized
   */
  void Init(const std::string &path);

  /*
   * Create a new branch
   *
   * @paramn name Name for the branch; this name is validated for consistency. It should also not
   * conflict with an already existing branch name.
   * @param target Commit to which this branch should point. This object must belong to the this
   * repository.
   * @throw Git::Exception if creating branch fails
   */
  void CreateBranch(const std::string &name, const Commit &target);

  /*
   * Create a commit in the target branch
   *
   * @param message Full message for this commit
   * @param author Signature with author of commit
   * @param commiter  Signature with  commiter of the commit
   * @param index Index with files to commit
   *
   */
  void CreateCommit(const std::string &message, const Signature &author, const Signature &commiter,
                    const Index &index, const std::string &target_branch = "master");
};

} // namespace Git
