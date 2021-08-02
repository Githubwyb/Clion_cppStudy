# 设置编译二进制名字
SET(LIB_NAME skfApi)

# aux_source_directory(<dir> <variable>) 将dir下面的所有源文件赋值给变量
AUX_SOURCE_DIRECTORY(./src SRC_FILES)

# 生成动态库，不带SHARED生成静态库
add_library(${LIB_NAME} SHARED ${SRC_FILES})

find_package(OpenSSL REQUIRED)

# 给可执行文件编译设置头文件目录
target_include_directories(${LIB_NAME} PRIVATE
    ../include
    ../../include
    ${OPENSSL_INCLUDE_DIR}
)

# 对外提供的头文件目录，当前目录的include
target_include_directories(${LIB_NAME} PUBLIC
    ./include
)

# 链接库
target_link_libraries(${LIB_NAME} PRIVATE
    ${OPENSSL_LIBRARIES}
)
