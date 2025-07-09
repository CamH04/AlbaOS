#ifndef __ALBAOS__EFRAME_H
#define __ALBAOS__EFRAME_H

#include <common/types.h>
#include <drivers/amd_am79c973.h>
#include <memorymanagement.h>

namespace albaos {
    namespace networking {
        //layer over header of ethernet frame
        struct EtherFrameHeader{
            common::uint64_t MacDestination_BigEndien : 48;
            common::uint64_t MacSource_BigEndien : 48;
            common::uint16_t EtherEtype_BigEndien;
        }__attribute__((packed));

        typedef common::uint32_t EtherFrameFooter;

        class EthernetFrameProv;
        class EthernetFrameHandler{
        protected:
            EthernetFrameProv* backend;
            common::uint16_t EtherEtype_BigEndien;
        public:
            EthernetFrameHandler(EthernetFrameProv* backend , common::uint16_t EtherEtype);
            ~EthernetFrameHandler();
            bool OnEthernetFrameRecived(common::uint8_t* EtherFramePayload, common::uint32_t size);
            void Send(common::uint64_t MacDestination_BigEndien, common::uint8_t* EtherFramePayload, common::uint32_t size);
        };
        class EthernetFrameProv : public albaos::drivers::RawDataHandler{
            friend class EthernetFrameHandler; // We are friends yippie! :D
        protected:
            EthernetFrameHandler* handlers[65535];
        public:
            EthernetFrameProv(albaos::drivers::amd_am79c973* backend);
            ~EthernetFrameProv();
            bool OnRawDataReceived(common::uint8_t* buffer , common::uint32_t size);
            void Send(common::uint64_t MacDestination_BigEndien, common::uint16_t EtherEtype_BigEndien , common::uint8_t* buffer , common::uint32_t size);
            common::uint32_t GetIPAddress();
            common::uint64_t GetMACAddress();


        };
    }
}

#endif
