#pragma once
#ifndef TREE_HPP
#define TREE_HPP

#include <git2.h>

#include "git2cpp/abstract.hpp"

namespace Git {

// Wrapper for git_tree
class Tree : public AbstractClass<Tree, git_tree, git_tree_free> {
public:
  /* Basic constructor
   *
   * Assign tree to nullptr
   */
  explicit Tree();

  /* Constructor
   *
   * @param tree Existing tree
   */
  explicit Tree(git_tree *tree);

  /*
   * Set a tree to given git_tree, old tree will be freed
   *
   * @param tree Existing tree
   */
  void SetTree(git_tree *tree);

  // returns the tree's id
  git_oid id() const;
};

}; // namespace Git

#endif // TREE_HPP
