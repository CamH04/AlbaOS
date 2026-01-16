#include <nests/terminalgame.h>
#include <common/asl.h>
#include <common/asl_maths.h>
#include <drivers/audio.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;

asl ASLTG;
asl_maths ASLMATHTG;
Speaker PCSPEAKERTG;


//keeps permenatly flickering because of spam printing
//firs tletter gets pritned 5 times (cant type an input properly)

struct TerminalGame {
    char inputPassword[32];
    int inputIndex;
    int attemptnum;
    bool lockedOut;
    int exp;
    bool gameWon;
};

TerminalGame tgame;

const char* words[] = {
    "VIRAL", "DARED", "WIRES", "LINES",
    "LIVES", "SORTS", "AGREE", "FARMS",
    "MINES", "LIKES", "PARTS", "HARAM",
    "LINED", "WARNS", "THREW", "SIDES"
};

const char correctPassword[] = "LIVES";

void terminalgameInit() {
    ASLTG.TUI(0x00, 0x00, 0, 0, 0, 0, false);

    tgame.inputIndex = 0;
    tgame.attemptnum = 4;
    tgame.lockedOut = false;
    tgame.exp = 0;
    tgame.gameWon = false;

    for (int i = 0; i < 32; i++) {
        tgame.inputPassword[i] = 0;
    }

    ASLTG.printfTUI("ROBCO INDUSTRIES (TM) TERMLINK PROTOCOL", 0x02, 0x00, 15, 1);
    ASLTG.printfTUI("ENTER PASSWORD NOW", 0x02, 0x00, 27, 3);

    ASLTG.printfTUI("4 ATTEMPT(S) LEFT: ####", 0x0f, 0x00, 25, 5);

    ASLTG.printfTUI("0xF4F0 VIRAL", 0x02, 0x00, 10, 8);
    ASLTG.printfTUI("0xF5BC MINES", 0x02, 0x00, 45, 8);

    ASLTG.printfTUI("0xF4FC DARED", 0x02, 0x00, 10, 9);
    ASLTG.printfTUI("0xF5C8 LIKES", 0x02, 0x00, 45, 9);

    ASLTG.printfTUI("0xF508 WIRES", 0x02, 0x00, 10, 10);
    ASLTG.printfTUI("0xF5D4 PARTS", 0x02, 0x00, 45, 10);

    ASLTG.printfTUI("0xF514 LINES", 0x02, 0x00, 10, 11);
    ASLTG.printfTUI("0xF5E0 HARAM", 0x02, 0x00, 45, 11);

    ASLTG.printfTUI("0xF520 LIVES", 0x02, 0x00, 10, 12);
    ASLTG.printfTUI("0xF5EC LINED", 0x02, 0x00, 45, 12);

    ASLTG.printfTUI("0xF52C SORTS", 0x02, 0x00, 10, 13);
    ASLTG.printfTUI("0xF5F8 WARNS", 0x02, 0x00, 45, 13);

    ASLTG.printfTUI("0xF538 AGREE", 0x02, 0x00, 10, 14);
    ASLTG.printfTUI("0xF604 THREW", 0x02, 0x00, 45, 14);

    ASLTG.printfTUI("0xF544 FARMS", 0x02, 0x00, 10, 15);
    ASLTG.printfTUI("0xF610 SIDES", 0x02, 0x00, 45, 15);

    ASLTG.printfTUI(">", 0x02, 0x00, 10, 18);
}

void updateAttempts() {
    char attempts[5] = "    ";
    for (int i = 0; i < tgame.attemptnum; i++) {
        attempts[i] = '#';
    }
    ASLTG.printfTUI(attempts, 0x0f, 0x00, 44, 5);
}

int checkLikeness(const char* guess, const char* answer) {
    int matches = 0;
    for (int i = 0; i < 5 && guess[i] != '\0' && answer[i] != '\0'; i++) {
        if (guess[i] == answer[i]) {
            matches++;
        }
    }
    return matches;
}

