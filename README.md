# dcq

- create: 2020.11.25
- author: wangyubo

## 工程说明

域名解析工具

## 编译说明

```shell
bash build.sh
```

- 生成文件会存放在build目录下

## 运行说明

```shell
# 运行单测
source envrc
cd src
make test
```

## 日志和调试

### 日志

存放在build/logs/dcq.log，默认只打info

## 调试

修改build/global.ini，`logDebug = 1`打开debug日志
