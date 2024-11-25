#ifndef __ALBAOS__GUI__GAMES__SHOOTER_H
#define __ALBAOS__GUI__GAMES__SHOOTER_H

#include <common/types.h>
#include <common/graphicscontext.h>
#include <drivers/vga.h>
#include <gui/raycasting.h>
#include <gui/guiart.h>


namespace albaos {

	namespace gui {

		class RaycastDemo {

			public:
				RaycastSpace raycaster;
			public:
				RaycastDemo();
				~RaycastDemo();

				void Draw(common::GraphicsContext* gc, common::uint16_t ticks,
						char keylog[16], common::uint8_t logIndex, bool keyPress,
						common::int32_t mouseX, common::int32_t mouseY);

				void Demo(char keylog[16], common::uint8_t logIndex, bool keyPress,
							common::int32_t mouseX, common::int32_t mouseY);
				bool LoadData();
		};
	};
}

#endif
