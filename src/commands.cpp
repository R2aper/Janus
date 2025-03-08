#include <filesystem>
#include <gpgme++/context.h>
#include <gpgme++/data.h>
#include <gpgme++/decryptionresult.h>
#include <gpgme++/encryptionresult.h>
#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>
#include <iostream>

#include "GPG.hpp"
#include "commands.hpp"
#include "libjanus.hpp"

namespace fs = std::filesystem;

namespace janus {

void Init() { // FIX: Should be repo
  if (fs::exists(".janus"))
    throw fs::filesystem_error(".janus", std::error_code(EEXIST, std::system_category()));

  fs::create_directory(".janus");

  if (!fs::exists(".janus"))
    throw fs::filesystem_error(".janus", std::error_code(EAGAIN, std::system_category())); //?

  std::cout << "Initialized Vault in " << fs::absolute(".janus") << std::endl;
}

bool isInit() { return fs::exists(".janus"); }

void list() {
  fs::path dir = fs::current_path();
  for (const auto &entry : fs::directory_iterator(dir)) {
    if (entry.path().extension() == ".gpg") {
      std::cout << entry.path().stem().string() << std::endl;
    }
  }
}

void RemovePassword(const std::string &name) {
  fs::path file_path = name + ".gpg";

  if (!fs::exists(file_path))
    throw fs::filesystem_error(file_path, std::error_code(ENOENT, std::system_category()));

  std::string input;
  std::cout << "Do you want to delete " << name << "?[y/n]: ";
  std::cin >> input;
  if (input == "y")
    fs::remove(file_path);
  else
    return;
}

void AddPassword(const std::string &name, const std::string &key_id) {
  auto ctx = CreateGpgContext();
  std::vector<GpgME::Key> keys = GetKeys(ctx, key_id);

  if (keys.empty())
    throw std::invalid_argument("No keys to encrypt with!");

  fs::path file_path = name + ".gpg";

  // Get content from user
  std::cout << "Enter password content:" << std::endl;
  std::vector<char> content = Input();

  // Encrypt content
  GpgME::Data plainData(content.data(), content.size()), cipherData;
  SecureClear(content);
  GpgME::EncryptionResult result =
      ctx->encrypt(keys, plainData, cipherData, GpgME::Context::EncryptionFlags::AlwaysTrust);

  if (result.error()) {
    throw std::runtime_error("GPG encryption error [" + std::to_string(result.error().code()) +
                             "] " + result.error().asString());
  }

  WriteToFile(file_path, cipherData.toString());
}

void ShowPassword(const std::string &name) {
  fs::path file_path = name + ".gpg";
  std::vector<char> cipherText = ReadFromFile(file_path);
  auto ctx = CreateGpgContext();

  GpgME::Data cipherData(cipherText.data(), cipherText.size()), plainData;
  SecureClear(cipherText);
  GpgME::DecryptionResult result = ctx->decrypt(cipherData, plainData);
  if (result.error()) {
    throw std::runtime_error("GPG decryption error [" + std::to_string(result.error().code()) +
                             "] " + result.error().asString());
  }

  std::cout << plainData.toString() << std::endl;
}

} // namespace janus
