cmake_minimum_required(VERSION 3.7)
project(eoip 
    VERSION 1.0.0
    LANGUAGES C
)


set(CMAKE_C_COMPILER gcc)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_BUILD_TYPE "debug")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE=1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")


configure_file(manifest.h.in manifest.h)
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)


# Objects
add_library(ini OBJECT ini.c ini.h)
add_library(options OBJECT options.c options.h)
add_library(tunnel OBJECT tunnel.c tunnel.h)
add_library(tunnelset OBJECT tunnelset.c tunnelset.h)
add_library(helpers OBJECT helpers.c helpers.h)
add_library(transport OBJECT transport.c transport.h)
add_library(protocol OBJECT protocol.c protocol.h)


# Entrypoint
add_executable(eoip 
    $<TARGET_OBJECTS:options> 
    $<TARGET_OBJECTS:tunnel> 
    $<TARGET_OBJECTS:tunnelset> 
    $<TARGET_OBJECTS:helpers> 
    $<TARGET_OBJECTS:transport> 
    $<TARGET_OBJECTS:ini> 
    $<TARGET_OBJECTS:protocol> 
    main.c
)
target_include_directories(eoip PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(eoip PUBLIC m clog caio )
add_custom_target(run $<TARGET_FILE:eoip>)
add_custom_target(start $<TARGET_FILE:eoip> start)


set(VALGRIND_FLAGS
    -s
    --tool=memcheck
    --leak-check=yes
    --show-reachable=yes
    --num-callers=20
    --track-fds=yes
)
add_custom_target(profile
    COMMAND "valgrind"
    ${VALGRIND_FLAGS}
    $<TARGET_FILE:eoip>
)


# Install
install(TARGETS eoip DESTINATION ${CMAKE_SYSROOT}/usr/local/bin)


# CPack
set(CPACK_PACKAGE_FILE_NAME "eoip-${PROJECT_VERSION}")
set(CPACK_SET_DESTDIR true)
set(CPACK_PACKAGE_NAME eoip)
set(CPACK_PACKAGE_CONTACT "Vahid Mardani <vahid.mardani@gmail.com>")
set(CPACK_GENERATOR DEB)
set(CPACK_DEBIAN_PACKAGE_DEPENDS 
    "libclog (>= 1.3.0), libcaio (>= 3.1.0)")
include(CPack)


# Lint
set(PRETTYC_FLAGS
    --recursive
    --verbose=0
    --repository=.
    --extensions=c,h,in
    --linelength=80
    --headers=h,in
    --includeorder=standardcfirst
    --root=.
    #--quiet
    ${PROJECT_SOURCE_DIR}
)
add_custom_target(lint COMMAND prettyc ${PRETTYC_FLAGS})
