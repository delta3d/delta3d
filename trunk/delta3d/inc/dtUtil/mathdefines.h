#ifndef __MATHDEFINES_H__
#define __MATHDEFINES_H__


//math defines

//returns absolute value
#define ABS(x) (( (x) < 0) ? (-1.0f * (x)) : (x))

//min and max defines
#define MIN(a,b) (( (a) < (b)) ? (a) : (b))
#define MAX(a,b) (( (a) > (b) ) ? (a) : (b))

#define CLAMP(x, a, b)\
   if((x) < (a)) (x) = (a);\
   if((x) > (b)) (x) = (b);


//a random number within a specified range
#define RAND_RANGE(min, max) ((min) + (rand() % (((max) - (min)) + 1)))

//a random float within range
#define RAND_FLOAT(min, max) ( (min) + (((rand() & 0x7FFF) / ((float)0x7FFF)) * ( (max) - (min)) )  )

// Returns a random float between 0 - 1
#define RAND_PERCENT() ((rand() & 0x7FFF) / ((float)0x7FFF))


//these are taken from
//http://developer.nvidia.com/object/fast_math_routines.html

#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000


// r = 1/p
#define FP_INV(r,p)                                                          \
{                                                                            \
   int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
   r = *(float *)&_i;                                                       \
   r = r * (2.0f - (p) * r);                                                \
}

#define FP_EXP(e,p)                                                          \
{                                                                            \
   int _i;                                                                  \
   e = -1.44269504f * (float)0x00800000 * (p);                              \
   _i = (int)e + 0x3F800000;                                                \
   e = *(float *)&_i;                                                       \
}

__forceinline void FloatToInt(int *int_pointer, float f) 
{
   __asm  fld  f
      __asm  mov  edx,int_pointer
      __asm  FRNDINT
      __asm  fistp dword ptr [edx];

}



#endif //MATHLIB_H