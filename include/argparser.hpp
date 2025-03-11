#pragma once

#include <string>
#include <vector>

namespace Janus {

enum class ArgType {
  FLAG,           // Bool argument
  MULTIPLEOPTION, // Argument with multiple values
  OPTION,         // Argument with single value
};

class ArgInfo {
public:
  ArgType type;
  std::vector<std::string> values; // Values passed to the option argument
  bool is_set = false;
  std::vector<std::string> names;
};

class ArgParser {};

} // namespace Janus
