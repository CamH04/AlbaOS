#include <common/types.h>
#include <gdt.h>
#include <owlart.h>
#include <playstart.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/audio.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>
#include <drivers/pit.h>


// to enable the GUI uncomment this
// #define GRAPHICSMODE

#define MODULUS    2147483647
#define MULTIPLIER 48271
#define CHECK      399268537
#define STREAMS    256
#define A256       22925

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;
using namespace albaos::gui;

void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0;
    static uint8_t y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

/*
BLACK 0
BLUE 1
GREEN 2
CYAN 3
RED 4
MAGENTA 5
ORANGE 6
WHITE 7
GRAY 8
BLUELIGHT 9
GREENLIGHT 10
CYANLIGHT 11
PINK 12
MAGENTALIGHT 13
YELLOW 14
WHITELIGHT 15
*/
uint32_t curx=0;
uint32_t curc=0;
void console_new_line()
{
   char *video=(char*)0xB8000;
   uint32_t x,i;
   char *m=" ";
   x = 80 - curx;
   for(i=0;i<x;i++)
   {
      *video=*m;
      video++;
      *video=0;
      video++;
      curc=curc+2;
      curx++;
   }
   curx=0;
}
void cprintf(uint32_t colour, char *string)
{
    uint32_t curc=0;

    char *video=(char*)0xB8000;

    video = video + curc;
    while(*string!=0)
    {
        *video=*string;
        string++;
        video++;
        *video=colour;
        video++;
        curc=curc+2;
        if(curx==80)
        {
         console_new_line();
        }
    }
}

void console_cls()
{
   char *video=(char*)0xB8000;
   char *x=" ";
   int i;


   for(i=0;i<2000;i++)
   {
      *video=*x;
      video++;
      *video=0;
      video++;
   }
}


void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}




class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(char c)
    {
        char* foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler
{
    uint8_t x, y;
public:
    MouseToConsole()
    {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);
    }

    virtual void OnMouseMove(int xoffset, int yoffset)
    {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);

        x += xoffset;
        if(x >= 80) x = 79;
        if(x < 0) x = 0;
        y += yoffset;
        if(y >= 25) y = 24;
        if(y < 0) y = 0;

        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);
    }

};


//sleeps zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
void sleep(uint32_t ms) {

	//like arduino (ms) zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
	PIT pit;

	for (uint32_t i = 0; i < ms; i++) {

		pit.setCount(1193182/1000);
		uint32_t start = pit.readCount();

		while ((start - pit.readCount()) < 1000) {}
	}
}

//RANDOM NUMBERS
//god help me random numbers are somthing else
//uses Lehmer random number generation
//Steve Park & Dave Geyer are legends btw read their stuff

double Random(void) // betwwen 1 and 0
{
    PIT pit;
    static long seed[STREAMS] = {(uint16_t)pit.readCount()};
    static int  stream        = 0;
    const long Q = MODULUS / MULTIPLIER;
    const long R = MODULUS % MULTIPLIER;
            long t;

    t = MULTIPLIER * (seed[stream] % Q) - R * (seed[stream] / Q);
    if (t > 0)
        seed[stream] = t;
    else
        seed[stream] = t + MODULUS;
    return ((double) seed[stream] / MODULUS);
}


typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}


extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{

    GlobalDescriptorTable gdt;
    TaskManager taskManager;
    //old multitasking debugging stuff
    //Task taskexample(&gdt, functionForTask);
    InterruptManager interrupts(0x20, &gdt, &taskManager);

    printf("Hardware init, Stage 1\n");
    #ifdef GRAPHICSMODE
        Desktop desktop(320,200, 0x00,0x00,0xA8);
    #endif

    DriverManager drvManager;

        #ifdef GRAPHICSMODE
            KeyboardDriver keyboard(&interrupts, &desktop);
        #else
            PrintfKeyboardEventHandler kbhandler;
            KeyboardDriver keyboard(&interrupts, &kbhandler);
        #endif
        drvManager.AddDriver(&keyboard);


        #ifdef GRAPHICSMODE
            MouseDriver mouse(&interrupts, &desktop);
        #else
            MouseToConsole mousehandler;
            MouseDriver mouse(&interrupts, &mousehandler);
        #endif
        drvManager.AddDriver(&mouse);

        //i forgot to add this here and wondered why it wasnt working, the blight of man
        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);

        VideoGraphicsArray vga;

        //activating drivers
        printf("Hardware init, Stage 2\n");
        drvManager.ActivateAll();

    //please dont spurt out 1billion GPF errors
    printf("Hardware init, Stage 3\n");

    #ifdef GRAPHICSMODE
        vga.SetMode(320,200,8);
        Window win1(&desktop, 10,10,20,20, 0xA8,0x00,0x00);
        desktop.AddChild(&win1);
        Window win2(&desktop, 40,15,30,30, 0x00,0xA8,0x00);
        desktop.AddChild(&win2);
    #endif

    interrupts.Activate();
    //art stuff
    owlart OA;
    OA.OwlArtLove();

    printf("Welcome To AlbaOS Version Beta 0.94");
    printf("\n");
    console_new_line();
    cprintf(13,"$>");




    playstart PS;
    PS.singasong();


    while(1)
    {
        //desktop gui RIP
        #ifdef GRAPHICSMODE
            desktop.Draw(&vga);
        #endif
    }
}










