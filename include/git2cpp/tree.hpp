#pragma once
#ifndef TREE_HPP
#define TREE_HPP

#include <git2.h>

namespace Git {

// Wrapper for git_tree
class Tree {
private:
  git_tree *_tree;

public:
  /* Basic constructor
   *
   * Assign tree to nullptr
   */
  Tree();

  /* Constructor
   *
   * @param tree Existing tree
   */
  Tree(git_tree *tree);

  /* Destructor
   *
   * Free tree if it is not nullptr
   */
  ~Tree();

  /*
   * Set a tree to given git_tree, old tree will be freed
   *
   * @param tree Existing tree
   */
  void SetTree(git_tree *tree);

  // returns the tree's id
  git_oid id() const;

  git_tree *ptr() const;

  Tree(const Tree &) = delete;
  Tree &operator=(const Tree &) = delete;

  Tree(Tree &&other) noexcept;
  Tree &operator=(Tree &&other) noexcept;
};

}; // namespace Git

#endif // TREE_HPP
