#include <iostream>
#include <string>

#include "libpm.hpp"

/*
TODO:

* Init arg .pm dir, what inside?
* Reload cache of keys
* Chose Keys?
* Maybe different interfaice
* git track?

*/

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " [add|remove|show]\n";
    return 1;
  }

  std::string command = argv[1];
  if (command == "add") {
    addPassword();
  } else if (command == "remove") {
    removePassword();
  } else if (command == "show") {
    showPassword();
  } else {
    std::cerr << "Invalid command!" << std::endl;
    return 1;
  }
  return 0;
}