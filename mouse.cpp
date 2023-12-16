#include "mouse.h"

MouseDriver::MouseDriver(InterruptManager* manager)
: InterruptHandler(manager, 0x2C),
dataport(0x60),
commandport(0x64)
{
    // if mouse i strange change offset (0,1,2)
    offset = 0;
    buttons = 0;

    uint16_t* VideoMemory = (uint16_t*)0xb8000;
    VideoMemory[80*12+40] = ((VideoMemory[80*12+40] &0xF000) >> 4)
                    | ((VideoMemory[80*12+40] &0x0F00) << 4)
                    | (VideoMemory[80*12+40] &0x00FF);

    commandport.Write(0xA8); // start interrupts
    commandport.Write(0x20); // read current state of pic
    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60); // set state (controller byte)
    dataport.Write(status);
    commandport.Write(0xD4);
    dataport.Write(0xF4);
    dataport.Read();
}

MouseDriver::~MouseDriver()
{
}

void printf(char*);

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    if(!(status & 0x20))
        return esp;

    //mouse starts at center of screen
    static int8_t x=40,y=12;

    buffer[offset] = dataport.Read();
    offset = (offset + 1) % 3;

    if(offset == 0)
    {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        //inversing the colours where the mouse is before moving it
        VideoMemory[80*y+x] = ((VideoMemory[80*y+x] &0xF000) >> 4)
                            | ((VideoMemory[80*y+x] &0x0F00) << 4)
                            | (VideoMemory[80*y+x] &0x00FF);

        // buffer 1 x , buffer 2 y
        x += buffer[1];
        //so it doesnt go of the screen and do strange things
        if(x < 0) x = 0;
        if(x >= 80) x = 79;

        y -= buffer[2];
        if(y < 0) y = 0;
        if(y >= 25) y = 24;

        //inversing the colours where the mouse is by moving the first bit is to where the 2nd bit is and vice versa
        VideoMemory[80*y+x] = ((VideoMemory[80*y+x] &0xF000) >> 4)
                            | ((VideoMemory[80*y+x] &0x0F00) << 4)
                            | (VideoMemory[80*y+x] &0x00FF);

        for(uint8_t i = 0; i < 3; i++)
        {
            if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
            {
                VideoMemory[80*y+x] = ((VideoMemory[80*y+x] &0xF000) >> 4)
                                    | ((VideoMemory[80*y+x] &0x0F00) << 4)
                                    | (VideoMemory[80*y+x] &0x00FF);
            }
        }
        buttons = buffer[0];


    }

    return esp;
}
