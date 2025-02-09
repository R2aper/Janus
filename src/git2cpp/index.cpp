#include <git2.h>
#include <strings.h>

#include "git2cpp/index.hpp"
#include "git2cpp/error.hpp"

namespace Git {

Index::Index() : _index(nullptr) {}
Index::Index(git_index *index) : _index(index) {}

Index::~Index() {
  if (_index)
    git_index_free(_index);
}

void Index::AddAllFiles() {
  if (git_index_add_all(_index, nullptr, 0, nullptr, nullptr) != 0)
    throw Exception();
  git_index_write(_index);
}

void Index::AddFile(const std::string &path) {
  if (git_index_add_bypath(_index, path.c_str()) != 0)
    throw Exception();
  git_index_write(_index);
}

git_oid Index::WriteTree() {
  git_oid tree_id;
  if (git_index_write_tree(&tree_id, _index) != 0)
    throw Exception();
  return tree_id;
}

git_index *Index::ptr() const { return _index; }

Index::Index(Index &&other) noexcept : _index(other._index) { other._index = nullptr; }

Index &Index::operator=(Index &&other) noexcept {
  if (this != &other) {
    if (_index) {
      git_index_free(_index);
    }
    _index = other._index;
    other._index = nullptr;
  }
  return *this;
}

} // namespace
