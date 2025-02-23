#include <git2.h>
#include <vector>

#include "git2cpp/commit.hpp"
#include "git2cpp/error.hpp"
#include "git2cpp/repository.hpp"
#include "git2cpp/tree.hpp"

namespace Git {

Repository::Repository() {
  this->Init("./");
  this->Open("./");
}

Repository::Repository(git_repository *repo) {
  this->Open(git_repository_workdir(_repo));
  initialized = true;
}

Repository::Repository(const std::string &path) {
  this->Open(path);
  initialized = true;
}

Repository::~Repository() {
  if (_repo)
    git_repository_free(_repo);
}

Index Repository::GetIndex() {
  git_index *_index;
  if (git_repository_index(&_index, _repo) != 0)
    throw Exception();
  return Index(_index);
}

Tree Repository::GetTree(const git_oid &oid) {
  git_tree *tree;
  if (git_tree_lookup(&tree, _repo, &oid) != 0)
    throw Exception();
  return Tree(tree);
}

const std::string Repository::Path() noexcept { return std::string(git_repository_workdir(_repo)); }

bool Repository::isOpen() const noexcept { return opened; }
bool Repository::isInit() const noexcept { return initialized; }

bool Repository::BranchExists(const std::string &name) const noexcept {
  git_reference *ref;
  return git_branch_lookup(&ref, _repo, name.c_str(), GIT_BRANCH_LOCAL) == 0;
}

std::vector<Commit> Repository::GetCommitHistory(const std::string &branch_name) const {
  git_revwalk *walker;
  git_revwalk_new(&walker, _repo);
  git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_REVERSE);

  git_reference *branch_ref;
  if (git_branch_lookup(&branch_ref, _repo, branch_name.c_str(), GIT_BRANCH_LOCAL) != 0) {
    git_revwalk_free(walker);
    throw Exception();
  }
  git_revwalk_push_ref(walker, git_reference_name(branch_ref));

  std::vector<Commit> history;
  git_oid oid;
  while (git_revwalk_next(&oid, walker) == 0) {
    git_commit *commit;
    git_commit_lookup(&commit, _repo, &oid);
    history.emplace_back(commit);
  }

  git_reference_free(branch_ref);
  git_revwalk_free(walker);
  return history;
}

std::vector<std::string> Repository::GetBranches() const {
  std::vector<std::string> branches;
  git_branch_iterator *iter;
  git_branch_iterator_new(&iter, _repo, GIT_BRANCH_ALL); // FIX: Can return error code

  git_reference *ref;
  git_branch_t type;
  while (git_branch_next(&ref, &type, iter) == 0) {
    const char *name;
    git_branch_name(&name, ref); // FIX: Can return error code
    branches.push_back(name);
    git_reference_free(ref);
  }

  git_branch_iterator_free(iter);
  return branches;
}

void Repository::Open(const std::string &path) {
  if (git_repository_open(&_repo, path.c_str()) != 0)
    throw Git::Exception();
  opened = true;
}

void Repository::Init(const std::string &path) {
  if (git_repository_init(&_repo, path.c_str(), false) < 0)
    throw Git::Exception();
  initialized = true;
}

void Repository::CreateBranch(const std::string &name, const Commit &target) {
  git_reference *branch;
  if (git_branch_create(&branch, _repo, name.c_str(), target.ptr(), 0) != 0) {
    throw Exception();
  }
  git_reference_free(branch);
}

void Repository::CreateCommit(const std::string &message, const Signature &author,
                              const Signature &commiter, const Index &index,
                              const std::string &target_branch) {

  git_oid tree_id = index.WriteTree();
  Tree tree = GetTree(tree_id);

  git_reference *branch_ref;
  Commit parent;
  if (git_branch_lookup(&branch_ref, _repo, target_branch.c_str(), GIT_BRANCH_LOCAL) ==
      0) { // FIX: Exception?
    parent.Lookup(*this, branch_ref);
    git_reference_free(branch_ref);
  }

  git_oid commit_id;
  // const std::vector<Commit> parents; //FIX:

  const git_commit *parents[] = {parent.ptr()};
  std::string full_ref = "refs/heads/" + target_branch;

  if (git_commit_create(&commit_id, _repo, full_ref.c_str(), author.ptr(), commiter.ptr(), nullptr,
                        message.c_str(), tree.ptr(), parent.ptr() ? 1 : 0, parents) != 0) {
    throw Exception();
  }
}

git_repository *Repository::ptr() const { return _repo; }

Repository::Repository(Repository &&other) noexcept : _repo(other._repo) { other._repo = nullptr; }

Repository &Repository::operator=(Repository &&other) noexcept {
  if (this != &other) {
    if (_repo)
      git_repository_free(_repo);
    _repo = other._repo;
    other._repo = nullptr;
  }
  return *this;
}

} // namespace Git
