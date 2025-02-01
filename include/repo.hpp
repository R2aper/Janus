#ifndef REPO_HPP
#define REPO_HPP

#include <git2.h>
#include <string>
namespace Git {

// .commit
//. newtree
// breanch
// другие команды git связанные с репозиторием

class Repo {
private:
  git_repository *_repo;
  bool initialized = false;
  bool opened = false;

public:
  // Открываем существующий репозиторий
  // Создаем новый

  /**
   * Constructor. Opens a given Git repository.
   *
   * @param rep The existing repository that would be open.
   * @throw Throw a std::runtime_error if the repository cannot be opened.
   */
  Repo(git_repository *repo);

  // Basic constructor. Create a new repository that is not initialized
  Repo();

  // Destructor. Free git_repository object
  ~Repo();

  /**
   * Opens an existing Git repository.
   *
   * @throw Throw a std::runtime_error if the repository cannot be opened.
   */
  void Open();

  /**
   * Checks if the repository is open.
   * @return True if the repository is open, false otherwise.
   */
  bool isOpen();

  /**
   * Initialize a repository in the given path
   * @param path The path where the repository will be initialized
   * @return The initialized repository
   *@throw Throw a std::runtime_error if the repository cannot be initialized
   */
  void Init(const std::string &path);

  /**
   * Checks if the repository is initialized.
   * @return True if the repository is initialized, false otherwise.
   */
  bool isInit();

  /**
   * @return The path of the repository.
   */
  const std::string Path();

  // void commit(const std::string &message, Index index, Tree tree, Signature signature);

  void commitAll(const std::string &message);
};

} // namespace Git

#endif // REPO_HPP