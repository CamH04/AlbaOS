#include <common/asl.h>
#include <common/asl_maths.h>
#include <common/asl_string.h>
#include <cli.h>
#include <owlart.h>
#include <playstart.h>
#include <multitasking.h>
#include <drivers/audio.h>
#include <cpuid.h>
#include <drivers/amd_am79c973.h>
#include <filesys/ofs.h>
#include <drivers/cmos.h>
#include <drivers/ata.h>
#include <hardwarecommunication/power.h>
#include <drivers/virtual_keyboard.h>

using namespace albaos;
using namespace albaos::drivers;
using namespace albaos::filesystem;
using namespace albaos::common;
using albaos::hardwarecommunication::apm;


asl ASLCLI;
asl_maths ASLMATHSCLI;
asl_string ASLSTRINGCLI;
AdvancedTechnologyAttachment ata(0x1F0, true);
owlart OA;


void printf(char* str);
uint32_t numOrVar(char* args, CommandLine* cli, uint8_t argNum);
uint32_t findarg(char* args, CommandLine* cli, uint8_t ArgNum);
albaos::drivers::VirtualKeyboard& GetVirtualKeyboard();

//commands =====================================================
void help_page1(){
    printf("=== Keybinds: ===\n");
    printf("tab: clears screen\n");
    printf("\n");
    printf("=== Useful Commands: ===\n");
    printf("help number(0 - 5): list of commands\n");
    printf("v : tells you the version of AlbaOS!\n");
    printf("hwi : tells you about your hardware\n");
    printf("rb : reboots lol\n");
    printf("shutdown : shuts down\n");
    printf("date: gives date\n");
    printf("benchmark: displays the amount of clock cyles since last cpu reset\n");
    printf("bid : displays your current BootID\n");
}
void help_page2(){
    printf("=== Fun Commands: ===\n");
    printf("scribe : generate some word soup\n");
    printf("owl number(0-9) : prints some art!\n");
    printf("hello : starts the conversation with Dusty\n");
    printf("speak : Dusty will speak\n");
    printf("thanks : say thanks to Dusty!\n");
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
    printf("dsec int: dumps all written file sectors\n");
    printf("vsec int: view hard drive sector \n");
}
void help_page4(){
    printf("=== Maths Commands: ===\n");
    printf("add (any amount of numbers): adds the numbers\n");
    printf("sub (any amount of numbers): subtracts the numbers\n");
    printf("mul (any amount of numbers): multiplies the numbers\n");
    printf("div (any amount of numbers): divides the numbers\n");
    printf("mod (any amount of numbers): gives the remainder\n");
}
void help_page5(){
    printf("=== Nests (Programs): ===\n");
    printf("ctrl+e : enter you into file editing nest\n");
    printf("    ->    ctrl+w to write to file :)\n");
    printf("    ->    ctrl+c : return to CLI \n");
    printf("==========================================\n");
    printf("ctrl+s : enter the snake game nest\n");
    printf("    ->    ctrl+c : return to CLI \n");
    printf("==========================================\n");
    printf("ctrl+m : enter the metaballs (meatballs) gui\n");
    printf("    ->    ctrl+c : return to CLI \n");
    printf("==========================================\n");
    printf("ctrl+g : Enter GUI\n");
    printf("ctrl+r : idk yet\n");
}
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
        case 4:
            help_page4();
            break;
        case 5:
            help_page5();
            break;

        default:
            printf("we dont have that many pages -v-\n");
    }
}
void version(char* args, CommandLine* cli){
    OA.FullCliArt();
    printf("==================================================\n");
    printf("\n");
    printf("                     (0v0)           \n");
    printf("       AlbaOS Version: v0.9-Offline.0.7\n");
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

void changetext(char* args, CommandLine* cli) {
    //chnage
    uint16_t newColour = (uint16_t)(ASLSTRINGCLI.StringToInt(args));
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
        char* numStr = ASLSTRINGCLI.IntToString(i);
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
    common::uint16_t prngresult = ASLMATHSCLI.Random();

    prngresult = prngresult % cli->cmdIndex++;

    printf(ASLSTRINGCLI.IntToString(prngresult));
    printf("\n");
}
void owl(char* args, CommandLine* cli){
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

    ASLCLI.PrintCpuSpeed();

    printf("== RAM: ==\n");
    Cmos CMOS;
    CMOS.GetRAMFromCMOS(true);
}

void emojiprint(char* args, CommandLine* cli){
    char* ch = " ";
    int chamount = 256;
    for(int i = 11; i <= chamount; i++){
        ch[0] = i;
        printf(ASLSTRINGCLI.IntToString(i));
        printf("-");
        printf(ch);
        printf(",");
    }
    for(int i = 1; i < 11; i++){
        ch[0] = i;
        printf(ASLSTRINGCLI.IntToString(i));
        printf("-");
        printf(ch);
        printf(",");
    }
    printf("\n");
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

			printf(ASLSTRINGCLI.IntToString(location));
			printf("    ");
			printf(name);
			printf("\n");
		}
	}


	char* strNum = ASLSTRINGCLI.IntToString(fileNum);

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
			printf(ASLSTRINGCLI.IntToString(size / 1000));
			printf("kb large. ");
            printf(" (");
            printf(ASLSTRINGCLI.IntToString(size));
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
    printf("volare sicut tyto noctua , to fly like a nightingale");
    ASLCLI.sleep(1000);
    ASLCLI.reboot();
}

