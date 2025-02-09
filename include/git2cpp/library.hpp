#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <git2.h>
#include <iostream>
#include <string>

namespace Git {

// Class to initialize and shutdown the libgit2 library
class Git2Library {
public:
  /// Initializes the libgit2 library.
  /// @throw Throw astd::runtime_error if the initialization of libgit2 fails.
  Git2Library();

  /// Shuts down the libgit2 library.
  ~Git2Library();
};

} // namespace Git

#endif // LIBRARY_HPP
