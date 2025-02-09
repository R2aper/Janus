#include "git2cpp/repository.hpp"
#include "git2cpp/error.hpp"
#include <git2.h>
#include <iostream>

namespace Git {

Repository::Repository(git_repository *repo) : _repo(repo) {}
Repository::Repository() : _repo(nullptr) {}
Repository::~Repository() { git_repository_free(_repo); }

void Repository::Open() {
  if (git_repository_open(&_repo, git_repository_workdir(_repo)) != 0) {
    throw Git::Exception();
  }
  opened = true;
}

bool Repository::isOpen() { return opened; }

void Repository::Init(const std::string &path) {
  if (git_repository_init(&_repo, path.c_str(), false) < 0)
    throw Git::Exception();
  initialized = true;
}

bool Repository::isInit() { return initialized; }

const std::string Repository::Path() { return std::string(git_repository_workdir(_repo)); }

git_repository* Repository::ptr() const {return _repo;}

void Repository::commitAll(const std::string &message) {
  git_index *index;
  if (git_repository_index(&index, _repo) != 0) {
    throw Git::Exception();
  }

  if (git_index_add_all(index, nullptr, 0, nullptr, nullptr) != 0) {
    git_index_free(index);
    throw Git::Exception();
  }

  if (git_index_write(index) != 0) {
    git_index_free(index);
    throw Git::Exception();
  }

  git_oid tree_id;
  if (git_index_write_tree(&tree_id, index) != 0) {
    git_index_free(index);
    throw Git::Exception();
  }

  git_tree *tree;
  if (git_tree_lookup(&tree, _repo, &tree_id) != 0) {
    git_index_free(index);
    throw Git::Exception();
  }

  git_signature *signature;
  if (git_signature_default(&signature, _repo) != 0) {
    git_tree_free(tree);
    git_index_free(index);
    throw Git::Exception();
  }

  git_oid commit_id;
  if (git_commit_create_v(&commit_id, _repo, "HEAD", signature, signature, nullptr, message.c_str(),
                          tree, 0) != 0) {
    git_tree_free(tree);
    git_index_free(index);
    git_signature_free(signature);
    throw Git::Exception();
  }

  git_tree_free(tree);
  git_index_free(index);
  git_signature_free(signature);
}
} // namespace Git
