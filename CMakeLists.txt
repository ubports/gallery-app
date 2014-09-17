project(gallery-app)
cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower)

include(EnableCoverageReport)
#####################################################################
# Enable code coverage calculation with gcov/gcovr/lcov
# Usage:
#  * Switch build type to coverage (use ccmake or cmake-gui)
#  * Invoke make, make test, make coverage (or ninja if you use that backend)
#  * Find html report in subdir coveragereport
#  * Find xml report feasible for jenkins in coverage.xml
#####################################################################
if(cmake_build_type_lower MATCHES coverage)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage" )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage" )
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --coverage" )
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage" )
    ENABLE_COVERAGE_REPORT(TARGETS ${SHELL_APP})
endif()

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Qml)
find_package(Qt5Quick)
find_package(Qt5Sql)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    find_package(GLESv2)
else()
    find_package(OpenGL)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(CONTENTHUB REQUIRED libcontent-hub)
pkg_check_modules(EXIV2 REQUIRED exiv2)
pkg_check_modules(MEDIAINFO REQUIRED libmediainfo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

find_program(INTLTOOL_MERGE intltool-merge)
find_program(INTLTOOL_EXTRACT intltool-extract)

# Standard install paths
include(GNUInstallDirs)

option(INSTALL_TESTS "Install the tests on make install" off)
option(CLICK_MODE "Installs to a contained location" on)

set(GALLERY gallery-app)
set(DESKTOP_FILE ${GALLERY}.desktop)
set(AUTOPILOT_DIR gallery_app)
set(UBUNTU_MANIFEST_PATH "click/manifest.json.in" CACHE INTERNAL "Relative path to the manifest file")

# make non compiled files visible in qtcreator
file(GLOB_RECURSE NON_COMPILED_FILES *.qml *.js *.py *.svg *.png *.in *.json *.desktop *.pot *.apparmor qmldir)
add_custom_target(NON_COMPILED_TARGET ALL SOURCES ${NON_COMPILED_FILES})

if(CLICK_MODE)
    if(NOT DEFINED BZR_SOURCE)
        set(BZR_SOURCE "lp:gallery-app")
    endif(NOT DEFINED BZR_SOURCE)
    set(APP_PACKAGE_NAME "com.ubuntu.gallery")
    set(CMAKE_INSTALL_PREFIX /)
    set(CMAKE_INSTALL_BINDIR /)
    set(GALLERY_DIR "")
    add_definitions("-DCLICK_MODE=1")
else(CLICK_MODE)
    set(GALLERY_DIR ${CMAKE_INSTALL_DATADIR}/${GALLERY})
endif(CLICK_MODE)

# for dh_translations to extract the domain
# (regarding syntax consistency, see http://pad.lv/1181187)
set (GETTEXT_PACKAGE "${GALLERY}")

# Tests
enable_testing()

add_subdirectory(click)
add_subdirectory(desktop)
add_subdirectory(po)
add_subdirectory(rc)
add_subdirectory(src)
add_subdirectory(tests)
