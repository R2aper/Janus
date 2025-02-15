#ifndef INDEX_HPP
#define INDEX_HPP

#include <git2.h>
#include <string>

namespace Git {

// Wrapper for git_index
class Index {
private:
  git_index *_index;

public:
  /* Basic constructor.
   *
   * Assign index to nullptr
   */
  Index();

  /* Constructor
   *
   * @param index Existing index
   */
  Index(git_index *index);

  /* Destructor
   *
   * Free index if it is not nullptr
   */
  ~Index();

  /* Add all file to index from working git directory
   *
   * @throw Git::Exception if git_index_add_all fails
   */
  void AddAllFiles();

  /* Add file to index by path
   *
   * @throw Git::exception if git_index_add_bypath fails
   */
  void AddFile(const std::string &path);

  /* Write the index as a tree
   *
   * @throw Git::Exception if git_index_write_tree fails
   * @return tree id
   */
  git_oid WriteTree() const;

  git_index *ptr() const;

  // Disallow copy
  Index(const Index &) = delete;
  // Disallow assign
  Index &operator=(const Index &) = delete;

  // Rules for move
  Index(Index &&other) noexcept;
  Index &operator=(Index &&other) noexcept;
};

} // namespace Git

#endif // INDEX_HPP
