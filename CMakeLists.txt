# cmake最小支持版本3.8
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)


# 项目工程名字
PROJECT(Clion_cppStudy
    LANGUAGES C CXX
    VERSION 1.0.0.0
    DESCRIPTION "Diy c++ project"
)

# 设置lib库生成路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/output/lib)
# 设置bin文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/output)
# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

# 设置选项
option(TEST "Generate test, default is ON" ON)
option(RELEASE "Build release, default is OFF" OFF)

if(RELEASE)
   SET(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "" FORCE)
else(RELEASE)
   SET(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "" FORCE)
endif(RELEASE)


# 设置编译二进制名字
SET(BIN_NAME run)
# 设置编译二进制名字
SET(LIB_NAME mylib)

# 先编译第三方库
add_subdirectory(threepart)
# 设置lib子目录
add_subdirectory(lib)
# 设置src子目录
add_subdirectory(src)

if(TEST)
    add_subdirectory(test)
endif(TEST)

# 是否有MSVC
if(MSVC)
    # 多线程编译
    if(ENABLE_MULTIPROCESS)
        add_compile_options(/MP)
    endif(ENABLE_MULTIPROCESS)
endif()
option(BUILD_SDP_TESTING "Generate SDP test project, default is YES" YES)
# 判断是否为win32并且使用msvc
if(WIN32 AND MSVC)
    add_compile_options(
        /wd4005             # thrift warning: WIN32_LEAN_AND_MEAN redefined
        /wd4244             # thrift warning: translate uint64_t to std::size_t
    )

    add_link_options(
        /MAP
    )
endif()
