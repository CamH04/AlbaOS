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
            void putchar(unsigned char ch, unsigned char forecolor,unsigned char backcolor, uint8_t x, uint8_t y);
            void cprintf(char* str, uint8_t forecolor, uint8_t backcolor, uint8_t x, uint8_t y);
        };
    }
}

#endif
