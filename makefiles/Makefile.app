CC = gcc
CXX = g++

CFLAGS += -Wall -Werror
LFLAGS +=

# gcov
ifeq ($(debug), 1)
	CFLAGS   := $(filter-out -O2, $(CFLAGS))
	CXXFLAGS := $(filter-out -O2, $(CXXFLAGS))
	CFLAGS   += -g -DDEBUG_GLOBAL --coverage
	CXXFLAGS += -g -DDEBUG_GLOBAL --coverage
	LFLAGS  += --coverage
endif

REPORTDIR=$(shell echo $(TARGET) |awk -F. '{print $$1}')

#如果为空，就是当前目录
ifeq ($(SRCDIR),)
	SRCDIR += ./
endif

#下面四个命令通过模式匹配获取当前目录下的所有C,CPP,O文件
CPP_SOURCES = $(foreach d,$(SRCDIR),$(wildcard $(d)*.cpp))
C_SOURCES = $(foreach d,$(SRCDIR),$(wildcard $(d)*.c))
CPP_OBJS = $(patsubst %.cpp, $(OBJDIR)%.o, $(CPP_SOURCES))
C_OBJS = $(patsubst %.c, $(OBJDIR)%.o, $(C_SOURCES))

#下面把一些其他目录下的源文件也放到里面
SOURCES += $(CPP_SOURCES) $(C_SOURCES) $(OBJS:%.o=%.c*)
OBJS += $(CPP_OBJS) $(C_OBJS)
ALL_GCNO = $(OBJS:%.o=%.gcno)
ALL_GCDA = $(OBJS:%.o=%.gcda)

.PHONY: clean all cp cpp cpso _clean _all _install test

.SUFFIXES: .c .cpp .o

.c.o:
	$(CC) -c $*.c -o $*.o $(INCLUDE) $(CFLAGS)
.cpp.o:
	$(CXX) -c $*.cpp -o $*.o $(INCLUDE) $(CXXFLAGS)


all: $(OUTDIR)$(TARGET) $(_all)

header:
ifneq ($(HEADERS),)
	install -d $(PREFIX_INC)
	install --verbose --mode=0644 $(HEADERS) $(PREFIX_INC)
endif

$(OUTDIR)$(TARGET): $(OBJS)
ifeq ($(CPP_OBJS),)
	$(CC)  $(LFLAGS) -o $(OUTDIR)$(TARGET) $(OBJS) -L$(PREFIX_LIB) $(LIB)
else
	$(CXX) $(LFLAGS) -o $(OUTDIR)$(TARGET) $(OBJS) -L$(PREFIX_LIB) $(LIB)
endif
	make header

install: all $(_install) $(HEADERS)
ifneq ($(debug), 1)
	strip $(OUTDIR)$(TARGET)
endif
	install -d $(INSTALL_TARGETDIR)
	install --verbose --mode=0755 $(OUTDIR)$(TARGET) $(INSTALL_TARGETDIR)


cp: $(_cp)

cppscan:clean
	install -d $(CPPTEST_WORKSPACE)/$(TARGET)
	cpptesttrace --cpptesttraceOutputFile=$(TARGET).bdf --cpptesttraceProjectName=$(TARGET) make || echo
	cpptestcli -data $(CPPTEST_WORKSPACE) -resource $(TARGET) -bdf $(TARGET).bdf -config user://mustfix -report $(CPPTEST_REPORT)/$(REPORTDIR) || echo

clean: $(_clean)
	rm -f $(OUTDIR)$(TARGET) $(OBJS) $(TARGET).bdf $(ALL_GCNO) $(ALL_GCDA)
	rm -f gcov.zip *.gcda *.gcno
	rm -f $(OUTDIR)$(TARGET)_test ./test/*.o

gcov:
	$(LCOV_GCOV_TOOL) `pwd`

test:
	@if [ -f "./Makefile.test" ]; then \
		make -f Makefile.test; \
	else \
		echo "dont exit Makefile.test, have no test"; \
	fi \
