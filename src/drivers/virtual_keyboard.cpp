#include <drivers/virtual_keyboard.h>
#include <common/types.h>
#include <common/asl.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;

asl ASLVK;

VirtualKeyboard::VirtualKeyboard(KeyboardEventHandler* handler){
    this->handler = handler;
}

void VirtualKeyboard::PressKey(char key){
    if (handler)
        handler->OnKeyDown(key);
}

void VirtualKeyboard::ReleaseKey(){
    if (handler)
        handler->OnKeyUp();
}

void VirtualKeyboard::SetShift(bool enabled){
    if (handler)
        handler->shift = enabled;
}

void VirtualKeyboard::SetCapsLock(bool enabled){
    if (handler)
        handler->caps = enabled;
}

void VirtualKeyboard::SetCtrl(bool enabled){
    if (handler)
        handler->ctrl = enabled;
}

void VirtualKeyboard::SetAlt(bool enabled){
    if (handler)
        handler->alt = enabled;
}

void VirtualKeyboard::vPrint(const char* str, int delay_ms)
{
    for (int i = 0; str[i] != '\0'; ++i)
    {
        PressKey(str[i]);
        ReleaseKey();

        if (delay_ms > 0)
            ASLVK.sleep(delay_ms);
    }
}
