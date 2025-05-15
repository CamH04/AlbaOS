#ifndef __ALBAOS__COMMON__ASLMATHS_H
#define __ALBAOS__COMMON__ASLMATHS_H

#include <common/types.h>


namespace albaos
{
    namespace common
    {
        class asl_maths{
        public:

            const double pi = 3.14159265358979323846;

            uint16_t hash(char* cmd);
            uint32_t Trollfnv1a(char* str);
            double Random(void);
            uint16_t betterRandom();
            uint32_t abs(int32_t x);
            double absD(double x);
            double factorial(double x);
            double sin(double x);
            double power(double x, double power);
            double fmod(double a, double b);
            double cos(double x);




            struct point {

			common::int32_t x;
			common::int32_t y;

            };

            void LineFillLow(common::int32_t x0, common::int32_t y0, common::int32_t x1, common::int32_t y1,
				   struct point arr[], common::uint16_t &index);
            void LineFillHigh(common::int32_t x0, common::int32_t y0,common::int32_t x1, common::int32_t y1,
				   struct point arr[], common::uint16_t &index);
            uint16_t LineFillArray(int32_t x0, int32_t y0,int32_t x1, int32_t y1, struct point arr[]);

        };
    }
}

#endif
