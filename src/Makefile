#============Makefile==========
#模块类型，lib库
TYPE = app
#要编译的目标
TARGET = $(OUTPUTDIR)/dcq

#源文件路径，跟makefile文件同目录不用填，目录使用/结束
SRCDIR = ./app/ ./utils/ ./threepart/iniReader/
#编译中间文件路径，跟SRCDIR目录不用填，目录使用/结束
OBJDIR =
#生成目标文件TARGET，存放路径，跟makefile文件同目录不用填，目录使用/结束
OUTDIR =
#目标安装模块（打包用）
INSTALL_TARGETDIR = $(APP_INSTALL)

#这些HEADERS将被安装到 $(PREFIX_INC) 目录下面，供其他模块使用，多个头文件空格分开
HEADERS =

#链接库名，带上-l，-lsvpn 静态库填写绝对路径
LIB += -pthread -ldl  threepart/libhv/lib/libhv.a -lcurl

#引用头文件路径，带上-I， -I$(PREFIX_INC)
INCLUDE += -I$(PREFIX_INC) -I./includes -I./threepart

CFLAGS += -DLINUX -g -Wall -Wshadow -std=c11
CXXFLAGS += -DLINUX -g -Wall -Wshadow -std=c++11
LFLAGS +=


#如果想增加all或者install的依赖性，可以设置下面变量的值，然后增加相应的目标依赖
_all = _all
_install = _install
_cp =
_clean = _clean

include $(MAKEFILEDIR)/Makefile.$(TYPE)

_all:

_install:


