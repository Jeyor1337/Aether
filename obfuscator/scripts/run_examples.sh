#!/bin/bash

# 运行示例的自动化脚本
# 编译并测试所有示例程序

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${GREEN}=== Code Obfuscator - Example Runner ===${NC}"
echo ""

# 检查可执行文件
if [ ! -f "build/obfuscator-cli" ]; then
    echo -e "${YELLOW}obfuscator-cli not found. Building...${NC}"
    ./scripts/build.sh
fi

# 创建输出目录
mkdir -p examples/output

# 示例1: simple_example.c
echo -e "${BLUE}[1/2] Processing simple_example.c${NC}"
echo "  Level 1 (Light)..."
./build/obfuscator-cli -i examples/simple_example.c \
    -o examples/output/simple_example_l1.c -l 1 -v

echo "  Level 2 (Medium)..."
./build/obfuscator-cli -i examples/simple_example.c \
    -o examples/output/simple_example_l2.c -l 2 -v

echo "  Level 3 (Heavy)..."
./build/obfuscator-cli -i examples/simple_example.c \
    -o examples/output/simple_example_l3.c -l 3 -v

# 示例2: password_checker.c
echo ""
echo -e "${BLUE}[2/2] Processing password_checker.c${NC}"
echo "  Level 2 (Medium)..."
./build/obfuscator-cli -i examples/password_checker.c \
    -o examples/output/password_checker_l2.c -l 2 -v

echo "  Level 3 (Heavy)..."
./build/obfuscator-cli -i examples/password_checker.c \
    -o examples/output/password_checker_l3.c -l 3 -v

# 编译测试
echo ""
echo -e "${GREEN}Compiling examples...${NC}"

if command -v gcc &> /dev/null; then
    echo "  Compiling original simple_example..."
    gcc examples/simple_example.c -o examples/output/simple_example_orig

    echo "  Compiling obfuscated versions..."
    gcc examples/output/simple_example_l1.c -o examples/output/simple_example_l1 || true
    gcc examples/output/simple_example_l2.c -o examples/output/simple_example_l2 || true

    echo ""
    echo -e "${GREEN}Test executables:${NC}"
    ls -lh examples/output/simple_example_* | grep -v ".c"

    echo ""
    echo "Run examples:"
    echo "  ./examples/output/simple_example_orig"
    echo "  ./examples/output/simple_example_l2"
else
    echo -e "${YELLOW}GCC not found, skipping compilation${NC}"
fi

echo ""
echo -e "${GREEN}=== Done ===${NC}"
echo "Obfuscated files are in examples/output/"
