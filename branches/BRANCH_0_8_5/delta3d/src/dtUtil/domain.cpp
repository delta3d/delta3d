#include "dtUtil/domain.h"
#include <math.h>

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
   type = dtype;
   switch(type)
   {
   case DPoint:
      sgSetVec3(p1, a0, a1, a2);
      break;
   case DLine:
      {
         sgVec3 p1 = {a0, a1, a2};
         sgVec3 tmp = {a3,a4,a5};

         // p2 is vector from p1 to other endpoint.
         sgSubVec3(p2, tmp, p1);
      }
      break;
   case DBox:
      // p1 is the min corner. p2 is the max corner.
      if(a0 < a3)
      {
         p1[0] = a0; p2[0] = a3;
      }
      else
      {
         p1[0] = a3; p2[0] = a0;
      }
      if(a1 < a4)
      {
         p1[1] = a1; p2[1] = a4;
      }
      else
      {
         p1[1] = a4; p2[1] = a1;
      }
      if(a2 < a5)
      {
         p1[2] = a2; p2[2] = a5;
      }
      else
      {
         p1[2] = a5; p2[2] = a2;
      }
      break;
   case DTriangle:
      {
         sgSetVec3(p1, a0, a1, a2);

         sgVec3 tp2 = {a3, a4, a5};
         sgVec3 tp3 = {a6, a7, a8};

         sgSubVec3(u, tp2, p1);
         sgSubVec3(v, tp3, p1);

         // The rest of this is needed for bouncing.         
         radius1Sqr = sgLengthVec3(u);
         sgVec3 tu; sgScaleVec2(tu, u, 1.f/radius1Sqr);
         radius2Sqr = sgLengthVec3(v);
         sgVec3 tv; sgScaleVec3(tv, v, 1.f/radius2Sqr);

         // This is the non-unit normal.
         sgVectorProductVec3(p2, tu, tv);         
         sgNormalizeVec3(p2);// Must normalize it.

         // radius1 stores the d of the plane eqn.
         radius1 = -sgScalarProductVec3(p1, p2);
      }
      break;
   case DRectangle:
      {
         sgSetVec3(p1, a0, a1, a2);
         sgSetVec3(u, a3, a4, a5);
         sgVec3 v = {a6, a7, a8};
         
         // The rest of this is needed for bouncing.
         radius1Sqr = sgLengthVec3(u);

         //pVector tu = u / radius1Sqr;   
         sgVec3 tu; sgScaleVec3(tu, u, 1.f/radius1Sqr);

         radius2Sqr = sgLengthVec3(v);

         sgVec3 tv; sgScaleVec3(tv, v, 1.f/radius2Sqr);
         
         sgVectorProductVec3(p2, tu, tv);// This is the non-unit normal.

         sgNormalizeVec3(p2); // Must normalize it.
         
         // radius1 stores the d of the plane eqn.
         radius1 = -sgScalarProductVec3(p1, p2);         
      }
      break;
   case DPlane:
      {
         sgSetVec3(p1, a0, a1, a2);
         sgSetVec3(u, a3, a4, a5);
         sgSetVec3(v, a6, a7, a8);


         // The rest of this is needed for bouncing.
         radius1Sqr = sgLengthVec3(u);
         sgVec3 tu; sgScaleVec3(tu, u, 1.f/radius1Sqr);
         radius2Sqr = sgLengthVec3(v);
         sgVec3 tv; sgScaleVec3(tv, v, 1.f/radius2Sqr);

         sgVectorProductVec3(p2, tu, tv); // This is the non-unit normal.
         sgNormalizeVec3(p2);// Must normalize it.

         // radius1 stores the d of the plane eqn.
         radius1 = -sgScalarProductVec3(p1, p2);        
      }
      break;
   case DSphere:

      sgSetVec3(p1, a0, a1, a2);
      if(a3 > a4)
      {
         radius1 = a3; radius2 = a4;
      }
      else
      {
         radius1 = a4; radius2 = a3;
      }
      radius1Sqr = radius1 * radius1;
      radius2Sqr = radius2 * radius2;

      break;
   case DCone:
   case DCylinder:
      {
         // p2 is a vector from p1 to the other end of cylinder.
         // p1 is apex of cone.
         
         sgSetVec3(p1, a0, a1, a2);
         sgVec3 tmp = {a3, a4, a5};
         sgSubVec3(p2, tmp, p1);

         if(a6 > a7)
         {
            radius1 = a6; radius2 = a7;
         }
         else
         {
            radius1 = a7; radius2 = a6;
         }
         radius1Sqr = radius1*radius1;

         // Given an arbitrary nonzero vector n, make two orthonormal
         // vectors u and v forming a frame [u,v,n.normalize()].
         sgVec3 n; sgCopyVec3(n, p2);
         float p2l2 = sgLengthSquaredVec3(n);// Optimize this.
         sgNormalizeVec3(n);

         // radius2Sqr stores 1 / (p2.p2)
         // XXX Used to have an actual if.
         radius2Sqr = p2l2 ? 1.0f / p2l2 : 0.0f;

         // Find a vector orthogonal to n.
         sgVec3 basis = {1.f, 0.f, 0.f};
         if (fabsf(sgScalarProductVec3(basis, n) > 0.999))
            sgSetVec3(basis, 0.f, 1.f, 0.f);

         // Project away N component, normalize and cross to get
         // second orthonormal vector.
         //u = basis - n * (basis * n);
         float tmpFloat = sgScalarProductVec3(basis, n);
         sgVec3 tmpv; sgScaleVec3(tmpv, n, tmpFloat);
         sgSubVec3(u, basis, tmpv);
         sgNormalizeVec3(u);
         sgVectorProductVec3(v, n, u);         
      }
      break;
   case DBlob:
      {
         sgSetVec3(p1, a0, a1, a2);
         radius1 = a3;
         float tmp = 1.f/radius1;
         radius2Sqr = -0.5f*(tmp*tmp);
         radius2 = (float)ONEOVERSQRT2PI * tmp;     
      }
      break;
   case DDisc:
      {
         sgSetVec3( p1, a0, a1, a2); // Center point         
         sgSetVec3( p2, a3, a4, a5);// Normal (not used in Within and Generate)
         sgNormalizeVec3(p2);

         if(a6 > a7)
         {
            radius1 = a6; radius2 = a7;
         }
         else
         {
            radius1 = a7; radius2 = a6;
         }

         // Find a vector orthogonal to n.
         sgVec3 basis = {1.f, 0.f, 0.f};
         if (fabsf(sgScalarProductVec3(basis, p2) > 0.999))
            sgSetVec3(basis, 0.f, 1.f, 0.f);

         // Project away N component, normalize and cross to get
         // second orthonormal vector.
         //u = basis - p2 * (basis * p2);
         float tmp = sgScalarProductVec3(basis, p2);
         sgVec3 tmpv; sgScaleVec3(tmpv, p2, tmp);
         sgSubVec3(u, basis, tmpv);
         sgNormalizeVec3(u);
         sgVectorProductVec3(v, p2, u);
         radius1Sqr = -sgScalarProductVec3(p1, p2); // D of the plane eqn.
      }
      break;
   }
}

