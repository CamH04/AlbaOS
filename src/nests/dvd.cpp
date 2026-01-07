#include <common/asl.h>
#include <common/asl_maths.h>

using namespace albaos;
using namespace albaos::common;

asl ASLDVD;
asl_maths ASLDVDMATHS;

struct DvdOwl {
    int8_t x;
    int8_t y;
    int8_t dx;
    int8_t dy;
    uint8_t color;
};

DvdOwl owl;

const uint8_t OWL_WIDTH = 5;
const uint8_t OWL_HEIGHT = 3;
const uint8_t SCREEN_WIDTH = 80;
const uint8_t SCREEN_HEIGHT = 25;
const uint8_t colors[] = {0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E}; // blue, green, cyan, red, magenta, yellow
const uint8_t NUM_COLORS = 6;

//move to ASL
uint8_t random_color() {
    uint32_t r = (uint32_t)ASLDVDMATHS.betterRandom();
    return colors[r % NUM_COLORS];
}

void dvdInit() {
    ASLDVD.TUI(0x00, 0x00, 0, 0, 0, 0, false);
    owl.x = (SCREEN_WIDTH - OWL_WIDTH) / 2;
    owl.y = (SCREEN_HEIGHT - OWL_HEIGHT) / 2;
    owl.dx = 1;
    owl.dy = 1;
    owl.color = random_color();
}

void drawOwl(uint8_t x, uint8_t y, uint8_t color, bool clear) {
    uint8_t drawColor = clear ? 0x00 : color;
    char clearChar = clear ? ' ' : 0xff;

    //"  (^,^)"
    if (!clear) {
        ASLDVD.putcharTUI(' ', drawColor, 0x00, x, y);
        ASLDVD.putcharTUI(' ', drawColor, 0x00, x + 1, y);
        ASLDVD.putcharTUI('(', drawColor, 0x00, x + 2, y);
        ASLDVD.putcharTUI('^', drawColor, 0x00, x + 3, y);
        ASLDVD.putcharTUI(',', drawColor, 0x00, x + 4, y);
        ASLDVD.putcharTUI('^', drawColor, 0x00, x + 5, y);
        ASLDVD.putcharTUI(')', drawColor, 0x00, x + 6, y);
    } else {
        for (uint8_t i = 0; i < 7; i++) {
            ASLDVD.putcharTUI(' ', 0x00, 0x00, x + i, y);
        }
    }

    //"  /)_)/"
    if (!clear) {
        ASLDVD.putcharTUI(' ', drawColor, 0x00, x, y + 1);
        ASLDVD.putcharTUI(' ', drawColor, 0x00, x + 1, y + 1);
        ASLDVD.putcharTUI('/', drawColor, 0x00, x + 2, y + 1);
        ASLDVD.putcharTUI(')', drawColor, 0x00, x + 3, y + 1);
        ASLDVD.putcharTUI('_', drawColor, 0x00, x + 4, y + 1);
        ASLDVD.putcharTUI(')', drawColor, 0x00, x + 5, y + 1);
        ASLDVD.putcharTUI('/', drawColor, 0x00, x + 6, y + 1);
    } else {
        for (uint8_t i = 0; i < 7; i++) {
            ASLDVD.putcharTUI(' ', 0x00, 0x00, x + i, y + 1);
        }
    }

    //"   **"
    if (!clear) {
        ASLDVD.putcharTUI(' ', drawColor, 0x00, x, y + 2);
        ASLDVD.putcharTUI(' ', drawColor, 0x00, x + 1, y + 2);
        ASLDVD.putcharTUI(' ', drawColor, 0x00, x + 2, y + 2);
        ASLDVD.putcharTUI('*', drawColor, 0x00, x + 3, y + 2);
        ASLDVD.putcharTUI('*', drawColor, 0x00, x + 4, y + 2);
    } else {
        for (uint8_t i = 0; i < 5; i++) {
            ASLDVD.putcharTUI(' ', 0x00, 0x00, x + i, y + 2);
        }
    }
}

void dvdUpdate() {
    drawOwl(owl.x, owl.y, owl.color, true);

    owl.x += owl.dx;
    owl.y += owl.dy;

    bool corner_hit = false;
    if (owl.x <= 0) {
        owl.x = 0;
        owl.dx = 1;
        corner_hit = true;
    } else if (owl.x >= SCREEN_WIDTH - 7) {
        owl.x = SCREEN_WIDTH - 7;
        owl.dx = -1;
        corner_hit = true;
    }
    if (owl.y <= 0) {
        owl.y = 0;
        owl.dy = 1;
        corner_hit = true;
    } else if (owl.y >= SCREEN_HEIGHT - OWL_HEIGHT) {
        owl.y = SCREEN_HEIGHT - OWL_HEIGHT;
        owl.dy = -1;
        corner_hit = true;
    }
    if (corner_hit) {
        owl.color = random_color();
    }
    drawOwl(owl.x, owl.y, owl.color, false);
}

void dvd(char key) {
    static bool initialized = false;
    static bool running = true;
    static uint32_t bounceCount = 0;
    if (!initialized) {
        dvdInit();
        initialized = true;
    }
    if (key == 'q' || key == 'Q') {
        running = false;
        initialized = false;
        return;
    }
    if (key == 'r' || key == 'R') {
        dvdInit();
        bounceCount = 0;
        return;
    }
    if (!running) {
        return;
    }
    dvdUpdate();
    ASLDVD.printfTUI("Q:Quit R:Restart", 0x08, 0x00, 60, 0);

    ASLDVD.sleep(100);
}
