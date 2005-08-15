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

#include <dtCore/export.h>
#include <dtUtil/deprecationmgr.h>

#include <osg/Vec3>
#include <sg.h>

namespace dtUtil
{

   class Domain
   {

   public:

      // Type codes for domains
      DT_EXPORT enum DomainEnum
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

      DT_EXPORT void Generate(osg::Vec3&) const;
      DT_EXPORT void Generate(sgVec3 mV) const
      {
         DEPRECATE(  "void Generate(sgVec3) const",
            "void Generate(osg::Vec3&) const")
            osg::Vec3 tmp(mV[0], mV[1], mV[2]);
         Generate(tmp);
         mV[0] = tmp[0]; mV[1] = tmp[1]; mV[2] = tmp[2];
      }

      DT_EXPORT Domain( DomainEnum dType,
         float a0=0.0f, float a1=0.0f, float a2=0.0f,
         float a3=0.0f, float a4=0.0f, float a5=0.0f,
         float a6=0.0f, float a7=0.0f, float a8=0.0f);

      DT_EXPORT ~Domain() {}

   private:

      Domain() {};

      DomainEnum mType;	// PABoxDomain, PASphereDomain, PAConeDomain...
      osg::Vec3 mP1, mP2;		// Box vertices, Sphere center, Cylinder/Cone e`nds
      osg::Vec3 mU, mV;		// Orthonormal basis vectors for Cylinder/Cone
      float mRadius1;		// Outer radius
      float mRadius2;		// Inner radius
      float mRadius1Sqr;	// Used for fast Within test of spheres,
      float mRadius2Sqr;	// and for mag. of mU and mV vectors for plane.

   };
}
#endif