void add (char* args, CommandLine* cli){
    uint32_t ArgsStringCount = ASLSTRINGCLI.strlen(args);
    uint32_t NumAmount = ArgsStringCount / 2;

    //getting amount of nums in args
    if (ArgsStringCount % 2 != 0){
        NumAmount += 1;
    }
    uint32_t total = 0;
    for (int i = 1; i <= NumAmount; i++){
        total = total + findarg(args, cli, i - 1);
    }
    printf(ASLSTRINGCLI.IntToString(total));
    printf("\n");
}
void mul (char* args, CommandLine* cli){
    uint32_t ArgsStringCount = ASLSTRINGCLI.strlen(args);
    uint32_t NumAmount = ArgsStringCount / 2;

    //getting amount of nums in args
    if (ArgsStringCount % 2 != 0){
        NumAmount += 1;
    }
    uint32_t total = 1;
    for (int i = 1; i <= NumAmount; i++){
        total = total * findarg(args, cli, i - 1);
    }
    printf(ASLSTRINGCLI.IntToString(total));
    printf("\n");
}
void sub (char* args, CommandLine* cli){
    uint32_t ArgsStringCount = ASLSTRINGCLI.strlen(args);
    uint32_t NumAmount = ArgsStringCount / 2;

    //getting amount of nums in args
    if (ArgsStringCount % 2 != 0){
        NumAmount += 1;
    }
    uint32_t total = findarg(args, cli, 0);
    for (int i = 1; i <= NumAmount; i++){
        total = total - findarg(args, cli, i);
    }
    printf(ASLSTRINGCLI.IntToString(total));
    printf("\n");

}
void mod (char* args, CommandLine* cli){
    uint32_t ArgsStringCount = ASLSTRINGCLI.strlen(args);
    uint32_t NumAmount = ArgsStringCount / 2;
    //getting amount of nums in args
    if (ArgsStringCount % 2 != 0){
        NumAmount += 1;
    }
    uint32_t total = findarg(args, cli, 0);
    for (int i = 1; i <= NumAmount - 1; i++){
        total = total % findarg(args, cli, i);
    }
    printf(ASLSTRINGCLI.IntToString(total));
    printf("\n");
}
void div (char* args, CommandLine* cli){
    uint32_t ArgsStringCount = ASLSTRINGCLI.strlen(args);
    uint32_t NumAmount = ArgsStringCount / 2;
    //getting amount of nums in args
    if (ArgsStringCount % 2 != 0){
        NumAmount += 1;
    }
    uint32_t total = findarg(args, cli, 0);
    for (int i = 1; i <= NumAmount - 1; i++){
        total = total / findarg(args, cli, i);
    }
    printf(ASLSTRINGCLI.IntToString(total));
    printf("\n");
}

void date(char* args, CommandLine* cli){
    Cmos CMOS;
    CMOS.ReadRTC(true);
}
void shutdown(char* args, CommandLine* cli){
    ASLCLI.shutdown();
}
void benchmark(char* args, CommandLine* cli){
    ASLCLI.benchmark();
}
void DisplayBID(char* args, CommandLine* cli){
    printf("Your Boot ID is:");
    printf(ASLSTRINGCLI.IntToString(ASLMATHSCLI.stored_bootID));
    printf("\n");
}

void thanksReply(char* args, CommandLine* cli){
    printf("You're Welcome !\n");
}

