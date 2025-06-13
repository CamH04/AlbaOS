#include <common/asl_string.h>

// (AlbaOS Standard Lib For String Based Methods)

using namespace albaos;
using namespace albaos::common;


int asl_string::atoi(char*& str, int base) {
    base = 10;
    if (base < 2 || base > 16) return 0;
    int result = 0;
    int sign = 1;
    while (*str == ' ' || *str == '\t') ++str;
    if (*str == '-') {
        sign = -1;
        ++str;
    } else if (*str == '+') {
        ++str;
    }
    while (*str) {
        char c = *str;
        int digit;

        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10;
        else break;
        if (digit >= base) break;
        result = result * base + digit;
        ++str;
    }
    return sign * result;
}
