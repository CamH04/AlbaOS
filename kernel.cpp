#include "types.h"
#include "gdt.h"

void justincasethingsbreak(char* str)
{
    unsigned short* VideoMemoryLocation = (unsigned short*)0xb8000;
    for(int i = 0; str[i] != '\0'; i++)
        VideoMemoryLocation[i] = (VideoMemoryLocation[i] & 0xFF00) | str[i];
}
void printf(char* str) {

	static uint8_t x = 0, y = 0;
	static bool cliCursor = false;

	//default gray on black text
	uint16_t attrib = 0x07;

	volatile uint16_t* vidmem;


	for (int i = 0; str[i] != '\0'; i++) {

		vidmem = (volatile uint16_t*)0xb8000 + (80*y+x);

		switch (str[i]) {

			case '\b':
				vidmem = (volatile uint16_t*)0xb8000 + (80*y+x);
				*vidmem = ' ' | (attrib << 8);
				vidmem--; *vidmem = '_' | (attrib << 8);
				x--;

				break;

			case '\n':
				*vidmem = ' ' | (attrib << 8);
				y++;
				x = 0;

				break;

			case '\t': //$: shell interface

				if (!i) {
					cliCursor = true;

					if (x < 3) {

						x = 3;
					}

					vidmem = (volatile uint16_t*)0xb8000 + (80*y);
					*vidmem = '$' | 0xc00;
					vidmem++; *vidmem = ':' | 0xf00;
					vidmem++; *vidmem = ' ';

				} else {
					*vidmem = '_' | (attrib << 8);
				}
				break;

			case '\v': //clear screen

				for (y = 0; y < 25; y++) {
					for (x = 0; x < 80; x++) {

						vidmem = (volatile uint16_t*)0xb8000 + (80*y+x);
						*vidmem = 0x00;
					}
				}
				x = 0;
				y = 0;

				break;


			default:
				*vidmem = str[i] | (attrib << 8);
				x++;

				break;
		}

		if (x >= 80) {

			y++;
			x = 0;
		}


		//scrolling
		if (y >= 25) {

			uint16_t scroll_temp;

			for (y = 1; y < 25; y++) {
				for (x = 0; x < 80; x++) {

					vidmem = (volatile uint16_t*)0xb8000 + (80*y+x);
					scroll_temp = *vidmem;

					vidmem -= 80;
					*vidmem = scroll_temp;

					if (y == 24) {

						vidmem = (volatile uint16_t*)0xb8000 + (1920+x);
						*vidmem = ' ' | (attrib << 8);
					}
				}
			}
			x = 0;
			y = 24;
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
    printf("                                   ###  ## #####   ");
    printf("                             ### #               #  ");
    printf("                          ###                   ####### ");
    printf("                         ##                           ##### ");
    printf("                       .#                                ###. ");
    printf("                      ##                                   ### ");
    printf("                     #                                      ### ");
    printf("                    #    . .####                 #           ### ");
    printf("                   #      .#   ##             #  .            ###");
    printf("                  ##      #######                              ##");
    printf("                  #       #######             # #####   #      ### ");
    printf("                  #       #### #             ###    ### ### #   ###");
    printf("                  #         ###             ##   ########## .#.  ##");
    printf("                  #                         ###########          ### ");
    printf("                  #                        ####### ##           ## ");
    printf("                  #                     ##       ##             ## ");
    printf("                  ##            #     ##                       ## ");
    printf("                  ##          # ## # #                        ##");
    printf("                   ###        #  .##                        ### ");
    printf("                    ##        # ###                        ### ");
    printf("                     ##        ##                        ###  ");
    printf("                      ###                             ##### ");
    printf("                        ##                       ####### ");
    printf("                         ###               ######### ");
    printf("                           ################## ");
    printf("");
    printf("");

    GlobalDescriptorTable gdt;
    while(1);
}












