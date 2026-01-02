#include <common/asl.h>
#include <common/asl_maths.h>
#include <drivers/virtual_keyboard.h>

using namespace albaos;
using namespace albaos::common;

albaos::drivers::VirtualKeyboard& GetVirtualKeyboard();

asl ASLPIPES;
asl_maths ASLPIPESMATHS;

const uint8_t MAX_PIPES = 8;
const uint8_t PIPE_LENGTH = 15;

const uint8_t DIR_UP = 0;
const uint8_t DIR_DOWN = 1;
const uint8_t DIR_LEFT = 2;
const uint8_t DIR_RIGHT = 3;

const uint8_t CHAR_HORIZONTAL = 0xcd;  // ═
const uint8_t CHAR_VERTICAL = 0xba;    // ║
const uint8_t CHAR_CORNER_DR = 0xc9;   // ╔
const uint8_t CHAR_CORNER_DL = 0xbb;   // ╗
const uint8_t CHAR_CORNER_UR = 0xc8;   // ╚
const uint8_t CHAR_CORNER_UL = 0xbc;   // ╝
const uint8_t CHAR_CROSS = 0xce;       // ╬

struct Pipe {
    uint8_t x;
    uint8_t y;
    uint8_t direction;
    uint8_t color;
    uint8_t segmentsLeft;
    bool active;
};

Pipe pipeArray[MAX_PIPES];

//move to ASL_MATHS
uint8_t random_range(uint8_t min, uint8_t max) {
    uint32_t r = (uint32_t)ASLPIPESMATHS.betterRandom();
    return min + (r % (max - min + 1));
}

void pipesInit() {
    ASLPIPES.TUI(0x00, 0x00, 0, 0, 0, 0, false);
    for (uint8_t i = 0; i < MAX_PIPES; i++) {
        pipeArray[i].x = random_range(10, 70);
        pipeArray[i].y = random_range(3, 22);
        pipeArray[i].direction = random_range(0, 3);
        // Use bright colors: cyan, green, red, yellow, magenta, blue
        uint8_t colors[] = {0x0B, 0x0A, 0x0C, 0x0E, 0x0D, 0x09};
        pipeArray[i].color = colors[random_range(0, 5)];
        pipeArray[i].segmentsLeft = PIPE_LENGTH;
        pipeArray[i].active = true;
    }
}

uint8_t getCornerChar(uint8_t oldDir, uint8_t newDir) {
    if (oldDir == DIR_RIGHT && newDir == DIR_DOWN) return CHAR_CORNER_DR;
    if (oldDir == DIR_RIGHT && newDir == DIR_UP) return CHAR_CORNER_UR;
    if (oldDir == DIR_LEFT && newDir == DIR_DOWN) return CHAR_CORNER_DL;
    if (oldDir == DIR_LEFT && newDir == DIR_UP) return CHAR_CORNER_UL;
    if (oldDir == DIR_DOWN && newDir == DIR_RIGHT) return CHAR_CORNER_UR;
    if (oldDir == DIR_DOWN && newDir == DIR_LEFT) return CHAR_CORNER_UL;
    if (oldDir == DIR_UP && newDir == DIR_RIGHT) return CHAR_CORNER_DR;
    if (oldDir == DIR_UP && newDir == DIR_LEFT) return CHAR_CORNER_DL;
    return CHAR_CROSS;
}

void pipesUpdate() {
    for (uint8_t i = 0; i < MAX_PIPES; i++) {
        if (!pipeArray[i].active) continue;
        if (pipeArray[i].segmentsLeft == 0 || random_range(0, 10) == 0) {
            uint8_t oldDir = pipeArray[i].direction;
            uint8_t newDir;
            do {
                newDir = random_range(0, 3);
            } while ((oldDir == DIR_UP && newDir == DIR_DOWN) ||
                     (oldDir == DIR_DOWN && newDir == DIR_UP) ||
                     (oldDir == DIR_LEFT && newDir == DIR_RIGHT) ||
                     (oldDir == DIR_RIGHT && newDir == DIR_LEFT));

            uint8_t cornerChar = getCornerChar(oldDir, newDir);
            ASLPIPES.putcharTUI(cornerChar, pipeArray[i].color, 0x00, pipeArray[i].x, pipeArray[i].y);
            pipeArray[i].direction = newDir;
            pipeArray[i].segmentsLeft = PIPE_LENGTH;
        }
        switch (pipeArray[i].direction) {
            case DIR_UP:
                pipeArray[i].y--;
                break;
            case DIR_DOWN:
                pipeArray[i].y++;
                break;
            case DIR_LEFT:
                pipeArray[i].x--;
                break;
            case DIR_RIGHT:
                pipeArray[i].x++;
                break;
        }
        if (pipeArray[i].x >= 80 || pipeArray[i].y >= 25 || pipeArray[i].y < 2) {
            // Reset pipe to new position
            pipeArray[i].x = random_range(10, 70);
            pipeArray[i].y = random_range(3, 22);
            pipeArray[i].direction = random_range(0, 3);
            uint8_t colors[] = {0x0B, 0x0A, 0x0C, 0x0E, 0x0D, 0x09};
            pipeArray[i].color = colors[random_range(0, 5)];
            pipeArray[i].segmentsLeft = PIPE_LENGTH;
            continue;
        }
        uint8_t pipeChar;
        if (pipeArray[i].direction == DIR_LEFT || pipeArray[i].direction == DIR_RIGHT) {
            pipeChar = CHAR_HORIZONTAL;
        } else {
            pipeChar = CHAR_VERTICAL;
        }
        ASLPIPES.putcharTUI(pipeChar, pipeArray[i].color, 0x00, pipeArray[i].x, pipeArray[i].y);
        pipeArray[i].segmentsLeft--;
    }
}

void pipes(char key) {
    static bool initialized = false;
    static bool running = true;
    static uint16_t frameCount = 0;
    if (!initialized) {
        pipesInit();
        initialized = true;
    }
    if (key == 'q' || key == 'Q') {
        //virtual keyboard press ctrl c (doent work rn dont know why tbh)
        GetVirtualKeyboard().SetCtrl(true);
        GetVirtualKeyboard().PressKey('c');
        running = false;
        initialized = false;
        return;
    }
    if (key == 'r' || key == 'R') {
        pipesInit();
        frameCount = 0;
        return;
    }
    if (!running) {
        return;
    }
    pipesUpdate();
    if (frameCount == 0) {
        ASLPIPES.printfTUI("PIPES", 0x0F, 0x00, 1, 0);
        ASLPIPES.printfTUI("Q:Quit R:Restart", 0x08, 0x00, 60, 0);
    }
    frameCount++;
    ASLPIPES.sleep(80);
}
