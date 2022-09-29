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

# 指定使用boost库的静态库还是动态库
set(Boost_USE_STATIC_LIBS ON)
# 1.70: 指定boost库最低版本1.70，可以不加
# REQUIRED: 必须找到，不然报错
# COMPONENTS: 找到filesystem和system两个模块，模块路径会加到Boost_LIBRARIES变量中，不加模块此变量为空
find_package(Boost 1.70 REQUIRED
    COMPONENTS system
)

# 将变量指定的源文件编译成可执行文件main
add_executable(${BIN_NAME} ${SRC_FILES})

# 使用spdlog作为日志库
# add_definitions(-DUSE_SPDLOG)
target_compile_definitions(${BIN_NAME} PRIVATE
    USE_SPDLOG
    BOOST_STACKTRACE_USE_BACKTRACE
)

# 给可执行文件编译设置头文件目录
target_include_directories(${BIN_NAME} PRIVATE
    ./include
    # 第三方库头文件
    spdlog
)
# 添加boost库的目录，不需要自己去选择模块
target_link_directories(
    ${BIN_NAME} PRIVATE
    # 第三方库
    ${Boost_LIBRARY_DIRS}
)

# 给可执行文件添加动态链接库
link_directories(${PROJECT_SOURCE_DIR}/build/output/lib) #添加动态连接库的路径

target_link_libraries(
    ${BIN_NAME} PRIVATE
    # 第三方库
    spdlog
    ${Boost_LIBRARIES}
    # 系统库
    pthread
    dl
    backtrace
    # 自定义库
    ${LIB_NAME}
)
