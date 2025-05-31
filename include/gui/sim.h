#ifndef __OS__GUI__SIM_H
#define __OS__GUI__SIM_H

#include <common/types.h>
#include <common/graphicscontext.h>
#include <drivers/vga.h>
#include <common/asl_maths.h>


namespace albaos {
	namespace gui {
		class Simulator {

			//protected:
			public:
				common::int32_t mouseX;
				common::int32_t mouseY;

				bool mousePress = false;

				common::int32_t albaX;
				common::int32_t albaY;

				struct common::asl_maths::point walkPixels[320];
				common::uint16_t stepsNum;
				common::uint8_t walkCycle;

				bool backwardsX = false;
				bool backwardsY = false;
				bool direction = false;

			public:
				Simulator();
				~Simulator();

				void OnMouseDown(common::int32_t MouseX,
						 common::int32_t MouseY,
						 common::uint8_t button);

				void Walk();

				void ComputeGameState();

				void DrawRoom(common::GraphicsContext* gc);
		};
	}
}

#endif
