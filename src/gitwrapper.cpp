#include <filesystem>
#include <git2.h>
#include <iostream>
#include <string>

/*
Задача Git:
* Создать если что репозиторий и коммитить изменения
* Возможно стоит создать другой репозиторий с попыткой переноса libgit2 на C++

*/

#include "error.hpp"
#include "gitwrapper.hpp"
#include "repo.hpp"

namespace Git {

Git2Library::Git2Library() {
  if (git_libgit2_init() < 0)
    throw Git::Error("Git error:");
}

Git2Library::~Git2Library() { git_libgit2_shutdown(); }

} // namespace Git