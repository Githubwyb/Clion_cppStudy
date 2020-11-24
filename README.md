# Clion_cppStudy

- create: 2018.09.15
- author: wangyubo

## 工程说明

自己用做练习C++编写学习使用

```shell
.
├── build                   # 编译生成目录
├── envrc                   # 环境变量文件
├── lib
│   └── threepart           # 第三方库
│       └── sqlite3
│           ├── sqlite3.c
│           ├── sqlite3ext.h
│           └── sqlite3.h
├── Makefile                # makefile入口
├── makefiles               # 通用makefile
│   ├── Makefile.app
│   ├── Makefile.dir
│   ├── Makefile.ko
│   ├── Makefile.so
│   └── Makefile.test
├── README.md               # 说明文档
└── src                     # 源代码目录
    ├── app                 # 主程序源文件
    │   └── main.cpp
    ├── includes            # 公共头文件
    │   ├── algorithm.hpp
    │   ├── baseInstance.hpp
    │   ├── log.hpp
    │   └── threadPool.hpp
    └── utils               # 工具源文件
        ├── algorithm.cpp
        ├── log.cpp
        └── threadPool.cpp
```

## 编译说明

```shell
# 加载环境变量
source envrc
# 编译
make
```

- 生成文件会存放在build目录下
