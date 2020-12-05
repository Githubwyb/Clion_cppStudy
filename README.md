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
- 安装可以将build目录整体拷贝到一个地方，将build/dcq添加一个软连接即可

## 运行说明

```shell
Usage: dcq [options...] <domain|-f file>
    -c <filePath> System config file, global.ini
                  default (program path)/global.ini
    -s <filePath> Server config to parse domain, server.json
                  default parse from global.ini::server.confPath
    -p <dirPath>  Parser so lib path, parserLibPath/
                  default parse from global.ini::parser.libPath
                  use parserLibPath/lib(type).so to parse domain which type defined in server.json
```

## 日志和调试

### 日志

存放在build/logs/dcq.log，默认只打info

## 调试

加上-d选项，将会打印debug日志，并会输出日志所在文件名、行数和函数
