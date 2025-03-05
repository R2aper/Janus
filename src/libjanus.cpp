#include <filesystem>
#include <fstream>
#include <gpgme++/context.h>
#include <gpgme++/data.h>
#include <gpgme++/decryptionresult.h>
#include <gpgme++/encryptionresult.h>
#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>
#include <iostream>
#include <string>
#include <vector>
#ifdef _WIN32 // For input
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include "libjanus.hpp"

namespace fs = std::filesystem;

namespace janus {

static bool gpgme_initialized = false;

void InitGpgME() {
  if (!gpgme_initialized) {

    GpgME::initializeLibrary(); // Can fails or not?
    gpgme_initialized = true;
  }
}

GpgME::Context *CreateGpgContext(GpgME::Protocol protocol) {
  auto ctx = GpgME::Context::createForProtocol(protocol);
  if (!ctx)
    throw GpgME::Error(); // FIX:

  return ctx;
}

std::vector<GpgME::Key> GetKeys(const std::string &key_id) { //!! Rewrite
  auto ctx = CreateGpgContext();
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

std::vector<char> Input() noexcept {
  std::vector<char> input;
  char ch;
#ifdef _WIN32
  while ((ch = _getch()) != '\r') {
    if (ch == '\b') { // '\b' - Backspace
      if (!input.empty()) {
        input.pop_back();
        std::cout << "\b \b";
      }
    } else {
      input.push_back(ch);
      std::cout << '*';
    }
  }
  std::cout << std::endl;
  input.push_back('\0');
#else
  termios oldTerm, newTerm;
  std::cout.flush();

  tcgetattr(STDIN_FILENO, &oldTerm); // get current settings
  newTerm = oldTerm;
  newTerm.c_lflag &= ~(ICANON | ECHO);        // Unset CANON and ECHO
  tcsetattr(STDIN_FILENO, TCSANOW, &newTerm); // set new settings

  while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n' && ch != '\r') {
    if (ch == 127 || ch == '\b') { // 127 or \b - Backspace
      if (!input.empty()) {
        input.pop_back();
        std::cout << "\b \b" << std::flush;
      }
    } else {
      input.push_back(ch);
      std::cout << '*' << std::flush;
    }
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm); // Restore old settings
  input.push_back('\0');
#endif
  return input;
}

void SecureClear(std::vector<char> &data) noexcept {
  std::fill(data.begin(), data.end(), '\0');
  data.clear();
}

void AddPassword(const std::string &name, const std::vector<GpgME::Key> &keys) {
  auto ctx = CreateGpgContext();
  fs::path file_path = name + ".gpg";

  // Get content from user
  std::cout << "Enter password content:" << std::endl;
  std::vector<char> content = Input();

  GpgME::Data plainData(content.data(), content.size()), cipherData;
  SecureClear(content);
  GpgME::EncryptionResult result =
      ctx->encrypt(keys, plainData, cipherData, GpgME::Context::EncryptionFlags::AlwaysTrust);

  if (result.error())
    throw result.error();

  std::ofstream file(file_path, std::ios::binary); //!

  if (!file.is_open())
    throw std::runtime_error("Error opening file!"); // FIX:

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

  auto ctx = CreateGpgContext();

  GpgME::Data cipherData(cipherText.c_str(), cipherText.size());
  GpgME::Data plainData;
  GpgME::DecryptionResult result = ctx->decrypt(cipherData, plainData);
  if (result.error()) //!!
    throw std::runtime_error(result.error().asString());

  std::cout << plainData.toString() << std::endl; // ?
}

} // namespace janus
