//give me that computers IP

#include<networking/arp.h>
#include<common/types.h>
#include<networking/eframe.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::networking;
AddressResolutionProtocol::AddressResolutionProtocol(EthernetFrameProv* backend)
            : EthernetFrameHandler(backend, 0x0608) // or 0x0806 depending on endianess
        {
            numCacheEntries = 0;
        }

        AddressResolutionProtocol::~AddressResolutionProtocol() {}

        bool AddressResolutionProtocol::OnEthernetFrameRecived(common::uint8_t* EtherFramePayload, common::uint32_t size) {
            if (size < sizeof(AddressResolutionProtocolMessage))
                return false; // ignore too small frames

            AddressResolutionProtocolMessage* arp = (AddressResolutionProtocolMessage*)EtherFramePayload;

            if (arp->hardwareType == 0x0100) {  // check your endian - 0x0001 in network byte order
                if (arp->protocolType == 0x0008 &&  // likely 0x0800 IP protocol, watch endian here
                    arp->hardwareAddressSize == 6 &&
                    arp->protocolAddressSize == 4 &&
                    arp->dstIP == backend->GetIPAddress()) {

                    switch (arp->command) {
                        case 0x0100: // ARP request (again, endian check!)
                            arp->command = 0x0200;  // ARP reply
                            arp->dstIP = arp->srcIP;
                            arp->dstMAC = arp->srcMAC;
                            arp->srcIP = backend->GetIPAddress();
                            arp->srcMAC = backend->GetMACAddress();

                            // Send the reply (you need to implement this Send function accordingly)
                            Send(arp->dstMAC, (common::uint8_t*)arp, sizeof(AddressResolutionProtocolMessage));
                            break;

                        case 0x0200: // ARP reply
                            // Cache the sender's IP and MAC address
                            if (numCacheEntries < 128) {
                                IPcache[numCacheEntries] = arp->srcIP;
                                MACcache[numCacheEntries] = arp->srcMAC;
                                numCacheEntries++;
                            }
                            break;
                    }
                    return true;
                }
            }
            return false;
        }

        void AddressResolutionProtocol::RequestMACAddress(common::uint32_t IP_BE) {
            AddressResolutionProtocolMessage arp = {0};

            arp.hardwareType = 0x0100;
            arp.protocolType = 0x0008;
            arp.hardwareAddressSize = 6;
            arp.protocolAddressSize = 4;
            arp.command = 0x0100;

            arp.srcMAC = backend->GetMACAddress();
            arp.srcIP = backend->GetIPAddress();

            arp.dstMAC = 0;
            arp.dstIP = IP_BE;

            Send(0xFFFFFFFFFFFF, (common::uint8_t*)&arp, sizeof(AddressResolutionProtocolMessage));
        }


uint64_t AddressResolutionProtocol::GetMACfromCache(uint32_t IP_BE){
    for(int i = 0; i < numCacheEntries; i++)
        if(IPcache[i] == IP_BE)
            return MACcache[i];
    return 0xFFFFFFFFFFFF;
}

uint64_t AddressResolutionProtocol::Resolve(common::uint32_t IP_BE){
    uint64_t result = GetMACfromCache(IP_BE);

    if(result == 0xFFFFFFFFFFFF){
        RequestMACAddress(IP_BE);
    }
    while(result == 0xFFFFFFFFFFFF){
        result = GetMACfromCache(IP_BE);
    }
    return result;
}

