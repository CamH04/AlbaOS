#ifndef __INTERRUPTSMANAGER_H
#define __INTERRUPTSMANAGER_H


    #include "types.h"
    #include "port.h"
    #include "gdt.h"

    class InterruptManager
    {
    protected:

        struct GateDiscriptor
        {
            uint16_t handlerAdressLowBits;
            uint16_t gdt_codeSegmentSelector;
            uint8_t reserved;
            uint8_t access;
            uint16_t handlerAdressHighBits;

        } __attribute__((packed));

        static GateDiscriptor interruptDescriptorTable[256];

        struct InterruptDescriptorTablePoniter
        {
            uint16_t size;
            uint32_t base;
        } __attribute__((packed));



        static void SetInterruptDescriptorTableEntry(uint8_t interrupt,
            uint16_t codeSegmentSelectorOffset, void (*handler)(),
            uint8_t DescriptorPrivilegeLevel, uint8_t DescriptorType);

        static void InterruptIgnore();

        static void HandleInterruptRequest 0x00();
        static void HandleInterruptRequest 0x01();
        static void HandleInterruptRequest 0x02();
        static void HandleInterruptRequest 0x03();
        static void HandleInterruptRequest 0x04();
        static void HandleInterruptRequest 0x05();
        static void HandleInterruptRequest 0x06();
        static void HandleInterruptRequest 0x07():
        static void HandleInterruptRequest 0x08();
        static void HandleInterruptRequest 0x09();
        static void HandleInterruptRequest 0x0A();
        static void HandleInterruptRequest 0x0B();
        static void HandleInterruptRequest 0x0C();
        static void HandleInterruptRequest 0x0D();
        static void HandleInterruptRequest 0x0E();
        static void HandleInterruptRequest 0x0F();
        static void HandleInterruptRequest 0x31();

        static void HandleException 0x00();
        static void HandleException 0x01();
        static void HandleException 0x02();
        static void HandleException 0x03();
        static void HandleException 0x04();
        static void HandleException 0x05();
        static void HandleException 0x06();
        static void HandleException 0x07();
        static void HandleException 0x08();
        static void HandleException 0x09();
        static void HandleException 0x0A();
        static void HandleException 0x0B();
        static void HandleException 0x0C();
        static void HandleException 0x0D();
        static void HandleException 0x0E();
        static void HandleException 0x0F();
        static void HandleException 0x10();
        static void HandleException 0x11();
        static void HandleException 0x12();
        static void HandleException 0x13();

        static uint32_t HandleInterrupt(uint8_t interrupt, uint32_t esp);

        Port8BitSlow programableInterruptControllerMasterCommandPort;
        Port8BitSlow programableInterruptControllerMasterDataPort;
        Port8BitSlow programableInterruptControllerSlaveCommandPort;
        Port8BitSlow programableInterruptControllerSlaveDataPort;


    public:

        InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable* globalDescriptorTable);
        ~InterruptManager();
        uint16_t HardwareInterruptOffset();
        void Activate();
        void Deactivate();
    };
#endif
