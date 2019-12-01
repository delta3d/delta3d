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

#ifndef DELTA_ISECTOR
#define DELTA_ISECTOR

// isector.h: interface for the Isector class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/transformable.h>
#include <dtCore/export.h>
#include <dtUtil/macros.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtCore/deltadrawable.h>

#include <osg/Vec3>
#include <osgUtil/IntersectVisitor>

namespace dtCore
{
   class Scene;

   /**
      This class is used to check for intersections of a line segment with
      geometry.  Supply the starting position, the direction vector, the length
      of the Isector, and the geometry to intersect with.

      If no geometry is specified with SetGeometry(), the whole Scene is searched.

      After calling Update(), the results of the intersection may be queried with
      GetHitPoint().

   \note Although derived from Transformable, this class does not use any
      inherited functionality from Transformable at this point.


      To search the whole Scene for the height of terrain at a given xyz:
   \code
      Isector *isect = new Isector( mScene );
      osg::Vec3 queryPoint( 500.0f, 500.0f, 1000.0f );
      isect->SetStartPosition( queryPoint );
      osg::Vec3 direction( 0.0f, 0.0f, -1.0f );
      isect->SetDirection( direction );

      isect->Update();

      osg::Vec3 hitPt;
      isect->GetHitPoint( hitPt );
   \endcode
   */

   class DT_CORE_EXPORT Isector : public Transformable
   {

      DECLARE_MANAGEMENT_LAYER(Isector)

   public:
      typedef osgUtil::IntersectVisitor::HitList HitList;

      /**
       * Constructs a new intersection query.
       * @param scene A Delta3D scene to intersect.  If this is 0, a root
       *  drawable must be set.
       * @note The default ray has a starting position of (0,0,0) and a
       *  direction of (0,1,0).
       */
      Isector(Scene* scene = NULL);

      /**
       * Constructs a new intersection query using a ray constructed with the
       * specified parameters.
       * @param start The start of the ray.
       * @param dir The direction the ray is traveling.
       * @param scene The Delta3D scene to intersect.
       */
      Isector(const osg::Vec3& start, const osg::Vec3& dir, Scene* scene = NULL);

      /**
       * Constructs a new intersection query using a line segment with the
       * specified parameters.
       * @param scene The Delta3D scene to intersect.
       * @param start The start of the line segment.
       * @param end The end point of the line segment.
       */
      Isector(Scene* scene, const osg::Vec3& start, const osg::Vec3& end);

   protected:
      virtual ~Isector();

   public:
      /**
       * Sets a drawable as the root of the intersection tests.  If this is specified,
       * it will take precedence over the currently assigned Delta3D scene.
       * @param drawable The drawable to intersect.
       */
      void SetGeometry(DeltaDrawable *drawable)
      {
         mSceneRoot = drawable;
      }

      /**
       * Clears the currently assigned root drawable of the intersection tests.
       */
      void ClearQueryRoot()
      {
         mSceneRoot = NULL;
      }

      /**
       * @return the root of the scene to query.  It will return if this is using the entire scene.
       */
      DeltaDrawable* GetQueryRoot()
      {
         return mSceneRoot.get();
      }

      /**
       * @return the root of the scene to query.  It will return if this is using the entire scene.
       */
      const DeltaDrawable* GetQueryRoot() const
      {
         return mSceneRoot.get();
      }

      ///Sets the scene to use as the base for the scene query.
      void SetScene(Scene* newScene);

      ///@return the scene being queried.
      Scene* GetScene()
      {
         return mScene;
      }

      ///@return the scene being queried.
      const Scene* GetScene() const
      {
         return mScene;
      }

      /**
       * Sets the eyepoint.  This is used for LOD calculations when doing intersections.
       * @param the eye point
       */
      void SetEyePoint(const osg::Vec3& newEyePoint)
      {
         mIntersectVisitor.setEyePoint(newEyePoint);
         mUpdateLineSegment = true;
      }

      /**
       * Gets the starting position of the intersection ray.
       * @return A vector containing the start position of the intersection ray.
       */
      const osg::Vec3 GetEyePoint() const
      {
         return mIntersectVisitor.getEyePoint();
      }

      /**
       * Sets if this Isector should consider the eye point for LOD calculations when
       * doing the intersection or if it should ignore it and always use the highest LOD when intersecting.S
       */
      void SetUseEyePoint(bool newValue);

      /**
       * @return true if the eye point is being considered for LOD calculation when doing an intersection.
       */
      bool GetUseEyePoint() const;

      /**
       * Sets the starting position of the intersection ray.
       * @param start The start position.
       */
      void SetStartPosition(const osg::Vec3& start)
      {
         mStart = start;
         mUpdateLineSegment = true;
      }

