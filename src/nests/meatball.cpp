#include <common/asl.h>
#include <common/asl_maths.h>

using namespace albaos;
using namespace albaos::common;

asl ASLMETABALLS;
asl_maths ASLMETAMATHS;

const uint8_t GRID_W = 80;
const uint8_t GRID_H = 25;
const uint8_t NUM_BALLS = 5;

struct Ball {
    float x;
    float y;
    float dx;
    float dy;
};

Ball balls[NUM_BALLS];

// TODO wanna move these to ASL_MATHS
float sqrt_approx(float x) {
    if (x <= 0) return 0;
    float guess = x / 2.0f;
    for (int i = 0; i < 10; i++) {
        guess = (guess + x / guess) / 2.0f;
    }
    return guess;
}

float dist(float x1, float y1, float x2, float y2) { //distance
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt_approx(dx * dx + dy * dy);
}

float random_float(float min, float max) {
    uint32_t r = (uint32_t)ASLMETAMATHS.betterRandom();
    float normalized = (r % 1000) / 1000.0f;
    return min + normalized * (max - min);
}
// =====================================================

void metaballsInit() {
    ASLMETABALLS.TUI(0x00, 0x00, 0, 0, 0, 0, false);

    //random vel and pos
    for (uint8_t i = 0; i < NUM_BALLS; i++) {
        balls[i].x = random_float(0, GRID_W);
        balls[i].y = random_float(0, GRID_H);
        balls[i].dx = random_float(0.3f, 0.8f);
        balls[i].dy = random_float(0.3f, 0.8f);
    }
}

void metaballsUpdate() {
    for (uint8_t i = 0; i < NUM_BALLS; i++) {
        balls[i].x += balls[i].dx;
        balls[i].y += balls[i].dy;
        //bounce off the walls
        if (balls[i].x >= GRID_W || balls[i].x <= 0) {
            balls[i].dx *= -1;
        }
        if (balls[i].y >= GRID_H || balls[i].y <= 0) {
            balls[i].dy *= -1;
        }
    }
}

void metaballsDisplay() {
    for (uint8_t x = 0; x < GRID_W; x++) {
        for (uint8_t y = 0; y < GRID_H; y++) {
            float sum = 0;
            for (uint8_t i = 0; i < NUM_BALLS; i++) {
                float d = dist(x, y, balls[i].x, balls[i].y);
                if (d > 0.1f) {
                    sum += 1.0f / d;
                }
            }
            if (sum > 0.15f) {
                uint8_t colour;
                if (sum > 0.5f) {
                    colour = 0x05; //MAGENTA
                } else if (sum > 0.3f) {
                    colour = 0x0C; // PINK
                } else if (sum > 0.2f) {
                    colour = 0x06; //ORANGE
                } else {
                    colour = 0x01; //BLUE
                }
                ASLMETABALLS.putcharTUI(0xdb, colour, 0x00, x, y); //first param is the printed char (hex)
            } else {
                ASLMETABALLS.putcharTUI(0x20, 0x00, 0x00, x, y);
            }
        }
    }
}

void metaballs(char key) {
    static bool initialized = false;
    static bool running = true;
    //tried to do a singleton style bcz i saw a yt video on it
    if (!initialized) {
        metaballsInit();
        initialized = true;
    }
    if (key == 'q' || key == 'Q') {
        running = false;
        return;
    }
    if (!running) {
        return;
    }
    metaballsUpdate();
    metaballsDisplay();
    ASLMETABALLS.sleep(50);
}
