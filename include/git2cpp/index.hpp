#pragma once
#ifndef INDEX_HPP
#define INDEX_HPP

#include <git2.h>
#include <string>

#include "git2cpp/abstract.hpp"

namespace Git {

// Wrapper for git_index
class Index : public AbstractClass<Index, git_index, git_index_free> {
public:
  /* Basic constructor.
   *
   * Assign index to nullptr
   */
  explicit Index();

  /* Constructor
   *
   * @param index Existing index
   */
  explicit Index(git_index *index);

  /*
   * Add all file to index from working git directory
   *
   * @throw Git::Exception if git_index_add_all fails
   */
  void AddAllFiles();

  /*
   * Add file to index by path
   *
   * @param path Path to file
   * @throw Git::exception if git_index_add_bypath fails
   */
  void AddFile(const std::string &path);

  /*
   * Write the index as a tree
   *
   * @throw Git::Exception if git_index_write_tree fails
   * @return tree id
   */
  git_oid WriteTree() const;
};

} // namespace Git

#endif // INDEX_HPP
