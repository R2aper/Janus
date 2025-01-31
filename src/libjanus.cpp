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

void Init() {
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
