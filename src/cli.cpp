#include <cli.h>
#include <owlart.h>
#include <drivers/audio.h>


using namespace albaos;
using namespace albaos::drivers;
using namespace albaos::common;

void putchar(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void printfTUI(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, bool);
void printf(char*);

//func from kernel
char* argparse(char*, uint8_t);
uint8_t argcount(char*);
uint32_t Random();
uint32_t StringToInt(char* args);

uint32_t findarg(char* args, CommandLine* cli, uint8_t ArgNum);







//commands
void help(char* args, CommandLine* cli){
    printf("=== Fun Commands: ===\n");
    printf("version : tells you the version! of AlbaOS\n");
    printf("owl number(0-8) : prints owl art!\n");
    printf("hello : starts the conversation with Dusty\n");
    printf("speak : Dusty will speak\n");
    printf("clear : clears the screen (you can also press tab)\n");
    printf("random: will generate a random number for you\n");
}

//TODO become a neofetch like command
void version(char* args, CommandLine* cli){
    printf("        (0v0)           \n");
    printf("AlbaOS: Ver: Beta - 0.90\n");
}
void hello(char* args, CommandLine* cli){
    printf("Hiiiii ^v^\n");
}

void clear(char* args, CommandLine* cli) {
    //you could also just press tab
    printf("\v");
}
void speak(char* args, CommandLine* cli){
    printf("Woooooo\n");
    drivers::Speaker PCSPEAKER;
    PCSPEAKER.PlaySound(1522);
	PCSPEAKER.NoSound();
}
void random(char* args, CommandLine* cli){
    //TODO print int
    common::uint64_t prngresult = Random();
    if(prngresult > 0.5)
    {
        printf("you generated a number larger than 0.5\n");
    }
    if(prngresult < 0.5)
    {
        printf("you generated a number less than 0.5\n");
    }
    else{
        //printf("how\n");
    }
}
void owl(char* args, CommandLine* cli){
    owlart OA;
    uint32_t ValueIn = findarg(args, cli, 0);
    switch (ValueIn){
        case 0:
            printf("0v0\n");
            break;
        case 1:
            OA.OwlArtStare();
            printf("\n");
            break;
        case 2:
            OA.OwlArtSleep();
            printf("\n");
            break;
        case 3:
            OA.OwlArtHeadphones();
            printf("\n");
            break;
        case 4:
            OA.OwlArtFly();
            printf("\n");
            break;
        case 5:
            OA.OwlArtGeekin();
            printf("\n");
            break;
        case 6:
            OA.OwlArtHappy();
            printf("\n");
            break;
        case 7:
            OA.OwlArtSad();
            printf("\n");
            break;
        case 8:
            OA.OwlArtLoveCLIVER();
            printf("\n");
            break;
        default:
            printf("Not a valid value -v-\n");
    }
}















void test(char* args, CommandLine* cli){

    printf("test");
}




//hashing stuff
void CommandLine::getTM(GlobalDescriptorTable* gdt, TaskManager* tm) {

    this->cli_gdt = gdt;
    this->cli_tm = tm;
}

uint32_t Trollfnv1a(char* str) {

	uint32_t hash = 0x811c9dc5;


	for (int i = 0; str[i] != '\0'; i++) {

		hash ^= str[i];
		hash *= 0x01000193;
	}


	//hash within sectors available on disk
	return (hash % 2048) + 1024;
}

uint16_t hash(char* cmd) {

    uint32_t val = Trollfnv1a(cmd);
    return (val >> 16) ^ (val & 0xffff);
}

//a small break to find args
uint32_t findarg(char* args, CommandLine* cli, uint8_t ArgNum){
        char* name = argparse(args, ArgNum);
        uint16_t HashTemp = hash(name) % 1024;

        if (cli->varTable[HashTemp] != 0xffffffff){
            return cli->varTable[HashTemp];
        }else if (name[0] == '>' && name[1] == 'R' && name[2] == '\0'){
            return cli->returnVal;
        }else{
            return StringToInt(name);
        }
}


void CommandLine::hash_add(char* cmd, void func(char*, CommandLine* cli)) {

    uint16_t hashIndex = hash(cmd);


    while (this->cmdTable[hashIndex] != nullptr) {

        hashIndex++;
    }


    this->cmdTable[hashIndex] = func;

    this->cmdList[this->cmdIndex] = hashIndex;
    this->cmdIndex++;
}


void CommandLine::hash_cli_init() {

    for (int i = 0; i < 65536; i++) {

        this->cmdTable[i] = nullptr;

        if (i < 1024) {

            this->varTable[i] = 0xffffffff;
        }
    }


    this->varTable[hash(">")] = 0x00;
    this->varTable[hash(">CTRL")] = 0x00;
    this->varTable[hash(">PRESS")] = 0x00;


    this->conditionIf = true;
    this->conditionLoop = true;
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //DONT FORGET TO HASH ALL FUCNTIONS HERE OR IT WONT BE RECOGNISED (so i dont forget to do this)
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    this->hash_add("help", help);
    this->hash_add("hello", hello);
    this->hash_add("clear",clear);
    this->hash_add("speak",speak);
    this->hash_add("random",random);
    this->hash_add("owl",owl);
    this->hash_add("version",version);




    this->hash_add("test",test);
}
//shoot me with an anti-m rifle plss
char* CommandLine::command(char* cmd, uint8_t length) {

        char* command = cmd;
        char arguments[length];


        bool args = false;
        uint8_t argLength = 0;
        uint8_t cmdLength = length;


        for (uint8_t i = 0; i < length; i++) {

            if (command[i] == ' ' && argLength == 0) {

                argLength = length - i;
                args = (argLength > 0);

                cmdLength = i;
            }

            if (args) {

                arguments[i - (cmdLength + 1)] = cmd[i];
            }
        }
        command[cmdLength] = '\0';
        arguments[argLength-1] = '\0';
        uint16_t result = hash(command);
        if (this->conditionIf == false && result != hash("fi")) {
            return "IF vjbsdjvysuvbsdjnvdkjsnvsjdbvsdi";
        }
        if (this->conditionLoop == false && result != hash("pool")) {
            return "LOOP fbaisaofnslalfnasjghfisdnld";
        }



        if (this->cmdTable[result] != nullptr)  {

            (*cmdTable[result])(arguments, this); //does command from the array
            arguments[0] = '\0';

        } else {
            if (this->varTable[result % 1024] != 0xffffffff) {
                printf("\n");
            //unknown command error handling
            } else {

                if (length < 0xff) {

                    printf("'");
                    printf(command);
                    printf("' is an unknown command sorry -v-\n");

                } else {

                    printf("command is too long @v@\n");
                }
            }
        }

        return cmd;
    }