void dumpWrittenSectors(char* args, CommandLine* cli) {
    const int maxSectorsToScan = 2048; // Can go up and down
    uint8_t buffer[512];
    bool inRange = false;
    int rangeStart = -1;
    if (!cli->mute) {
        printf("Scanning for written sectors...\n");
    }
    for (int sector = 0; sector < maxSectorsToScan; sector++) {
        ata.Read28(sector, buffer, 512, 0);
        bool nonZero = false;
        for (int i = 0; i < 512; i++) {
            if (buffer[i] != 0x00) {
                nonZero = true;
                break;
            }
        }
        if (nonZero) {
            if (!inRange) {
                inRange = true;
                rangeStart = sector;
            }
        } else {
            if (inRange) {
                char startStr[16], endStr[16];
                ASLSTRINGCLI.itoa(rangeStart, startStr, 10);
                ASLSTRINGCLI.itoa(sector - 1, endStr, 10);

                if (rangeStart == sector - 1) {
                    printf("Sector ");
                    printf(startStr);
                    printf(" has been written to.\n");
                } else {
                    printf("Sector ");
                    printf(startStr);
                    printf(" to ");
                    printf(endStr);
                    printf(" has been written to.\n");
                }

                inRange = false;
            }
        }
    }
    if (inRange) {
        char startStr[16], endStr[16];
        ASLSTRINGCLI.itoa(rangeStart, startStr, 10);
        ASLSTRINGCLI.itoa(maxSectorsToScan - 1, endStr, 10);

        if (rangeStart == maxSectorsToScan - 1) {
            printf("Sector ");
            printf(startStr);
            printf(" has been written to.\n");
        } else {
            printf("Sector ");
            printf(startStr);
            printf(" to ");
            printf(endStr);
            printf(" has been written to.\n");
        }
    }

    if (!cli->mute) {
        printf("Done.\n");
    }
}
//TODO convert hex dump to ascii string
void viewSector(char* args, CommandLine* cli) {
    if (args == 0 || ASLSTRINGCLI.strlen(args) == 0) {
        printf("Usage: viewSector int \n");
        return;
    }
    uint32_t sector = ASLSTRINGCLI.atoi(args, 10);
    uint8_t buffer[512];
    ata.Read28(sector, buffer, 512, 0);
    if (!cli->mute) {
        printf("Sector Dump :\n");
        printf(args);
        for (int i = 0; i < 512; i++) {
            if (i % 16 == 0) printf(ASLCLI.printfHex(i));
            ASLCLI.printfHex(buffer[i]);
        }
    printf("\n");
    }
}





void scribe(char* args, CommandLine* cli){
        //i have a feeling this will implode one day
    char* stringArray[] = {
        (char*)"Hello, world",
        (char*)"Jumping in a puddle With no boots All soaked And I get nosebleeds But I always get back up",
        (char*)"I dream Ill see you in the afterlife",
        (char*)"I know why the caged bird sings , Maya Angelou",
        (char*)"Wonder is the beginning of wisdom , Socrates",
        (char*)"Is it blissful? It's like a dream I want to dream",
        (char*)"ALERT , A RED SPY IS IN THE BASE",
        (char*)"Murr hurr mphuphurrur, hurr mph phrr",
        (char*)"I may be but small , but I will die a colossus . Ludleth of Courland Dark Souls 3",
        (char*)"Goodbye then . Stay safe friend . Don't you dare go hollow . Solare Dark Souls",
        (char*)"Protocol 1 : link to pilot Protocol 2 : uphold the mission Protocol 3 : protect the pilot",
        (char*)"Wonder is the beginning of wisdom",
        (char*)"This song is new to me , but I am honored to be a part of it",
        (char*)"When the heart speaks , the mind finds it indecent to object",
        (char*)"Dear Diary , Did i tell you i saw a bird . it had brown feathers and a red chest - it gleamed againts the sky",
        (char*)"they may face their greatest adversaries , eachother , and ultimately themselves"
    };
    const uint32_t arraySize = 16; //TODO just dynamicly get array size
    uint32_t repetitions = (ASLMATHSCLI.betterRandom() % 8) + 3;
    char pullString[512] = {0};
    uint32_t pullStringLen = 0;

    for(uint32_t i = 0; i < repetitions; i++){
        uint32_t stringIndex = ASLMATHSCLI.betterRandom() % arraySize;
        char* selectedString = stringArray[stringIndex];
        uint32_t stringLen = ASLSTRINGCLI.strlen(selectedString);
        uint32_t start = ASLMATHSCLI.betterRandom() % stringLen;
        uint32_t maxEnd = stringLen - start;
        uint32_t length = ASLMATHSCLI.betterRandom() % (maxEnd + 1);
        uint32_t end = start + length;
        char substring[256] = {0};
        uint32_t subLen = 0;
        for(uint32_t j = start; j < end && subLen < 255; j++){
            substring[subLen++] = selectedString[j];
        }
        substring[subLen] = '\0';
        char words[20][50]; // max 20 words, 50 chars each
        uint32_t wordCount = 0;
        uint32_t wordStart = 0;
        bool inWord = false;

        for(uint32_t j = 0; j <= subLen && wordCount < 20; j++){
            char c = (j < subLen) ? substring[j] : '\0';
            if(c != ' ' && c != ',' && c != '\0' && !inWord){
                wordStart = j;
                inWord = true;
            }
            else if((c == ' ' || c == ',' || c == '\0') && inWord){
                uint32_t wordLen = j - wordStart;
                if(wordLen > 0 && wordLen < 50){
                    for(uint32_t k = 0; k < wordLen; k++){
                        words[wordCount][k] = substring[wordStart + k];
                    }
                    words[wordCount][wordLen] = '\0';
                    wordCount++;
                }
                inWord = false;
            }
        }
        if(wordCount > 0){
            uint32_t wordIndex = ASLMATHSCLI.betterRandom() % wordCount;
            char* selectedWord = words[wordIndex];
            for(uint32_t j = 0; selectedWord[j] != '\0' && pullStringLen < 510; j++){
                pullString[pullStringLen++] = selectedWord[j];
            }
            if(pullStringLen < 511){
                pullString[pullStringLen++] = ' ';
            }
        }
    }
    pullString[pullStringLen] = '\0';
    printf(pullString);
    printf("\n");
}

