#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

int main() {
    char buf[100];
    int n;
    int num1, num2;

    n = read(0, buf, sizeof(buf));
    if (n <= 0) {
        printf("Ошибка чтения\n");
        exit(1);
    }

    if (buf[n - 1] == '\n') {
        buf[n - 1] = '\0';
    } else {
        buf[n] = '\0';
    }

    printf("|%s|\n", buf);

    char *p = buf;

    if (!*p) {
        printf("Ошибка: пустая строка\n");
        exit(1);
    }

    int is_neg1 = 0;
    if (*p == '-') {
        is_neg1 = 1;
        p++;
    }
    if (!is_digit(*p)) {
        printf("Ошибка: первое число содержит недопустимые символы\n");
        exit(1);
    }
    num1 = atoi(p);
    if (is_neg1) num1 *= -1;

    while (*p && (is_digit(*p) || *p == '-')) p++;

    if (*p != ' ') {
        printf("Ошибка: между числами должен быть ровно один пробел\n");
        exit(1);
    }
    p++;

    int is_neg2 = 0;
    if (*p == '-') {
        is_neg2 = 1;
        p++;
    }
    if (!is_digit(*p)) {
        printf("Ошибка: второе число содержит недопустимые символы\n");
        exit(1);
    }
    num2 = atoi(p);
    if (is_neg2) num2 *= -1;


    p++;
    while (*p && is_digit(*p)) p++;
    if (*p) {
        printf("Ошибка: после второго числа обнаружены лишние символы\n");
        exit(1);
    }

    int ans = add(num1, num2);
    printf("Сумма: %d\n", ans);

    exit(0);
}