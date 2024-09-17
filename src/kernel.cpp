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
#include <nests/snake.h>
#include <gui/sim.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;
using namespace albaos::filesystem;
using namespace albaos::gui;

asl ASL;

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
            case 2:
					snake(ch);
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
                        // edit file
						this->cliMode = 2;
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
                case 2:
					//snake game
					snake('r');
					snakeTUI();
					snakeInit();
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
				default:
                    printf("\v");
					break;
			}
			this->cliMode = 0;
            printf("\v");
		}


};

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
    //now in graphics mode
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
            kbhandler.nestSelect(kbhandler.cliMode, kbhandler.pressed,kbhandler.keyChar, kbhandler.ctrl, 0);
		}
	}
	 vga.SetMode(320, 200, 8);
    KeyboardDriver keyboardDesktop(&interrupts, &desktop);
	drvManager.Replace(&keyboardDesktop, 0);
    while(keyboard.keyHex != 0x2E)
    {
        desktop.Draw(&vga);
        //sleep(15);
    }
    //TODO return to cli / reboot
}
