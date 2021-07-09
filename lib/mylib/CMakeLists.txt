# aux_source_directory(<dir> <variable>) 将dir下面的所有源文件赋值给变量
AUX_SOURCE_DIRECTORY(. SRC_FILES)
AUX_SOURCE_DIRECTORY(../common COMMON_SRC_FILES)

list(APPEND SRC_FILES ${COMMON_SRC_FILES})

# 生成动态库，不带SHARED生成静态库
add_library(${LIB_NAME} SHARED ${SRC_FILES})

# 给可执行文件编译设置头文件目录
target_include_directories(${LIB_NAME} PRIVATE ../includes ../../includes)
