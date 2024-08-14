
#include <drivers/keyboard.h>

using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;

// when press key , get 2 interrupts (one for key press, one for key releace. where the first bit is flipped)
KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::OnKeyDown(char)
{
}

// only recognises key release for interrupts
void KeyboardEventHandler::OnKeyUp()
{
}


void KeyboardEventHandler::resetMode(uint8_t)
{
}

void KeyboardEventHandler::modeSet(uint8_t)
{
}





KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler *handler)
: InterruptHandler(manager, 0x21),
dataport(0x60),
commandport(0x64)
{
    this->handler = handler;
}

KeyboardDriver::~KeyboardDriver()
{
}

void KeyboardDriver::Activate()
{
    while(commandport.Read() & 0x1){
        dataport.Read();
        commandport.Write(0xAE); // activate
        commandport.Write(0x20); //read controller command byte (getting current state)

        uint8_t status = (dataport.Read() | 1) & ~0x10;

        commandport.Write(0x60); // set controller command byte
        dataport.Write(status);

        dataport.Write(0xF4);
    }
}

void printf(char*);
void printfHex(uint8_t);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    /*
    char* ch = " ";
    ch[0] = 2;
    */

    uint8_t key = dataport.Read();
    this->handler->keyValue = key;

    if(handler == 0){
        return esp;
    }
    static uint8_t mode = 0;
    //starts the cli keyboard stuff
    if (handler->cli) {
        printf("\t");
    }
        //if you look at prev commits you will see a if key > 0x80 here, that was whats been breaking the keyboard drivers for months I AM SO HAPPY ITS FIXED
        //IVE BEEN LOOSING MY MIND OVER THIS FOR SO LOOOOOONNNNNGGG NOW ITS DONE!!!!!!!!!!!!!!!!!!!!
        switch(key)
        {
            //keycodes do not make sense
            //these are qwerty uk keycodes

            //gun
            case 0x29: if (handler->shift ^ handler->caps) { handler->OnKeyDown('¬'); } else { handler->OnKeyDown('`'); } break;

            //top numbers
            case 0x02: if (handler->shift) { handler->OnKeyDown('!'); } else { handler->OnKeyDown('1'); } break;
            case 0x03: if (handler->shift) { handler->OnKeyDown('"'); } else { handler->OnKeyDown('2'); } break;
            case 0x04: if (handler->shift) { handler->OnKeyDown('£'); } else { handler->OnKeyDown('3'); } break;
            case 0x05: if (handler->shift) { handler->OnKeyDown('$'); } else { handler->OnKeyDown('4'); } break;
            case 0x06: if (handler->shift) { handler->OnKeyDown('%'); } else { handler->OnKeyDown('5'); } break;
            case 0x07: if (handler->shift) { handler->OnKeyDown('^'); } else { handler->OnKeyDown('6'); } break;
            case 0x08: if (handler->shift) { handler->OnKeyDown('&'); } else { handler->OnKeyDown('7'); } break;
            case 0x09: if (handler->shift) { handler->OnKeyDown('*'); } else { handler->OnKeyDown('8'); } break;
            case 0x0A: if (handler->shift) { handler->OnKeyDown('('); } else { handler->OnKeyDown('9'); } break;
            case 0x0B: if (handler->shift) { handler->OnKeyDown(')'); } else { handler->OnKeyDown('0'); } break;

            case 0x0C: if (handler->shift) { handler->OnKeyDown('_'); } else { handler->OnKeyDown('-'); } break;
            case 0x0D: if (handler->shift) { handler->OnKeyDown('+'); } else { handler->OnKeyDown('='); } break;

            //top row letters
            case 0x10: if (handler->shift ^ handler->caps) { handler->OnKeyDown('Q'); } else { handler->OnKeyDown('q'); } break;
            case 0x11: if (handler->shift ^ handler->caps) { handler->OnKeyDown('W'); } else { handler->OnKeyDown('w'); } break;
            case 0x12: if (handler->shift ^ handler->caps) { handler->OnKeyDown('E'); } else { handler->OnKeyDown('e'); } break;
            case 0x13: if (handler->shift ^ handler->caps) { handler->OnKeyDown('R'); } else { handler->OnKeyDown('r'); } break;
            case 0x14: if (handler->shift ^ handler->caps) { handler->OnKeyDown('T'); } else { handler->OnKeyDown('t'); } break;
            case 0x15: if (handler->shift ^ handler->caps) { handler->OnKeyDown('Y'); } else { handler->OnKeyDown('y'); } break;
            case 0x16: if (handler->shift ^ handler->caps) { handler->OnKeyDown('U'); } else { handler->OnKeyDown('u'); } break;
            case 0x17: if (handler->shift ^ handler->caps) { handler->OnKeyDown('I'); } else { handler->OnKeyDown('i'); } break;
            case 0x18: if (handler->shift ^ handler->caps) { handler->OnKeyDown('O'); } else { handler->OnKeyDown('o'); } break;
            case 0x19: if (handler->shift ^ handler->caps) { handler->OnKeyDown('P'); } else { handler->OnKeyDown('p'); } break;

            case 0x1A: if (handler->shift) { handler->OnKeyDown('{'); } else { handler->OnKeyDown('['); } break;
            case 0x1B: if (handler->shift) { handler->OnKeyDown('}'); } else { handler->OnKeyDown(']'); } break;

            // 2nd row letters
            case 0x1E: if (handler->shift ^ handler->caps) { handler->OnKeyDown('A'); } else { handler->OnKeyDown('a'); } break;
            case 0x1F: if (handler->shift ^ handler->caps) { handler->OnKeyDown('S'); } else { handler->OnKeyDown('s'); } break;
            case 0x20: if (handler->shift ^ handler->caps) { handler->OnKeyDown('D'); } else { handler->OnKeyDown('d'); } break;
            case 0x21: if (handler->shift ^ handler->caps) { handler->OnKeyDown('F'); } else { handler->OnKeyDown('f'); } break;
            case 0x22: if (handler->shift ^ handler->caps) { handler->OnKeyDown('G'); } else { handler->OnKeyDown('g'); } break;
            case 0x23: if (handler->shift ^ handler->caps) { handler->OnKeyDown('H'); } else { handler->OnKeyDown('h'); } break;
            case 0x24: if (handler->shift ^ handler->caps) { handler->OnKeyDown('J'); } else { handler->OnKeyDown('j'); } break;
            case 0x25: if (handler->shift ^ handler->caps) { handler->OnKeyDown('K'); } else { handler->OnKeyDown('k'); } break;
            case 0x26: if (handler->shift ^ handler->caps) { handler->OnKeyDown('L'); } else { handler->OnKeyDown('l'); } break;

            case 0x2B: if (handler->shift) { handler->OnKeyDown('~'); } else { handler->OnKeyDown('#'); } break;
            case 0x28: if (handler->shift) { handler->OnKeyDown('@'); } break;
            case 0x27: if (handler->shift) { handler->OnKeyDown(':'); } else { handler->OnKeyDown(';'); } break;

            //3rd row letters
            case 0x56: break;
            case 0x2C: if (handler->shift ^ handler->caps) { handler->OnKeyDown('Z'); } else { handler->OnKeyDown('z'); } break;
            case 0x2D: if (handler->shift ^ handler->caps) { handler->OnKeyDown('X'); } else { handler->OnKeyDown('x'); } break;
            case 0x2E: if (handler->shift ^ handler->caps) { handler->OnKeyDown('C'); } else { handler->OnKeyDown('c'); keyHex = 0x2E; } break;
            case 0x2F: if (handler->shift ^ handler->caps) { handler->OnKeyDown('V'); } else { handler->OnKeyDown('v'); } break;
            case 0x30: if (handler->shift ^ handler->caps) { handler->OnKeyDown('B'); } else { handler->OnKeyDown('b'); } break;
            case 0x31: if (handler->shift ^ handler->caps) { handler->OnKeyDown('N'); } else { handler->OnKeyDown('n'); } break;
            case 0x32: if (handler->shift ^ handler->caps) { handler->OnKeyDown('M'); } else { handler->OnKeyDown('m'); } break;

            case 0x33: if (handler->shift) { handler->OnKeyDown('<'); } else { handler->OnKeyDown(','); } break;
            case 0x34: if (handler->shift) { handler->OnKeyDown('>'); } else { handler->OnKeyDown('.'); } break;
            case 0x35: if (handler->shift) { handler->OnKeyDown('?'); } else { handler->OnKeyDown('/'); } break;

            // funny keys
            //enter
            case 0x1C: handler->OnKeyDown('\n'); break;
            case 0x39: handler->OnKeyDown(' '); break;
            //caps
            case 0x3A: handler->caps = handler->caps ^ 1; break;
            //win key
            case 0x5B: break;
            //backspace
            case 0x0E: handler->OnKeyDown('\b'); break;
            //esc
            case 0x01: handler->OnKeyDown('\x1b'); break;
            //right ctrl
            case 0x1D: handler->ctrl = true; break;
            case 0x9D: handler->ctrl = false; break;
            //tab
            case 0x0F: handler->OnKeyDown('\v'); break;
            //alt keys
            case 0x38: handler->alt = true; break;
            case 0xB8: handler->alt = true; break;
            case 0xE0: handler->alt =  false; break;
            //left arrow
            case 0x4B: handler->OnKeyDown('\xfc'); break;
            //up arrow
            case 0x48: handler->OnKeyDown('\xfd'); break;
            //down arrow
            case 0x50: handler->OnKeyDown('\xfe'); break;
            //right arrow
            case 0x4D: handler->OnKeyDown('\xff'); break;
            //shift keys
            case 0x2a:
                handler->shift = true;
                break;
            case 0x36:
                handler->shift = true;
                break;
            case 0xaa:
                handler->shift = false;
                break;
            case 0xb6:
                handler->shift = false;
                break;

            //strange keys
            //pintscrn
            case 0x37: break;
            //scrollock
            case 0x46: break;
            //break
            case 0x45: break;
            //ins
            case 0x52: break;
            //home
            case 0x47: break;
            //pgup
            case 0x49: break;
            //del
            case 0x53: break;
            //end
            case 0x4F: break;
            //pgdown
            case 0x51: break;


            default:
                //used for debugging keyboard, if you dont want to see hex codes just comment out the printf's

                //printf("KEYBOARD 0x");
                //printfHex(key);

                handler->OnKeyUp();
                break;
    }

    return esp;
}
