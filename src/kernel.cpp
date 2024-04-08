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
#include <cli.h>
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

uint16_t SetTextColor(bool set, uint16_t color = 0x07) {

    static uint16_t newColor = 0x07;

    if (set) {
        newColor = color;
    }

    return newColor;
}

void printf(char* str) {

    static uint8_t x = 0, y = 0;
    static bool cliCursor = false;

    uint16_t attrib = SetTextColor(false);
    volatile uint16_t* VideoMemory;


    uint8_t* strWhole = (uint8_t*)strWhole;


    for (int i = 0; str[i] != '\0'; i++) {

        VideoMemory = (volatile uint16_t*)0xb8000 + (80*y+x);

        switch (str[i]) {

            case '\b':
                VideoMemory = (volatile uint16_t*)0xb8000 + (80*y+x);
                *VideoMemory = ' ' | (attrib << 8);
                VideoMemory--; *VideoMemory = '_' | (attrib << 8);
                x--;
                break;

            case '\n':
                *VideoMemory = ' ' | (attrib << 8);
                y++;
                x = 0;
                break;
            //command line stuff
            case '\t':

                if (!i) {
                    cliCursor = true;

                    if (x < 3) { x = 3; }

                    VideoMemory = (volatile uint16_t*)0xb8000 + (80*y);
                    *VideoMemory = '>' | 0xc00;
                    VideoMemory++; *VideoMemory = ':' | 0xf00;
                    VideoMemory++; *VideoMemory = ' ';
                } else {
                    *VideoMemory = '_' | (attrib << 8);
                }
                break;
            //clear screen
            case '\v':

                for (y = 0; y < 25; y++) {
                    for (x = 0; x < 80; x++) {

                        VideoMemory = (volatile uint16_t*)0xb8000 + (80*y+x);
                        *VideoMemory = 0x00;
                    }
                }
                x = 0;
                y = 0;
                break;
            default:
                *VideoMemory = str[i] | (attrib << 8);
                x++;
                break;
        }

        if (x >= 80) {

            y++;
            x = 0;
        }
        //TODO add scrolling effect
    }
}


void printfhere(const char* str, uint8_t line) {

    for (uint16_t i = 0; str[i] != '\0'; i++) {

        volatile uint16_t* VideoMemory = (volatile uint16_t*)0xb8000 + (80*line+i);
        *VideoMemory = str[i] | 0x700;
    }
}

/*
BLACK 0x00
BLUE 0x01
GREEN 0x02
CYAN 0x03
RED 0x04
MAGENTA 0x05
ORANGE 0x06
WHITE 0x07
GRAY 0x08
BLUELIGHT 0x09
GREENLIGHT 0x0A
CYANLIGHT 0x0B
PINK 0x0C
MAGENTALIGHT 0x0D
YELLOW 0x0E
WHITELIGHT 0x0F
*/
void putchar(unsigned char ch, unsigned char forecolor,
		unsigned char backcolor, uint8_t x, uint8_t y) {

    uint16_t attrib = (backcolor << 4) | (forecolor & 0x0f);
    volatile uint16_t* VideoMemory;
    VideoMemory = (volatile uint16_t*)0xb8000 + (80*y+x);
    *VideoMemory = ch | (attrib << 8);
}
void cprintf(char* str, uint8_t forecolor, uint8_t backcolor, uint8_t x, uint8_t y) {

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            y++;
            x = 0;

        } else {
            putchar(str[i], forecolor, backcolor, x, y);
            x++;
        }

        if (x >= 80) {
            y++;
            x = 0;
        }
        if (y >= 25) {
            y = 0;
        }
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

uint16_t strlen(char* args) {

        uint16_t length = 0;

        for (length = 0; args[length] != '\0'; length++) {

        }
        return length;
}


class CLIKeyboardEventHandler : public KeyboardEventHandler, public CommandLine {
public:

        uint8_t index = 0;
		char input[256];

