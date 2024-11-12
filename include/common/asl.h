#ifndef __ALBAOS__COMMON__ASL_H
#define __ALBAOS__COMMON__ASL_H

#include <common/types.h>


namespace albaos
{
    namespace common
    {
        class asl{
        public:
            void PrintCpuSpeed();
            double calculateClockSpeed();
            void benchmark();
            uint64_t getTicks();
            uint64_t rdtsc(void);
            //memory
            uint16_t inw (unsigned short int __port);
            void outb (unsigned char __value, unsigned short int __port);
            void outw (unsigned short int __value, unsigned short int __port);
            int memcmp(const void *s1, const void *s2, size_t n);
            void memmove(void *dest, void *src, size_t n);
            void memcpy(void *dest, void *src, size_t n);
            uint32_t memRead(albaos::common::uint32_t memory);
            void memWrite(albaos::common::uint32_t memory, albaos::common::uint32_t inputVal);
            uint32_t Trollfnv1a(char* str);
            uint16_t hash(char* cmd);
            uint16_t SetTextColor(bool set, albaos::common::uint16_t color = 0x07);
            double Random(void);
            uint16_t betterRandom();
            char* IntToString(uint32_t num);
            uint32_t StringToInt(char* args);
            void sleep(uint32_t ms);
            //printstuff
            uint8_t Web2EGA(uint32_t colour);
            void putchar(unsigned char ch, unsigned char forecolor,unsigned char backcolor, albaos::common::uint8_t x, albaos::common::uint8_t y);
            void cprintf(char* str, albaos::common::uint8_t forecolor, albaos::common::uint8_t backcolor, albaos::common::uint8_t x, albaos::common::uint8_t y);
            void printc(char c);
            void printfhere(const char* str, albaos::common::uint8_t line);
            void printfHex(uint8_t key);
            void printfHex16(uint16_t key);
            void printfHex32(uint32_t key);
            void reboot();
            void shutdown();
            void putcharTUI(unsigned char ch, unsigned char forecolor,unsigned char backcolor, uint8_t x, uint8_t y);
            void TUI(uint8_t forecolor, uint8_t backcolor,uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,bool shadow);
            void printfTUI(char* str, uint8_t forecolor, uint8_t backcolor, uint8_t x, uint8_t y);
            uint16_t strlen(char* args);
            char* argparse(char* args, uint8_t num);
            uint8_t argcount(char* args);

        };
    }
}

#endif
