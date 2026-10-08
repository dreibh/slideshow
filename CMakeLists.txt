CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(slideshow LANGUAGES C CXX)

SET(BUILD_MAJOR "2")
SET(BUILD_MINOR "3")
SET(BUILD_PATCH "7")
SET(BUILD_VERSION ${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH})


#############################################################################
#### CMAKE INCLUDES                                                      ####
#############################################################################

INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCXXSourceCompiles)
INCLUDE(CheckCSourceRuns)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckStructHasMember)
INCLUDE(FindPackageHandleStandardArgs)
INCLUDE(GNUInstallDirs)


#############################################################################
#### PACKAGING                                                           ####
#############################################################################

SET(CPACK_SOURCE_GENERATOR "TXZ")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH}")
SET(CPACK_SOURCE_IGNORE_FILES "\\\\.git;\\\\.swp$;~$;\\\\.\\\\#;/\\\\#")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "^${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}[_-]")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.make$|\\\\.log$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/CMakeCache\\\\.txt$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/(CMakeFiles|CMakeScripts|_CPack_Packages)/")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/package-version\\\\.h$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/packaging\\\\.conf$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "^${PROJECT_SOURCE_DIR}/(po.*/|src.*/|)Makefile$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "^${PROJECT_SOURCE_DIR}/(po.*|src.*|[^/]*)\\\\.cmake$")
INCLUDE(CPack)

ADD_CUSTOM_TARGET(dist COMMAND ${CMAKE_MAKE_PROGRAM} clean package_source)


#############################################################################
#### REQUIREMENTS                                                        ####
#############################################################################

# ====== GraphicsMagick =====================================================
FIND_LIBRARY(GraphicsMagick_LIBRARY NAMES GraphicsMagick)
FIND_PATH(GraphicsMagick_INCLUDE_DIR NAMES magick/api.h PATHS
   /opt/homebrew/include/GraphicsMagick
   /home/linuxbrew/.linuxbrew/include/GraphicsMagick
   /usr/pkg/include/GraphicsMagick
   /usr/local/include/GraphicsMagick
   /usr/local/include/graphicsmagick
   /usr/include/GraphicsMagick
   /usr/include/graphicsmagick
)
IF (GraphicsMagick_LIBRARY AND GraphicsMagick_INCLUDE_DIR)
   MESSAGE(STATUS "GraphicsMagick library found:")
   MESSAGE(STATUS " GraphicsMagick_LIBRARY:     ${GraphicsMagick_LIBRARY}")
   MESSAGE(STATUS " GraphicsMagick_INCLUDE_DIR: ${GraphicsMagick_INCLUDE_DIR}")
ELSE()
   MESSAGE(FATAL_ERROR
          "Cannot find GraphicsMagick library! Try:\n"
          " * Ubuntu/Debian: sudo apt install -y libgraphicsmagick1-dev\n"
          " * Fedora:        sudo dnf install -y GraphicsMagick-devel\n"
          " * SuSE:          sudo zypper install -y GraphicsMagick-devel\n"
          " * Alpine:        sudo apk add graphicsmagick-dev\n"
          " * FreeBSD:       sudo pkg install -y GraphicsMagick\n"
          " * NetBSD:        sudo pkgin install graphicsmagick\n"
          " * OpenBSD:       doas pkg_add graphicsmagick\n"
          " * Solaris:       sudo pkg install image/graphicsmagick\n"
          " * Apple:         brew install graphicsmagick")
ENDIF()


#############################################################################
#### SUBDIRECTORIES                                                      ####
#############################################################################

ADD_SUBDIRECTORY(src)
