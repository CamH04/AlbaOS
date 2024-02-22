#ifndef __ALBAOS__DRIVERS__VGA_H
#define __ALBAOS__DRIVERS__VGA_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>

namespace albaos
{
    namespace drivers
    {

        class VideoGraphicsArray
        {
        protected:
            hardwarecommunication::Port8Bit miscPort;
            hardwarecommunication::Port8Bit crtcIndexPort;
            hardwarecommunication::Port8Bit crtcDataPort;
            hardwarecommunication::Port8Bit sequencerIndexPort;
            hardwarecommunication::Port8Bit sequencerDataPort;
            hardwarecommunication::Port8Bit graphicsControllerIndexPort;
            hardwarecommunication::Port8Bit graphicsControllerDataPort;
            hardwarecommunication::Port8Bit attributeControllerIndexPort;
            hardwarecommunication::Port8Bit attributeControllerReadPort;
            hardwarecommunication::Port8Bit attributeControllerWritePort;
            hardwarecommunication::Port8Bit attributeControllerResetPort;

            void WriteRegisters(common::uint8_t* registers);
            common::uint8_t* GetFrameBufferSegment();

            virtual common::uint8_t GetColorIndex(common::uint8_t r, common::uint8_t g, common::uint8_t b);


        public:
            VideoGraphicsArray();
            ~VideoGraphicsArray();

            virtual bool SupportsMode(common::uint32_t width, common::uint32_t height, common::uint32_t colordepth);
            virtual bool SetMode(common::uint32_t width, common::uint32_t height, common::uint32_t colordepth);
            virtual void PutPixel(common::uint32_t x, common::uint32_t y,  common::uint8_t r, common::uint8_t g, common::uint8_t b);
            virtual void PutPixel(common::uint32_t x, common::uint32_t y, common::uint8_t colorIndex);
        };

    }
}

#endif
