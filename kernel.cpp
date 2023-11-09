#include "types.h"
#include "gdt.h"

void oldprint(char* str)
{
    unsigned short* VideoMemoryLocation = (unsigned short*)0xb8000;
    for(int i = 0; str[i] != '\0'; i++)
        VideoMemoryLocation[i] = (VideoMemoryLocation[i] & 0xFF00) | str[i];
}

void printf(char* str)
{
    static uint16_t* VideoMemoryLocation = (uint16_t*)0xb8000;

    static uint8_t x=0,y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemoryLocation[80*y+x] = (VideoMemoryLocation[80*y+x] & 0xFF00) | str[i];
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
                    VideoMemoryLocation[80*y+x] = (VideoMemoryLocation[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}


typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors()
{
    for (constructor* i = &start_ctors; i != &end_ctors; i++){
        (*i)();
    }
}
extern "C" void kernelMain(void* multiboot_structure, unsigned int magicnumber)
{
    printf("                   OOO  OO OOOOO\n");
    printf("           OOO O               O\n");
    printf("        OOO                   OOOOOOO\n");
    printf("       OO                           OOOOO\n");
    printf("      O                                OOO\n");
    printf("    OO                                   OOO\n");
    printf("   O                                      OOO\n");
    printf("  O       OOOO                             OOO\n");
    printf(" O       O   OO                             OOO\n");
    printf("OO      OOOOOOO                              OO\n");
    printf("O       OOOOOOO             O OOOOO          OOO\n");
    printf("O       OOOO O             OOO    OOO OO      OOO\n");
    printf("O         OOO             OO   OOOOOOOOO       OO\n");
    printf("O                         OOOOOOOOOOO          OOO\n");
    printf("O                        OOOOOOO OO           OO\n");
    printf("O                     OO                      OO\n");
    printf("OO            O     OO                       OO\n");
    printf("OO          O OO O O                        OO\n");
    printf(" OOO        O   OO                        OOO\n");
    printf("  OO        O OOO                        OOO\n");
    printf("   OO        OO                        OOO\n");
    printf("    OOO                             OOOOO\n");
    printf("      OO                       OOOOOOO\n");
    printf("       OOO               OOOOOOOOO\n");
    printf("         OOOOOOOOOOOOOOOOOO\n");
    printf("                   Test--------------------\n");
    printf("-------------------AlbaOS--------------------");

    GlobalDescriptorTable gdt;
    while(1);
}