// Generate a random point uniformly distrbuted within the domain
void Domain::Generate(sgVec3 pos) const
{
   switch (type)
   {
   case DPoint:
      sgCopyVec3(pos, p1);
      break;
   case DLine:
      //pos = p1 + p2 * drand48();
      sgVec3 tmpP2;
      sgCopyVec3(tmpP2, p2);    sgScaleVec3(tmpP2, drand48() );
      sgAddVec3(pos, p1, tmpP2);
      break;
   case DBox:
      // Scale and translate [0,1] random to fit box
      pos[0] = p1[0] + (p2[0]-p1[0]) * drand48();
      pos[1] = p1[1] + (p2[1]-p1[1]) * drand48();
      pos[2] = p1[2] + (p2[2]-p1[2]) * drand48();
      break;
   case DTriangle:
      {
         float r1 = drand48();
         float r2 = drand48();
         if(r1 + r2 < 1.0f)
         {
           // pos = p1 + u * r1 + v * r2;
            sgAddScaledVec3(pos, p1, u, r1);
            sgAddScaledVec3(pos, v, r2);
         }
         else
         {
           // pos = p1 + u * (1.0f-r1) + v * (1.0f-r2);
            sgAddScaledVec3(pos, p1, u, (1.f-r1));
            sgAddScaledVec3(pos, v, (1.f-r2));
         }
      }
      break;
   case DRectangle:
      //pos = p1 + u * drand48() + v * drand48();
      sgAddScaledVec3(pos, p1, u, drand48());
      sgAddScaledVec3(pos, v, drand48());
      break;
   case DPlane: // How do I sensibly make a point on an infinite plane?
      //pos = p1;
      //sgCopyVec3(pos, p1);
      sgAddScaledVec3(pos, p1, u, drand48() );
      sgAddScaledVec3(pos, v, drand48() );
      break;
   case DSphere:
      {
         // Place on [-1..1] sphere
         sgVec3 randVec = {drand48(), drand48(), drand48()};
         sgVec3 vHalf = {0.5f, 0.5f, 0.5f};
         sgSubVec3(pos, randVec, vHalf);
         sgNormalizeVec3(pos);

         // Scale unit sphere pos by [0..r] and translate
         // (should distribute as r^2 law)
         if(radius1 == radius2)
         {
            //pos = p1 + pos * radius1;
            sgAddScaledVec3(pos, p1, pos, radius1);
         }
         else
         {
            //pos = p1 + pos * (radius2 + drand48() * (radius1 - radius2));
            sgAddScaledVec3(pos, p1, pos, (radius2 + drand48() * (radius1 - radius2)));
         }
      }
      break;

   case DCylinder:
   case DCone:
      {
         // For a cone, p2 is the apex of the cone.
         float dist = drand48(); // Distance between base and tip
         float theta = drand48() * 2.0f * float(SG_PI); // Angle around axis
         // Distance from axis
         float r = radius2 + drand48() * (radius1 - radius2);

         float x = r * cosf(theta); // Weighting of each frame vector
         float y = r * sinf(theta);

         // Scale radius along axis for cones
         if(type == DCone)
         {
            x *= dist;
            y *= dist;
         }

         //pos = p1 + p2 * dist + u * x + v * y;
         sgAddScaledVec3(pos, p1, p2, dist);
         sgAddScaledVec3(pos, u, x);
         sgAddScaledVec3(pos, v, y);
      }
      break;
   case DBlob:
      pos[0] = p1[0] + NRand(radius1);
      pos[1] = p1[1] + NRand(radius1);
      pos[2] = p1[2] + NRand(radius1);
      break;
   case DDisc:
      {
         float theta = drand48() * 2.0f * float(SG_PI); // Angle around normal
         // Distance from center
         float r = radius2 + drand48() * (radius1 - radius2);

         float x = r * cosf(theta); // Weighting of each frame vector
         float y = r * sinf(theta);

         //pos = p1 + u * x + v * y;
         sgAddScaledVec3(pos, p1, u, x);
         sgAddScaledVec3(pos, v, y);
      }
      break;
   default:
      sgZeroVec3(pos);
   }
}
