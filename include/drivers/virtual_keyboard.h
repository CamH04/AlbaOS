#ifndef __ALBAOS__DRIVERS__VIRTUALKEYBOARD_H
#define __ALBAOS__DRIVERS__VIRTUALKEYBOARD_H

#include <drivers/keyboard.h>

#define KEY_TAB        '\v'
#define KEY_ENTER      '\n'
#define KEY_BACKSPACE  '\b'
#define KEY_ESC        '\x1b'
#define KEY_LEFT       '\xfc'
#define KEY_UP         '\xfd'
#define KEY_DOWN       '\xfe'
#define KEY_RIGHT      '\xff'
#define KEY_SPACE      ' '
#define KEY_DELETE     '\x7f'
#define KEY_NULL       '\0'

namespace albaos {
    namespace drivers {
        class VirtualKeyboard {
        protected:
            KeyboardEventHandler* handler;

        public:
            VirtualKeyboard(KeyboardEventHandler* handler);

            void PressKey(char key);
            void ReleaseKey();

            void SetShift(bool enabled);
            void SetCapsLock(bool enabled);
            void SetCtrl(bool enabled);
            void SetAlt(bool enabled);

            void vPrint(const char* str, int delay_ms = 100);
        };
    }
}
extern albaos::drivers::VirtualKeyboard vkeyboard;

#endif
