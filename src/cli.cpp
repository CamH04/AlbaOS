#include <cli.h>
#include <owlart.h>
#include <playstart.h>
#include <multitasking.h>
#include <drivers/audio.h>
#include <cpuid.h>
#include <drivers/amd_am79c973.h>
#include <filesys/ofs.h>


using namespace albaos;
using namespace albaos::drivers;
using namespace albaos::filesystem;
using namespace albaos::common;

void putchar(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void printfTUI(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, bool);
void printf(char*);
uint16_t hash(char* cmd);

//func from kernel
char* argparse(char*, uint8_t);
uint8_t argcount(char*);
uint32_t Random();
uint32_t StringToInt(char* args);
char* IntToString(uint32_t num);
uint16_t SetTextColor(bool set, uint16_t color);
void initnetwork(char* string);
uint16_t strlen(char* args);

uint32_t numOrVar(char* args, CommandLine* cli, uint8_t argNum);

uint32_t findarg(char* args, CommandLine* cli, uint8_t ArgNum);

void help_page1(){
    printf("=== Keybinds: ===\n");
    printf("ctrl+e : enter you into file editing nest (program)\n");
    printf("ctrl+c : return to CLI \n");
    printf("\n");
    printf("=== Useful Commands: ===\n");
    printf("help number(0 - 3): list of commands\n");
    printf("random: will generate a random number for you\n");
    printf("version : tells you the version of AlbaOS!\n");
    printf("hardwareinfo : tells you about your hardware\n");
    printf("senddata (text): sends string across network\n");
    printf("opengui: gives key to open gui WARNING: IF YOU HAVE EPILEPSY DONT USE THE GUI, TRUST ME\n");
    printf("clear : clears the screen (you can also press tab)\n");
}
void help_page2(){
    printf("=== Fun Commands: ===\n");
    printf("hackterminal: hack terminal game!\n");
    printf("owl number(0-8) : prints owl art!\n");
    printf("hello : starts the conversation with Dusty\n");
    printf("speak : Dusty will speak\n");
    printf("changetext number(0-16): changes text colour\n");
    printf("textnum: gives numbers for text colours\n");
    printf("pic: will print a coloured picture for you! (its very underwhelming)\n");
    printf("singsong number(0-idkyet): Dusty will sing you a song!\n");
    printf("emojiprint: prints emojis and the offset code (for devs)\n");
}
void help_page3(){
    printf("=== File Commands: ===\n");
    printf("files: lists files \n");
    printf("size number(sector): tells size of file \n");
    printf("delete number(sector): deletes file XvX \n");
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
    printf("             AlbaOS: Ver: Beta - 0.96\n");
    printf("\n");
    printf("==================================================\n");
    //print colours
    int numOfAvalibeCol = 16;
    for (int i = 0; i < numOfAvalibeCol; i++)
    {
        char* ch = " ";
        ch[0] = 2;

        SetTextColor(true,i);
        printf(ch);
    }
    SetTextColor(true,7);//back to white text
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
        SetTextColor(true, newColour);
    }
    printf("\v");
}
void textnum(char* args, CommandLine* cli) {
    int numOfAvalibeCol = 16;
    for (int i = 0; i < numOfAvalibeCol; i++)
    {
        SetTextColor(true,i);
        char* numStr = IntToString(i);
        printf(numStr);
        printf(" , ");
    }
    SetTextColor(true,7);//back to white text
    printf("\n");
}
void speak(char* args, CommandLine* cli){
    printf("Hoo Hoo\n");
    drivers::Speaker PCSPEAKER;
    PCSPEAKER.PlaySound(1522);
	PCSPEAKER.NoSound();
}
void random(char* args, CommandLine* cli){
    common::uint16_t prngresult = Random();

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
        default:
            printf("Not a valid value -v-\n");
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
        default:
            printf("We dont have that many songs yet -V- \n");
            break;
    }
}

void hardwareinfo(char* args, CommandLine* cli){
    cpuidentif CPUFINDER;
    printf("== Cpu Info: ==\n");
    CPUFINDER.cpufind();
    printf("\n");
}

void senddata(char* args, CommandLine* cli){

    uint32_t ValueIn = findarg(args, cli, 0);
    char* StrValueIn = IntToString(ValueIn);
    initnetwork(StrValueIn);
    printf("\n");
}

void debugata(char* args, CommandLine* cli){
    printf("this command has been archived VvV\n");
    /*
    printf("S-ATA primary master: \n");
    AdvancedTechnologyAttachment ata0m(true, 0x1F0);
    ata0m.Identify();
    ata0m.Write28(0, (uint8_t*)"0v0 Test",9);
    ata0m.Flush();
    ata0m.Read28(0, 9);
    */
}

void opengui(char* args, CommandLine* cli){
    printf("press esc to enter Gui 0v0 Doesnt work Rn... @v@\n");
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
			printf(IntToString(size));
			printf("bytes large. 0v0\n");
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
    this->hash_add("version",version);
    this->hash_add("changetext",changetext);
    this->hash_add("textnum",textnum);
    this->hash_add("pic",pic);
    this->hash_add("singsong",SingMeASong);
    this->hash_add("hardwareinfo",hardwareinfo);
    this->hash_add("senddata",senddata);
    this->hash_add("debugata",password);
    this->hash_add("opengui",opengui);
    this->hash_add("emojiprint",emojiprint);
    this->hash_add("hackterminal",password);
    //file commands
	this->hash_add("files", files);
	this->hash_add("size", size);
	this->hash_add("delete", deleteFile);




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

