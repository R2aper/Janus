#pragma once

#include <gpgme++/context.h>
#include <string>
#include <vector>

namespace Janus {

#define JANUS_VERSION "1.3"

/* Get user input until the empty line and hide it by '*'
 *
 * @return Vector of user input
 */
std::vector<char> Input() noexcept;

// Fill a vector with null characters
void SecureClear(std::vector<char> &data) noexcept;

/*
 *  Open and write string to file
 *
 *  @param file_path Path to file
 *  @param content Data to write
 *  @throw std::ios_base::failure if writing to file fails
 */
void WriteToFile(const std::string &file_path, const std::string &content);

/*
 * Open and read content of file
 *
 *  @param file_path Path to file
 *  @throw std::ios_base::failure if reading from file fails
 */
std::vector<char> ReadFromFile(const std::string &file_path);

} // namespace Janus
