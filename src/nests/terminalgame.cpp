
//TODO finish terminal game
int attemptnum = 4;
void terminalgamestart(){

    printf("Alba Software TERMLINK PROTOCOL\n");
    printf("ENTER PASSWORD NOW\n");
    printf("ATTEMPT(S) LEFT: ");
    printf(IntToString(attemptnum));
    printf("\n");
    printf("\n");

    printf("0xF4F0 VIRAL 0F5BC MINES\n");
    printf("0xF4FC DARED 0F5C8 LIKES\n");
    printf("0xF508 WIRES 0F5D4 PARTS\n");
    printf("0xF4F0 LINES 0F5BC HARAM\n");
    printf("0xF520 LIVES 0xF5E LINED\n");
    printf("0xF52C SORTS 0xF5F WARNS\n");
    printf("0xF538 AGREE 0xF60 THREW\n");
    printf("0xF544 FARMS 0xF61 SIDES\n");
}
void password(char* args, CommandLine* cli){
    terminalgamestart();
    uint32_t strlength = numOrVar(args, cli, 0);

    char* cmp = argparse(args, 0);


    if(strlen(cmp) > 0){
        int n;
        int y;
        while(attemptnum > 0){
            if(cmp != "LIVES"){
                //idkman
            }
            else{
                printf("Exact Match! +10EXP\n");
            }
        }
        printf("You have been temporarily locked out! Contact the Administrator! \n");
    }
    else
    {
        printf("error: no arg\n");
    }
}
