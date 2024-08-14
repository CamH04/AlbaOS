#include <gui/sim.h>

using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::gui;
using namespace albaos::math;


void sleep(uint32_t);


Simulator::Simulator() {
}

Simulator::~Simulator() {
}


void Simulator::OnMouseDown(int32_t newMouseX, int32_t newMouseY, uint8_t button) {
}



void Simulator::Walk() {
}



void Simulator::ComputeGameState() {
}


void Simulator::DrawRoom(GraphicsContext* gc) {

	gc->FillRectangle(0, 0, 320, 200, 0x01);
}
