#include <owlart.h>
#include <common/types.h>

using namespace albaos;
using namespace albaos::common;

void printf(char* str);
void cprintf(char* str, uint8_t forecolor, uint8_t backcolor, uint8_t x, uint8_t y);
void putchar(unsigned char ch, unsigned char forecolor, unsigned char backcolor, uint8_t x, uint8_t y);
uint16_t SetTextColor(bool set, uint16_t color);


// primary use of this is to ovveride lines if need be
void owlart::printfhere(char *str, uint8_t line) {
    uint16_t attrib = 0x07;

    volatile uint16_t* vidmem;
    for (int i = 0; str[i] != '\0'; i++) {
        vidmem = (volatile uint16_t*)0xb8000 + (80*line+i);
        *vidmem = str[i] | (attrib << 8);

    }
}
// eg printfhere("help me god",0);
//    printfhere("its too late for this",1);
void owlart::MenuHello(){
    printfhere("                                                                                ",0);
    printfhere("                                                                                ",1);
    printfhere("                                  (^,^)  (^,^)                                  ",2);
    printfhere("                                  /)_)/  /)_)/                                  ",3);
    printfhere("                                ---**-----**----                                ",4);
    printfhere("                                                                                ",5);
    printfhere("================================================================================",6);
    printfhere("              Welcome To AlbaOS!      0v0      Welcome To AlbaOS!               ",7);
    printfhere("================================================================================",8);
    printfhere("                                                                                ",9);
    printfhere("                                                                                ",10);
    printfhere("                                                                                ",11);
    printfhere("                                                                                ",12);
    printfhere("                                                                                ",13);
    printfhere("                                                                                ",14);
    printfhere("                                                                                ",15);
    printfhere("                                                                                ",16);
    printfhere("                                                                                ",17);
    printfhere("                                                                                ",18);
    printfhere("                                                                                ",19);
    printfhere("                                                                                ",20);
    printfhere("                                                                                ",21);
    printfhere("                                                                                ",22);
    printfhere("                         Press Any Key To Get Started!                          ",23);
    printfhere("                                                                                ",24);
    //cprintfs have to go last
    cprintf("<3", 0x0C, 0x0C, 39, 1);


}
void owlart::OwlArtStare()
{
    printf("\n");
    printf("(0,0)\n");
    printf("/)_)/\n");
    printf(" **  \n");

}

void owlart::OwlArtSleep()
{
    printf("\n");
    printf("(-,-_)  \n");
    printf("(x-x(v)7\n");
}

void owlart::OwlArtHeadphones()
{
    printf("  _____       \n");
    printf(" / ___ \\     \n");
    printf("(|(0,0)|)     \n");
    printf("  /)_)/       \n");
    printf("   **         \n");

}

void owlart::OwlArtFly()
{
    printf("\n");
    printf(" (0,0) \n");
    printf("==)_)==\n");
    printf("  **   \n");
}

void owlart::OwlArtGeekin()
{
    printf("\n");
    printf("(@,@)\n");
    printf("[(_)]\n");
    printf(" **  \n");
}

void owlart::OwlArtHappy()
{
    printf("\n");
    printf("(^,^)\n");
    printf("/)_)/\n");
    printf(" **  \n");
}

void owlart::OwlArtSad()
{
    printf("\n");
    printf("(v,v)\n");
    printf("/)_)/\n");
    printf(" **  \n");

}

void owlart::OwlArtLove()
{
    printf("\n");
    cprintf("<3", 0x0C, 0x0C, 7, 0);
    printf("  (^,^)  (^,^)  \n");
    printf("  /)_)/  /)_)/  \n");
    printf("---**-----**----\n");
}

void owlart::OwlArtLoveCLIVER()
{
    printf("\n");
    printf("       <3       \n");
    printf("  (^,^)  (^,^)  \n");
    printf("  /)_)/  /)_)/  \n");
    printf("---**-----**----\n");
}

void owlart::pic(){
    int numOfAvalibeCol = 16;
    for (int j = 0; j < 80; j++){ //80 so we can get 16 rows
        for (int i = 0; i < numOfAvalibeCol; i++)
        {
            SetTextColor(true,i);
            printf("+");
        }
    }
    //back to white text
    SetTextColor(false,7);
    printf("+");
}

void owlart::seal1(){
        printf("\n");
        printf("   bannana!\n");
        printf("     / |        \n");
        printf("     | \\        \n");
        printf("     |  |     \n");
        printf("\\   | \\|\n");
        printf("| \\_/ \\ \\ \n");
        printf(" \\____/\\/ \n");

}