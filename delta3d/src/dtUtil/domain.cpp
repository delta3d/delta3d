#include "dtUtil/domain.h"
#include <cstdlib>

#ifdef WIN32
#define drand48() (((float) rand())/((float) RAND_MAX))
#define srand48(x) srand(x)

// This is because their stupid compiler thinks it's smart.
#define inline __forceinline
#endif


#define SQRT2PI 2.506628274631000502415765284811045253006
#define ONEOVERSQRT2PI (1.0 / SQRT2PI)

using namespace dtUtil;


// Return a random number with a normal distribution.
static inline float NRand(float sigma = 1.0f)
{
#define ONE_OVER_SIGMA_EXP (1.0f / 0.7975f)

   if(sigma == 0) return 0;

   float y;
   do
   {
      y = -logf(drand48());
   }
   while(drand48() > expf(-((y - 1.0f)*(y - 1.0f))*0.5f));

   if(rand() & 0x1)
      return y * sigma * ONE_OVER_SIGMA_EXP;
   else
      return -y * sigma * ONE_OVER_SIGMA_EXP;
}

Domain::Domain(DomainEnum dtype, float a0, float a1,
                                 float a2, float a3, float a4, float a5,
                                 float a6, float a7, float a8)
{
   mType = dtype;
   switch(mType)
   {
   case DPoint:
      mP1.set(a0, a1, a2);
      break;
   case DLine:
      {
         osg::Vec3 mP1(a0, a1, a2);
         osg::Vec3 tmp(a3,a4,a5);

         // mP2 is vector from mP1 to other endpoint.
         mP2 = tmp - mP1;
      }
      break;
   case DBox:
      // mP1 is the min corner. mP2 is the max corner.
      if(a0 < a3)
      {
         mP1[0] = a0; mP2[0] = a3;
      }
      else
      {
         mP1[0] = a3; mP2[0] = a0;
      }
      if(a1 < a4)
      {
         mP1[1] = a1; mP2[1] = a4;
      }
      else
      {
         mP1[1] = a4; mP2[1] = a1;
      }
      if(a2 < a5)
      {
         mP1[2] = a2; mP2[2] = a5;
      }
      else
      {
         mP1[2] = a5; mP2[2] = a2;
      }
      break;
   case DTriangle:
      {
         mP1.set(a0, a1, a2);

         osg::Vec3 tp2(a3, a4, a5);
         osg::Vec3 tp3(a6, a7, a8);
         
         mU = tp2 - mP1;
         mV = tp3 - mP1;

         // The rest of this is needed for bouncing.         
         mRadius1Sqr = mU.length();
         osg::Vec3 tu = mU / mRadius1Sqr; 
         mRadius2Sqr = mV.length();
         osg::Vec3 tv = mV / mRadius2Sqr; 

         // This is the non-unit normal.
         mP2 = tu ^ tv;         
         mP2.normalize();

         // mRadius1 stores the d of the plane eqn.
         mRadius1 = -(mP1 * mP2);
      }
      break;
   case DRectangle:
      {
         mP1.set(a0, a1, a2);
         mU.set(a3, a4, a5);
         osg::Vec3 mV(a6, a7, a8);
         
         // The rest of this is needed for bouncing.
         mRadius1Sqr = mU.length();
  
         osg::Vec3 tu = mU / mRadius1Sqr;

         mRadius2Sqr = mV.length();

         osg::Vec3 tv = mV / mRadius2Sqr; 
         
         mP2 = tu ^ tv;// This is the non-unit normal.

         mP2.normalize(); // Must normalize it.
         
         // mRadius1 stores the d of the plane eqn.
         mRadius1 = -(mP1 * mP2);         
      }
      break;
   case DPlane:
      {
         mP1.set(a0, a1, a2);
         mP1.set( a0, a1, a2);
         mU.set(a3, a4, a5);
         mV.set(a6, a7, a8);


         // The rest of this is needed for bouncing.
         mRadius1Sqr = mU.length();
         osg::Vec3 tu = mU / mRadius1Sqr;
         mRadius2Sqr = mV.length();
         osg::Vec3 tv = mV / mRadius2Sqr;

         mP2 = tu ^ tv; // This is the non-unit normal.
         mP2.normalize();// Must normalize it.

         // mRadius1 stores the d of the plane eqn.
         mRadius1 = -mP1 * mP2;        
      }
      break;
   case DSphere:

      mP1.set(a0, a1, a2);
      if(a3 > a4)
      {
         mRadius1 = a3; mRadius2 = a4;
      }
      else
      {
         mRadius1 = a4; mRadius2 = a3;
      }
      mRadius1Sqr = mRadius1 * mRadius1;
      mRadius2Sqr = mRadius2 * mRadius2;

      break;
   case DCone:
   case DCylinder:
      {
         // mP2 is a vector from mP1 to the other end of cylinder.
         // mP1 is apex of cone.
         
         mP1.set(a0, a1, a2);
         osg::Vec3 tmp(a3, a4, a5);
         mP2 = tmp - mP1;

         if(a6 > a7)
         {
            mRadius1 = a6; mRadius2 = a7;
         }
         else
         {
            mRadius1 = a7; mRadius2 = a6;
         }
         mRadius1Sqr = mRadius1*mRadius1;

         // Given an arbitrary nonzero vector n, make two orthonormal
         // vectors mU and mV forming a frame [mU,mV,n.normalize()].
         osg::Vec3 n = mP2;
         float p2l2 = n.length2();// Optimize this.
         n.normalize();

         // mRadius2Sqr stores 1 / (mP2.mP2)
         // XXX Used to have an actual if.
         mRadius2Sqr = p2l2 ? 1.0f / p2l2 : 0.0f;

         // Find a vector orthogonal to n.
         osg::Vec3 basis(1.f, 0.f, 0.f);
         if (fabsf(basis * n) > 0.999)
            basis.set(0.f, 1.f, 0.f);

         // Project away N component, normalize and cross to get
         // second orthonormal vector.
         //mU = basis - n * (basis * n);
         float tmpFloat = basis * n;
         osg::Vec3 tmpv = n * tmpFloat;
         mU = basis - tmpv;
         mU.normalize();
         mV = n ^ mU;         
      }
      break;
   case DBlob:
      {
         mP1.set(a0, a1, a2);
         mRadius1 = a3;
         float tmp = 1.f/mRadius1;
         mRadius2Sqr = -0.5f*(tmp*tmp);
         mRadius2 = (float)ONEOVERSQRT2PI * tmp;     
      }
      break;
   case DDisc:
      {
         mP1.set(a0, a1, a2); // Center point         
         mP2.set(a3, a4, a5);// Normal (not used in Within and Generate)
         mP2.normalize();

         if(a6 > a7)
         {
            mRadius1 = a6; mRadius2 = a7;
         }
         else
         {
            mRadius1 = a7; mRadius2 = a6;
         }

         // Find a vector orthogonal to n.
         osg::Vec3 basis(1.f, 0.f, 0.f);
         if (fabsf(basis * mP2) > 0.999)
            basis.set(0.f, 1.f, 0.f);

         // Project away N component, normalize and cross to get
         // second orthonormal vector.
         //mU = basis - mP2 * (basis * mP2);
         float tmp = basis * mP2;
         osg::Vec3 tmpv = mP2 * tmp;
         mU = basis - tmpv;
         mU.normalize();
         mV = mP2 ^ mU;
         mRadius1Sqr = -mP1 * mP2; // D of the plane eqn.
      }
      break;
   }
}

