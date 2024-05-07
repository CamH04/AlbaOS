#ifndef __ALBAOS__MEMORYMANAGEMENT_H
#define __ALBAOS__MEMORYMANAGEMENT_H

#include <common/types.h>


namespace albaos
{

    struct MemoryChunk
    {
        MemoryChunk *next;
        MemoryChunk *prev;
        bool allocated;
        common::size_t size;
    };


    class MemoryManager
    {

    protected:
        MemoryChunk* first;
    public:

        static MemoryManager *activeMemoryManager;

        MemoryManager(common::size_t first, common::size_t size);
        ~MemoryManager();
        //i feel like a real developer now (i wrote the word malloc)
        void* malloc(common::size_t size);
        //how much is it? its free (freeing memory method im sorry for the bad joke)
        void free(void* ptr);
    };
}


void* operator new(unsigned size);
void* operator new[](unsigned size);

// in a new place 0v0
void* operator new(unsigned size, void* ptr);
void* operator new[](unsigned size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);


#endif
