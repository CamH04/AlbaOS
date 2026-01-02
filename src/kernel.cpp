#include <common/asl.h>
#include <common/asl_maths.h>
#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <owlart.h>
#include <playstart.h>
#include <syscalls.h>
#include <hardwarecommunication/power.h>
#include <hardwarecommunication/acpi.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/ata.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/audio.h>
#include <drivers/virtual_keyboard.h>
#include <drivers/vga.h>
#include <filesys/ofs.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>
#include <cli.h>
#include <app.h>
#include <drivers/pit.h>
#include <nests/filenest.h>
#include <nests/snake.h>
#include <nests/raycastdemo.h>
#include <nests/terminalgame.h>
#include <gui/sim.h>
//31st jan 2025 8:08pm : networking stuff im starting this now
#include <drivers/amd_am79c973.h>
#include <networking/eframe.h>// 4 days later: help me
#include <networking/arp.h> // im back 9th july 25
#include <nests/meatball.h>
#include <nests/pipe.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;
using namespace albaos::filesystem;
using namespace albaos::networking;
using namespace albaos::gui;

asl ASL;
asl_maths ASLMATHS;

void printf(char* str) {
    static uint8_t x = 0, y = 0;
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



bool EnterGUI = false;
namespace albaos {
namespace drivers {
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
            case 1:
                if (type) {fileMain(pressed, ch, ctrl);}
                break;
            case 2:
					snake(ch);
					break;
            case 3:
                    //doesnt work rn
                    //password();
                    break;
            case 4:
                    metaballs(ch);
                    break;
            case 5:
                    pipes(ch);
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
                    case 's':
                        // snake
						this->cliMode = 2;
						nestSet(this->cliMode);
						break;
                    case 'g':
                        EnterGUI = true;
                        break;
                    case 'r':
                        this->cliMode = 3;
                        nestSet(this->cliMode);
                        break;
                    case 'm':
                        this->cliMode = 4;
                        nestSet(this->cliMode);
                        break;
                    case 'p':
                        this->cliMode = 5;
                        nestSet(this->cliMode);
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
                case 2:
					//snake game
					snake('r');
					snakeTUI();
					snakeInit();
					break;
                case 3:
                    //raycast here
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
					printf("\nExiting file edit nest\n\n");
					break;
                case 2:
					printf("\nExiting snake nest \n\n");
					break;
                case 3:
                    printf("\nExiting Raycast nest \n\n");
					break;
				default:
                    printf("\v");
					break;
			}
			this->cliMode = 0;
            printf("\v");
		}


};
}
}
Desktop* LoadDesktopForTask(bool set, Desktop* desktop = 0) {

	static Desktop* retDesktop = 0;

	if (set) {
		retDesktop = desktop;
	}

	return retDesktop;
}
void DrawDesktopTask() {

	Desktop* desktop = LoadDesktopForTask(false);
	desktop->gc->SetMode(320, 200, 8);

	while (1) {

		desktop->Draw(desktop->gc);
	}
}





//Global Getters
static albaos::drivers::VirtualKeyboard* g_vkeyboard = nullptr;
VirtualKeyboard& GetVirtualKeyboard() {
    return *g_vkeyboard;
}



typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}


