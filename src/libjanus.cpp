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
// Check that library was not initialized before
bool gpgme_initialized = false;

/// Initialize gpgme library and create GpgME::Context
/// @return GpgME::Context pointer
GpgME::Context *GpgInit() {
  if (!janus::gpgme_initialized) {
    GpgME::initializeLibrary();
    janus::gpgme_initialized = true;
  }

  auto ctx = GpgME::Context::createForProtocol(GpgME::OpenPGP);
  if (!ctx) {
    std::cerr << "Error creating GPG context!" << std::endl;
    exit(1);
  }

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
 *         does not correspond to a valid encryption key.
 */
std::vector<GpgME::Key> GetKeys(const std::string &key_id) { //!! Rewrite
  auto ctx = GpgInit();

  if (key_id.empty()) {
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

    if (keys.empty()) {
      std::cerr << "No suitable encryption keys found!" << std::endl;
      exit(1);
    }
    return keys;
  } else {
    GpgME::Error err;
    GpgME::Key key = ctx->key(key_id.c_str(), err);

    if (key.isNull()) {
      std::cerr << "Key not found!" << std::endl;
      exit(1);
    }

    if (!key.canEncrypt()) {
      std::cerr << "Key can't encrypt!" << std::endl;
      exit(1);
    }

    return {key};
  }
}

/**
 * Initializes a new vault in the current working directory.
 *
 * @throw If the directory ".janus" already exists in the current working
 * directory or if the creation of the directory fails, an error message is printed to the standard
 error stream and the program exits with a non-zero status code.
 *
 */
void Init() {
  if (fs::exists(".janus")) {
    std::cerr << "Error: .janus already exist!" << std::endl;
    exit(1);
  }

  fs::create_directory(".janus");

  if (!fs::exists(".janus")) {
    std::cerr << "Error creating .janus dir!" << std::endl;
    exit(1);
  }

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
 * @throw If the file "<name>.gpg" does not exist, an error message is printed to the
 * standard error stream and the program exits with a non-zero status code.
 */
void RemovePassword(const std::string &name) {
  fs::path file_path = name + ".gpg";

  if (!fs::exists(file_path)) {
    std::cerr << "Password not found!" << std::endl;
    exit(1);
  }

  fs::remove(file_path);
  std::cout << "Password removed successfully!" << std::endl;
}

/**
 * Adds a password to the vault.
 *
 * @param name The name of the password to add.
 * @param key The keys to encrypt the password with.
 * @throw If the file "<name>.gpg" already exists, an error message is printed to the
 * standard error stream and the program exits with a non-zero status code.
 * @throw If encryption fails, an error message is printed to the standard error stream and the
 * program exits with a non-zero status code.
 */
void AddPassword(const std::string &name, const std::vector<GpgME::Key> &keys) {
  std::string content;
  std::vector<std::string> input;
  fs::path file_path = name + ".gpg";

  if (fs::exists(file_path)) {
    std::cerr << "Password already exists!" << std::endl;
    exit(1);
  }

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
  if (result.error()) {
    std::cerr << "Encryption failed: " << result.error().asString() << std::endl;
    exit(1);
  }

  std::ofstream file(file_path, std::ios::binary);

  std::string cipherText = cipherData.toString();
  file.write(cipherText.c_str(), cipherText.size()); //!!
  file.close();
}

/**
 * Shows a password from the vault.
 *
 * @param name The name of the password to show.
 * @throw If the file "<name>.gpg" does not exist, an error message is printed to the
 * standard error stream and the program exits with a non-zero status code.
 * @throw If decryption fails, an error message is printed to the standard error stream and the
 * program exits with a non-zero status code.
 */
void ShowPassword(const std::string &name) {
  fs::path file_path = name + ".gpg";

  if (!fs::exists(file_path)) {
    std::cerr << "Password not found!" << std::endl;
    exit(1);
  }

  std::ifstream file(file_path, std::ios::binary);
  std::string cipherText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  file.close();

  auto ctx = GpgInit();

  GpgME::Data cipherData(cipherText.c_str(), cipherText.size());
  GpgME::Data plainData;
  GpgME::DecryptionResult result = ctx->decrypt(cipherData, plainData);
  if (result.error()) {
    std::cerr << "Decryption failed: " << result.error().asString() << std::endl;
    exit(1);
  }

  std::cout << plainData.toString();
}

} // namespace janus
