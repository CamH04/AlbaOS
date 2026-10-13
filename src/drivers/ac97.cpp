#include <drivers/ac97.h>
#include <common/asl.h>
// 16-bit signed PCM, stereo, 44100 Hz (or 48000 Hz fallback)

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;

void printf(char*);
static asl ASLAC97;


AC97AudioDriver::AC97AudioDriver(InterruptManager* interrupts,
                                 PeripheralComponentInterconnectController* pci)
    : InterruptHandler(interrupts, interrupts->HardwareInterruptOffset() + 0x07),
      namBase(0), nabmBase(0),
      currentBuffer(0), sampleRate(44100), running(false), found(false),
      fillCallback(0)
{
    this->pci = pci;

    // Zero out the BDL and audio buffers
    for (int i = 0; i < AC97_BDL_ENTRIES; i++) {
        bdl[i].address = 0;
        bdl[i].samples = 0;
        bdl[i].flags   = 0;
    }
    for (int b = 0; b < 2; b++)
        for (uint32_t i = 0; i < BUFFER_SIZE; i++)
            audioBuffers[b][i] = 0;
}

AC97AudioDriver::~AC97AudioDriver() {
    stop();
}


void     AC97AudioDriver::writeNAM16(uint8_t reg, uint16_t val) {
    Port16Bit p(namBase + reg); p.Write(val); }
uint16_t AC97AudioDriver::readNAM16 (uint8_t reg) {
    Port16Bit p(namBase + reg); return p.Read(); }

void     AC97AudioDriver::writeNABM8 (uint8_t reg, uint8_t  val) {
    Port8Bit  p(nabmBase + reg); p.Write(val); }
void     AC97AudioDriver::writeNABM16(uint8_t reg, uint16_t val) {
    Port16Bit p(nabmBase + reg); p.Write(val); }
void     AC97AudioDriver::writeNABM32(uint8_t reg, uint32_t val) {
    Port32Bit p(nabmBase + reg); p.Write(val); }

uint8_t  AC97AudioDriver::readNABM8 (uint8_t reg) {
    Port8Bit  p(nabmBase + reg); return p.Read(); }
uint16_t AC97AudioDriver::readNABM16(uint8_t reg) {
    Port16Bit p(nabmBase + reg); return p.Read(); }
uint32_t AC97AudioDriver::readNABM32(uint8_t reg) {
    Port32Bit p(nabmBase + reg); return p.Read(); }


void AC97AudioDriver::resetCodec() {
    //global reset via NABM global control: set cold reset bit
    uint32_t ctrl = readNABM32(AC97_NABM_GLOB_CNT);
    ctrl |= 0x02;   //cold reset bit
    writeNABM32(AC97_NABM_GLOB_CNT, ctrl);

    for (volatile int i = 0; i < 10000; i++) readNABM8(AC97_NABM_GLOB_CNT);

    //write to NAM reset register (writing anything triggers a codec reset)
    writeNAM16(AC97_NAM_RESET, 0xFFFF);
    for (volatile int i = 0; i < 10000; i++) readNABM8(AC97_NABM_GLOB_CNT);
}

void AC97AudioDriver::setVolume(uint8_t master, uint8_t pcm) {
    // Bits[5:0] = right, bits[13:8] = left, bit 15 = mute
    uint16_t masterVol = ((master & 0x3F) << 8) | (master & 0x3F);
    uint16_t pcmVol    = ((pcm    & 0x1F) << 8) | (pcm    & 0x1F);  // PCM uses 5-bit
    writeNAM16(AC97_NAM_MASTER_VOL, masterVol);
    writeNAM16(AC97_NAM_PCM_VOL,    pcmVol);
}


bool AC97AudioDriver::setSampleRate(uint32_t hz) {
    //does codec supports vra
    uint16_t extId = readNAM16(AC97_NAM_EXT_AUDIO_ID);
    if (extId & 0x0001) {
        //enable vra
        uint16_t extCtrl = readNAM16(AC97_NAM_EXT_AUDIO_CTRL);
        extCtrl |= 0x0001;
        writeNAM16(AC97_NAM_EXT_AUDIO_CTRL, extCtrl);
        writeNAM16(AC97_NAM_SAMPLE_RATE, (uint16_t)hz);
        uint16_t actual = readNAM16(AC97_NAM_SAMPLE_RATE);
        if (actual != 0)
            sampleRate = actual;
        return (actual == (uint16_t)hz);
    }
    //locked to 48000 Hz (no vras)
    if (hz == 48000) {
        sampleRate = 48000;
        return true;
    }
    return false;
}

