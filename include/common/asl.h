#ifndef __ALBAOS__COMMON__ASL_H
#define __ALBAOS__COMMON__ASL_H

#include <common/types.h>


namespace albaos
{
    namespace common
    {
        class asl{
        public:

            uint16_t SetTextColor(bool set, albaos::common::uint16_t color = 0x07);


            double Random(void);

            //printstuff
            void putchar(unsigned char ch, unsigned char forecolor,unsigned char backcolor, albaos::common::uint8_t x, albaos::common::uint8_t y);
            void cprintf(char* str, albaos::common::uint8_t forecolor, albaos::common::uint8_t backcolor, albaos::common::uint8_t x, albaos::common::uint8_t y);
            void printc(char c);
            void printfhere(const char* str, albaos::common::uint8_t line);
            void printfHex(uint8_t key);
            void printfHex16(uint16_t key);
            void printfHex32(uint32_t key);





            uint32_t memRead(albaos::common::uint32_t memory);
            void memWrite(albaos::common::uint32_t memory, albaos::common::uint32_t inputVal);
        };
    }
}

#endif
