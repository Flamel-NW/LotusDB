# 自己写的一个python脚本小工具
# 可以自动检测目录下所有包含某类型文件的路径名


import argparse
import os
from fnmatch import fnmatch

parse = argparse.ArgumentParser(description="一个python脚本小工具 \n\
    可以自动检测目录下所有包含某类型文件的路径名\n\n")

parse.add_argument('-p', '--prefix', type=str, default="-I", 
    help="在每条路径名前面加上一个字符串前缀, 默认PREFIX=-I")
parse.add_argument('-n', '--name', type=str, default="*.h",
    help="按照文件名通配符查找文件, 默认NAME=*.h")
parse.add_argument('-d', '--dir', type=str, default="./",
    help="指定要查找的目录, 默认DIR=./")

args = parse.parse_args()


for root, dirs, files in os.walk(args.dir):
    for file in files:
        if fnmatch(file, args.name):
            print(args.prefix + ' ' + root)
            break


# 主要用于在Makefile中生成INCLUDE路径

# PYTHON := python3
# SCRIPT := include.py

# INCLUDE := $(shell $(PYTHON) $(SCRIPT))
