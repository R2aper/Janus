cmake_minimum_required(VERSION 3.10)
project(janus VERSION 1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True

add_compile_options(
    -Werror

    -Wall
    -Wextra
    -Wpedantic

    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wctor-dtor-privacy
    -Wenum-compare
    -Wfloat-equal
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Woverloaded-virtual
    -Wredundant-decls
    -Wsign-conversion
    -Wsign-promo
)

if (WIN32)
    # Настройки для Windows
    add_definitions(-DWIN32)
elseif (UNIX)
    # Настройки для Unix-подобных систем
    add_definitions(-DUNIX)
endif())


include_directories(include)
aux_source_directory(src SOURCES)
aux_source_directory(src/git2cpp GIT2CPP_SOURCES)

find_package(Gpgmepp REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES} ${GIT2CPP_SOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGIT2 REQUIRED IMPORTED_TARGET libgit2)

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::LIBGIT2 Gpgmepp)
