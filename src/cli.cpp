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

//commands
void hello(char* args, CommandLine* cli){
    printf("hello\n");
}
void clear(char* args, CommandLine* cli) {

    printf("\v");
}

void CommandLine::getTM(GlobalDescriptorTable* gdt, TaskManager* tm) {

    this->cli_gdt = gdt;
    this->cli_tm = tm;
}


//hashing stuff
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

    //DONT FORGET TO HASH ALL FUCNTIONS HERE OR IT WONT BE RECOGNISED (so i dont forget to do this)
    this->hash_add("hello", hello);
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

