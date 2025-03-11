#pragma once

#include <map>
#include <memory>
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

class ArgParser {
private:
  std::map<std::string, std::shared_ptr<ArgInfo>> args_map;

  /*
   * Get all values after MULTIPLEOPTION and write them to values
   *
   * @param arg_info Argument info of MULTIPLEOPTION
   * @param argc Number of arguments
   * @param argv Array of arguments
   */
  void get_multiple_values(ArgInfo *arg_info, int &index, int argc, char *argv[]);

public:
  // Constructors
  ArgParser() = default;
  ArgParser(std::map<std::string, std::shared_ptr<ArgInfo>> args_map);

  /*
   * Add argument to args_map
   *
   * @param short_name Short name of argument
   * @param long_name Long name of argument
   * @param type Type of argument
   */
  void AddArg(const std::string &short_name, const std::string &long_name, ArgType type) noexcept;

  /*
   * Parse command line arguments
   *
   * @param argc Number of arguments
   * @param argv Array of arguments
   * @throw std::invalid_argument If argument is not found in args_map
   * @throw std::invalid_argument If argument has invalid type
   * @throw std::runtime_error If MULTIPLEOPTION or OPTION argument has no values
   */
  void Parse(int argc, char *argv[]);

  /*
   * Check if argument is set
   *
   * @param name Argument name
   * @return true if argument is set
   */
  bool isSet(const std::string &name) const noexcept;

  /*
   * Get value passed to argument(For OPTION)
   *
   * @param name Argument name
   * @return Value passed to argument
   */
  std::string getValue(const std::string &name) const noexcept;

  /*
   * Get values passed to argument
   *
   * @param name Argument name
   * @return Values passed to argument
   */
  std::vector<std::string> getValues(const std::string &name) const noexcept;

  // return args_map
  std::map<std::string, std::shared_ptr<ArgInfo>> getMap() const;
};
} // namespace Janus
