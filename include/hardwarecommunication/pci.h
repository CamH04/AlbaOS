
#ifndef __ALBAOS__HARDWARECOMMUNICATION__PCI_H
#define __ALBAOS__HARDWARECOMMUNICATION__PCI_H

#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>

namespace albaos
{
    namespace hardwarecommunication
    {

        class PeripheralComponentInterconnectDeviceDescriptor
        {
        public:
            albaos::common::uint32_t portBase;
            albaos::common::uint32_t interrupt;

            albaos::common::uint16_t bus;
            albaos::common::uint16_t device;
            albaos::common::uint16_t function;

            albaos::common::uint16_t vendor_id;
            albaos::common::uint16_t device_id;

            albaos::common::uint8_t class_id;
            albaos::common::uint8_t subclass_id;
            albaos::common::uint8_t interface_id;

            albaos::common::uint8_t revision;

            PeripheralComponentInterconnectDeviceDescriptor();
            ~PeripheralComponentInterconnectDeviceDescriptor();

        };


        class PeripheralComponentInterconnectController
        {
            Port32Bit dataPort;
            Port32Bit commandPort;

        public:
            PeripheralComponentInterconnectController();
            ~PeripheralComponentInterconnectController();

            albaos::common::uint32_t Read(albaos::common::uint16_t bus, albaos::common::uint16_t device, albaos::common::uint16_t function, albaos::common::uint32_t registeroffset);
            void Write(albaos::common::uint16_t bus, albaos::common::uint16_t device, albaos::common::uint16_t function, albaos::common::uint32_t registeroffset, albaos::common::uint32_t value);
            bool DeviceHasFunctions(albaos::common::uint16_t bus, albaos::common::uint16_t device);

            void SelectDrivers(albaos::drivers::DriverManager* driverManager);
            PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(albaos::common::uint16_t bus, albaos::common::uint16_t device, albaos::common::uint16_t function);
        };

    }
}

#endif
