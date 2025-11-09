/*
 * 简单示例 - 用于测试混淆器
 *
 * 这个文件包含简单的函数，用于演示各种混淆技术
 */

#include <stdio.h>
#include <stdlib.h>

// 简单的加法函数
int add(int a, int b) {
    return a + b;
}

// 简单的乘法函数
int multiply(int x, int y) {
    int result = 0;
    for (int i = 0; i < y; i++) {
        result += x;
    }
    return result;
}

// 包含条件分支的函数
int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

// 包含循环的函数
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// 字符串处理函数（用于测试字符串加密）
void print_message(const char* name) {
    printf("Hello, %s!\n", name);
    printf("Welcome to Code Obfuscator!\n");
}

// 包含多个分支的函数
const char* get_grade(int score) {
    if (score >= 90) {
        return "A";
    } else if (score >= 80) {
        return "B";
    } else if (score >= 70) {
        return "C";
    } else if (score >= 60) {
        return "D";
    } else {
        return "F";
    }
}

// 主函数
int main() {
    printf("=== Code Obfuscation Example ===\n\n");

    // 测试加法
    int sum = add(10, 20);
    printf("10 + 20 = %d\n", sum);

    // 测试乘法
    int product = multiply(5, 6);
    printf("5 * 6 = %d\n", product);

    // 测试最大值
    int maximum = max(15, 25);
    printf("max(15, 25) = %d\n", maximum);

    // 测试阶乘
    int fact = factorial(5);
    printf("5! = %d\n", fact);

    // 测试字符串
    print_message("User");

    // 测试分数等级
    printf("Score 85 -> Grade: %s\n", get_grade(85));
    printf("Score 55 -> Grade: %s\n", get_grade(55));

    return 0;
}
