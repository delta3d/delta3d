/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2006 MOVES Institute
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
* Michael Guerrero 4/01/2007
*/

#ifndef _pd_celestial_math_h_
#define _pd_celestial_math_h_

#include <osg/Vec3>
#include <osg/Vec2>

#include <dtUtil/export.h>

namespace dtUtil
{
   void DT_UTIL_EXPORT GetCelestialCoordinates(const osg::Vec3& reference_point,
                                               const osg::Vec3 &target_point,
                                               const osg::Vec3 &lookForward,
                                               const osg::Vec3 &lookUp,
                                               float &azimuth, float &elevation );

   void DT_UTIL_EXPORT GetCelestialDirection(const float azimuth,
                                             const float elevation,
                                             const osg::Vec3& forward_reference,
                                             osg::Vec3 &outDirection);

   /// Looks up a celestial triangle using azimuth and elevation
   // @return id if found, -1 otherwise
   int DT_UTIL_EXPORT FindCelestialTriangleID(const struct CelestialMesh &mesh, float azimuth, float elevation); 

   void DT_UTIL_EXPORT GetClosestPointOnSegment(const osg::Vec3 &startPoint,
                                                const osg::Vec3 &endPoint,
                                                const osg::Vec3 &refPoint,
                                                osg::Vec3 &closestPoint);  

   /// Determines if a point is between the vectors origin to A and origin to B
   bool DT_UTIL_EXPORT IsPointBetweenVectors(const osg::Vec3f &point,
                                             const osg::Vec3f &origin,
                                             const osg::Vec3f &A,
                                             const osg::Vec3f &B); 

   void DT_UTIL_EXPORT MapCelestialToScreen(float azimuth,
                                            float elevation,
                                            float maxDistance,                             
                                            float windowWidth,
                                            float windowHeight,
                                            const osg::Vec2 &screenOrigin,
                                            osg::Vec2 &outScreenPos);
}

#endif // _pd_celestial_math_h_
