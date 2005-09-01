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



#endif //MATHLIB_H