#include<networking/eframe.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::networking;

void printf(char*);

EthernetFrameHandler::EthernetFrameHandler(EthernetFrameProv* backend , uint16_t EtherEtype){
    this-> EtherEtype_BigEndien = ((EtherEtype & 0x00FF)<< 8)
                                | ((EtherEtype & 0xFF00)>> 8); //conv to bigE
    this->backend = backend;
    backend->handlers[EtherEtype_BigEndien] = this;
}
EthernetFrameHandler::~EthernetFrameHandler(){
    backend->handlers[EtherEtype_BigEndien] = 0;
}
bool EthernetFrameHandler::OnEthernetFrameRecived(uint8_t* EtherFramePayload, uint32_t size){
    return false;
}
void EthernetFrameHandler::Send(uint64_t MacDestination_BigEndien, uint8_t* data, uint32_t size){
    backend->Send(MacDestination_BigEndien, EtherEtype_BigEndien, data, size);
}


//Provider

EthernetFrameProv::EthernetFrameProv(amd_am79c973* backend): RawDataHandler(backend){
    //set all handlers to 0 by looping through array of pointers to handlers
    printf("ETHFRAME: removing all handlers\n");
    for(uint32_t i = 0; i < 65535; i++){
        handlers[i] = 0;
    }
}
EthernetFrameProv::~EthernetFrameProv(){
}
bool EthernetFrameProv::OnRawDataReceived(uint8_t* buffer , uint32_t size){
    EtherFrameHeader* frame = (EtherFrameHeader*)buffer;
    bool sendBack = false;
    if(frame->MacDestination_BigEndien == 0xFFFFFFFFFFFF || frame->MacDestination_BigEndien == backend->GetMACAddress()){ //if broadcast or for us
        printf("ETHFRAME: recived :)\n");
        if(handlers[frame->EtherEtype_BigEndien] != 0){  //do we have a handler for frame type
            sendBack = handlers[frame->EtherEtype_BigEndien]->OnEthernetFrameRecived(buffer + sizeof(EtherFrameHeader), size - sizeof(EtherFrameHeader));
            //NOTE: we do not have to remove checksum here because its done in the driver for amd_am79c973
        }
    }
    if(sendBack){
        printf("ETHFRAME: switiching\n");
        // this avoids issues with sendbacks from a broadcast
        frame->MacDestination_BigEndien = frame->MacSource_BigEndien;
        frame->MacSource_BigEndien = backend->GetMACAddress();
    }
    return sendBack;
}
void EthernetFrameProv::Send(uint64_t MacDestination_BigEndien, uint16_t EtherEtype_BigEndien, uint8_t* buffer , uint32_t size){
    printf("ETHFRAME: malloc frame header\n");
    uint8_t* bufferNumber2 = (uint8_t*)MemoryManager::activeMemoryManager->malloc(sizeof(EtherFrameHeader)+size);
    EtherFrameHeader* frame = (EtherFrameHeader*)bufferNumber2;

    frame->MacDestination_BigEndien = MacDestination_BigEndien;
    frame->MacSource_BigEndien = backend->GetMACAddress();
    frame->EtherEtype_BigEndien = EtherEtype_BigEndien;
    printf("ETHFRAME: last send\n");
    uint8_t* src = buffer;
    uint8_t* dst = bufferNumber2 + sizeof(EtherFrameHeader);
    for(uint32_t i = 0; i < size; i++){
        dst[i] = src[i];
    }
    backend->Send(bufferNumber2, size + sizeof(EtherFrameHeader));


}