void clear(char* args, CommandLine* cli){
    GetVirtualKeyboard().PressKey(KEY_TAB);
}


void test(char* args, CommandLine* cli){

}













//a small break to find args
uint32_t findarg(char* args, CommandLine* cli, uint8_t ArgNum){
        char* name = ASLCLI.argparse(args, ArgNum);
        uint16_t HashTemp = ASLMATHSCLI.hash(name) % 1024;

        if (cli->varTable[HashTemp] != 0xffffffff){
            return cli->varTable[HashTemp];
        }else if (name[0] == '>' && name[1] == 'R' && name[2] == '\0'){
            return cli->returnVal;
        }else{
            return ASLSTRINGCLI.StringToInt(name);
        }
}

uint32_t numOrVar(char* args, CommandLine* cli, uint8_t argNum) {
	char* name = ASLCLI.argparse(args, argNum);
	uint16_t hashVar = ASLMATHSCLI.hash(name) % 1024;
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
		return ASLSTRINGCLI.StringToInt(name);
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

    uint16_t hashIndex = ASLMATHSCLI.hash(cmd);


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

    ASLMATHSCLI.stored_bootID = ASLMATHSCLI.bootID();
    printf("Your Boot ID is:");
    printf(ASLSTRINGCLI.IntToString(ASLMATHSCLI.stored_bootID));
    printf("\n");


    this->varTable[ASLMATHSCLI.hash(">")] = 0x00;
    this->varTable[ASLMATHSCLI.hash(">CTRL")] = 0x00;
    this->varTable[ASLMATHSCLI.hash(">PRESS")] = 0x00;


    this->conditionIf = true;
    this->conditionLoop = true;
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //DONT FORGET TO HASH ALL FUCNTIONS HERE OR IT WONT BE RECOGNISED (so i dont forget to do this)
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    this->hash_add("help", help);
    this->hash_add("hello", hello);
    this->hash_add("speak",speak);
    this->hash_add("thanks",thanksReply);
    this->hash_add("random",random);
    this->hash_add("owl",owl);
    this->hash_add("v",version);
    this->hash_add("changetext",changetext);
    this->hash_add("textnum",textnum);
    this->hash_add("pic",pic);
    this->hash_add("singsong",SingMeASong);
    this->hash_add("hwi",hardwareinfo);
    this->hash_add("date", date);
    this->hash_add("emojiprint",emojiprint);
    this->hash_add("shutdown",shutdown);
    this->hash_add("rb",rebootCLI);
    this->hash_add("scribe",scribe);
    this->hash_add("bid",DisplayBID);
    this->hash_add("clear",clear);
    //file commands
	this->hash_add("files", files);
	this->hash_add("fs", size);
	this->hash_add("d", deleteFile);
    this->hash_add("dsec", dumpWrittenSectors);
    this->hash_add("vsec", viewSector);
    //maths commands
    this->hash_add("add", add);
    this->hash_add("sub", sub);
    this->hash_add("mul", mul);
    this->hash_add("div", div);
    this->hash_add("mod", mod);

    this->hash_add("benchmark",benchmark);

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
        uint16_t result = ASLMATHSCLI.hash(command);
        if (this->conditionIf == false && result != ASLMATHSCLI.hash("fi")) {
            return "IF vjbsdjvysuvbsdjnvdkjsnvsjdbvsdi";
        }
        if (this->conditionLoop == false && result != ASLMATHSCLI.hash("pool")) {
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

