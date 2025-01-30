#include <filesystem>
#include <fstream>
#include <gpgme++/context.h>
#include <gpgme++/data.h>
#include <gpgme++/decryptionresult.h>
#include <gpgme++/encryptionresult.h>
#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>
#include <gpgme.h>
#include <iostream>
#include <string>
#include <vector>

#include "libjanus.hpp"

namespace fs = std::filesystem;

namespace janus {

/// Initialize gpgme library and create GpgME::Context
/// @return GpgME::Context pointer
/// @throw Program throw runtime error if GpgME::Context creation fails
GpgME::Context *GpgInit() {
  // Check that library was not initialized before
  static bool gpgme_initialized = false;

  if (!gpgme_initialized) {
    GpgME::initializeLibrary();
    gpgme_initialized = true;
  }

  auto ctx = GpgME::Context::createForProtocol(GpgME::OpenPGP);
  if (!ctx)
    throw std::runtime_error("Error creating GPG context!");

  //? Set pinentry mode to ask for password(Doesn't work)
  ctx->setPinentryMode(GpgME::Context::PinentryMode::PinentryAsk);

  return ctx;
}

/**
 * Retrieves a list of GpgME::Key objects based on the provided key_id.
 *
 * If key_id is empty, it returns all available keys that can be used for encryption.
 * If key_id is specified, it retrieves the key associated with the given key_id and
 * ensures that the key can be used for encryption.
 *
 * @param key_id The identifier of the key to retrieve. If empty, all keys are listed.
 * @return A vector of GpgME::Key objects that can be used for encryption.
 * @throw If no suitable encryption keys are found or if the specified key_id
 *         does not correspond to a valid encryption key, program throw a runtime error.
 */
std::vector<GpgME::Key> GetKeys(const std::string &key_id) { //!! Rewrite
  auto ctx = GpgInit();
  std::vector<GpgME::Key> keys;
  GpgME::Error err;
  GpgME::Key key;

  if (key_id.empty()) {
    err = ctx->startKeyListing("", false);
    while (!err) {
      key = ctx->nextKey(err);
      if (key.isNull())
        break;
      if (key.canEncrypt())
        keys.push_back(key);
    }
    ctx->endKeyListing();

    if (keys.empty())
      throw std::runtime_error("No suitable encryption keys found!");

  } else {
    key = ctx->key(key_id.c_str(), err);

    if (key.isNull() || !key.canEncrypt())
      throw std::runtime_error(
          "Specified fingerprint does not correspond to a valid encryption key!");

    keys.push_back(key);
  }

  return keys;
}

/**
 * Initializes a new vault in the current working directory.
 *
 * @throw If the directory ".janus" already exists in the current working
 * directory or if the creation of the directory fails, program thrown a filesystem error.
 */
void Init() {
  if (fs::exists(".janus"))
    throw fs::filesystem_error(".janus", std::error_code(EEXIST, std::system_category()));

  fs::create_directory(".janus");

  if (!fs::exists(".janus"))
    throw fs::filesystem_error(".janus", std::error_code(EAGAIN, std::system_category())); //?

  std::cout << "Initialized Vault in " << fs::absolute(".janus") << std::endl;
}

// Check if .janus directory exists
bool isInit() { return fs::exists(".janus"); }

// Lists all files in the working directory with a ".gpg" extension and print filenames without
// extension.
void list() {
  fs::path dir = fs::current_path();
  for (const auto &entry : fs::directory_iterator(dir)) {
    if (entry.path().extension() == ".gpg") {
      std::cout << entry.path().stem().string() << std::endl;
    }
  }
}

/**
 * Removes a password from the vault.
 *
 * @param name The name of the password to remove.
 * @throw If the file "<name>.gpg" does not exist, program throw a filesystem error.
 */
void RemovePassword(const std::string &name) {
  fs::path file_path = name + ".gpg";

  if (!fs::exists(file_path))
    throw fs::filesystem_error(file_path, std::error_code(ENOENT, std::system_category()));

  fs::remove(file_path);
}

/**
 * Adds a password to the vault.
 *
 * @param name The name of the password to add.
 * @param key The keys to encrypt the password with.
 * @throw If the file "<name>.gpg" already exists, program throw a filesystem error
 * @throw If the file is not open, program throw a runtime error
 * @throw If encryption fails, program throw a runtime error
 */
void AddPassword(const std::string &name, const std::vector<GpgME::Key> &keys) {
  std::string content;
  std::vector<std::string> input;
  fs::path file_path = name + ".gpg";

  if (fs::exists(file_path))
    throw fs::filesystem_error(file_path, std::error_code(EEXIST, std::system_category()));

  std::cout << "Enter password content:" << std::endl;

  while (std::getline(std::cin, content)) {
    if (content.empty()) {
      break;
    }
    input.push_back(content);
  }
  content.clear();
  for (std::string &it : input) {
    content += it + "\n";
  }

  auto ctx = GpgInit();

  GpgME::Data plainData(content.c_str(), content.size());
  GpgME::Data cipherData;
  GpgME::EncryptionResult result =
      ctx->encrypt(keys, plainData, cipherData, GpgME::Context::EncryptionFlags::AlwaysTrust);

  if (result.error())
    throw std::runtime_error(result.error().asString());

  std::ofstream file(file_path, std::ios::binary); //!

  if (!file.is_open())
    throw std::runtime_error("Error opening file!"); //!!

  std::string cipherText = cipherData.toString();
  file.write(cipherText.c_str(), cipherText.size()); //!!
  file.close();
}

/**
 * Shows a password from the vault.
 *
 * @param name The name of the password to show.
 * @throw If the file "<name>.gpg" does not exist, program throw a filesystem error.
 * @throw If the file is not open, program throw a runtime error.
 * @throw If decryption fails, program throw a runtime error.
 */
void ShowPassword(const std::string &name) {
  fs::path file_path = name + ".gpg";

  if (!fs::exists(file_path))
    throw fs::filesystem_error(file_path, std::error_code(ENOENT, std::system_category()));

  std::ifstream file(file_path, std::ios::binary); //!

  if (!file.is_open()) //!!
    throw std::runtime_error("Error opening file!");

  std::string cipherText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  file.close();

  auto ctx = GpgInit();

  GpgME::Data cipherData(cipherText.c_str(), cipherText.size());
  GpgME::Data plainData;
  GpgME::DecryptionResult result = ctx->decrypt(cipherData, plainData);
  if (result.error()) //!!
    throw std::runtime_error(result.error().asString());

  std::cout << plainData.toString();
}

} // namespace janus