		char keyChar;
		bool pressed;

		char lastCmd[256];


public:
        CLIKeyboardEventHandler(GlobalDescriptorTable* gdt,TaskManager* tm) {
            this->getTM(gdt, tm);
            this->cli = true;
        }

        void OnKeyDown(char c) {

            this->pressed = true;
            this->keyChar = c;

            if (this->cliMode == 0) {

                char* foo = " \t";
                foo[0] = c;

                switch (c) {

                    //history
                    case '\xff':
                        break;
                    case '\xfc':
                        break;
                    //up
                    case '\xfd':

                        if (index > 0) {

                            for (int i = 0; i < index; i++) {

                                input[index] = 0x00;
                                printf("\b");
                            }
                        }

                        for (index = 0; lastCmd[index] != '\0'; index++) {

                            input[index] = lastCmd[index];
                        }
                        input[index] = '\0';
                        printf(input);
                        break;
                    //down
                    case '\xfe':

                        for (int i = 0; i < index; i++) {

                            input[index] = 0x00;
                            printf("\b");
                        }
                        index = 0;
                        break;
                    case '\b':
                        if (index > 0) {

                            printf(foo);
                            index--;
                            input[index] = 0;
                        }
                        break;
                    case '\n':
                        printf(foo);
                        input[index] = '\0';
                        //DOYOSHITMYBROTHER (executes the command)
                        if (index > 0 && input[0] != ' ') {

                            for (int i = 0; input[i] != '\0'; i++) {
                                lastCmd[i] = input[i];
                            }
                            lastCmd[index] = '\0';

                            command(input, index);
                        }

                        input[0] = 0x00;
                        index = 0;
                        break;
                    //type
                    default:
                        printf(foo);
                        input[index] = c;
                        index++;
                        break;
                }
            } else {

                index = 0;
            }
        }


        void OnKeyUp() {

            this->pressed = false;
        }



        void resetCmd() {

            for (uint16_t i = 0; i < 256; i++) {

                input[i] = 0x00;
            }
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

char* argparse(char* args, uint8_t num) {

    char buffer[256];

    bool valid = false;
    uint8_t argIndex = 0;
    uint8_t bufferIndex = 0;


    for (int i = 0; i < (strlen(args) + 1); i++) {

        if (args[i] == ' ' || args[i] == '\0') {

            if (valid) {

                if (argIndex == num) {

                    buffer[bufferIndex] = '\0';
                    char* arg = buffer;
                    return arg;
                }
                argIndex++;
            }
            valid = false;

        } else {
            if (argIndex == num) {

                buffer[bufferIndex] = args[i];
                bufferIndex++;
            }
            valid = true;
        }
    }


    return "wtf";
}

uint8_t argcount(char* args) {

    uint8_t i = 0;
    char* foo = argparse(args, i);

    while (foo != "wtf") {

        foo = argparse(args, i);
        i++;
    }

    return i-1;
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
            //this was a paint in the ass because i kept calling the objs insted of the pointers to the objs lol
            CLIKeyboardEventHandler kbhandler(&gdt, &taskManager);
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
    printf("Welcome To AlbaOS Version Beta 0.94");
    printf("\n  ");
    printf("\v");


    //the user stuff from here -------------------------------------------
    owlart OA;
    OA.MenuHello();

    playstart PS;
    PS.singasong();
    while (kbhandler.pressed == false) {
    }
    printf("\v");
    //initialize command line hash table
	kbhandler.cli = true;
	kbhandler.hash_cli_init();



	//this is the command line :D
	while (keyboard.keyHex != 0x5b) { //0x5b = command/windows key

		kbhandler.cli = true;

		while (kbhandler.cliMode) {

			kbhandler.cli = false;
		}
	}

    while(1)
    {
        //desktop gui RIP
        #ifdef GRAPHICSMODE
            desktop.Draw(&vga);
        #endif
    }
}










