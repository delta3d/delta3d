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

#ifndef DELTA_INFINITETERRAIN
#define DELTA_INFINITETERRAIN

// infiniteterrain.h: Declaration of the InfiniteTerrain class.
//
//////////////////////////////////////////////////////////////////////

#include <osg/Group>
#include <osg/LOD>
#include <osg/Node>
#include <osg/Vec3>

#include "sg.h"

#include "dtCore/physical.h"
#include "dtUtil/deprecationmgr.h"

namespace dtCore
{
   class InfiniteTerrainCallback;
   
   
   /**
    * An infinite terrain surface.
    */
   class DT_EXPORT InfiniteTerrain : public Physical                                     
   {
      friend class InfiniteTerrainCallback;
      
      
      DECLARE_MANAGEMENT_LAYER(InfiniteTerrain)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          * @param textureImage An image to apply to the terrain
          */
         InfiniteTerrain(const std::string& name = "infiniteTerrain",
                         osg::Image* textureImage = 0);

         /**
          * Destructor.
          */
         virtual ~InfiniteTerrain();

         /**
          * Returns this object's OpenSceneGraph node.
          *
          * @return the OpenSceneGraph node
          */
         //virtual osg::Node* GetOSGNode();
         
         /**
          * Regenerates the terrain surface.
          */
         void Regenerate();
         
         /**
          * Sets the size of the terrain segments. (def  = 1000.0)
          *
          * @param segmentSize the new segment size
          */
         void SetSegmentSize(float segmentSize);
         
         /**
          * Returns the size of the terrain segments.
          *
          * @return the current segment size
          */
         float GetSegmentSize() const;
         
         /**
          * Sets the number of divisions in each segment. (def = 128)
          *
          * @param segmentDivisions the new segment divisions
          */
         void SetSegmentDivisions(int segmentDivisions);
         
         /**
          * Returns the number of divisions in each segment.
          *
          * @return the current segment divisions
          */
         int GetSegmentDivisions() const;
         
         /**
          * Sets the horizontal scale, which affects the
          * feature frequency. (def = 0.01)
          *
          * @param horizontalScale the new horizontal scale
          */
         void SetHorizontalScale(float horizontalScale);
         
         /**
          * Returns the horizontal scale.
          *
          * @return the horizontal scale
          */
         float GetHorizontalScale() const;
         
         /**
          * Sets the vertical scale, which affects the feature
          * amplitude. (def = 25.0) 
          *
          * @param verticalScale the new vertical scale
          */
         void SetVerticalScale(float verticalScale);
         
         /**
          * Returns the vertical scale.
          *
          * @return the vertical scale
          */
         float GetVerticalScale() const;

         /**
          * Sets the build distance: the distance from the eyepoint for
          * which terrain is guaranteed to be generated. (def = 3000.0)
          *
          * @param buildDistance the new build distance
          */
         void SetBuildDistance(float buildDistance);
         
         /**
          * Returns the build distance.
          *
          * @return the build distance
          */
         float GetBuildDistance() const;

         /**
          * Enables or disables smooth collision detection (collision detection
          * based on the underlying noise function, rather than the triangle
          * mesh).
          *
          * @param enable true to enable, false to disable
          */
         void EnableSmoothCollisions(bool enable);
         
         /**
          * Checks whether smooth collision detection is enabled.
          *
          * @return true if enabled, false if disabled
          */
         bool SmoothCollisionsEnabled() const;
         
         /**
          * Determines the height of the terrain at the specified location.
          *
          * @param x the x coordinate to check
          * @param y the y coordinate to check
          * @param smooth if true, use height of underlying noise function
          * instead of triangle mesh height
          * @return the height at the specified location
          */
         float GetHeight(float x, float y, bool smooth = false);
         
         /**
          * Retrieves the normal of the terrain at the specified location.
          *
          * @param x the x coordinate to check
          * @param y the y coordinate to check
          * @param normal the location at which to store the normal
          * @param smooth if true, use height of underlying noise function
          * instead of triangle mesh height
          */
         void GetNormal(float x, float y, osg::Vec3& normal, bool smooth = false);
 
         //depreciated version
         void GetNormal(float x, float y, sgVec3 normal, bool smooth = false)
         {
            DEPRECATE("void GetNormal(float x, float y, sgVec3 normal, bool smooth = false)", "void GetNormal(float x, float y, osg::Vec3& normal, bool smooth = false)")
            osg::Vec3 tmp(normal[0], normal[1], normal[2]);
            GetNormal(x, y, tmp, smooth);
            normal[0] = tmp[0]; normal[1] = tmp[1]; normal[2] = tmp[2];
         }
            

      private:

         /**
          * Builds a single terrain segment.
          *
          * @param x the x coordinate at which to build the segment
          * @param y the y coordinate at which to build the segment
          */
         void BuildSegment(int x, int y);

         /**
          * ODE collision function: Gets the contact points between two
          * geoms.
          *
          * @param o1 the first (InfiniteTerrain) geom
          * @param o2 the second geom
          * @param flags collision flags
          * @param contact the array of contact geoms to fill
          * @param skip the distance between contact geoms in the array
          * @return the number of contact points generated
          */
         static int Collider(dGeomID o1, dGeomID o2, int flags,
                             dContactGeom* contact, int skip);
         
         /**
          * ODE collision function: Finds the collider function appropriate
          * to detect collisions between InfiniteTerrain geoms and other
          * geoms.
          *
          * @param num the class number of the geom class to check
          * @return the appropriate collider function, or NULL for none
          */
         static dColliderFn* GetColliderFn(int num);
         
         /**
          * ODE collision function: Computes the axis-aligned bounding box
          * for InfiniteTerrain instances.
          *
          * @param g the geom to check
          * @param aabb the location in which to store the axis-aligned
          * bounding box
          */
         static void GetAABB(dGeomID g, dReal aabb[6]);
         
         /**
          * ODE collision function: Checks whether the specified axis-aligned
          * bounding box intersects with an InfiniteTerrain instance.
          *
          * @param o1 the first (InfiniteTerrain) geom
          * @param o2 the second geom
          * @param aabb2 the axis-aligned bounding box of the second geom
          * @return 1 if it intersects, 0 if it does not
          */
         static int AABBTest(dGeomID o1, dGeomID o2, dReal aabb2[6]);
         
         /**
          * The container node.
          */
         //RefPtr<osg::Group> mNode;
         
         /**
          * The noise object.
          */
         sgPerlinNoise_2D mNoise;
         
         /**
          * The size of each terrain segment.
          */
         float mSegmentSize;
         
         /**
          * The number of divisions in each terrain segment.
          */
         int mSegmentDivisions;
         
         /**
          * The horizontal scale.
          */
         float mHorizontalScale;
         
         /**
          * The vertical scale.
          */
         float mVerticalScale;
         
         /**
          * The build distance.
          */
         float mBuildDistance;

         /**
          * Whether or not smooth collision detection is enabled.
          */
         bool mSmoothCollisionsEnabled;
         
         /**
          * Identifies a single terrain segment.
          */
         struct Segment
         {
            int mX, mY;
            
            Segment(int x, int y)
               : mX(x), mY(y)
            {}
            
            bool operator<(const Segment& segment) const
            {
               if(mX < segment.mX) return true;
               else if(mX > segment.mX) return false;
               else return (mY < segment.mY);
            }
         };
         
         /**
          * The set of constructed segments.
          */
         std::set<Segment> mBuiltSegments;
         
         /**
          * Flags the segments as needing to be cleared.
          */
         bool mClearFlag;
   };
};


#endif // DELTA_INFINITETERRAIN