      /**
       * Gets the starting position of the intersection ray.
       * @return A vector containing the start position of the intersection ray.
       */
      const osg::Vec3& GetStartPosition() const
      {
         return mStart;
      }

      // sets a new end position
      void SetEndPosition(const osg::Vec3& endXYZ)
      {
         mDirection = endXYZ-mStart;
         mLineLength = mDirection.length();
         mUpdateLineSegment = true;
      }


      /**
       * Sets the direction of the intersection ray.
       * @param dir The direction vector.  This is normalized before being assigned.
       */
      void SetDirection(const osg::Vec3& dir)
      {
         mDirection = dir;
         mUpdateLineSegment = true;
      }

      /**
       * Gets the direction of the intersection ray.
       * @return The direction unit vector.
       */
      const osg::Vec3& GetDirection() const
      {
         return mDirection;
      }

      ///Set the length of the isector
      void SetLength(float distance)
      {
         mLineLength = distance;
         mUpdateLineSegment = true;
      }

      ///Get the length of the isector
      float GetLength() const
      {
         return mLineLength;
      }

      /**
       * Set the mask defining what geometry types to test for intersection.
       *
       * @param mask The bit mask defining what geometry types to test for intersection.
       */
      void SetTraversalMask(int mask);

      /**
       * Get the mask defining what geometry types to test for intersection.
       * @return The bit mask defining what geometry types to test for intersection.
       */
      int GetTraversalMask() const;

      ///@return the intersected point
      void GetHitPoint(osg::Vec3& xyz, int pointNum = 0) const;

      ///@return the normal at the intersected point
      void GetHitPointNormal(osg::Vec3& normal, int pointNum = 0) const;

      ///Get the number of intersected items
      int GetNumberOfHits() const;

      /**
       * Ray traces the scene.
       *
       * @return True if any intersections were detected.
       * @note If the query root has been set, only the query root drawable and its
       *  children are candidates for intersection.  If not, all drawables in the scene
       *  are possibilities.
       */
      bool Update();

      /**
       * Resets the intersection query.  Call this in between disjoint intersection
       * executions.
       */
      void Reset();

      /**
       * Gets the DeltaDrawable that is closest to the query's starting point
       * along the intersection ray.
       * @return A valid DeltaDrawable.
       */
      DeltaDrawable* GetClosestDeltaDrawable()
      {
         return mClosestDrawable.get();
      }

      /**
       * Gets the DeltaDrawable that is closest to the query's starting point
       * along the intersection ray.
       * @return A valid DeltaDrawable.
       */
      const DeltaDrawable* GetClosestDeltaDrawable() const
      {
         return mClosestDrawable.get();
      }

      /**
       * Gets the line segment used for this intersection query.
       * @return
       */
      const osg::LineSegment* GetLineSegment() const
      {
         return mLineSegment.get();
      }

      /**
       * Gives access to the underlying intersect visitor.  This is useful when a more
       * detailed description of the intersection tests are required.
       * @return
       */
      osgUtil::IntersectVisitor& GetIntersectVisitor()
      {
         return mIntersectVisitor;
      }

      /**
       * Finds the DeltaDrawable that contains the given geometry node.
       * @param geode The node to search for.
       * @return A valid DeltaDrawable if one was found or 0 otherwise.
       */
      DeltaDrawable* MapNodePathToDrawable(const osg::NodePath& geode);

      /**
       * Get the Hitlist member. Do not assume this list is sorted based on distance from
       * the starting point.
       * @return a const reference to the entire hit list
       */
      const HitList& GetHitList() const { return mHitList; }

      /**
       * Get the Hitlist member. Do not assume this list is sorted based on distance from
       * the starting point.
       * @return a non-const reference to the entire hit list
       */
      HitList& GetHitList() { return mHitList; }

   private:
      // Disallowed to prevent compile errors on VS2003. It apparently
      // creates this functions even if they are not used, and if
      // this class is forward declared, these implicit functions will
      // cause compiler errors for missing calls to "ref".
      Isector& operator=(const Isector&);
      Isector(const Isector&);

      osg::Vec3 mStart;
      osg::Vec3 mDirection;
      float mLineLength;
      bool mUpdateLineSegment;

      Scene* mScene;
      RefPtr<osg::LineSegment> mLineSegment;
      dtCore::ObserverPtr<DeltaDrawable> mSceneRoot;
      dtCore::ObserverPtr<DeltaDrawable> mClosestDrawable;
      osgUtil::IntersectVisitor mIntersectVisitor;
      HitList mHitList;
      void CalcLineSegment();
   };
} // namespace dtCore

#endif // DELTA_ISECTOR
