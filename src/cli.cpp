#include <common/asl.h>
#include <cli.h>
#include <owlart.h>
#include <playstart.h>
#include <multitasking.h>
#include <drivers/audio.h>
#include <cpuid.h>
#include <drivers/amd_am79c973.h>
#include <filesys/ofs.h>
#include <drivers/cmos.h>


using namespace albaos;
using namespace albaos::drivers;
using namespace albaos::filesystem;
using namespace albaos::common;


asl ASLCLI;

void putchar(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void printfTUI(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, bool);
void printf(char*);
uint16_t hash(char* cmd);

//func from kernel
char* argparse(char*, uint8_t);
uint8_t argcount(char*);
uint32_t StringToInt(char* args);
char* IntToString(uint32_t num);
void initnetwork(char* string);
uint16_t strlen(char* args);

uint32_t numOrVar(char* args, CommandLine* cli, uint8_t argNum);

uint32_t findarg(char* args, CommandLine* cli, uint8_t ArgNum);

void help_page1(){
    printf("=== Keybinds: ===\n");
    printf("ctrl+e : enter you into file editing nest\n");
    printf("        ctrl+w to write to file :)\n");
    printf("ctrl+c : return to CLI \n");
    printf("ctrl+g : Enter GUI\n");
    printf("\n");
    printf("=== Useful Commands: ===\n");
    printf("help number(0 - 3): list of commands\n");
    printf("v : tells you the version of AlbaOS!\n");
    printf("hwi : tells you about your hardware\n");
    printf("clear : clears the screen (you can also press tab)\n");
    printf("rb : reboots lol\n");
}
void help_page2(){
    printf("=== Fun Commands: ===\n");
    printf("owl number(0-9) : prints some art!\n");
    printf("hello : starts the conversation with Dusty\n");
    printf("speak : Dusty will speak\n");
    printf("changetext number(0-16): changes text colour\n");
    printf("textnum: gives numbers for text colours\n");
    printf("pic: will print a coloured picture for you! (its very underwhelming)\n");
    printf("singsong number(0-idkyet): Dusty will sing you a song!\n");
    printf("emojiprint: prints emojis and the offset code (for devs)\n");
    printf("random: will generate a random number for you\n");
}
void help_page3(){
    printf("=== File Commands: ===\n");
    printf("files: lists files \n");
    printf("fs filename: tells size of file \n");
    printf("d filename: deletes file XvX \n");
}


//commands
void help(char* args, CommandLine* cli){

    uint32_t ValueIn = findarg(args, cli, 0);
    switch (ValueIn){
        case 0:
            help_page1();
            break;
        case 1:
            help_page1();
            break;
        case 2:
            help_page2();
            break;
        case 3:
            help_page3();
            break;
        default:
            printf("we dont have that many pages -v-");
    }
}
void version(char* args, CommandLine* cli){
    printf("\n");
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
    printf("\n");
    printf("==================================================\n");
    printf("\n");
    printf("                     (0v0)           \n");
    printf("             AlbaOS: Ver: 1.0\n");
    printf("\n");
    printf("==================================================\n");
    //print colours
    int numOfAvalibeCol = 16;
    for (int i = 0; i < numOfAvalibeCol; i++)
    {
        char* ch = " ";
        ch[0] = 2;

        ASLCLI.SetTextColor(true,i);
        printf(ch);
    }
    ASLCLI.SetTextColor(true,7);//back to white text
    printf("\n");
}

void hello(char* args, CommandLine* cli){
    printf("Hiiiii ^v^\n");
}

void clear(char* args, CommandLine* cli) {
    //you could also just press tab
    printf("\v");
}
void changetext(char* args, CommandLine* cli) {
    //chnage
    uint16_t newColour = (uint16_t)(StringToInt(args));
    if (!newColour) {
        return;
    }
    else{
        //i spelt it the american way in kernel . ffs
        ASLCLI.SetTextColor(true, newColour);
    }
    printf("\v");
}
void textnum(char* args, CommandLine* cli) {
    int numOfAvalibeCol = 16;
    for (int i = 0; i < numOfAvalibeCol; i++)
    {
        ASLCLI.SetTextColor(true,i);
        char* numStr = IntToString(i);
        printf(numStr);
        printf(" , ");
    }
    ASLCLI.SetTextColor(true,7);//back to white text
    printf("\n");
}
void speak(char* args, CommandLine* cli){
    printf("Hoo Hoo\n");
    drivers::Speaker PCSPEAKER;
    PCSPEAKER.PlaySound(1522);
	PCSPEAKER.NoSound();
}
void random(char* args, CommandLine* cli){
    common::uint16_t prngresult = ASLCLI.Random();

    prngresult = prngresult % cli->cmdIndex++;

    printf(IntToString(prngresult));
    printf("\n");
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
            break;
        case 9:
            OA.seal1();
            break;
        default:
            printf("I have that many yet -v-\n");
    }
}

