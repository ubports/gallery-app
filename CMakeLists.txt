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
#pkg_check_modules(CONTENTHUB REQUIRED libcontent-hub)
pkg_check_modules(EXIV2 REQUIRED exiv2)
pkg_check_modules(GSTLIB REQUIRED gstreamer-0.10)
pkg_check_modules(MEDIAINFO REQUIRED libmediainfo)

set(CONTENTHUB_INCLUDE_DIRS "/usr/include/com/ununtu/content")
set(CONTENTHUB_LIBRARIES "content-hub")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Standard install paths
include(GNUInstallDirs)

set(GALLERY_DIR ${CMAKE_INSTALL_DATADIR}/gallery-app)

# for dh_translations to extract the domain
# (regarding syntax consistency, see http://pad.lv/1181187)
set (GETTEXT_PACKAGE "gallery-app")

file(GLOB_RECURSE I18N_SRC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     rc/**.qml desktop/**.desktop.in)
list(SORT I18N_SRC_FILES)

# Tests
enable_testing()

add_subdirectory(desktop)
add_subdirectory(po)
add_subdirectory(rc)
add_subdirectory(src)
add_subdirectory(tests)
