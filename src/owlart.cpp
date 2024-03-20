#include <owlart.h>
#include <common/types.h>



using namespace albaos;
using namespace albaos::common;

void printf(char* str);

// primary use of this is to ovveride lines if need be
void owlart::printfline(char *str, uint8_t line) {
    uint16_t attrib = 0x07;

    volatile uint16_t* vidmem;
    for (int i = 0; str[i] != '\0'; i++) {
        vidmem = (volatile uint16_t*)0xb8000 + (80*line+i);
        *vidmem = str[i] | (attrib << 8);

    }
}
// eg printfline("help me god",0);
//    printfline("its too late for this",1);

void owlart::OwlArtStare()
{
    printf("(0,0)\n");
    printf("/)_)/\n");
    printf(" **  \n");

}

void owlart::OwlArtSleep()
{
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
    printf(" (0,0) \n");
    printf("==)_)==\n");
    printf("  **   \n");
}

void owlart::OwlArtGeekin()
{
    printf("(@,@)\n");
    printf("[(_)]\n");
    printf(" **  \n");
}

void owlart::OwlArtHappy()
{
    printf("(^,^)\n");
    printf("/)_)/\n");
    printf(" **  \n");
}

void owlart::OwlArtSad()
{
    printf("(v,v)\n");
    printf("/)_)/\n");
    printf(" **  \n");

}

void owlart::OwlArtLove()
{
    printf("       <3       \n");
    printf("  (^,^)  (^,^)  \n");
    printf("  /)_)/  /)_)/  \n");
    printf("---**-----**----\n");
}
