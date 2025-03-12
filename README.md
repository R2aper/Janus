# Janus

Janus is a simple password manager based on GnuPG. This project is designed to provide a secure and simple way to manage your passwords.

## Why?
It's my first real project on C++, so for the most part is learning, but i also want to just make storing password easier

## Features

- **Secure Storage**: Passwords are encrypted using GnuPG.
- **Simple Interface**: Easy to use and navigate.
- **Lightweight**: Minimal dependencies and low resource usage.

## Installation

### Prerequisites

Make sure you have the following installed:

- gpg
- gpgme(C++ bindings)
- libgit2
- CMake
- A C++ compiler (e.g., GCC)

### Installing

```bash
git clone https://github.com/R2aper/Janus.git
cd Janus
mkdir build
cd build
cmake ..
make
```

## Usage

 ```bash
 janus [Options] <command> 
```


### Options: 
-  `-h, --help`             Show help message 
-  `-v, --version`          Show version of program and used libraries
-  `-q, --quiet`            Tell program to not commit changes
-  `-k <fingerprints>`      Pass fingerprints of the keys to encrypt the password with(By default passwords encrypted with all available keys)

### Commands: 
-  `init`                   Initialize a new git repository in the current working directory(Similar to git init)
-  `list`                   List all passwords in the vault
-  `add <name>`             Add a new password to the vault
-  `remove <name>`          Remove a password from the vault
-  `show <name>`            Show a password from the vault




### Example

```bash
janus -k Myemail@gmail.com add Google  Add password with name "Google" that encrypt with key that have fingerprint "Myemail@gmail.com"

janus -q remove Github         Remove password with name and not commit changes
```

## License

This project is licensed under the MIT License. See the [LICENSE](https://github.com/R2aper/Janus/blob/main/LICENSE.md) file for details.