void AC97AudioDriver::setupBDL() {
    const uint16_t samplesPerBuffer = BUFFER_SIZE / 4;
    for (int i = 0; i < 2; i++) {
        bdl[i].address = (uint32_t)(size_t)audioBuffers[i];
        bdl[i].samples = samplesPerBuffer;
        bdl[i].flags   = AC97_BDL_FLAG_IOC;
    }
    writeNABM8(AC97_NABM_PCM_OUT + AC97_NABM_CR, AC97_CR_RR);
    for (volatile int i = 0; i < 1000; i++);
    writeNABM32(AC97_NABM_PCM_OUT + AC97_NABM_BDBAR, (uint32_t)(size_t)bdl);
    writeNABM8(AC97_NABM_PCM_OUT + AC97_NABM_LVI, 1);
    writeNABM8(AC97_NABM_PCM_OUT + AC97_NABM_CR,
               AC97_CR_IOCE | AC97_CR_FEIE | AC97_CR_LVBIE);
}

bool AC97AudioDriver::activate() {
    // Scan all PCI buses/devices/functions for class 04:01
    uint16_t bus, device, function;
    found = false;
    printf("AC97: scanning PCI for class 04:01\n");
    for (bus = 0; bus < 256 && !found; bus++) {
        for (device = 0; device < 32 && !found; device++) {
            for (function = 0; function < 8 && !found; function++) {
                uint32_t vendorDev = pci->Read(bus, device, function, 0x00);
                if ((vendorDev & 0xFFFF) == 0xFFFF)
                    continue;
                uint32_t classRev = pci->Read(bus, device, function, 0x08);
                uint8_t  classCode = (classRev >> 24) & 0xFF;
                uint8_t  subClass  = (classRev >> 16) & 0xFF;
                printf("  bus ");   ASLAC97.printfHex(bus);
                printf(" dev ");    ASLAC97.printfHex(device);
                printf(" fn ");     ASLAC97.printfHex(function);
                printf(" vend ");   ASLAC97.printfHex16(vendorDev & 0xFFFF);
                printf(" class ");  ASLAC97.printfHex(classCode);
                printf(" sub ");    ASLAC97.printfHex(subClass);
                printf("\n");

                if (classCode == 0x04 && subClass == 0x01) {
                    found = true;
                    uint32_t bar0 = pci->Read(bus, device, function, 0x10);
                    uint32_t bar1 = pci->Read(bus, device, function, 0x14);
                    namBase  = (uint16_t)(bar0 & 0xFFFC);
                    nabmBase = (uint16_t)(bar1 & 0xFFFC);
                    uint32_t cmd = pci->Read(bus, device, function, 0x04);
                    cmd |= 0x05;
                    pci->Write(bus, device, function, 0x04, cmd);
                    uint32_t intLine = pci->Read(bus, device, function, 0x3C);
                    uint8_t  irq = intLine & 0xFF;
                    setInterruptNumber(interruptManager->HardwareInterruptOffset() + irq);
                }
            }
        }
    }

    if (!found || namBase == 0 || nabmBase == 0) {
        found = false;
        return false;
    }
    printf("AC97: NAM 0x");
    ASLAC97.printfHex32(namBase);
    printf(" NABM 0x");
    ASLAC97.printfHex32(nabmBase);
    printf(" IRQ ");
    ASLAC97.printfHex32(InterruptNumber & 0xFF);
    printf("\n");
    resetCodec();
    setVolume(0, 0);
    writeNAM16(AC97_NAM_HEADPHONE_VOL, 0x0000);
    if (!setSampleRate(44100)) {
        setSampleRate(48000);
        sampleRate = 48000;
    }
    setupBDL();
    return true;
}

void AC97AudioDriver::play() {
    if (!found || running) return;
    running = true;
    uint8_t cr = readNABM8(AC97_NABM_PCM_OUT + AC97_NABM_CR);
    cr |= AC97_CR_RPBM;
    writeNABM8(AC97_NABM_PCM_OUT + AC97_NABM_CR, cr);
}

void AC97AudioDriver::stop() {
    running = false;
    if (!found) return;
    //clear RPBM bit
    uint8_t cr = readNABM8(AC97_NABM_PCM_OUT + AC97_NABM_CR);
    cr &= ~AC97_CR_RPBM;
    writeNABM8(AC97_NABM_PCM_OUT + AC97_NABM_CR, cr);
}

uint8_t* AC97AudioDriver::getNextBuffer(uint32_t* outSizeBytes) {
    uint8_t nextBuf = currentBuffer ^ 1;
    if (outSizeBytes) *outSizeBytes = BUFFER_SIZE;
    return audioBuffers[nextBuf];
}

void AC97AudioDriver::submitBuffer() {
    uint8_t nextBuf = currentBuffer ^ 1;
    bdl[nextBuf].address = (uint32_t)(size_t)audioBuffers[nextBuf];
    bdl[nextBuf].samples = BUFFER_SIZE / 4;
    bdl[nextBuf].flags   = AC97_BDL_FLAG_IOC;
    currentBuffer = nextBuf;
}

