#include <filesystem>
#include <iostream>
#include <string>

#include "GPG.hpp"
#include "commands.hpp"
#include "git2cpp/error.hpp"
#include "git2cpp/library.hpp"
#include "git2cpp/repository.hpp"

using namespace Janus;
namespace fs = std::filesystem;

/*
TODO:
* Reload cache of keys
? Parsing arguments, multiple passwords and etc
* Error handling!!

*/

// --create-branch <name_of_branch>
// -b <name_of_branch> commit in specified branch
// sign
// symetric
// to file
void usage() {
  std::cout << "Usage: janus [Options] <command> \n\
Options: \n\
  -h, --help            Show this help message\n\
  -k <fingerprint>      Enter fingerprint of the key to encrypt the password with(In default passwords encrypted with all available keys)\n\
Commands: \n\
  init                  Initialize a new git repository in the current working directory(Similar to git init)\n\
  list                  List all passwords in the vault\n\
  add <name>            Add a new password to the vault\n\
  remove <name>         Remove a password from the vault\n\
  show <name>           Show a password from the vault\n";
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage();
    return 1;
  }

  Git::Git2Library lib;
  InitGpgME();
  std::string fingerprint;

  for (int i = 1; i < argc; i++) {
    std::string command = argv[i];

    if (command == "-h" || command == "--help") {
      usage();
      return 0;

    } else if (command == "init") {
      try {
        Git::Repository rep;
        std::cout << "Initializing git repository at " << rep.Path() << std::endl;

      } catch (const Git::Exception &e) {
        std::cerr << std::endl << e.what() << std::endl;
        return 1;
      }
      return 0;

    } else if (command == "-k" && argc > i + 1) {
      fingerprint = argv[i + 1];
      i++;
      continue;

    } else if (!fs::exists(".git")) {
      std::cerr << std::endl << "Fatal!: directory is not a git repository" << std::endl;
      return 1;

    } else if (command == "remove" && argc > i + 1) {
      if (!fs::exists(std::string(argv[i + 1]) + ".gpg")) {
        std::cerr << std::endl << "Fatal: " << argv[i + 1] << " does not exist!" << std::endl;
        return 1;
      }

      try {
        RemovePassword(argv[i + 1]);

      } catch (const std::exception &e) {
        std::cerr << std::endl << e.what() << std::endl;
        return 1;
      }
      return 0;

    } else if (command == "list") {
      List();
      return 0;

    } else if (command == "add" && argc > i + 1) {
      if (fs::exists(std::string(argv[i + 1]) + ".gpg")) {
        std::cerr << std::endl << "Fatal: " << argv[i + 1] << " already exist!" << std::endl;
        return 1;
      }

      try {
        AddPassword(argv[i + 1], fingerprint);

      } catch (const std::exception &e) {
        std::cerr << std::endl << e.what() << std::endl;
        return 1;
      }
      return 0;

    } else if (command == "show" && argc > i + 1) {
      if (!fs::exists(std::string(argv[i + 1]) + ".gpg")) {
        std::cerr << std::endl << "Fatal: " << argv[i + 1] << " does not exist!" << std::endl;
        return 1;
      }

      try {
        ShowPassword(argv[i + 1]);

      } catch (const std::exception &e) {
        std::cerr << std::endl << e.what() << std::endl;
        return 1;
      }
      return 0;

    } else {
      std::cerr << std::endl << "Invalid command: " << command << std::endl;
      return 1;
    }
  }

  return 0;
}
