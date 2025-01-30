#include <iostream>
#include <string>

#include "libpm.hpp"

/*
TODO:

? What inside .pm
* Reload cache of keys
? Parsing arguments, multiple passwords and etc
* git track?
* if else -> try catch
* Write usage

*/

void usage() { std::cout << "[init |add <name> |remove <name> |show <name> ]" << std::endl; }

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage();
    return 1;
  }
  std::string fingerprint;

  for (int i = 1; i < argc; i++) {
    std::string command = argv[i];

    if (command == "-h" || command == "--help") {
      usage();
      return 0;

    } else if (command == "init") {
      Init();
      return 0;

    } else if (command == "-k" && argc > i + 1) {
      fingerprint = argv[i + 1];
      i++;
      continue;
    } else if (!isInit()) {
      std::cerr << "Error: not a vault directory: .pm" << std::endl;
      return 1;

    } else if (command == "remove" && argc > i + 1) {
      RemovePassword(argv[i + 1]);
      return 0;

    } else if (command == "add" && argc > i + 1) {
      AddPassword(argv[i + 1], GetKeys(fingerprint));
      return 0;

    } else if (command == "show" && argc > i + 1) {
      ShowPassword(argv[i + 1]);
      return 0;

    } else {
      std::cerr << "Invalid command: " << command << std::endl;
      return 1;
    }
  }
  return 0;
}