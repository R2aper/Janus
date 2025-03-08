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
  while ((ch = _getch()) != '\r') {
    if (ch == '\b') { // '\b' - Backspace
      if (!input.empty()) {
        input.pop_back();
        std::cout << "\b \b";
      }
    } else {
      input.push_back(ch);
      std::cout << '*';
    }
  }
  std::cout << std::endl;
  input.push_back('\0');
#else
  termios oldTerm, newTerm;
  std::cout.flush();

  tcgetattr(STDIN_FILENO, &oldTerm); // get current settings
  newTerm = oldTerm;
  newTerm.c_lflag &= ~(ICANON | ECHO);        // Unset CANON and ECHO
  tcsetattr(STDIN_FILENO, TCSANOW, &newTerm); // set new settings

  while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n' && ch != '\r') {
    if (ch == 127 || ch == '\b') { // 127 or \b - Backspace
      if (!input.empty()) {
        input.pop_back();
        std::cout << "\b \b" << std::flush;
      }
    } else {
      input.push_back(ch);
      std::cout << '*' << std::flush;
    }
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm); // Restore old settings
  input.push_back('\0');
#endif
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
