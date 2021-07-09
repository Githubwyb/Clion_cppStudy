SET(TEST_BIN_NAME "${BIN_NAME}_tst")

SET(SRC_FILES)
AUX_SOURCE_DIRECTORY(. SRC_FILES)

add_executable(${TEST_BIN_NAME} ${SRC_FILES})

target_include_directories(
  ${TEST_BIN_NAME} PRIVATE
  ../includes
)

target_link_libraries(
  ${TEST_BIN_NAME} PRIVATE
  gtest
  gtest_main
)
