#include <git2.h>

#include "git2cpp/signature.hpp"
#include "git2cpp/error.hpp"

namespace Git {

Signature::Signature() : _sig(nullptr) {}
Signature::Signature(git_signature *sig) : _sig(sig) {}

Signature::~Signature() {
  if (_sig)
    git_signature_free(_sig);
}

void Signature::Create(Repository repo) {
  if (_sig) {
    git_signature_free(_sig);
    _sig = nullptr;
  }

  if (git_signature_default(&_sig, repo.ptr()) != 0)
    throw Exception();
}

void Signature::Set(git_signature *sig) {
  if (_sig)
    git_signature_free(_sig);

  _sig = sig;
}

const git_signature *Signature::ptr() const { return _sig; }

Signature::Signature(Signature &&other) noexcept : _sig(other._sig) { other._sig = nullptr; }

Signature &Signature::operator=(Signature &&other) noexcept {
  if (this != &other) {
    if (_sig) {
      git_signature_free(_sig);
    }
    _sig = other._sig;
    other._sig = nullptr;
  }
  return *this;
}

} // namespace Git
