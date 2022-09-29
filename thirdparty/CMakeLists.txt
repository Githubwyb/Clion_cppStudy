# 第三方库编译

# 日志库
add_subdirectory(spdlog)

# googletest
add_subdirectory(gtest)

# 禁止生成curl测试工程
set(BUILD_TESTING NO CACHE INTERNAL "disable curl testing")
# curl生成动态库
set(BUILD_SHARED_LIBS YES CACHE INTERNAL "enable curl dynamic library")
# 禁止curl使用zlib
set(CURL_ZLIB NO CACHE INTERNAL "disable curl zlib")
# 启动libcurl的openssl
set(CMAKE_USE_OPENSSL ON CACHE INTERNAL "libcurl enable ssl")
# curl
add_subdirectory(curl)

# curlEngine
add_subdirectory(curlEngine)

# promise-cpp
add_subdirectory(promise-cpp)
