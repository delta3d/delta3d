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
 * Allen 'Morgas' Danklefsen
 */

#ifndef DELTA_BATCHISECTOR
#define DELTA_BATCHISECTOR

#include <dtCore/export.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtCore/deltadrawable.h>

#include <osg/Vec3>
#include <osgUtil/IntersectVisitor>

namespace dtCore
{
   class Scene;

   /**
    * Used to perform intersection tests using multiple line segments.
    *
    * Example : To search the whole Scene for intersections:
    *\code
    * BatchIsector *isect = new BatchIsector( mScene );
    * osg::Vec3 eyePoint( 500.0f, 500.0f, 1000.0f );
    * osg::Vec3 startPos(100.f0, 100.0f, 10.0f);
    * osg::Vec3 endPos(100.0f, 1000.0f, 10.0f);
    * isect->EnableAndGetISector(0).SetSectorAsLineSegment(startPos, endPos);
    * isect->Update(eyePoint, true);
    * osg::Vec3 hitPt;
    * isect->GetSingleISector(0).GetHitPoint(hitPt);
    *\endcode
    *
    * @see dtCore::Isector
    */
   class DT_CORE_EXPORT BatchIsector : public osg::Referenced
   {
   public:
      typedef osgUtil::IntersectVisitor::HitList HitList;
      typedef osgUtil::Hit Hit;

      /**
       * @param scene The Delta3D scene to intersect.
       */
      BatchIsector(Scene *scene = NULL);

   protected:
      virtual ~BatchIsector();

      ///////////////////////////////////////////////////////////////////////////
      bool CheckBoundsOnArray(int index);

   public:
      ///////////////////////////////////////////////////////////////////////
      class DT_CORE_EXPORT SingleISector : public osg::Referenced
      {
         friend class BatchIsector;

      public:
         SingleISector(const int idForISector, const std::string& nameForISector, bool checkClosestDrawables = false);

         SingleISector(const int idForISector, bool checkClosestDrawables = false);

      protected:
         ~SingleISector();

      public:
         ///////////////////////////////////////////////////////////////////////////////
         void SetSectorAsRay(const osg::Vec3& startPos, osg::Vec3& direction, const float lineLength);

         ///////////////////////////////////////////////////////////////////////////////
         void SetSectorAsLineSegment(const osg::Vec3& startPos, const osg::Vec3& endPos);

         ///////////////////////////////////////////////////////////////////////////////
         void ResetSingleISector();

         ///@return the intersected point
         void GetHitPoint(osg::Vec3& xyz, int pointNum = 0) const;

         ///@return the normal at the intersected point
         void GetHitPointNormal(osg::Vec3& normal, int pointNum = 0) const;

         ///Get the number of intersected items
         unsigned GetNumberOfHits() const;

         /// get the hit information...
         Hit GetIntersectionHit(int num) const
         {
            // error check this....
            return mHitList[num];
         }

         ////////////////////////////////////////////////////////////////////
         /// gets for the class.
         ////////////////////////////////////////////////////////////////////
         osg::LineSegment* GetLineSegment()  { return mLineSegment.get(); }
         DeltaDrawable* GetClosestDrawable() { return mClosestDrawable.get(); }
         osgUtil::IntersectVisitor::HitList& GetHitList() { return mHitList; }
         const osgUtil::IntersectVisitor::HitList& GetHitList() const { return mHitList; }
         bool CheckForClosestDrawable()      { return mCheckClosestDrawables; }
         std::string GetReferenceName()      { return mNameForReference; }
         int GetReferenceID()                { return mIDForReference; }
         bool GetIsOn()                      { return mIsOn; }
         ////////////////////////////////////////////////////////////////////

         ////////////////////////////////////////////////////////////////////
         // sets
         void SetToCheckForClosestDrawable(bool value);
         void ToggleIsOn(bool value);
         // other sets are purposely not here, you should not touch them
         ////////////////////////////////////////////////////////////////////

