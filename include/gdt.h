#ifndef __ALBAOS__GDT_H
#define __ALBAOS__GDT_H

#include <common/types.h>

namespace albaos
{

    class GlobalDescriptorTable
    {
        public:

            class SegmentDescriptor
            {
                private:
                    albaos::common::uint16_t limit_lo;
                    albaos::common::uint16_t base_lo;
                    albaos::common::uint8_t base_hi;
                    albaos::common::uint8_t type;
                    albaos::common::uint8_t limit_hi;
                    albaos::common::uint8_t base_vhi;

                public:
                    SegmentDescriptor(albaos::common::uint32_t base, albaos::common::uint32_t limit, albaos::common::uint8_t type);
                    albaos::common::uint32_t Base();
                    albaos::common::uint32_t Limit();
            } __attribute__((packed));

        private:
            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;

        public:

            GlobalDescriptorTable();
            ~GlobalDescriptorTable();

            albaos::common::uint16_t CodeSegmentSelector();
            albaos::common::uint16_t DataSegmentSelector();
    };

}

#endif