uint32_t AC97AudioDriver::HandleInterrupt(uint32_t esp) {
    uint16_t status = readNABM16(AC97_NABM_PCM_OUT + AC97_NABM_SR);

    if (status & AC97_SR_BCIS) {
        writeNABM16(AC97_NABM_PCM_OUT + AC97_NABM_SR, AC97_SR_BCIS);

        uint8_t civ    = readNABM8(AC97_NABM_PCM_OUT + AC97_NABM_CIV);
        uint8_t refilled = (civ == 0) ? 1 : (uint8_t)(civ - 1);

        if (fillCallback != 0) {
            fillCallback(audioBuffers[refilled], BUFFER_SIZE, sampleRate);
        } else {
            fillTestTone(audioBuffers[refilled], BUFFER_SIZE, sampleRate);
        }
    }

    if (status & AC97_SR_LVBCI) {
        writeNABM16(AC97_NABM_PCM_OUT + AC97_NABM_SR, AC97_SR_LVBCI);
        writeNABM8(AC97_NABM_PCM_OUT + AC97_NABM_LVI, 1);
    }

    if (status & AC97_SR_FIFOE) {
        writeNABM16(AC97_NABM_PCM_OUT + AC97_NABM_SR, AC97_SR_FIFOE);
    }

    return esp;
}

static int16_t intSin(uint32_t phase, uint32_t period) {
    static const int16_t sinTable[256] = {
           0,  804, 1608, 2410, 3212, 4011, 4808, 5602,
        6393, 7179, 7962, 8739, 9512,10278,11039,11793,
       12539,13279,14010,14732,15446,16151,16846,17530,
       18204,18868,19519,20159,20787,21403,22005,22594,
       23170,23731,24279,24811,25329,25832,26319,26790,
       27245,27683,28105,28510,28898,29268,29621,29956,
       30273,30571,30852,31113,31356,31580,31785,31971,
       32137,32285,32412,32521,32609,32678,32728,32757,
       32767,32757,32728,32678,32609,32521,32412,32285,
       32137,31971,31785,31580,31356,31113,30852,30571,
       30273,29956,29621,29268,28898,28510,28105,27683,
       27245,26790,26319,25832,25329,24811,24279,23731,
       23170,22594,22005,21403,20787,20159,19519,18868,
       18204,17530,16846,16151,15446,14732,14010,13279,
       12539,11793,11039,10278, 9512, 8739, 7962, 7179,
        6393, 5602, 4808, 4011, 3212, 2410, 1608,  804,
           0, -804,-1608,-2410,-3212,-4011,-4808,-5602,
       -6393,-7179,-7962,-8739,-9512,-10278,-11039,-11793,
      -12539,-13279,-14010,-14732,-15446,-16151,-16846,-17530,
      -18204,-18868,-19519,-20159,-20787,-21403,-22005,-22594,
      -23170,-23731,-24279,-24811,-25329,-25832,-26319,-26790,
      -27245,-27683,-28105,-28510,-28898,-29268,-29621,-29956,
      -30273,-30571,-30852,-31113,-31356,-31580,-31785,-31971,
      -32137,-32285,-32412,-32521,-32609,-32678,-32728,-32757,
      -32767,-32757,-32728,-32678,-32609,-32521,-32412,-32285,
      -32137,-31971,-31785,-31580,-31356,-31113,-30852,-30571,
      -30273,-29956,-29621,-29268,-28898,-28510,-28105,-27683,
      -27245,-26790,-26319,-25832,-25329,-24811,-24279,-23731,
      -23170,-22594,-22005,-21403,-20787,-20159,-19519,-18868,
      -18204,-17530,-16846,-16151,-15446,-14732,-14010,-13279,
      -12539,-11793,-11039,-10278, -9512, -8739, -7962, -7179,
       -6393, -5602, -4808, -4011, -3212, -2410, -1608, -804
    };

    uint32_t idx = (phase * 256) / period;
    return sinTable[idx & 255];
}

void AC97AudioDriver::fillTestTone(uint8_t* buffer, uint32_t sizeBytes, uint32_t hz) {
    const uint32_t frequency  = 440;
    const uint32_t period     = hz / frequency;
    const int16_t  amplitude  = 16000;

    int16_t* samples = (int16_t*)buffer;
    uint32_t numStereoSamples = sizeBytes / 4;

    for (uint32_t i = 0; i < numStereoSamples; i++) {
        int16_t s = (int16_t)((intSin(i % period, period) * (int32_t)amplitude) / 32767);
        samples[i * 2 + 0] = s;
        samples[i * 2 + 1] = s;
    }
}
