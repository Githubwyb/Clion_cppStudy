SET(TEST_BIN_NAME "mylib_tst")

SET(SRC_FILES)
AUX_SOURCE_DIRECTORY(. SRC_FILES)

add_executable(${TEST_BIN_NAME} ${SRC_FILES})
get_target_property(TMP run SOURCE_DIR)
MESSAGE(${TMP})
target_include_directories(
    ${TEST_BIN_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/../src/includes
)

target_link_libraries(
    ${TEST_BIN_NAME} PRIVATE
    # 第三方库
    spdlog
    gtest
    gtest_main
    # 系统库
    # pthread
    # dl
    # 自定义库
    ${LIB_NAME}
)