         /**
          * Assigns the hitlist for this single isector.
          */
         void SetHitList(HitList& newList);

      protected:
         dtCore::RefPtr<osg::LineSegment>    mLineSegment;
         dtCore::ObserverPtr<DeltaDrawable>  mClosestDrawable;
         HitList                             mHitList;
         bool                                mCheckClosestDrawables;
         std::string                         mNameForReference;
         int                                 mIDForReference;
         bool                                mIsOn;
      };

      /**
       * Sets a drawable as the root of the intersection tests.  If this is specified,
       * it will take precedence over the currently assigned Delta3D scene.
       * @param drawable The drawable to intersect.
       */
      void SetQueryRoot(DeltaDrawable* drawable) { mQueryRoot = drawable; }

      ///Clears the currently assigned root drawable of the intersection tests.
      void ClearQueryRoot() { mQueryRoot = NULL; }

      ///@return the root of the scene to query.  It will return if this is using the entire scene.
      DeltaDrawable* GetQueryRoot() { return mQueryRoot.get(); }

      ///@return the root of the scene to query.  It will return if this is using the entire scene.
      const DeltaDrawable* GetQueryRoot() const { return mQueryRoot.get(); }

      /**
       * Set the mask defining what geometry types to test for intersection.
       *
       * @param mask The bit mask defining what geometry types to test for intersection.
       */
      void SetTraversalMask(int mask) { mTraversalMask = mask; }

      /**
       * Get the mask defining what geometry types to test for intersection.
       * @return The bit mask defining what geometry types to test for intersection.
       */
      int GetTraversalMask() const { return mTraversalMask; }

      ///Sets the scene to use as the base for the scene query.
      void SetScene(Scene* newScene) { mScene = newScene; }

      ///@return the scene being queried.
      Scene* GetScene() { return mScene; }

      ///@return the scene being queried.
      const Scene* GetScene() const { return mScene; }

      /// Create an isector if not made already, else makes one
      SingleISector& EnableAndGetISector(int nID);

      /// Get an SingleISector for reference vars
      const SingleISector& GetSingleISector(int nIndexID);

      /// removes a single isector from the list
      void StopUsingSingleISector(int nIndexID);

      /// removes a single isector from the list
      void StopUsingAllISectors();

      /**
       * Ray traces the scene.
       *
       * @param cameraEyePoint The eye point to be used for LOD calculations, if used
       * @param useHighestLvlOfDetail If false, use eye point for LOD calculations
       * @return True if any intersections were detected.
       * @note If the query root has been set, only the query root drawable and its
       *  children are candidates for intersection.  If not, all drawables in the scene
       *  are possibilities.
       */
      bool Update(const osg::Vec3& cameraEyePoint = osg::Vec3(0,0,0), bool useHighestLvlOfDetail = true);

      /**
       * Resets the intersection query.  Call this in between disjoint intersection
       * executions.
       */
      void Reset();

      /**
       * Finds the DeltaDrawable that contains the given geometry node.
       * @param geode The node to search for.
       * @return A valid DeltaDrawable if one was found or 0 otherwise.
       */
      DeltaDrawable* MapNodePathToDrawable(const osg::NodePath& geode);

   private:
      // Disallowed to prevent compile errors on VS2003. It apparently
      // creates this functions even if they are not used, and if
      // this class is forward declared, these implicit functions will
      // cause compiler errors for missing calls to "ref".
      BatchIsector& operator=(const BatchIsector&);
      BatchIsector(const BatchIsector&);

      Scene*                              mScene;           // the scene in which we start at
      dtCore::ObserverPtr<DeltaDrawable>  mQueryRoot;
      dtCore::RefPtr<SingleISector>       mISectors[32];    // all the isectors to be sent down in one batch call.
      const int                           mFixedArraySize;
      int                                 mTraversalMask;
   };

} // namespace dtCore

#endif // DELTA_BATCHISECTOR
