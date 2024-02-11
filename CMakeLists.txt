cmake_minimum_required(VERSION 3.15)

project(hmap VERSION 1.0.0 DESCRIPTION "hash table for C")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -pedantic")

set(CMAKE_C_STANDARD 11)

add_library(hmap SHARED src/hmap.c)
set_target_properties(hmap PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(hmap PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(hmap PROPERTIES PUBLIC_HEADER src/hmap.h)
configure_file(hmap.pc.in hmap.pc @ONLY)
target_include_directories(hmap PRIVATE .)

include(GNUInstallDirs)

install(TARGETS hmap
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/hmap.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

add_subdirectory(tests)
