#ifndef __ALBAOS__DRIVERS__AC97_H
#define __ALBAOS__DRIVERS__AC97_H

#include "../hardwarecommunication/port.h"
#include "../hardwarecommunication/pci.h"
#include "../hardwarecommunication/interrupts.h"


#define AC97_NAM_RESET          0x00
#define AC97_NAM_MASTER_VOL     0x02
#define AC97_NAM_HEADPHONE_VOL  0x04
#define AC97_NAM_MONO_VOL       0x06
#define AC97_NAM_PCM_VOL        0x18
#define AC97_NAM_SAMPLE_RATE    0x2C
#define AC97_NAM_EXT_AUDIO_ID   0x28
#define AC97_NAM_EXT_AUDIO_CTRL 0x2A

#define AC97_NABM_PCM_OUT       0x10

#define AC97_NABM_BDBAR         0x00   // Buffer Descriptor Base Address (32-bit)
#define AC97_NABM_CIV           0x04   // Current Index Value          (8-bit)
#define AC97_NABM_LVI           0x05   // Last Valid Index             (8-bit)
#define AC97_NABM_SR            0x06   // Status Register              (16-bit)
#define AC97_NABM_PICB          0x08   // Position in Current Buffer   (16-bit)
#define AC97_NABM_PIV           0x0A   // Prefetched Index Value       (8-bit)
#define AC97_NABM_CR            0x0B   // Control Register             (8-bit)

#define AC97_NABM_GLOB_CNT      0x2C   // Global Control    (32-bit)
#define AC97_NABM_GLOB_STS      0x30   // Global Status     (32-bit)

#define AC97_CR_RPBM            (1 << 0)
#define AC97_CR_RR              (1 << 1)
#define AC97_CR_LVBIE           (1 << 2)
#define AC97_CR_FEIE            (1 << 3)
#define AC97_CR_IOCE            (1 << 4)
//status
#define AC97_SR_DCH             (1 << 0)   // DMA Controller Halted
#define AC97_SR_CELV            (1 << 1)   // Current Equals Last Valid
#define AC97_SR_LVBCI           (1 << 2)   // Last Valid Buffer Completion Interrupt
#define AC97_SR_BCIS            (1 << 3)   // Buffer Completion Interrupt Status
#define AC97_SR_FIFOE           (1 << 4)   // FIFO Error

#define AC97_BDL_ENTRIES        32

struct AC97BufferDescriptor {
    uint32_t address;    // physical address of the audio buffer
    uint16_t samples;    // num of samples in the buffer (PCM samples, not bytes)
    uint16_t flags;      // Bit 15 = IOC (interrupt on completion), Bit 14 = BUP (buffer underrun policy)
} __attribute__((packed));

#define AC97_BDL_FLAG_IOC   (1 << 15)
#define AC97_BDL_FLAG_BUP   (1 << 14)

namespace albaos {
namespace drivers {

class AC97AudioDriver : public albaos::hardwarecommunication::InterruptHandler {

private:
    albaos::hardwarecommunication::Port16Bit namPort;    // Mixer (NAM)
    albaos::hardwarecommunication::Port8Bit  nabmPort8;  // Bus-master 8-bit
    albaos::hardwarecommunication::Port16Bit nabmPort16; // Bus-master 16-bit
    albaos::hardwarecommunication::Port32Bit nabmPort32; // Bus-master 32-bit

    uint16_t namBase;    // I/O base for NAM  (from PCI BAR0)
    uint16_t nabmBase;   // I/O base for NABM (from PCI BAR1)

    AC97BufferDescriptor bdl[AC97_BDL_ENTRIES] __attribute__((aligned(8)));

    //double-buffered audio data (two 4 KB pages, ~1365 stereo 16-bit samples each)
    static const uint32_t BUFFER_SIZE = 4096;
    uint8_t audioBuffers[2][BUFFER_SIZE] __attribute__((aligned(4)));

    uint8_t currentBuffer;
    bool    running;

    void writeNAM16(uint8_t reg, uint16_t val);
    uint16_t readNAM16(uint8_t reg);

    void writeNABM8 (uint8_t reg, uint8_t  val);
    void writeNABM16(uint8_t reg, uint16_t val);
    void writeNABM32(uint8_t reg, uint32_t val);
    uint8_t  readNABM8 (uint8_t reg);
    uint16_t readNABM16(uint8_t reg);
    uint32_t readNABM32(uint8_t reg);

    void resetCodec();
    void setVolume(uint8_t master, uint8_t pcm);
    bool setSampleRate(uint32_t hz);
    void setupBDL();

public:
    AC97AudioDriver(albaos::hardwarecommunication::InterruptManager* interrupts,
                    albaos::hardwarecommunication::PCIController*     pci);
    ~AC97AudioDriver();
    bool activate();
    uint8_t* getNextBuffer(uint32_t* outSizeBytes);
    void submitBuffer();
    void play();
    void stop();
    bool isRunning() const { return running; }
    uint32_t handleInterrupt(uint32_t esp) override;
    void fillTestTone(uint8_t* buffer, uint32_t sizeBytes, uint32_t sampleRate = 44100);
};

} // namespace drivers
} // namespace albaos

#endif // __ALBAOS__DRIVERS__AC97_H