extern "C" void kernelMain(const void* multiboot_structure, uint32_t multiboot_magic)
{
    GlobalDescriptorTable* gdt;

    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);

    printf("heap: 0x");
    ASL.printfHex((heap >> 24) & 0xFF);
    ASL.printfHex((heap >> 16) & 0xFF);
    ASL.printfHex((heap >> 8 ) & 0xFF);
    ASL.printfHex((heap      ) & 0xFF);

    void* allocated = memoryManager.malloc(1024);
    printf("\nallocated: 0x");
    ASL.printfHex(((size_t)allocated >> 24) & 0xFF);
    ASL.printfHex(((size_t)allocated >> 16) & 0xFF);
    ASL.printfHex(((size_t)allocated >> 8 ) & 0xFF);
    ASL.printfHex(((size_t)allocated      ) & 0xFF);
    printf("\n");

    TaskManager taskManager;
    //Task taskexample(&gdt, functionForTask);
    InterruptManager interrupts(0x20, gdt, &taskManager);
    SyscallHandler syscalls(&interrupts, 0x80);
    printf("Hardware init, Stage 1\n");
    DriverManager drvManager;
    AdvancedTechnologyAttachment ata0m(0x1F0, true);

	CLIKeyboardEventHandler kbhandler(gdt, &taskManager, &ata0m);
    printf("Init Virtual Keyboard\n");
    albaos::drivers::VirtualKeyboard vkeyboard(&kbhandler);
    printf("Virtual Keyboard Init Complete\n");
    g_vkeyboard = &vkeyboard;

	KeyboardDriver keyboard(&interrupts, &kbhandler);
    drvManager.AddDriver(&keyboard);

    VideoGraphicsArray vga;
	Simulator alba;
	Desktop desktop(320, 200, 0x01, &vga, gdt, &taskManager, &alba);
	MouseDriver mouse(&interrupts, &desktop);

	drvManager.AddDriver(&mouse);

        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);

    ASL.benchmark();
        printf("Hardware init, Stage 2\n");
    drvManager.ActivateAll();
    ASL.benchmark();


    printf("Hardware init, Stage 3\n");
    interrupts.Activate();
    printf("ALBA KERNEL : interrupts activated\n");
	interrupts.boot = true;


    printf("Network init, Stage 4\n");
        amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);
        printf("ALBA KERNEL : eth0 init\n");

        EtherFrameProvider etherframe(eth0);
        printf("ALBA KERNEL : ethframe obj init\n");

        //etherframe.Send(0xFFFFFFFFFFFF, 0x0608, (uint8_t*)"FOO", 3);
        printf("ALBA KERNEL : ethframe sent\n");

    printf("ACPI POWER INIT!\n");
    acpi ACPI;
    ACPI.initAcpi();
    ACPI.acpiEnable();
    printf("APM POWER INIT\n");
    apm APM;
    APM.init();;

    uint8_t ip1 = 10, ip2 = 0, ip3 = 2, ip4 = 15;

    uint32_t ip_be = ((uint32_t)ip4 << 24)
                    | ((uint32_t)ip3 << 16)
                    | ((uint32_t)ip2 << 8)
                    | (uint32_t)ip1;

    uint8_t gip1 = 10, gip2 = 0, gip3 = 2, gip4 = 2;
    uint32_t gip_be = ((uint32_t)gip4 << 24)
                    | ((uint32_t)gip3 << 16)
                    | ((uint32_t)gip2 << 8)
                    | (uint32_t)gip1;

    printf("ip_be =");
    ASL.printfHex(ip_be);
    printf("\n");
    /*
    eth0->SetIPAddress(ip_be);
    printf("ip address set\n");
    AddressResolutionProtocol arp(&etherframe);

    arp.Resolve(gip_be);
    printf("ARP Resolve Successful");

    printf("\nWelcome To AlbaOS!");
    printf("\n");
    printf("\n\nPress Any Key To Continue ...");
    */




    //the user stuff from here -------------------------------------------
    printf("Press Any Key To Continue\n");
    owlart OA;
    //OA.MenuHello();

    playstart PS;
    PS.singasong();
    while (kbhandler.pressed == false) {
        //hi the main menu is being printed rn
    }
    printf("\v");
    printf("Welcome to the AlbaOS Command Line! (ACL) \n");
	printf("use the help command (  help  ) if you need assistance ^v^\n");
    printf("press tab to clear terminal @v@\n");
    kbhandler.cli = true;
	kbhandler.hash_cli_init();
    kbhandler.OnKeyDown('\b');
	while (EnterGUI == false) {

		kbhandler.cli = true;

		while (kbhandler.cliMode) {

			kbhandler.cli = false;
            kbhandler.nestSelect(kbhandler.cliMode, kbhandler.pressed,kbhandler.keyChar, kbhandler.ctrl, 0);
		}
	}
    vga.SetMode(320, 200, 8);
    KeyboardDriver keyboardDesktop(&interrupts, &desktop);
	drvManager.Replace(&keyboardDesktop, 0);
    while(keyboard.keyHex != 0x2E)
    {
        desktop.Draw(&vga);
    }
}
