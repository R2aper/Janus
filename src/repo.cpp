#include "repo.hpp"
#include "error.hpp"
#include <git2.h>
#include <iostream>

namespace Git {

Repo::Repo(git_repository *repo) : _repo(repo) {}
Repo::Repo() : _repo(nullptr) {}
Repo::~Repo() { git_repository_free(_repo); }

void Repo::Open() {
  if (git_repository_open(&_repo, git_repository_workdir(_repo)) != 0) {
    throw Git::Error("Git error:");
  }
  opened = true;
}

bool Repo::isOpen() { return opened; }

void Repo::Init(const std::string &path) {
  if (git_repository_init(&_repo, path.c_str(), false) < 0)
    throw Git::Error("Git error:");
  initialized = true;
}

bool Repo::isInit() { return initialized; }

const std::string Repo::Path() { return std::string(git_repository_workdir(_repo)); }

void Repo::commitAll(const std::string &message) {
  git_index *index;
  if (git_repository_index(&index, _repo) != 0) {
    throw Git::Error("Git error:");
  }

  if (git_index_add_all(index, nullptr, 0, nullptr, nullptr) != 0) {
    git_index_free(index);
    throw Git::Error("Git error:");
  }

  if (git_index_write(index) != 0) {
    git_index_free(index);
    throw Git::Error("Git error:");
  }

  git_oid tree_id;
  if (git_index_write_tree(&tree_id, index) != 0) {
    git_index_free(index);
    throw Git::Error("Git error:");
  }

  git_tree *tree;
  if (git_tree_lookup(&tree, _repo, &tree_id) != 0) {
    git_index_free(index);
    throw Git::Error("Git error:");
  }

  git_signature *signature;
  if (git_signature_default(&signature, _repo) != 0) {
    git_tree_free(tree);
    git_index_free(index);
    throw Git::Error("Git error:");
  }

  git_oid commit_id;
  if (git_commit_create_v(&commit_id, _repo, "HEAD", signature, signature, nullptr, message.c_str(),
                          tree, 0) != 0) {
    git_tree_free(tree);
    git_index_free(index);
    git_signature_free(signature);
    throw Git::Error("Git error:");
  }

  git_tree_free(tree);
  git_index_free(index);
  git_signature_free(signature);
}
} // namespace Git