void terminalgame(char key) {
    static bool initialized = false;
    static int messageY = 19;

    if (!initialized) {
        terminalgameInit();
        initialized = true;
        messageY = 19;
    }

    if (key == 'q' || key == 'Q') {
        initialized = false;
        return;
    }

    if (key == 'r' || key == 'R') {
        terminalgameInit();
        messageY = 19;
        return;
    }

    if (tgame.lockedOut || tgame.gameWon) {
        return;
    }

    if (key == '\b' && tgame.inputIndex > 0) {
        tgame.inputIndex--;
        tgame.inputPassword[tgame.inputIndex] = 0;
        ASLTG.putcharTUI(' ', 0x02, 0x00, 11 + tgame.inputIndex, 18);
    }
    else if (key == '\n' && tgame.inputIndex > 0) {
        tgame.inputPassword[tgame.inputIndex] = '\0';

        for (int i = 0; i < tgame.inputIndex; i++) {
            if (tgame.inputPassword[i] >= 'a' && tgame.inputPassword[i] <= 'z') {
                tgame.inputPassword[i] -= 32;
            }
        }

        bool wordFound = false;
        for (int i = 0; i < 16; i++) {
            bool match = true;
            for (int j = 0; j < 5 && words[i][j] != '\0'; j++) {
                if (tgame.inputPassword[j] != words[i][j]) {
                    match = false;
                    break;
                }
            }
            if (match && tgame.inputPassword[5] == '\0') {
                wordFound = true;
                break;
            }
        }

        if (!wordFound) {
            ASLTG.printfTUI(">Error: Invalid word", 0x0C, 0x00, 10, messageY++);
            PCSPEAKERTG.Speak(200);
            ASLTG.sleep(100);
        }
        else {
            bool correct = true;
            for (int i = 0; i < 5; i++) {
                if (tgame.inputPassword[i] != correctPassword[i]) {
                    correct = false;
                    break;
                }
            }

            if (correct) {
                ASLTG.printfTUI(">Entry granted.", 0x0A, 0x00, 10, messageY++);
                ASLTG.printfTUI(">Exact match!", 0x0A, 0x00, 10, messageY++);
                ASLTG.printfTUI(">Likeness=5", 0x0A, 0x00, 10, messageY++);
                PCSPEAKERTG.Speak(800);
                ASLTG.sleep(200);
                PCSPEAKERTG.Speak(1000);
                tgame.gameWon = true;
            }
            else {
                tgame.attemptnum--;
                updateAttempts();

                int likeness = checkLikeness(tgame.inputPassword, correctPassword);

                ASLTG.printfTUI(">Entry denied.", 0x0C, 0x00, 10, messageY++);

                char likeMsg[20] = ">Likeness=";
                likeMsg[10] = '0' + likeness;
                likeMsg[11] = '\0';
                ASLTG.printfTUI(likeMsg, 0x0C, 0x00, 10, messageY++);

                PCSPEAKERTG.Speak(300);
                ASLTG.sleep(100);

                if (tgame.attemptnum == 0) {
                    ASLTG.printfTUI(">Terminal locked.", 0x0C, 0x00, 10, messageY++);
                    ASLTG.printfTUI(">Please contact administrator.", 0x0C, 0x00, 10, messageY++);
                    PCSPEAKERTG.Speak(200);
                    ASLTG.sleep(200);
                    PCSPEAKERTG.Speak(150);
                    tgame.lockedOut = true;
                }
            }
        }

        tgame.inputIndex = 0;
        for (int i = 0; i < 32; i++) {
            tgame.inputPassword[i] = 0;
        }

        for (int i = 0; i < 30; i++) {
            ASLTG.putcharTUI(' ', 0x02, 0x00, 11 + i, 18);
        }
    }
    else if (key >= 'a' && key <= 'z' && tgame.inputIndex < 5) {
        key -= 32;
        tgame.inputPassword[tgame.inputIndex] = key;
        ASLTG.putcharTUI(key, 0x02, 0x00, 11 + tgame.inputIndex, 18);
        tgame.inputIndex++;
    }
    else if (key >= 'A' && key <= 'Z' && tgame.inputIndex < 5) {
        tgame.inputPassword[tgame.inputIndex] = key;
        ASLTG.putcharTUI(key, 0x02, 0x00, 11 + tgame.inputIndex, 18);
        tgame.inputIndex++;
    }
}
