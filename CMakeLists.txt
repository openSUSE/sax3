cmake_minimum_required(VERSION 3.10)
project(SaX3)

find_package(PkgConfig)
pkg_check_modules(AUGEAS augeas)
pkg_check_modules(LIBYUI libyui)
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
add_subdirectory(src)
