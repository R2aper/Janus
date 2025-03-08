#pragma once
#ifndef LIBJANUS_HPP
#define LIBJANUS_HPP

#include <gpgme++/context.h>
#include <string>
#include <vector>

namespace janus {

/* Get user input and hide it
 * NOTE:: Add possibility to add multiple lines
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

} // namespace janus

#endif
