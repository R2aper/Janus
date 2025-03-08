#pragma once

#include <gpgme++/context.h>
#include <gpgme++/key.h>

namespace janus {

static bool gpgme_initialized = false;

/*
 * Initialize GpgME library
 */
void InitGpgME();

/*
 *Create GpgME::Context with the provided protocol
 *
 * @param protocol Protocol to use(Default is OpenPGP)
 * @throw std::runtime_error if context is not created
 */
GpgME::Context *CreateGpgContext(GpgME::Protocol protocol = GpgME::Protocol::OpenPGP);

/*
  *Retrieves a list of GpgME::Key objects based on the provided key_id.
  *
  * If key_id is empty, it returns all available keys that can be used for encryption.
  * If key_id is specified, it retrieves the key associated with the given key_id
  * @param ctx The GpgME::Context object to use for retrieving keys
  * @param key_id The identifier of key to retrieves(Default is empty)
  * @return A vector of GpgME::Key objects

 */
std::vector<GpgME::Key> GetKeys(GpgME::Context *ctx, const std::string &key_id = "");

/*
 * Returns a list of all available keys that can be used for encryption
 *
 * @param ctx The GpgME::Context object to use for retrieving keys
 * @return All available and encryptable keys
 */
std::vector<GpgME::Key> GetAllKeys(GpgME::Context *ctx);

/* Return a key by it's fingerprint
 *
 * @param ctx The GpgME::Context object to use for retrieving key
 * @param key_id The key identifier
 * @return A vector of GpgME::Key objects with only one element
 */
std::vector<GpgME::Key> GetKeyById(GpgME::Context *ctx, const std::string &key_id);

} // namespace janus
