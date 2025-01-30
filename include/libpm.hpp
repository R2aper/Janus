#ifndef LIBPM_HPP
#define LIBPM_HPP

#include <gpgme++/context.h>

GpgME::Context *GpgInit();
std::vector<GpgME::Key> GetKeys(const std::string &key_id);

bool isInit();
void Init();
void RemovePassword(const std::string &name);
void AddPassword(const std::string &name, const std::vector<GpgME::Key> &keys);
void ShowPassword(const std::string &name);

#endif