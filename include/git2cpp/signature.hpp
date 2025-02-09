#pragma once
#ifndef SIGNATURE_HPP
#define SIGNATURE_HPP

#include <git2.h>

#include "git2cpp/repository.hpp"

namespace Git {

class Signature {
private:
  git_signature *_sig;

public:
  /* Basic constructor.
   *
   * Assign signature to nullptr
   */
  Signature();

  /* Constructor
   *
   * @param sig Existing signature
   */
  Signature(git_signature *sig);

  /* Destructor
   *
   * Free signature if it is not nullptr
   */
  ~Signature();

  /* Create signature by taking default signature from git repository
   *
   * @param repo Existing git repository
   * @throw Git::Exception if taking default signature fails
   */
  void Create(Repository repo);

  /* Set git_signature manually
   *
   * Existing signature will be freed
   */
  void Set(git_signature *sig);

  const git_signature *ptr() const;

  Signature(const Signature &) = delete;
  Signature &operator=(const Signature &) = delete;

  Signature(Signature &&other) noexcept;
  Signature &operator=(Signature &&other) noexcept;
};

} // namespace Git

#endif // SIGNATURE_HPP
