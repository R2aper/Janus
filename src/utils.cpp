#include <fstream>
#include <iostream>
#include <vector>
#ifdef _WIN32 // For input
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include "utils.hpp"

namespace Janus {

std::vector<char> Input() noexcept {
  std::vector<char> input;
  char ch;
#ifdef _WIN32
  while (true) {
    ch = _getch();

    // if user press enter twice
    if (ch == '\r' && input.size() > 1 && input.back() == '\n')
      break;

    if (ch == '\r') {
      input.push_back('\n');
      std::cout << std::endl;
      continue;
    }
    if (ch == '\b') {
      if (!input.empty()) {
        input.pop_back();
        std::cout << "\b \b";
      }
    } else {
      input.push_back(ch);
      std::cout << '*';
    }
  }
#else
  termios oldTerm, newTerm;
  std::cout.flush();

  tcgetattr(STDIN_FILENO, &oldTerm); // get current settings
  newTerm = oldTerm;
  newTerm.c_lflag &= ~(ICANON | ECHO);        // Unset CANON and ECHO
  tcsetattr(STDIN_FILENO, TCSANOW, &newTerm); // set new settings

  while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\r') {

    // if user press enter twice
    if (input.size() > 1 && input.back() == '\n' && ch == '\n')
      break;

    if (ch == 127 || ch == '\b') { // Backspace
      if (!input.empty()) {
        input.pop_back();
        std::cout << "\b \b" << std::flush;
      }
    } else {
      if (ch == '\n') {
        input.push_back(ch);
        std::cout << std::endl;
        continue;
      }
      input.push_back(ch);
      std::cout << '*' << std::flush;
    }
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm); // Restore old settings
#endif
  input.push_back('\0');
  return input;
}

void SecureClear(std::vector<char> &data) noexcept {
  std::fill(data.begin(), data.end(), '\0');
  data.clear();
}

void WriteToFile(const std::string &file_path, const std::string &content) {
  std::ofstream file;

  file.exceptions(std::ifstream::failbit |
                  std::ifstream::badbit); // Let std::ofstream throw exceptions

  file.open(file_path, std::ios::binary);
  file.write(content.c_str(), content.size());

  file.close();
}

std::vector<char> ReadFromFile(const std::string &file_path) {
  std::ifstream file;

  file.exceptions(std::ifstream::failbit |
                  std::ifstream::badbit); // Let std::ifstream throw exceptions

  file.open(file_path, std::ios::binary);
  std::vector<char> a((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  file.close();
  return a;
}

} // namespace Janus
