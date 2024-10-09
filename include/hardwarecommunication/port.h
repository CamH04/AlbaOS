#ifndef __ALBAOS__HARDWARECOMMUNICATION__PORT_H
#define __ALBAOS__HARDWARECOMMUNICATION__PORT_H

#include <common/types.h>

namespace albaos
{
    namespace hardwarecommunication
    {

        class Port
        {
            protected:
                Port(albaos::common::uint16_t portnumber);
                ~Port();
                albaos::common::uint16_t portnumber;
        };


        class Port8Bit : public Port
        {
            public:
                Port8Bit(albaos::common::uint16_t portnumber);
                ~Port8Bit();

                virtual albaos::common::uint8_t Read();
                virtual void Write(albaos::common::uint8_t data);

            protected:
                static inline albaos::common::uint8_t Read8(albaos::common::uint16_t _port)
                {
                    albaos::common::uint8_t result;
                    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (_port));
                    return result;
                }

                static inline void Write8(albaos::common::uint16_t _port, albaos::common::uint8_t _data)
                {
                    __asm__ volatile("outb %0, %1" : : "a" (_data), "Nd" (_port));
                }
        };



        class Port8BitSlow : public Port8Bit
        {
            public:
                Port8BitSlow(albaos::common::uint16_t portnumber);
                ~Port8BitSlow();

                virtual void Write(albaos::common::uint8_t data);
            protected:
                static inline void Write8Slow(albaos::common::uint16_t _port, albaos::common::uint8_t _data)
                {
                    __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (_data), "Nd" (_port));
                }

        };



        class Port16Bit : public Port
        {
            public:
                Port16Bit(albaos::common::uint16_t portnumber);
                ~Port16Bit();

                virtual albaos::common::uint16_t Read();
                virtual void Write(albaos::common::uint16_t data);

            protected:
                static inline albaos::common::uint16_t Read16(albaos::common::uint16_t _port)
                {
                    albaos::common::uint16_t result;
                    __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (_port));
                    return result;
                }

                static inline void Write16(albaos::common::uint16_t _port, albaos::common::uint16_t _data)
                {
                    __asm__ volatile("outw %0, %1" : : "a" (_data), "Nd" (_port));
                }
        };



        class Port32Bit : public Port
        {
            public:
                Port32Bit(albaos::common::uint16_t portnumber);
                ~Port32Bit();

                virtual albaos::common::uint32_t Read();
                virtual void Write(albaos::common::uint32_t data);

            protected:
                static inline albaos::common::uint32_t Read32(albaos::common::uint16_t _port)
                {
                    albaos::common::uint32_t result;
                    __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (_port));
                    return result;
                }

                static inline void Write32(albaos::common::uint16_t _port, albaos::common::uint32_t _data)
                {
                    __asm__ volatile("outl %0, %1" : : "a"(_data), "Nd" (_port));
                }
        };

    }
}


#endif
