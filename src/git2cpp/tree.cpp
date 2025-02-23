#include <git2.h>

#include "git2cpp/abstract.hpp"
#include "git2cpp/tree.hpp"

namespace Git {

Tree::Tree() : AbstractClass(nullptr) {};

Tree::Tree(git_tree *tree) : AbstractClass(tree) {};

void Tree::SetTree(git_tree *tree) {
  this->release();
  _handle = tree;
}

git_oid Tree::id() const { return *git_tree_id(_handle); }

}; // namespace Git
