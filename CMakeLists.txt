# cmake最小支持版本3.8
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)


# 项目工程名字
PROJECT(Clion_cppStudy
    LANGUAGES C CXX
    VERSION 1.0.0.0
    DESCRIPTION "Diy c++ project"
)

# 设置lib库生成路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/output)
# 设置bin文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/output)
# 编译命令导出json给其他地方使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

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

if (MSVC)
    # windows的msvc编译，需要将gtest的选项改一下，不然编译有问题
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # 对msvc关闭uincode的warning提示
    add_compile_options(/wd4819)
endif()

# 先编译第三方库
add_subdirectory(threepart)
# 设置lib子目录
add_subdirectory(lib)
# 设置src子目录
add_subdirectory(src)

if(TEST)
    # 添加单元测试目录
    add_subdirectory(test)
endif(TEST)