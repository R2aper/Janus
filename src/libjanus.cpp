#include <iostream>
#include <vector>
#ifdef _WIN32 // For input
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include "libjanus.hpp"

namespace janus {

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

} // namespace janus
