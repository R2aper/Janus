#ifndef LIBJANUS_HPP
#define LIBJANUS_HPP

#include <gpgme++/context.h>

namespace janus {

GpgME::Context *GpgInit();
std::vector<GpgME::Key> GetKeys(const std::string &key_id);

bool isInit();
void Init();
void list();
void RemovePassword(const std::string &name);
void AddPassword(const std::string &name, const std::vector<GpgME::Key> &keys);
void ShowPassword(const std::string &name);

} // namespace janus

#endif