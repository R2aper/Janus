#include <gpgme++/context.h>
#include <gpgme++/data.h>
#include <gpgme++/decryptionresult.h>
#include <gpgme++/encryptionresult.h>
#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "GPG.hpp"

namespace Janus {

void InitGpgME() {
  if (!gpgme_initialized) {

    GpgME::initializeLibrary();
    gpgme_initialized = true;
  }
}

GpgME::Context *CreateGpgContext(GpgME::Protocol protocol) {
  auto ctx = GpgME::Context::createForProtocol(protocol);
  if (!ctx)
    throw std::runtime_error("GpgME error: Failed to create GpgME context!");

  return ctx;
}

std::vector<GpgME::Key> GetKeys(GpgME::Context *ctx, const std::vector<std::string> &key_id) {
  std::vector<GpgME::Key> keys;

  if (key_id.empty())
    keys = GetAllKeys(ctx);

  else {
    for (auto it : key_id)
      keys.push_back(GetKeyById(ctx, it));
  }

  return keys;
}

std::vector<GpgME::Key> GetAllKeys(GpgME::Context *ctx) {
  std::vector<GpgME::Key> keys;
  GpgME::Error err = ctx->startKeyListing("", false);

  while (!err) {
    GpgME::Key key = ctx->nextKey(err);
    if (key.isNull())
      break;
    if (key.canEncrypt())
      keys.push_back(key);
  }
  ctx->endKeyListing();

  return keys;
}

GpgME::Key GetKeyById(GpgME::Context *ctx, const std::string &key_id) {
  GpgME::Error err;
  GpgME::Key key = ctx->key(key_id.c_str(), err);

  if (key.isBad())
    throw std::invalid_argument("No key was found for " + key_id + "!");

  return key;
}

} // namespace Janus
