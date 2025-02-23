#pragma once
#ifndef SIGNATURE_HPP
#define SIGNATURE_HPP

#include <git2.h>

#include "git2cpp/abstract.hpp"
#include "git2cpp/repository.hpp"

// TODO: Signauture  from buffer
// int git_signature_from_buffer(git_signature **out, const char *buf);

// TODO: Signature from env
// int git_signature_default_from_env(git_signature **author_out, git_signature **committer_out,
// git_repository *repo);

namespace Git {

class Repository;

// Wrapper for git_signature
class Signature : public AbstractClass<Signature, git_signature, git_signature_free> {
public:
  /* Basic constructor.
   *
   * Assign signature to nullptr
   */
  explicit Signature();

  /* Constructor
   *
   * Create a new action signature with a timestamp of 'now'.
   *
   * @param name Name of the person
   * @param email Email of the person
   * @throw Git::Exception if creating signature fails
   */
  explicit Signature(const std::string &name, const std::string &email);

  /* Constructor
   *
   * Create a new signature from given name, email and timestamp
   *
   * @param name Name of the person
   * @param email Email of the person
   * @param time Time (in seconds from epoch) when the action happened
   * @param offset Timezone offset (in minutes) for the time
   * @throw Git::Exception if creating signature fails
   */
  explicit Signature(const std::string &name, const std::string &email, git_time_t time,
                     int offset);

  /* Constructor
   *
   * @param sig Existing signature
   */
  explicit Signature(git_signature *sig);

  /* Create signature by taking default signature from git repository
   *
   * Existing signature will be freed
   *
   * @param repo Existing git repository
   * @throw Git::Exception if taking default signature fails
   */
  void CreateDefault(const Repository &repo);

  /* Set git_signature manually
   *
   * Existing signature will be freed
   */
  void SetSig(git_signature *sig);
};

} // namespace Git

#endif // SIGNATURE_HPP
