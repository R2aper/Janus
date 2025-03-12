#include <exception>
#include <filesystem>
#include <gpgme++/gpgmepp_version.h>
#include <iostream>
#include <ostream>
#include <string>

#include "GPG.hpp"
#include "argparser.hpp"
#include "commands.hpp"
#include "git2cpp/library.hpp"
#include "git2cpp/repository.hpp"
#include "utils.hpp"

using namespace Janus;
namespace fs = std::filesystem;

// argparse
/*
TODO:
* Add new options
* Class options?
* Git2
* Readme + license + wiki

*/

// --quiet don't create commit
// --create-branch <name_of_branch>
// -b --branch <name_of_branch> commit in specified branch
// sign <name> encrypt and sign passw(Add passw to vault)
// verify <name> verify a signature of passw(Prints signature not content of passw)
// --symetric(encrypt flags)
// -o <name_of_file> <name> save passw to file
void usage() {
  std::cout << "Usage: janus [Options] <command> \n\
Options: \n\
  -h, --help            Show this help message\n\
  -v, --version         Show version of janus and used libraries\n\
  -k <fingerprints>      Enter fingerprints of the keys to encrypt the password with(In default passwords encrypted with all available keys)\n\
Commands: \n\
  init                  Initialize a new git repository in the current working directory(Similar to git init)\n\
  list                  List all passwords in the vault\n\
  add <name>            Add a new password to the vault\n\
  remove <name>         Remove a password from the vault\n\
  show <name>           Show a password from the vault\n";
}

void version(const Git::Git2Library &lib) {
  std::cout << "janus " << JANUS_VERSION << std::endl
            << "libgit2 " << lib.Version() << std::endl
            << "gpgmepp " << GPGMEPP_VERSION_STRING << std::endl;
  // TODO: dirhome
}

void arguments(ArgParser &parser) {
  parser.AddArg("-h", "--help", ArgType::FLAG);
  parser.AddArg("-k", "", ArgType::MULTIPLEOPTION);
  parser.AddArg("-v", "--version", ArgType::FLAG);
  //  parser.AddArg("-q", "--quiet", ArgType::FLAG);
  parser.AddArg("", "list", ArgType::FLAG);
  parser.AddArg("", "init", ArgType::FLAG);
  parser.AddArg("", "add", ArgType::OPTION);
  //  parser.AddArg("", "sign", ArgType::OPTION);
  parser.AddArg("", "remove", ArgType::OPTION);
  parser.AddArg("", "show", ArgType::OPTION);
  //  parser.AddArg("", "verify", ArgType::OPTION);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage();
    return 1;
  }

  Git::Git2Library lib;
  InitGpgME();
  std::vector<std::string> fingerprints;
  std::string name;
  ArgParser parser;
  arguments(parser);

  try {
    parser.Parse(argc, argv);

    if (parser.isSet("-h")) {
      usage();
      return 0;
    }

    if (parser.isSet("-v")) {
      version(lib);
      return 0;
    }

    if (!fs::exists(".git"))
      throw std::runtime_error("Fatal! Not a git repository");

    if (parser.isSet("list")) {
      List();
      return 0;
    }

    if (parser.isSet("-k"))
      fingerprints = parser.getValues("-k");

    if (parser.isSet("init")) {
      Git::Repository rep;
      //  rep.Init("./");
      std::cout << "Initializing git repository at " << rep.Path() + "/.git" << std::endl;
      return 0;
    }

    if (parser.isSet("add")) {
      name = parser.getValue("add");

      if (fs::exists(name + ".gpg"))
        throw std::runtime_error("Fatal! " + name + ".gpg exists!");

      AddPassword(name, fingerprints);
      return 0;
    }

    if (parser.isSet("remove")) {
      name = parser.getValue("remove");

      if (!fs::exists(name + ".gpg"))
        throw std::runtime_error("Fatal! " + name + ".gpg doesn't exists!");

      RemovePassword(name);
      return 0;
    }

    if (parser.isSet("show")) {
      name = parser.getValue("show");

      if (!fs::exists(name + ".gpg"))
        throw std::runtime_error("Fatal! " + name + ".gpg doesn't exists!");

      ShowPassword(name);
      return 0;
    }

  } catch (const std::exception &e) {
    std::cerr << std::endl << e.what() << std::endl;
    return 1;
  }
}
