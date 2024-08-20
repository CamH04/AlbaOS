#include <common/asl.h>
#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <owlart.h>
#include <playstart.h>
#include <syscalls.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/amd_am79c973.h>
#include <drivers/ata.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/audio.h>
#include <drivers/vga.h>
#include <filesys/ofs.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>
#include <cli.h>
#include <app.h>
#include <drivers/pit.h>
#include <nests/filenest.h>
#include <gui/sim.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;
using namespace albaos::filesystem;
using namespace albaos::gui;

asl ASL;

//TODO add this stuff to the ASL
//=======================================================================================================================================================================================

static uint8_t x = 0, y = 0;

void printf(char* str) {

    static bool cliCursor = false;

    uint16_t attrib = ASL.SetTextColor(false);
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
                    *VideoMemory = '>' | 0xd00;
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
        //scrolling effect
        if (y >= 25) {

            uint16_t scroll_temp;

            for (y = 1; y < 25; y++) {
                for (x = 0; x < 80; x++) {

                    VideoMemory = (volatile uint16_t*)0xb8000 + (80*y+x);
                    scroll_temp = *VideoMemory;

                    VideoMemory -= 80;
                    *VideoMemory = scroll_temp;

                    if (y == 24) {

                        VideoMemory = (volatile uint16_t*)0xb8000 + (1920+x);
                        *VideoMemory = ' ' | (attrib << 8);
                    }
                }
            }
            x = 0;
            y = 24;
        }
    }
}

void printc(char c){
    uint16_t Ccolour = ASL.SetTextColor(false);
    volatile uint16_t* CVideoMemory;
    CVideoMemory = (volatile uint16_t *)0xb8000 + (y * 80 + x) ;
    *CVideoMemory = c | (Ccolour << 8);
}

void printfhere(const char* str, uint8_t line) {

    for (uint16_t i = 0; str[i] != '\0'; i++) {

        volatile uint16_t* VideoMemory = (volatile uint16_t*)0xb8000 + (80*line+i);
        *VideoMemory = str[i] | 0x700;
    }
}

// UI  Prints =============================================
void putcharTUI(unsigned char ch, unsigned char forecolor,
        unsigned char backcolor, uint8_t x, uint8_t y) {

    uint16_t attrib = (backcolor << 4) | (forecolor & 0x0f);
    volatile uint16_t* vidmem;
    vidmem = (volatile uint16_t*)0xb8000 + (80*y+x);
    *vidmem = ch | (attrib << 8);
}

void TUI(uint8_t forecolor, uint8_t backcolor,
        uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
        bool shadow) {

    for (uint8_t y = 0; y < 25; y++) {

        for (uint8_t x = 0; x < 80; x++) {

            putcharTUI(0xff, 0x00, backcolor, x, y);
        }
    }

    uint8_t resetX = x1;

    while (y1 < y2) {

        while (x1 < x2) {

            putcharTUI(0xff, 0x00, forecolor, x1, y1);
            x1++;
        }
        y1++;

        //side shadow
        if (shadow) {

            putcharTUI(0xff, 0x00, 0x00, x1, y1);
        }
        x1 = resetX;
    }

    //bottom shadow
    if (shadow) {

        for (resetX++; resetX < (x2 + 1); resetX++) {

            putcharTUI(0xff, 0x00, 0x00, resetX, y1);
        }
    }
}