void pic(char* args, CommandLine* cli) {
    owlart OA;
    OA.pic();
    printf("\n");
}

void SingMeASong(char* args, CommandLine* cli){
    //class init
    playstart PS;
    Speaker s;
    uint32_t ValueIn = findarg(args, cli, 0);

    switch (ValueIn){
        case 0:
            //song 1
            PS.song1();
            s.NoSound();
            break;
        case 1:
            //song 1 again
            PS.song1();
            s.NoSound();
            break;
        case 2:
            //song 2
            PS.song2();
            s.NoSound();
            break;
        case 3:
            PS.song3();
            s.NoSound();
        default:
            printf("We dont have that many songs yet -V- \n");
            break;
    }
}

void hardwareinfo(char* args, CommandLine* cli){
    printf("== Video ==\n");
    printf("320,200 px\n");
    printf("\n");

    cpuidentif CPUFINDER;
    printf("== Cpu: ==\n");
    CPUFINDER.cpufind();
    printf("\n");

    printf("== RAM: ==\n");
    Cmos CMOS;
    CMOS.GetRAMFromCMOS(true);
}

void emojiprint(char* args, CommandLine* cli){
    char* ch = " ";
    int chamount = 256;
    for(int i = 11; i <= chamount; i++){
        ch[0] = i;
        printf(IntToString(i));
        printf("-");
        printf(ch);
        printf(",");
    }
    for(int i = 1; i < 11; i++){
        ch[0] = i;
        printf(IntToString(i));
        printf("-");
        printf(ch);
        printf(",");
    }
    printf("\n");
}

uint32_t numOrVar(char* args, CommandLine* cli, uint8_t argNum) {
	char* name = argparse(args, argNum);
	uint16_t hashVar = hash(name) % 1024;
	if (cli->varTable[hashVar] != 0xffffffff) {
		return cli->varTable[hashVar];

	} else if (name[0] == '$' && name[2] == '\0') {
		if (name[1] <= '9' && name[1] >= '0') {
			return cli->argTable[name[1]-'0'];
		}
		if (name[1] == 'R') {
			return cli->returnVal;
		}
		return 0;
	} else if (name[0] == '@' && name[2] == '\0') {
		return (uint8_t)(name[1]);
	} else {
		return StringToInt(name);
	}
}

void files(char* args, CommandLine* cli) {

	char name[33];
	for (int i = 0; i < 33; i++) { name[i] = 0x00; }
	uint32_t location = 0;
	uint32_t fileNum = GetFileCount();
	cli->returnVal = fileNum;

	for (int i = 0; i < fileNum; i++) {

		location = GetFileName(i, name);

		if (cli->mute == false) {

			printf(IntToString(location));
			printf("    ");
			printf(name);
			printf("\n");
		}
	}


	char* strNum = IntToString(fileNum);

	if (cli->mute == false) {

		printf("\n");
		printf(strNum);
		printf(" files have been allocated.\n");
	}
}

void size(char* args, CommandLine* cli) {

	uint32_t size = GetFileSize(args);
	cli->returnVal = size;


	if (cli->mute == false) {

		if (size) {

			printf("'");
			printf(args);
			printf("' is ");
			printf(IntToString(size / 1000));
			printf("kb large. ");
            printf(" (");
            printf(IntToString(size));
            printf(" bytes) 0v0\n");
		} else {
			printf("doesnt exist @v@\n");
		}
	}

}



void deleteFile(char* args, CommandLine* cli) {

	bool deleted = DeleteFile(args);


	if (cli->mute == false) {

		if (deleted) {

			printf("'");
			printf(args);
			printf("' was deleted XvX\n");
		} else {
			printf(args);
			printf(" doesnt exist @v@\n");
		}
	}
}

void rebootCLI(char* args, CommandLine* cli){
    ASLCLI.reboot();
}






















void test(char* args, CommandLine* cli){

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

CommandLine::CommandLine(GlobalDescriptorTable* gdt,
			TaskManager* tm,
			AdvancedTechnologyAttachment* ata0m) {

    this->gdt = gdt;
    this->tm = tm;
    this->ata0m = ata0m;
}

CommandLine::~CommandLine() {
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
    this->hash_add("v",version);
    this->hash_add("changetext",changetext);
    this->hash_add("textnum",textnum);
    this->hash_add("pic",pic);
    this->hash_add("singsong",SingMeASong);
    this->hash_add("hwi",hardwareinfo);
    this->hash_add("emojiprint",emojiprint);
    this->hash_add("rb",rebootCLI);
    //file commands
	this->hash_add("files", files);
	this->hash_add("fs", size);
	this->hash_add("d", deleteFile);




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

                    printf(command);
                    printf(" is an unknown command sorry -v-\n");

                } else {

                    printf("command is too long @v@\n");
                }
            }
        }

        return cmd;
    }

