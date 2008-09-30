/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2008 MOVES Institute
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
* Michael Guerrero 
*/

#ifndef _POSE_MATH_H_
#define _POSE_MATH_H_

#include "export.h"

#include <osg/Vec3>
#include <osg/Vec2>

namespace dtAnim
{   
   /**  
   *  GetCelestialCoordinates - calculates the azimuth and elevation w.r.t. a forward vector
   *  @param targetDirection - a direction vector pointing to the point of interest
   *  @param lookForward - current forward vector 
   *  @param azimuth     - the horizontal angle between our forward and our target
   *  @param elevation   - the vertical angle between our forward and our target
   */
   void DT_ANIM_EXPORT GetCelestialCoordinates(osg::Vec3 targetDirection,
                                               const osg::Vec3 &lookForward,                                              
                                               float &azimuth, 
                                               float &elevation );

   /**
   *  GetCelestialDirection    - calculates the direction that a given azimuth and elevation points
   *  @param azimuth           - the horizontal angle of interest
   *  @param elevation         - the vertical angle of interest
   *  @param forwardDirection  - the current forward direction the azimuth and elevation are w.r.t 
   *  @param outDirection      - the direction that a given azimuth and elevation points 
   */
   void DT_ANIM_EXPORT GetCelestialDirection(const float azimuth,
                                             const float elevation,
                                             const osg::Vec3 &forwardDirection,
                                             const osg::Vec3 &upDirection,
                                             osg::Vec3 &outDirection);   

   /**
   *  GetClosestPointOnSegment - calculates the point on a segment that is closest to a reference point
   *  @param startPoint        - the start point of a line segment
   *  @param endPoint          - the end point of a line segment
   *  @param refPoint          - the point whose closest distance to the segment we are interested in
   *  @param closestPoint      - the final calculated point that is closest to refPoint
   */
   void DT_ANIM_EXPORT GetClosestPointOnSegment(const osg::Vec3 &startPoint,
                                                const osg::Vec3 &endPoint,
                                                const osg::Vec3 &refPoint,
                                                osg::Vec3 &closestPoint);     
   /**
   *  IsPointBetweenVectors - determines whether a points is between the vectors origin to A and origin to B
   *  @param point          - the point that we want to test 
   *  @param origin         - the point which is shared by both vectors
   *  @param A              - the end point for the first vector
   *  @param B              - the end point for the second vector
   */
   bool DT_ANIM_EXPORT IsPointBetweenVectors(const osg::Vec3f &point,
                                             const osg::Vec3f &origin,
                                             const osg::Vec3f &A,
                                             const osg::Vec3f &B); 

   /**
   *  MapCelestialToScreen - maps azimuth and elevation to a screen position
   *  @param azimuth       - the azimuth to be mapped
   *  @param elevation     - the elevation to be mapped
   *  @param windowWidth   - the desired max width of the screen sub window 
   *  @param windowHeight  - the desired max height of the screen sub window 
   *  @param screenOrigin  - the position on the screen from which the sub window begins
   *  @param outScreenPos  - the transformed final position in normalized screen coordinates
   */
   void DT_ANIM_EXPORT MapCelestialToScreen(float azimuth,
                                            float elevation,
                                            float maxDistance,                             
                                            float windowWidth,
                                            float windowHeight,
                                            const osg::Vec2 &screenOrigin,
                                            osg::Vec2 &outScreenPos);

}


#endif // _POSE_MATH_H_
