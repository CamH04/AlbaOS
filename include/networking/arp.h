#ifndef __ALBAOS__ARP_H
#define __ALBAOS__ARP_H

#include<common/types.h>
#include<networking/eframe.h>

namespace albaos
{
    namespace networking
    {
         //1.Hows Is it being transsered, 2bytes, big endien
        //2.Protocal, 2bytes, big endien
        //3. Size of hardware address, 1byte
        //4. Size of logical address, 1byte
        //5. What does the sender want, 2bytes
        //6 Mac address of sender, 6bytes
        //7 IP of sender , 4bytes
        //8 Mac address of reciver, 6bytes
        //9 IP of reciver , 4bytes
        struct AddressResolutionProtocolMessage{
            common::uint16_t hardwareType;
            common::uint16_t protocol;
            common::uint8_t hardwareAddressSize;
            common::uint8_t protocolAddressSize;
            common::uint16_t command;
            common::uint64_t srcMAC : 48;
            common::uint32_t srcIP;
            common::uint64_t dstMAC : 48;
            common::uint32_t dstIP;

        } __attribute__((packed));



        class AddressResolutionProtocol : public albaos::networking::EtherFrameHandler{
            common::uint32_t IPcache[128];
            common::uint64_t MACcache[128];
            int numCacheEntries;

        public:
            AddressResolutionProtocol(EtherFrameProvider* backend);
            ~AddressResolutionProtocol();

            bool OnEtherFrameReceived(common::uint8_t* etherframePayload, common::uint32_t size);

            void RequestMACAddress(common::uint32_t IP_BE);
            common::uint64_t GetMACFromCache(common::uint32_t IP_BE);
            common::uint64_t Resolve(common::uint32_t IP_BE);
        };


    }
}


#endif
