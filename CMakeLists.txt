project(gallery-app)
cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Qml)
find_package(Qt5Quick)
find_package(Qt5Gui)
find_package(OpenGL)
find_package(Exiv2)

include(FindPkgConfig)

# Standard install paths
include(GNUInstallDirs)

set(GALLERY_DIR ${CMAKE_INSTALL_DATADIR}/gallery-app)

# Tests
enable_testing()

add_subdirectory(desktop)
add_subdirectory(rc)
add_subdirectory(src)
add_subdirectory(tests)
