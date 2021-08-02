cmake_minimum_required(VERSION 3.11)

project(CURL-ENGINE)

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 禁止生成curl测试工程
set(BUILD_TESTING NO CACHE INTERNAL "disable curl testing")
# curl生成动态库
set(BUILD_SHARED_LIBS YES CACHE INTERNAL "enable curl dynamic library")
# 禁止curl使用zlib
set(CURL_ZLIB NO CACHE INTERNAL "disable curl zlib")

# 启动libcurl的openssl
set(CMAKE_USE_OPENSSL ON CACHE INTERNAL "libcurl enable ssl")

add_subdirectory(curl/curl-7.69.1)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    $<TARGET_PROPERTY:libcurl,SOURCE_DIR>/include
)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    libcurl
    skfApi
)
