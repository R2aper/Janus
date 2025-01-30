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

bool isInit() { return fs::exists(".janus"); }

GpgME::Context *GpgInit() { //! Multiple inizialization?

  GpgME::initializeLibrary();
  auto ctx = GpgME::Context::createForProtocol(GpgME::OpenPGP);
  if (!ctx) {
    std::cerr << "Error creating GPG context!" << std::endl;
    exit(1);
  }

  ctx->setPinentryMode(GpgME::Context::PinentryMode::PinentryAsk);

  return ctx;
}

std::vector<GpgME::Key> GetKeys(const std::string &key_id) { //!!
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

void RemovePassword(const std::string &name) {
  fs::path file_path = name + ".gpg";

  if (!fs::exists(file_path)) {
    std::cerr << "Password not found!" << std::endl;
    exit(1);
  }

  fs::remove(file_path);
  std::cout << "Password removed successfully!" << std::endl;
}

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
