#!/bin/bash

# 自动化构建脚本
# 用于编译整个项目

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Code Obfuscator Build Script ===${NC}"
echo ""

# 检查是否在项目根目录
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}Error: Must run from project root directory${NC}"
    exit 1
fi

# 清理选项
CLEAN=false
BUILD_TYPE="Release"
BUILD_TESTS=true
BUILD_LLVM_PASS=true

# 解析参数
while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN=true
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --no-tests)
            BUILD_TESTS=false
            shift
            ;;
        --no-llvm)
            BUILD_LLVM_PASS=false
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --clean       Clean build directory before building"
            echo "  --debug       Build in debug mode"
            echo "  --no-tests    Skip building tests"
            echo "  --no-llvm     Skip LLVM Pass (if LLVM not available)"
            echo "  -h, --help    Show this help message"
            exit 0
            ;;
        *)
            echo -e "${YELLOW}Warning: Unknown option $1${NC}"
            shift
            ;;
    esac
done

# 清理构建目录
if [ "$CLEAN" = true ] && [ -d "build" ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf build
fi

# 创建构建目录
echo "Creating build directory..."
mkdir -p build
cd build

# 配置CMake
echo ""
echo -e "${GREEN}Configuring CMake...${NC}"
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if [ "$BUILD_TESTS" = false ]; then
    CMAKE_OPTS="$CMAKE_OPTS -DBUILD_TESTS=OFF"
fi

if [ "$BUILD_LLVM_PASS" = false ]; then
    CMAKE_OPTS="$CMAKE_OPTS -DBUILD_LLVM_PASS=OFF"
fi

cmake .. $CMAKE_OPTS

# 编译
echo ""
echo -e "${GREEN}Building project...${NC}"
CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)
echo "Using $CORES cores for compilation"
make -j$CORES

# 运行测试
if [ "$BUILD_TESTS" = true ]; then
    echo ""
    echo -e "${GREEN}Running tests...${NC}"
    ctest --output-on-failure || {
        echo -e "${YELLOW}Warning: Some tests failed${NC}"
    }
fi

# 完成
echo ""
echo -e "${GREEN}=== Build Complete ===${NC}"
echo ""
echo "Executable: build/obfuscator-cli"
echo ""
echo "Try it:"
echo "  ./obfuscator-cli --help"
echo "  ./obfuscator-cli -i ../examples/simple_example.c -o output.c"
