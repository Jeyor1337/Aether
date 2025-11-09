/*
 * 密码验证器示例
 *
 * 演示如何使用混淆器保护密码验证逻辑
 * 注意：这只是教学示例，实际应用应使用加密哈希
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// 简单的密码验证（将被混淆）
bool validate_password(const char* password) {
    // 这个简单的验证逻辑将被混淆
    // 实际应用应使用bcrypt等安全哈希
    const char* correct = "SecretPass123";

    if (strlen(password) != strlen(correct)) {
        return false;
    }

    for (size_t i = 0; i < strlen(correct); i++) {
        if (password[i] != correct[i]) {
            return false;
        }
    }

    return true;
}

// 检查密码强度
int check_password_strength(const char* password) {
    int strength = 0;
    size_t len = strlen(password);

    // 长度检查
    if (len >= 8) strength++;
    if (len >= 12) strength++;

    // 包含数字
    bool has_digit = false;
    bool has_upper = false;
    bool has_lower = false;
    bool has_special = false;

    for (size_t i = 0; i < len; i++) {
        if (password[i] >= '0' && password[i] <= '9') has_digit = true;
        if (password[i] >= 'A' && password[i] <= 'Z') has_upper = true;
        if (password[i] >= 'a' && password[i] <= 'z') has_lower = true;
        if (password[i] == '!' || password[i] == '@' || password[i] == '#') {
            has_special = true;
        }
    }

    if (has_digit) strength++;
    if (has_upper) strength++;
    if (has_lower) strength++;
    if (has_special) strength++;

    return strength;
}

// 加密函数（简单XOR，用于演示）
void simple_encrypt(char* data, size_t len, unsigned char key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
}

int main() {
    printf("=== Password Validation System ===\n");
    printf("(This code will be obfuscated)\n\n");

    char input[100];
    printf("Enter password: ");

    if (fgets(input, sizeof(input), stdin) != NULL) {
        // 移除换行符
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
        }

        // 验证密码
        if (validate_password(input)) {
            printf("\n[SUCCESS] Password is correct!\n");
            printf("Access granted.\n");
        } else {
            printf("\n[FAILED] Invalid password!\n");
            printf("Access denied.\n");
        }

        // 显示密码强度
        int strength = check_password_strength(input);
        printf("\nPassword strength: %d/7\n", strength);

        if (strength < 3) {
            printf("Weak password!\n");
        } else if (strength < 5) {
            printf("Medium password.\n");
        } else {
            printf("Strong password!\n");
        }
    }

    return 0;
}

/*
 * 混淆后的效果：
 * 1. validate_password函数的控制流将被平坦化
 * 2. 字符串 "SecretPass123" 将被加密
 * 3. 插入垃圾指令干扰分析
 * 4. 使用不透明谓词隐藏真实逻辑
 */
