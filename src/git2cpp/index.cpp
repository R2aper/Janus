#include <git2.h>
#include <string>

#include "git2cpp/abstract.hpp"
#include "git2cpp/error.hpp"
#include "git2cpp/index.hpp"

namespace Git {

Index::Index() : AbstractClass(nullptr) {}
Index::Index(git_index *index) : AbstractClass(index) {}

void Index::AddAllFiles() {
  if (git_index_add_all(_handle, nullptr, 0, nullptr, nullptr) != 0)
    throw Exception();
  git_index_write(_handle);
}

void Index::AddFile(const std::string &path) {
  if (git_index_add_bypath(_handle, path.c_str()) != 0)
    throw Exception();
  git_index_write(_handle);
}

git_oid Index::WriteTree() const {
  git_oid tree_id;
  if (git_index_write_tree(&tree_id, _handle) != 0)
    throw Exception();
  return tree_id;
}

} // namespace Git
