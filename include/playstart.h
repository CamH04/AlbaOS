#ifndef __ALBAOS__MIXER_H
#define __ALBAOS__MIXER_H

#include <common/types.h>

    namespace albaos {

        class  playstart{

            public:
                void singasong();
                void song1();
                void song2();
                void PlayNote(char ch, common::uint8_t octave, common::uint16_t time);

        };
    }

#endif
