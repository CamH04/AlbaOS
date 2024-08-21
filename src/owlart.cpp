#include <common/asl.h>
#include <owlart.h>
#include <common/types.h>

using namespace albaos;
using namespace albaos::common;

asl ASLOA;

void printf(char* str);


// eg ASLOA.printfhere("help me god",0);
//    ASLOA.printfhere("its too late for this",1);
void owlart::MenuHello(){
    ASLOA.printfhere("                                                                                ",0);
    ASLOA.printfhere("                                                                                ",1);
    ASLOA.printfhere("                                  (^,^)  (^,^)                                  ",2);
    ASLOA.printfhere("                                  /)_)/  /)_)/                                  ",3);
    ASLOA.printfhere("                                ---**-----**----                                ",4);
    ASLOA.printfhere("                                                                                ",5);
    ASLOA.printfhere("================================================================================",6);
    ASLOA.printfhere("              Welcome To AlbaOS!      0v0      Welcome To AlbaOS!               ",7);
    ASLOA.printfhere("================================================================================",8);
    ASLOA.printfhere("                                                                                ",9);
    ASLOA.printfhere("                                                                                ",10);
    ASLOA.printfhere("                                                                                ",11);
    ASLOA.printfhere("                                                                                ",12);
    ASLOA.printfhere("                                                                                ",13);
    ASLOA.printfhere("                                                                                ",14);
    ASLOA.printfhere("                                                                                ",15);
    ASLOA.printfhere("                                                                                ",16);
    ASLOA.printfhere("                                                                                ",17);
    ASLOA.printfhere("                                                                                ",18);
    ASLOA.printfhere("                                                                                ",19);
    ASLOA.printfhere("                                                                                ",20);
    ASLOA.printfhere("                                                                                ",21);
    ASLOA.printfhere("                                                                                ",22);
    ASLOA.printfhere("                         Press Any Key To Get Started!                          ",23);
    ASLOA.printfhere("                                                                                ",24);
    //cprintfs have to go last
    ASLOA.cprintf("<3", 0x0C, 0x0C, 39, 1);


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
    ASLOA.cprintf("<3", 0x0C, 0x0C, 7, 0);
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
            ASLOA.SetTextColor(true,i);
            printf("+");
        }
    }
    //back to white text
    ASLOA.SetTextColor(false,7);
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