void printfTUI(char* str, uint8_t forecolor, uint8_t backcolor, uint8_t x, uint8_t y) {

    for (int i = 0; str[i] != '\0'; i++) {

        if (str[i] == '\n') {

            y++;
            x = 0;
        } else {
            putcharTUI(str[i], forecolor, backcolor, x, y);
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

//==========================
void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}

//string manip functions
uint16_t strlen(char* args) {

        uint16_t length = 0;

        for (length = 0; args[length] != '\0'; length++) {

        }
        return length;
}

uint32_t StringToInt(char* args){
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


char* IntToString(uint32_t num) {

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

void printfHex16(uint16_t key)
{
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}
void printfHex32(uint32_t key)
{
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}


void memWrite(uint32_t memory, uint32_t inputVal) {

	volatile uint32_t* value;
	value = (volatile uint32_t*)memory;
	*value = inputVal;
}

uint32_t memRead(uint32_t memory) {

	volatile uint32_t* value;
	value = (volatile uint32_t*)memory;

	return *value;
}



//for cool chars :)
void AltCharCode(uint8_t c, uint8_t &NumCharCode) {

    static uint8_t count = 0;
    bool bitShift = (count % 2 == 0);
    count++;

    if (c <= '9' && c >= '0') {

        NumCharCode += (c - '0');
    }

    if (c <= 'f' && c >= 'a') {

        NumCharCode += (c - 'a') + 10;
    }

    NumCharCode <<= (4 * bitShift);
}
//=======================================================================================================================================================================================

bool EnterGUI = false;
//"put Keybaord in command line pls" the class
class CLIKeyboardEventHandler : public KeyboardEventHandler, public CommandLine {
public:
    bool pressed;

public:
    CLIKeyboardEventHandler(GlobalDescriptorTable* gdt, TaskManager* tm, AdvancedTechnologyAttachment* ata0m): CommandLine(gdt, tm, ata0m) {

        this->cli = true;
    }

    void nestSelect(uint16_t nest, bool pressed, unsigned char ch,
            bool ctrl, bool type) {

        switch (this->cliMode) {
            //file edit program
            case 1:
                if (type) {fileMain(pressed, ch, ctrl);}
                break;
            default:
                break;
        }
    }

        void OnKeyDown(char c) {

            this->pressed = true;
            this->keyChar = c;
            if (this->ctrl) {
				switch (c) {
					case 'c':
                        // cli
						this->resetMode();
						return; break;
					case 'e':
                        // edit file
						this->cliMode = 1;
						nestSet(this->cliMode);
						break;
                    case 'g':
                        EnterGUI = true;
                        break;
					default:
						break;
				}
			}

            //the whole ass CLI
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
				this->nestSelect(this->cliMode, this->pressed, this->keyChar, this->ctrl, 1);

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

        void nestSet(uint8_t nest) {

			//reset nest before entering next one
			this->resetMode();
			this->cliMode = nest;

			switch (this->cliMode) {
				case 1:
					//file editor
					fileTUI();
					fileMain(0, 'c', 1);
					break;
				default:
					printf("Mode not found.\n");
					break;
			}
		}
		void resetMode() {
			printf("\v");
			switch (this->cliMode) {
				case 1:
                    printf("\v");
                    fileMain(0, 'c', 1);
					printf("Exiting file edit nest\n");
					break;
				default:
                    printf("\v");
					break;
			}
			this->cliMode = 0;
            printf("\v");
		}


};

/*
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
*/

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


    return "how";
}

uint8_t argcount(char* args) {

    uint8_t i = 0;
    char* foo = argparse(args, i);

    while (foo != "how") {

        foo = argparse(args, i);
        i++;
    }

    return i-1;
}


void initnetwork(char* string){

    DriverManager drvManager;
    amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);
    eth0->Send((uint8_t*)string, strlen(string));
}

Desktop* LoadDesktopForTask(bool set, Desktop* desktop = 0) {

	static Desktop* retDesktop = 0;

	if (set) {
		retDesktop = desktop;
	}

	return retDesktop;
}

void reboot() {

	asm volatile ("cli");

	uint8_t read = 0x02;
	Port8Bit resetPort(0x64);

	while (read & 0x02) {

		read = resetPort.Read();
	}

	resetPort.Write(0xfe);
	asm volatile ("hlt");
}

void DrawDesktopTask() {

	Desktop* desktop = LoadDesktopForTask(false);
	desktop->gc->SetMode(320, 200, 8);

	while (1) {

		desktop->Draw(desktop->gc);
	}
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

    GlobalDescriptorTable* gdt;
    TaskManager taskManager;
    //old multitasking debugging stuff
    //Task taskexample(&gdt, functionForTask);
    InterruptManager interrupts(0x20, gdt, &taskManager);
    SyscallHandler syscalls(&interrupts, 0x80);

    printf("Hardware init, Stage 1\n");

    DriverManager drvManager;
    AdvancedTechnologyAttachment ata0m(0x1F0, true);
	CLIKeyboardEventHandler kbhandler(gdt, &taskManager, &ata0m);
	KeyboardDriver keyboard(&interrupts, &kbhandler);
    drvManager.AddDriver(&keyboard);


     VideoGraphicsArray vga;
	Simulator alba;
	Desktop desktop(320, 200, 0x01, &vga, gdt, &taskManager, &alba);
	MouseDriver mouse(&interrupts, &desktop);

	drvManager.AddDriver(&mouse);

        //i forgot to add this here and wondered why it wasnt working, the blight of man
        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);

        //activating drivers
        printf("Hardware init, Stage 2\n");
        drvManager.ActivateAll();

    //please dont spurt out 1billion GPF errors
    printf("Hardware init, Stage 3\n");

    interrupts.Activate();
    printf("Welcome To AlbaOS!");
    printf("\n  ");
    printf("\v");

	interrupts.boot = true;
    //the user stuff from here -------------------------------------------
    owlart OA;
    OA.MenuHello();

    playstart PS;
    PS.singasong();
    while (kbhandler.pressed == false) {
        //hi the main menu is being printed rn
    }
    printf("\v");
    //initialize command line hash table
    printf("Welcome to the AlbaOS Command Line! (ACL) \n");
	printf("use the help command (  help  ) if you need assistance ^v^\n");
    printf("press tab or use the clear command (  clear  ) to clear terminal @v@\n");
    kbhandler.cli = true;
	kbhandler.hash_cli_init();
    kbhandler.OnKeyDown('\b');
	while (EnterGUI == false) {

		kbhandler.cli = true;

		while (kbhandler.cliMode) {

			kbhandler.cli = false;
		}
	}


    KeyboardDriver keyboardDesktop(&interrupts, &desktop);
	drvManager.Replace(&keyboardDesktop, 0);


    //now in graphics mode
    vga.SetMode(320, 200, 8);

    /*
	Window win1(&desktop, 10, 10, 40, 20, "1", 0x04, &kbhandler);
	desktop.AddChild(&win1);

	Window win2(&desktop, 100, 15, 50, 30, "2", 0x19, &kbhandler);
	desktop.AddChild(&win2);

	Window win3(&desktop, 60, 45, 80, 65, "3", 0x32, &kbhandler);
	desktop.AddChild(&win3);
    */

    while(keyboard.keyHex != 0x2E)
    {
        desktop.Draw(&vga);
        //sleep(15);
    }
    //TODO return to cli / reboot
}










