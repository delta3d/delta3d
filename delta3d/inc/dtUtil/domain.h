/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DOMAIN_INCLUDED
#define DOMAIN_INCLUDED

#include "dtCore/export.h"

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
