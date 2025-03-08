#pragma once

#include <string>

namespace janus {

/**
 * Initializes a new vault in the current working directory.
 *
 * @throw If the directory ".janus" already exists in the current working
 * directory or if the creation of the directory fails, program thrown a filesystem error.
 */
void Init();

// Check if .janus directory exists
bool isInit();

// Lists all files in the working directory with a ".gpg" extension and print filenames without
// extension.
void list();

/**
 * Removes a password from the vault.
 *
 * @param name The name of the password to remove.
 * @throw If the file "<name>.gpg" does not exist, program throw a filesystem error.
 */
void RemovePassword(const std::string &name);

/*
 * Adds a password to the vault.
 *
 * @param name The name of the password to add.
 * @param key_id The identifier of key. Should be used if you want to encrypt data with specific key
 * @throw std::invalid_argument if no keys are available
 * @throw std::ios_base::failure if writing to file fails
 * @throw std::runtime_error if encryption fails
 */
void AddPassword(const std::string &name, const std::string &key_id = "");

/*
 * Shows a password from the vault.
 *
 * @param name The name of the password to show.
 * @throw std::ios_base::failure if reading from file fails
 * @throw std::runtime_error if decryption fails
 */
void ShowPassword(const std::string &name);

} // namespace janus
