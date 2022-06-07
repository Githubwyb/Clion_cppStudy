project(skfApi
    VERSION 1.0.0.0
    DESCRIPTION "GM skf api wrapper"
)

# aux_source_directory(<dir> <variable>) 将dir下面的所有源文件赋值给变量
AUX_SOURCE_DIRECTORY(./src SRC_FILES)

# SHARED生成动态库，STATIC生成静态库
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

target_compile_definitions(
    ${PROJECT_NAME} PRIVATE
    # USE_SPDLOG
    SKFAPI_EXPORT
    __LOG_TAG__="${PROJECT_NAME}"
)

find_package(OpenSSL REQUIRED)

# 对外提供的头文件目录，当前目录的include
target_include_directories(${PROJECT_NAME} PUBLIC
    ./include
)

# 给可执行文件编译设置头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ../../include
    ${OPENSSL_INCLUDE_DIR}
    ./include/internal
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OPENSSL_LIBRARIES}
    # spd_log
)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        dl
    )
endif (UNIX)
