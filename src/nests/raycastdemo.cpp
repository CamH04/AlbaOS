#include <nests/raycastdemo.h>


using namespace albaos;
using namespace albaos::gui;
using namespace albaos::common;
using namespace albaos::drivers;


RaycastDemo::RaycastDemo() {
}
RaycastDemo::~RaycastDemo() {
}

void RaycastDemo::Draw(GraphicsContext* gc, uint16_t ticks,
		   char keylog[16], uint8_t logIndex, bool keyPress,
		   int32_t mouseX, int32_t mouseY) {

	this->raycaster.keyDown = keyPress;
	this->raycaster.ComputeSpace(gc, keylog, logIndex, mouseX);
}

void RaycastDemo::Demo(char keylog[16], uint8_t logIndex, bool keyPress,
				int32_t mouseX, int32_t mouseY) {
}

bool RaycastDemo::LoadData() {

	this->raycaster.posX = 22.0;
	this->raycaster.posY = 11.5;
	this->raycaster.dirX = -1.0;
	this->raycaster.dirY = 0.0;
	this->raycaster.planeX = 0.0;
	this->raycaster.planeY = 0.66;
	this->raycaster.keyDown = false;

	return true;
}
