#/**********************************************************\ 
# Auto-generated X11 project definition file for the
# WebEmber project
#\**********************************************************/

# X11 template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in X11/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    X11/*.cpp
    X11/*.h
    X11/*.cmake
    )


SOURCE_GROUP(X11 FILES ${PLATFORM})

find_package(PkgConfig)
pkg_check_modules(SDL_PKGCONF REQUIRED sdl)
pkg_check_modules(X11_PKGCONF REQUIRED x11)

# use this to add preprocessor definitions
add_definitions(
    -DPREFIX="${CMAKE_INSTALL_PREFIX}"
    -DUSE_X11
    ${SDL_PKGCONF_CFLAGS}
    ${X11_PKGCONF_CFLAGS}
)


set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

add_x11_plugin(${PROJECT_NAME} SOURCES)

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME}
    ${PLUGIN_INTERNAL_DEPS}
    ${X11_PKGCONF_LDFLAGS}
    ${SDL_PKGCONF_LDFLAGS}
    )
