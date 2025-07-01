#include <nests/terminalgame.h>
#include <common/asl.h>
#include <common/asl_maths.h>
#include <common/asl_string.h>
#include <cli.h>
#include <drivers/keyboard.h>
#include <drivers/audio.h>


using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;

asl ASLTG;
asl_string ASLSTRTG;
Speaker PCSPEAKERS;

int attemptnum = 4;
bool lockedOut = false;
int exp = 0;

KeyboardEventHandler keyboardEventHandler;
KeyboardDriver* keyboardDriver;

void terminalgamestart() {
    ASLTG.TUI(0x00, 0x00, 0, 0, 0, 0, false);
    ASLTG.printfTUI("Alba Software TERMLINK PROTOCOL\n", 0x0f, 0x00, 0, 0);
    ASLTG.printfTUI("ENTER PASSWORD NOW\n", 0x0f, 0x00, 0, 2);
    ASLTG.printfTUI("ATTEMPT(S) LEFT: ", 0x0f, 0x00, 20, 4);
    ASLTG.printfTUI(ASLSTRTG.IntToString(attemptnum), 0x0f, 0x00, 20, 4);
    ASLTG.printfTUI("\n", 0x0f, 0x00, 0, 6);
    ASLTG.printfTUI("0xF4F0 VIRAL 0F5BC MINES\n", 0x0f, 0x00, 0, 7);
    ASLTG.printfTUI("0xF4FC DARED 0F5C8 LIKES\n", 0x0f, 0x00, 0, 8);
    ASLTG.printfTUI("0xF508 WIRES 0F5D4 PARTS\n", 0x0f, 0x00, 0, 9);
    ASLTG.printfTUI("0xF4F0 LINES 0F5BC HARAM\n", 0x0f, 0x00, 0, 10);
    ASLTG.printfTUI("0xF520 LIVES 0xF5E LINED\n", 0x0f, 0x00, 0, 11);
    ASLTG.printfTUI("0xF52C SORTS 0xF5F WARNS\n", 0x0f, 0x00, 0, 12);
    ASLTG.printfTUI("0xF538 AGREE 0xF60 THREW\n", 0x0f, 0x00, 0, 13);
    ASLTG.printfTUI("0xF544 FARMS 0xF61 SIDES\n", 0x0f, 0x00, 0, 14);
}

void passwordInputBox(char* input) {
    ASLTG.TUI(0x07, 0x05, 15, 12, 50, 3, true);
    ASLTG.printfTUI("Enter password: ", 0x0f, 0x00, 16, 13);
    ASLTG.printfTUI(input, 0x0f, 0x00, 16, 14);
}

void password() {
    terminalgamestart();
    static char inputPassword[32] = {0};
    static int inputIndex = 0;

    char correctPassword[] = "LIVES";

    while (attemptnum > 0 && !lockedOut) {
        passwordInputBox(inputPassword);

        if (keyboardEventHandler.keyValue != 0) {
            char key = keyboardEventHandler.keyValue;

            if (key == '\x1b') {
                break;
            }

            if (key == '\b' && inputIndex > 0) {
                inputIndex--;
                inputPassword[inputIndex] = 0;
            } else if (key == '\n') {
                if (ASLSTRTG.strcmp(inputPassword, correctPassword) == 0) {
                    exp += 10;
                    ASLTG.printfTUI("Exact Match! +10 EXP\n", 0x0f, 0x00, 0, 16);
                    PCSPEAKERS.Speak(1000);
                    ASLTG.sleep(500);
                    ASLTG.printfTUI("Total EXP: ", 0x0f, 0x00, 0, 17);
                    ASLTG.printfTUI(ASLSTRTG.IntToString(exp), 0x0f, 0x00, 30, 17);
                    break;
                } else {
                    attemptnum--;
                    ASLTG.printfTUI("Incorrect password. Attempts left: ", 0x0f, 0x00, 0, 16);
                    ASLTG.printfTUI(ASLSTRTG.IntToString(attemptnum), 0x0f, 0x00, 30, 16);

                    if (attemptnum == 0) {
                        lockedOut = true;
                        ASLTG.printfTUI("You have been temporarily locked out! Contact the Administrator!\n", 0x0f, 0x00, 0, 18);
                        PCSPEAKERS.Speak(500);
                        ASLTG.sleep(3000);
                        break;
                    }
                }
            } else {
                if (inputIndex < 31) {
                    inputPassword[inputIndex++] = key;
                }
            }

            keyboardEventHandler.OnKeyUp();
        }
    }
}
