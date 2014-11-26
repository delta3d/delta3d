/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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

#include <set>
#include <dtCore/transformable.h>
#include <dtUtil/noiseutility.h>
#include <osg/Vec3>
#include <osg/Vec4>

namespace dtCore
{
   class InfiniteTerrainCallback;

   /**
    * An infinite terrain surface.
    */
   class DT_CORE_EXPORT InfiniteTerrain : public Transformable
   {
      friend class InfiniteTerrainCallback;

      DECLARE_MANAGEMENT_LAYER(InfiniteTerrain)

      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          * @param textureImage An image to apply to the terrain.  The appearance
          * of this texture will be effected by HighColor and LowColor
          */
         InfiniteTerrain(const std::string& name = "infiniteTerrain",
                         osg::Image* textureImage = 0);

      protected:

         /**
          * Destructor.
          */
         virtual ~InfiniteTerrain();

      public:

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

         /**
          * We want we collision to happen by default with this class.
          */
         virtual bool FilterContact(dContact*, Transformable*) { return true; }

         /**
          * Given pointOne and pointTwo, both in world space and with all coordinates
          * in Cartesian space (essentially in meters along X, Y and Z),
          * returns true if there is a clear line of sight and false if the view
          * is blocked.
          * @note This method only samples points at a predetermined distance from
          *       pointOne to pointTwo and not be 100% accurate with actual terrrain.
          *
          * @param pointOne The start point.
          * @param pointTwo The end point.
          * @return Returns true if there is a clear line of sight from pointOne to
          * pointTwo and false if the view is blocked.
          * @see SetLineOfSightSpacing()
          */
         bool IsClearLineOfSight(const osg::Vec3& pointOne, const osg::Vec3& pointTwo);

         /**
          * Set the stepping distance to sample points for the line of sight calculation (meters).
          * Defaults to 25 meters.
          */
         void SetLineOfSightSpacing(float spacing) {mLOSPostSpacing = spacing;}

         float GetLineOfSightSpacing() const {return mLOSPostSpacing;}

         /**
          * Set the color used for low areas of the terrain.
          *
          * @param rgb The colour (0..255, 0..255, 0..255) of the low areas.
          */
         void SetMinColor(const osg::Vec3& rgb);

         /**
          * Set the color used for high areas of the terrain.
          *
          * @param rgb The colour (0..255, 0..255, 0..255) of the high areas.
          */
         void SetMaxColor(const osg::Vec3& rgb);

      private:

         /**
          * Builds a single terrain segment.
          *
          * @param x the x coordinate at which to build the segment
          * @param y the y coordinate at which to build the segment
          */
         void BuildSegment(int x, int y);


         //returns an interpolated color based on the height
         osg::Vec4 GetColor(float pHeight);

         //initializes info used for the GetColor function
         void SetupColorInfo();

         /**
          * The noise object.
          */
         dtUtil::Noise2f mNoise;

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
               if (mX < segment.mX) return true;
               else if (mX > segment.mX) return false;
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

         //added for vertex coloring
         float mMinHeight, mIdealHeight, mMaxHeight;
         float mMinColorIncrement, mMaxColorIncrement;
         osg::Vec3 mMinColor, mMaxColor;

         float mLOSPostSpacing; ///<used to samples points for LOS calculations
   };
}

#endif // DELTA_INFINITETERRAIN
