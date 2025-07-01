#include <common/asl_string.h>

// (AlbaOS Standard Lib For String Based Methods)

using namespace albaos;
using namespace albaos::common;

//string to int std
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

//signed int to nul terminated string
void asl_string::itoa(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;
    if (value < 0 && base == 10) { //negative numbers bad
        *ptr++ = '-';
        value = -value;
    }
    do { //num to str
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789ABCDEF"[tmp_value - value * base];
    } while (value);

    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

void asl_string::utoa(unsigned int value, char* str, int base) { //same as itoa for unsigned
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    unsigned int tmp_value;
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789ABCDEF"[tmp_value - value * base];
    } while (value);
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}


int asl_string::strcmp(const char* str1, const char* str2) {
    while (*str1 != '\0' && *str2 != '\0') {
        if (*str1 != *str2) {
            return (unsigned char)*str1 - (unsigned char)*str2;
        }
        str1++;
        str2++;
    }
    return (unsigned char)*str1 - (unsigned char)*str2;
}
uint32_t asl_string::StringToInt(char* args){
    uint32_t number = 0;
    uint16_t i = 0;
    bool foundNum = false;

    for(uint16_t i = 0; args[i] != '\0'; i++){
        if((args[i] >= 58 || args[i] <= 47) && args[i] != ' '){
            return 0;
        }
        if(args[i] != ' '){
            number *= 10;
            number += ((uint32_t)args[i] -  48);
            foundNum = true;
            args[i] = ' ';
        }
        else{
            if(foundNum){
                return number;
            }
        }
    }
    return number;
}

char* asl_string::IntToString(uint32_t num) {
        uint32_t numChar = 1;
        uint8_t i = 1;
        if (num % 10 != num) {
                while ((num / (numChar)) >= 10) {
                        numChar *= 10;
                        i++;
                }
                char* str = "4294967296";
                uint8_t strIndex = 0;
                while (i) {
                        str[strIndex] = (char)(((num / (numChar)) % 10) + 48);
                        if (numChar >= 10) {

                                numChar /= 10;
                        }
                        strIndex++;
                        i--;
                }
                str[strIndex] = '\0';
                return str;
        }
        char* str = " ";
        str[0] = (num + 48);
        return str;
}

uint16_t asl_string::strlen(char* args) {
        uint16_t length = 0;
        for (length = 0; args[length] != '\0'; length++) {

        }
        return length;
}
