
#ifndef __ALBAOS__HARDWARECOMMUNICATION__INTERRUPTMANAGER_H
#define __ALBAOS__HARDWARECOMMUNICATION__INTERRUPTMANAGER_H

#include <gdt.h>
#include <multitasking.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>


namespace albaos
{
    namespace hardwarecommunication
    {

        class InterruptManager;

        class InterruptHandler
        {
        protected:
            albaos::common::uint8_t InterruptNumber;
            InterruptManager* interruptManager;
            InterruptHandler(InterruptManager* interruptManager, albaos::common::uint8_t InterruptNumber);
            ~InterruptHandler();
        public:
            virtual albaos::common::uint32_t HandleInterrupt(albaos::common::uint32_t esp);
        };


        class InterruptManager
        {
            friend class InterruptHandler;
            protected:

                static InterruptManager* ActiveInterruptManager;
                InterruptHandler* handlers[256];

                TaskManager *taskManager;

                struct GateDescriptor
                {
                    albaos::common::uint16_t handlerAddressLowBits;
                    albaos::common::uint16_t gdt_codeSegmentSelector;
                    albaos::common::uint8_t reserved;
                    albaos::common::uint8_t access;
                    albaos::common::uint16_t handlerAddressHighBits;
                } __attribute__((packed));

                static GateDescriptor interruptDescriptorTable[256];

                struct InterruptDescriptorTablePointer
                {
                    albaos::common::uint16_t size;
                    albaos::common::uint32_t base;
                } __attribute__((packed));

                albaos::common::uint16_t hardwareInterruptOffset;
                static void SetInterruptDescriptorTableEntry(albaos::common::uint8_t interrupt,
                    albaos::common::uint16_t codeSegmentSelectorOffset, void (*handler)(),
                    albaos::common::uint8_t DescriptorPrivilegeLevel, albaos::common::uint8_t DescriptorType);


                static void InterruptIgnore();

                static void HandleInterruptRequest0x00();
                static void HandleInterruptRequest0x01();
                static void HandleInterruptRequest0x02();
                static void HandleInterruptRequest0x03();
                static void HandleInterruptRequest0x04();
                static void HandleInterruptRequest0x05();
                static void HandleInterruptRequest0x06();
                static void HandleInterruptRequest0x07();
                static void HandleInterruptRequest0x08();
                static void HandleInterruptRequest0x09();
                static void HandleInterruptRequest0x0A();
                static void HandleInterruptRequest0x0B();
                static void HandleInterruptRequest0x0C();
                static void HandleInterruptRequest0x0D();
                static void HandleInterruptRequest0x0E();
                static void HandleInterruptRequest0x0F();
                static void HandleInterruptRequest0x31();

                static void HandleInterruptRequest0x80();


                static void HandleException0x00();
                static void HandleException0x01();
                static void HandleException0x02();
                static void HandleException0x03();
                static void HandleException0x04();
                static void HandleException0x05();
                static void HandleException0x06();
                static void HandleException0x07();
                static void HandleException0x08();
                static void HandleException0x09();
                static void HandleException0x0A();
                static void HandleException0x0B();
                static void HandleException0x0C();
                static void HandleException0x0D();
                static void HandleException0x0E();
                static void HandleException0x0F();
                static void HandleException0x10();
                static void HandleException0x11();
                static void HandleException0x12();
                static void HandleException0x13();

                static albaos::common::uint32_t HandleInterrupt(albaos::common::uint8_t interrupt, albaos::common::uint32_t esp);
                albaos::common::uint32_t DoHandleInterrupt(albaos::common::uint8_t interrupt, albaos::common::uint32_t esp);

                Port8BitSlow programmableInterruptControllerMasterCommandPort;
                Port8BitSlow programmableInterruptControllerMasterDataPort;
                Port8BitSlow programmableInterruptControllerSlaveCommandPort;
                Port8BitSlow programmableInterruptControllerSlaveDataPort;

            public:
                InterruptManager(albaos::common::uint16_t hardwareInterruptOffset, albaos::GlobalDescriptorTable* globalDescriptorTable, albaos::TaskManager* taskManager);
                ~InterruptManager();
                albaos::common::uint16_t HardwareInterruptOffset();
                void Activate();
                void Deactivate();
        };

    }
}

#endif
