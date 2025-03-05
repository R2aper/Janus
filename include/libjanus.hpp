#ifndef LIBJANUS_HPP
#define LIBJANUS_HPP

#include <global.h>
#include <gpgme++/context.h>

namespace janus {

/*
 * Initialize GpgME library
 */
void InitGpgME();

/*
 *Create GpgME::Context with the provided protocol
 *
 * @param protocol Protocol to use(Default is OpenPGP)
 * @throw GpgME::Error if context is not created
 */
GpgME::Context *CreateGpgContext(GpgME::Protocol protocol = GpgME::Protocol::OpenPGP);

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
std::vector<GpgME::Key> GetKeys(const std::string &key_id);

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

/* Get user input and hide it
 * NOTE:: Add possibility to add multiple lines
 *
 * @return Vector of user input
 */
std::vector<char> Input() noexcept;

// Fill a vector with null characters
void SecureClear(std::vector<char> &data) noexcept;

/**
 * Adds a password to the vault.
 *
 * @param name The name of the password to add.
 * @param key The keys to encrypt the password with.
 * @throw If the file "<name>.gpg" already exists, program throw a filesystem error
 * @throw If the file is not open, program throw a runtime error
 * @throw If encryption fails, program throw a runtime error
 */
void AddPassword(const std::string &name, const std::vector<GpgME::Key> &keys);

/**
 * Shows a password from the vault.
 *
 * @param name The name of the password to show.
 * @throw If the file "<name>.gpg" does not exist, program throw a filesystem error.
 * @throw If the file is not open, program throw a runtime error.
 * @throw If decryption fails, program throw a runtime error.
 */
void ShowPassword(const std::string &name);

} // namespace janus

#endif
