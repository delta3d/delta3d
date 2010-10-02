#include <prefix/dtutilprefix.h>
#include <dtUtil/seamlessnoise.h>
#include <cstdlib>

namespace dtUtil
{

////////////////////////////////////////////////////////////////////////////////

int p[512];
int permutation[256];


SeamlessNoise::SeamlessNoise(unsigned int seed)
{
   BuildTable(seed);
   mDefaultRepeat = 1;
}


SeamlessNoise::~SeamlessNoise()
{

}


void SeamlessNoise::Reseed(unsigned int seed)
{
   BuildTable(seed);
}


float SeamlessNoise::Grad(int hash, float x, float y, float z)
{
   int   h = hash & 15;
   float u = h < 8 ? x : y,
         v = h < 4 ? y : h==12||h==14 ? x : z;
   return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}


void SeamlessNoise::BuildTable(unsigned int seed)
{
   srand(seed);

   int i, n, j;

   // Fill with zeros
   for (i = 0; i < 256; ++i)
   {
      permutation[i] = 0;
   }

   // Fill randomly the array with number form 0 to 255
   for (i = 0; i < 255; ++i)
   {
      n = rand() % 255;

      for (j = 0; j < 256; ++j)
      {

         if (permutation[j] == n)
         {
            n = rand() % 255;
            j=0;
         }
      }

      permutation[i]  = n;
      p[256+i] = p[i] = permutation[i];
   }
}


float SeamlessNoise::GetNoise(const osg::Vec3f& vect_in, int repeat)
{
   if (repeat == -1)
   {
      repeat = mDefaultRepeat;
   }

   float x = vect_in[0];
   float y = vect_in[1];
   float z = vect_in[2];


   int X = (int)floor(x) & 255,
       Y = (int)floor(y) & 255,
       Z = (int)floor(z) & 255;

   x -= floor(x);
   y -= floor(y);
   z -= floor(z);

   float  u = Fade(x),
      v = Fade(y),
      w = Fade(z);


   int Xmod = (X+1) % repeat;
   int Ymod = (Y+1) % repeat;
   int Zmod = (Z+1) % repeat;

   int A2 = (p[p[X]    + Y]   + Z ),
       A3 = (p[p[X]    + Y]   + Zmod),

       A5 = (p[p[X]    + Ymod]   + Z),
       A6 = (p[p[X]    + Ymod]   + Zmod),

       B2 = (p[p[Xmod] + Y]   + Z),
       B3 = (p[p[Xmod] + Y]   + Zmod),

       B5 = (p[p[Xmod] + Ymod]   + Z),
       B6 = (p[p[Xmod] + Ymod]   + Zmod);

#define LERP(t, a, b) ((a) + (t) * ((b) - (a)))

   return LERP(w,
          LERP(v,
          LERP(u, Grad(p[A2], x, y,   z),   Grad(p[B2], x-1, y,   z)),
          LERP(u, Grad(p[A5], x, y-1, z),   Grad(p[B5], x-1, y-1, z))),

          LERP(v,
          LERP(u, Grad(p[A3], x, y,   z-1), Grad(p[B3], x-1, y,   z-1)),
          LERP(u, Grad(p[A6], x, y-1, z-1), Grad(p[B6], x-1, y-1, z-1))));
}

////////////////////////////////////////////////////////////////////////////////

} // namespace dtUtil