// Generate a random point uniformly distrbuted within the domain
void Domain::Generate(osg::Vec3& pos) const
{
   switch (mType)
   {
   case DPoint:
      pos.set(mP1);
      break;
   case DLine:
      pos = mP1 + (mP2 * drand48());
      break;
   case DBox:
      // Scale and translate [0,1] random to fit box
      pos[0] = mP1[0] + (mP2[0]-mP1[0]) * drand48();
      pos[1] = mP1[1] + (mP2[1]-mP1[1]) * drand48();
      pos[2] = mP1[2] + (mP2[2]-mP1[2]) * drand48();
      break;
   case DTriangle:
      {
         float r1 = drand48();
         float r2 = drand48();
         if(r1 + r2 < 1.0f)
         {
            pos = mP1 + (mU * r1) + (mV * r2);
         }
         else
         {
           pos = mP1 + (mU * (1.0f-r1)) + (mV * (1.0f-r2));
         }
      }
      break;
   case DRectangle:
      pos = mP1 + mU * drand48() + mV * drand48();
      break;
   case DPlane: // How do I sensibly make a point on an infinite plane?
      //pos = mP1;
      //sgCopyVec3(pos, mP1);
      pos = mP1 + (mU * drand48())+ (mV * drand48());
      break;
   case DSphere:
      {
         // Place on [-1..1] sphere
         osg::Vec3 randVec(drand48(), drand48(), drand48());
         osg::Vec3 vHalf(0.5f, 0.5f, 0.5f);
         pos = randVec - vHalf;
         pos.normalize();

         // Scale unit sphere pos by [0..r] and translate
         // (should distribute as r^2 law)
         if(mRadius1 == mRadius2)
         {
            pos = mP1 + pos * mRadius1;
         }
         else
         {
            pos = mP1 + pos * (mRadius2 + drand48() * (mRadius1 - mRadius2));
         }
      }
      break;

   case DCylinder:
   case DCone:
      {
         // For a cone, mP2 is the apex of the cone.
         float dist = drand48(); // Distance between base and tip
         float theta = drand48() * 2.0f * float(osg::PI); // Angle around axis
         // Distance from axis
         float r = mRadius2 + drand48() * (mRadius1 - mRadius2);

         float x = r * cosf(theta); // Weighting of each frame vector
         float y = r * sinf(theta);

         // Scale radius along axis for cones
         if(mType == DCone)
         {
            x *= dist;
            y *= dist;
         }

         pos = mP1 + mP2 * dist + mU * x + mV * y;
      }
      break;
   case DBlob:
      pos[0] = mP1[0] + NRand(mRadius1);
      pos[1] = mP1[1] + NRand(mRadius1);
      pos[2] = mP1[2] + NRand(mRadius1);
      break;
   case DDisc:
      {
         float theta = drand48() * 2.0f * float(osg::PI); // Angle around normal
         // Distance from center
         float r = mRadius2 + drand48() * (mRadius1 - mRadius2);

         float x = r * cosf(theta); // Weighting of each frame vector
         float y = r * sinf(theta);

         pos = mP1 + mU * x + mV * y;
      }
      break;
   default:
      pos.set(0.0f, 0.0f, 0.0f);
   }
}
