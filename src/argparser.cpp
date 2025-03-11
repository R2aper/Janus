#include <stdexcept>

#include "argparser.hpp"

namespace Janus {

void ArgParser::get_multiple_values(ArgInfo *arg_info, int &index, int argc, char *argv[]) {
  index++;

  while (index < argc && !args_map.count(argv[index])) {
    arg_info->values.push_back(argv[index]);
    index++;
  }

  --index;
}

ArgParser::ArgParser(std::map<std::string, std::shared_ptr<ArgInfo>> args_map)
    : args_map(args_map) {}

void ArgParser::AddArg(const std::string &short_name, const std::string &long_name,
                       ArgType type) noexcept {

  auto arg_info = std::make_shared<ArgInfo>();
  arg_info->type = type;

  if (!short_name.empty()) {
    arg_info->names.push_back(short_name);
    args_map[short_name] = arg_info;
  }
  if (!long_name.empty()) {
    arg_info->names.push_back(long_name);
    args_map[long_name] = arg_info;
  }
}

void ArgParser::Parse(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    auto it = args_map.find(arg);

    if (it != args_map.end()) {
      ArgInfo *arg_info = it->second.get();

      switch (arg_info->type) {
      case ArgType::FLAG:
        arg_info->is_set = true;
        break;
      case ArgType::MULTIPLEOPTION:
        if (!(i + 1 < argc))
          throw std::runtime_error("Error: missing value for " + arg);

        get_multiple_values(arg_info, i, argc, argv);
        arg_info->is_set = true;
        break;
      case ArgType::OPTION:
        if (!(i + 1 < argc))
          throw std::runtime_error("Error: missing value for " + arg);

        arg_info->values.push_back(argv[++i]);
        arg_info->is_set = true;
        break;
      default:
        throw std::runtime_error("Unsupported argument type" + arg);
      }

    } else
      throw std::runtime_error("Unknown argument " + arg);
  }
}

bool ArgParser::isSet(const std::string &name) const noexcept {
  auto it = args_map.find(name);
  if (it != args_map.end())
    return it->second->is_set;

  return false;
}

std::string ArgParser::getValue(const std::string &name) const noexcept {
  auto it = args_map.find(name);
  if (it != args_map.end() && it->second->type == ArgType::OPTION)
    return it->second->values.front();

  return {};
}

std::vector<std::string> ArgParser::getValues(const std::string &name) const noexcept {
  auto it = args_map.find(name);
  if (it != args_map.end() && it->second->type == ArgType::MULTIPLEOPTION)
    return it->second->values;

  return {};
}

std::map<std::string, std::shared_ptr<ArgInfo>> ArgParser::getMap() const { return args_map; }

} // namespace Janus
