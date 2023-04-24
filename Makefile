TARGET := main

SOURCES := $(wildcard *.c) $(wildcard */*.c)

OBJECTS := $(SOURCES:.c=.o)

LOGDIR := ./log
VLDIR := $(LOGDIR)/vl
INDEXDIR := $(LOGDIR)/index

# 自己写了一个python自动检测头文件路径的脚本小工具

PYTHON := python3
SCRIPT := include.py

INCLUDE := $(shell $(PYTHON) $(SCRIPT))

# 当然你也可以像这样手动一条一条添加
# INCLUDE := -I ./ \
# 	-I ./pch 

CFLAGS := -g -Wall -Werror -Wno-unused


$(TARGET): $(OBJECTS) 
	mkdir -p $(LOGDIR)
	mkdir -p $(VLDIR)
	mkdir -p $(INDEXDIR)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

init:
	make clean
	rm -rf $(LOGDIR)
	make

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

