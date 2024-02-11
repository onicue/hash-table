enable_testing()

add_executable(hmap_tests main.c tools.c tests.c)

add_definitions(-DTXT_FILE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/words.txt")

target_link_libraries(hmap_tests hmap)

add_test(NAME HMapTests COMMAND hmap_tests)

