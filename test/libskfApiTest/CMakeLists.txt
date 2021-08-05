SET(TEST_BIN_NAME "libskfApi_tst")

SET(SRC_FILES)
AUX_SOURCE_DIRECTORY(. SRC_FILES)
get_target_property(SRC_DIR skfApi SOURCE_DIR)
AUX_SOURCE_DIRECTORY(${SRC_DIR}/../common SRC_FILES)

find_package(OpenSSL REQUIRED)

add_executable(${TEST_BIN_NAME} ${SRC_FILES})
target_include_directories(
    ${TEST_BIN_NAME} PRIVATE
    ${OPENSSL_INCLUDE_DIR}
)

# 使用spdlog作为日志库
# add_definitions(-DUSE_SPDLOG)
target_compile_definitions(${TEST_BIN_NAME} PRIVATE
    USE_SPDLOG
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
    skfApi
    ${OPENSSL_LIBRARIES}
)
# message(${CMAKE_BINARY_DIR})
# add_custom_command(TARGET ${TEST_BIN_NAME}
#                     POST_BUILD
#                     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bin/Debug/gtestd.dll ${CMAKE_BINARY_DIR}/output/Debug
#                     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bin/Debug/gtest_maind.dll ${CMAKE_BINARY_DIR}/output/Debug)
