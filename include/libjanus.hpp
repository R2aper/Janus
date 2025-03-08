#pragma once
#ifndef LIBJANUS_HPP
#define LIBJANUS_HPP

#include <global.h>
#include <gpgme++/context.h>

namespace janus {

/* Get user input and hide it
 * NOTE:: Add possibility to add multiple lines
 *
 * @return Vector of user input
 */
std::vector<char> Input() noexcept;

// Fill a vector with null characters
void SecureClear(std::vector<char> &data) noexcept;

} // namespace janus

#endif
