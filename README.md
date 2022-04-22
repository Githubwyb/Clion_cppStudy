# Clion\_cppStudy

- create: 2020.11.25
- author: wangyubo

## 工程说明

练习c++使用

## 编译说明

### 1. makefile版本

- 修改envrc

```shell
bash build.sh
```

- 生成文件会存放在build目录下
- 安装可以将build目录整体拷贝到一个地方，将`build/${BINNAME}`软连接到一个地方即可

### 2. cmake版本

- 已经写好CMakeLists.txt，可以直接使用cmake工具进行编译

## 目录说明

```shell
.
├── build                       # 编译生成的文件目录
│   └── lib
├── build.sh                    # 编译脚本
├── envrc                       # 编译需要的环境变量
├── includes                    # 库和二进制通用头文件
│   └── common.hpp
├── lib                         # 自己写的一下静态库
│   ├── Makefile
│   ├── myhook
│   │   ├── Makefile
│   │   └── myhook.cpp
│   ├── mylib
│   │   ├── Makefile
│   │   └── mylib.cpp
│   └── threepart
├── Makefile                    # 最上层makefile
├── makefiles                   # 通用类型的makefile文件
│   ├── Makefile.app
│   ├── Makefile.dir
│   ├── Makefile.ko
│   ├── Makefile.so
│   └── Makefile.test
├── README.md
└── src                         # 二进制源文件目录
    ├── app                     # 自己编写的源文件
    │   └── main.cpp
    ├── includes                # 自己编写的头文件
    │   ├── algorithm.hpp
    │   ├── baseInstance.hpp
    │   ├── bugReport.hpp
    │   ├── log.hpp
    │   ├── threadPool.hpp
    │   └── utils.hpp
    ├── Makefile                # 二进制makefile
    ├── threepart               # 第三方库，用于二进制
    │   ├── ensure              # 智能断言库
    │   │   ├── autofree.h
    │   │   ├── ensure.h
    │   │   ├── smart_assert.h
    │   │   ├── utlCore11.h
    │   │   ├── utlCoreMacro.h
    │   │   ├── utlSTDHeaders.h
    │   │   └── utlTypeinfo.h
    │   ├── iniReader           # ini读取库
    │   │   ├── ini.c
    │   │   ├── ini.h
    │   │   ├── INIReader.cpp
    │   │   └── INIReader.h
    │   ├── libhv               # 网络请求库
    │   └── spdlog              # 日志库
    └── utils                   # 工具函数，较为通用的，区别于业务源文件
        ├── algorithm.cpp
        ├── log.cpp
        ├── threadPool.cpp
        └── utils.cpp
```
