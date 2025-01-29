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

#include "libpm.hpp"

namespace fs = std::filesystem;

void addPassword() {
  std::string name, content;
  std::cout << "Enter name:" << std::endl;
  std::getline(std::cin, name);
  fs::path file_path = name + ".gpg";

  if (fs::exists(file_path)) {
    std::cerr << "Entry already exists!" << std::endl;
    return;
  }

  std::cout << "Enter password content:" << std::endl;
  std::getline(std::cin, content);

  GpgME::initializeLibrary();
  auto ctx = GpgME::Context::createForProtocol(GpgME::OpenPGP);
  if (!ctx) {
    std::cerr << "Error creating GPG context" << std::endl;
    return;
  }

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
    std::cerr << "No suitable encryption keys found" << std::endl;
    return;
  }

  GpgME::Data plainData(content.c_str(), content.size());
  GpgME::Data cipherData;
  GpgME::EncryptionResult result =
      ctx->encrypt(keys, plainData, cipherData, GpgME::Context::EncryptionFlags::AlwaysTrust);
  if (result.error()) {
    std::cerr << "Encryption failed: " << result.error().asString() << std::endl;
    return;
  }

  std::ofstream file(file_path, std::ios::binary);

  std::string cipherText = cipherData.toString();
  file.write(cipherText.c_str(), cipherText.size()); //!!
  file.close();
}

void removePassword() {
  std::string name;
  std::cout << "Enter name to remove:" << std::endl;
  std::getline(std::cin, name);
  fs::path file_path = name + ".gpg";

  if (!fs::exists(file_path)) {
    std::cerr << "Entry not found!" << std::endl;
    return;
  }

  fs::remove(file_path);
  std::cout << "Entry removed successfully!" << std::endl;
}

void showPassword() {
  std::string name;
  std::cout << "Enter name to show:" << std::endl;
  std::getline(std::cin, name);
  fs::path file_path = name + ".gpg";

  if (!fs::exists(file_path)) {
    std::cerr << "Entry not found!" << std::endl;
    return;
  }

  std::ifstream file(file_path, std::ios::binary);
  std::string cipherText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  file.close();

  GpgME::initializeLibrary();
  auto ctx = GpgME::Context::createForProtocol(GpgME::OpenPGP);
  if (!ctx) {
    std::cerr << "Error creating GPG context" << std::endl;
    return;
  }

  // ctx->setPinentryMode(GpgME::Context::PinentryMode::PinentryLoopback); //!!

  GpgME::Data cipherData(cipherText.c_str(), cipherText.size());
  GpgME::Data plainData;
  GpgME::DecryptionResult result = ctx->decrypt(cipherData, plainData);
  if (result.error()) {
    std::cerr << "Decryption failed: " << result.error().asString() << std::endl;
    return;
  }

  std::cout << "Content: " << plainData.toString() << std::endl;
}
