#include <owlart.h>



using namespace albaos;
using namespace albaos::common;

void owlart::printfline(char *str, uint8_t line) {
    uint16_t attrib = 0x07;

    volatile uint16_t* vidmem;
    for (int i = 0; str[i] != '\0'; i++) {
        vidmem = (volatile uint16_t*)0xb8000 + (80*line+i);
        *vidmem = str[i] | (attrib << 8);

    }
}

void printf(char* str);

void owlart::OwlArtStare()
{
    printf("(0,0)\n");
    printf("/)_)/\n");
    printf(" **  \n");

}

void owlart::OwlArtSleep()
{
    printfline("(-,-_)  \n",0);
    printfline("(x-x(v)7\n",1);
}

void owlart::OwlArtHeadphones()
{
    printfline("  _____       \n",0);
    printfline(" / ___ \      \n",1);
    printfline("(|(0,0)|)     \n",2);
    printfline("  /)_)/       \n",3);
    printfline("   **         \n",4);

}

void owlart::OwlArtFly()
{
    printfline(" (0,0) \n",0);
    printfline("==)_)==\n",1);
    printfline("  **   \n",2);
}

void owlart::OwlArtGeekin()
{
    printfline("(@,@)\n",0);
    printfline("[(_)]\n",1);
    printfline(" **  \n",2);
}

void owlart::OwlArtHappy()
{
    printfline("(^,^)\n",0);
    printfline("/)_)/\n",1);
    printfline(" **  \n",2);
}

void owlart::OwlArtSad()
{
    printfline("(v,v)\n",0);
    printfline("/)_)/\n",1);
    printfline(" **  \n",2);

}

void owlart::OwlArtLove()
{
    printfline("       <3       \n",0);
    printfline("  (^,^)  (^,^)  \n",1);
    printfline("  /)_)/  /)_)/  \n",2);
    printfline("---**-----**----\n",3);
}
