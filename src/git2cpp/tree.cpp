#include <git2.h>

#include "git2cpp/tree.hpp"

namespace Git {

Tree::Tree() : _tree(nullptr) {}
Tree::Tree(git_tree *tree) : _tree(tree) {}

Tree::~Tree() {
  if (_tree)
    git_tree_free(_tree);
}

void Tree::SetTree(git_tree *tree) {
  if (_tree)
    git_tree_free(_tree);
  _tree = tree;
}

git_oid Tree::id() const { return *git_tree_id(_tree); }

git_tree *Tree::ptr() const { return _tree; }

}; // namespace Git
