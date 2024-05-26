#ifndef __ALBAOS__CLI_H
#define __ALBAOS__CLI_H

#include <common/types.h>
#include <drivers/ata.h>
#include <filesys/ofs.h>
#include <multitasking.h>
#include <gdt.h>


namespace albaos {

    class CommandLine {


        public:
            void (*cmdTable[65536])(char*, CommandLine*);
            common::uint32_t varTable[1024];
            bool conditionIf;
            bool conditionLoop;
            common::uint32_t returnVal = 0;


            common::uint16_t cmdList[65536];
            common::uint16_t cmdIndex = 0;;


            GlobalDescriptorTable* gdt;
            TaskManager* tm;
            drivers::AdvancedTechnologyAttachment* ata0m;

            common::uint16_t cliMode = 0;

            public:
                CommandLine(GlobalDescriptorTable* gdt,
                        TaskManager* tm,
                        drivers::AdvancedTechnologyAttachment* ata0m);
                ~CommandLine();

            char* commandStr;
            char* command(char *cmd, common::uint8_t length);

            void hash_add(char* cmd, void func(char*, CommandLine*));
            void hash_cli_init();


            void getTM(GlobalDescriptorTable* gdt, TaskManager* tm);

    };
}


#endif
