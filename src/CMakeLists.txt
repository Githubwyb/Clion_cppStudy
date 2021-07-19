# 设置源文件目录
SET(SRC_DIRS ./app ./utils)

# 使用循环将上面定义的源文件目录下的所有文件取出
SET(SRC_FILES)
foreach(SRC_DIR ${SRC_DIRS})
    # aux_source_directory(<dir> <variable>) 将dir下面的所有源文件赋值给变量（追加）
    AUX_SOURCE_DIRECTORY(${SRC_DIR} SRC_FILES)
endforeach(SRC_DIR)

if(WIN32)
    # windows下epollworker编译不过，因为都是linuxapi
    list(REMOVE_ITEM SRC_FILES "./app/epollWorker.cpp")
endif()

# 将变量指定的源文件编译成可执行文件main
ADD_EXECUTABLE(${BIN_NAME} ${SRC_FILES})

# 给可执行文件编译设置头文件目录
target_include_directories(${BIN_NAME} PRIVATE ./includes ../includes spdlog)

# 给可执行文件添加动态链接库
link_directories(${PROJECT_SOURCE_DIR}/build/output/lib) #添加动态连接库的路径

target_link_libraries(
    ${BIN_NAME} PRIVATE
    # 第三方库
    spdlog
    # 系统库
    pthread
    dl
    # 自定义库
    ${LIB_NAME}
)
