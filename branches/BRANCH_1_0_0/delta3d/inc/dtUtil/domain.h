#ifndef DOMAIN_INCLUDED
#define DOMAIN_INCLUDED

#include "export.h"

#include "sg.h"

namespace dtUtil
{

   struct DT_EXPORT Domain
   {
      // Type codes for domains
      enum DomainEnum
      {
         DPoint = 0, // Single point
         DLine = 1, // Line segment
         DTriangle = 2, // Triangle
         DPlane = 3, // Arbitrarily-oriented plane
         DBox = 4, // Axis-aligned box
         DSphere = 5, // Sphere
         DCylinder = 6, // Cylinder
         DCone = 7, // Cone
         DBlob = 8, // Gaussian blob
         DDisc = 9, // Arbitrarily-oriented disc
         DRectangle = 10 // Rhombus-shaped planar region
      };

      DomainEnum type;	// PABoxDomain, PASphereDomain, PAConeDomain...
      sgVec3 p1, p2;		// Box vertices, Sphere center, Cylinder/Cone e`nds
      sgVec3 u, v;		// Orthonormal basis vectors for Cylinder/Cone
      float radius1;		// Outer radius
      float radius2;		// Inner radius
      float radius1Sqr;	// Used for fast Within test of spheres,
      float radius2Sqr;	// and for mag. of u and v vectors for plane.

      void Generate(sgVec3 ) const;

      inline Domain(void){};

      Domain( DomainEnum dType,
         float a0=0.0f, float a1=0.0f, float a2=0.0f,
         float a3=0.0f, float a4=0.0f, float a5=0.0f,
         float a6=0.0f, float a7=0.0f, float a8=0.0f);
   };
}
#endif
