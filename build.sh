#!/bin/bash

# 启用错误检查（任何命令失败则停止执行）
set -e

# 设置颜色代码
GREEN='\033[0;32m'
CYAN='\033[0;36m'
NC='\033[0m' # 恢复默认颜色

echo -e "${GREEN}[1/4] 正在清理旧构建文件...${NC}"
rm -rf build

echo -e "${GREEN}[2/4] 创建构建目录...${NC}"
mkdir -p build
cd build

echo -e "${GREEN}[3/4] 生成构建系统...${NC}"
cmake ..

echo -e "${GREEN}[4/4] 编译项目...${NC}"
make

echo -e "\n${CYAN}✅ 构建成功！执行以下命令运行程序：${NC}"
echo -e "${CYAN}   ./build/mian${NC}"

# 自动运行程序（如需取消自动运行，删除以下三行）
echo -e "\n${CYAN}🔄 自动运行程序...${NC}"
cowsay "hello 玲玲"
./main