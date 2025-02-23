#include <git2.h>

#include "git2cpp/abstract.hpp"
#include "git2cpp/error.hpp"
#include "git2cpp/signature.hpp"

namespace Git {

Signature::Signature() : AbstractClass(nullptr) {}
Signature::Signature(git_signature *sig) : AbstractClass(sig) {}

Signature::Signature(const std::string &name, const std::string &email) : AbstractClass(nullptr) {
  if (git_signature_now(&_handle, name.c_str(), email.c_str()) != 0)
    throw Git::Exception();
}

Signature::Signature(const std::string &name, const std::string &email, git_time_t time, int offset)
    : AbstractClass(nullptr) {
  if (git_signature_new(&_handle, name.c_str(), email.c_str(), time, offset) != 0)
    throw Git::Exception();
}

void Signature::CreateDefault(const Repository &repo) {
  release();

  if (git_signature_default(&_handle, repo.ptr()) != 0)
    throw Exception();
}

void Signature::SetSig(git_signature *sig) {
  release();

  _handle = sig;
}

} // namespace Git
