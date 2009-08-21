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
#include <dtUtil/macros.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h> 
#include <dtCore/deltadrawable.h>

#include <osg/Vec3>
#include <osgUtil/LineSegmentIntersector>

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
   * osg::Vec3 startPos(100.f, 100.f, 10.f);
   * osg::Vec3 endPos(100.f, 1000.f, 10.f);
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
      typedef osgUtil::LineSegmentIntersector::Intersections HitList;
      typedef osgUtil::LineSegmentIntersector::Intersection Hit;
      /**
      * @param scene The Delta3D scene to intersect.
      */
      BatchIsector(Scene *scene = NULL, unsigned numIsectors = 32U);

   protected:

      virtual ~BatchIsector();

      ///////////////////////////////////////////////////////////////////////////
      bool CheckBoundsOnArray(unsigned index);

   public:

      ///////////////////////////////////////////////////////////////////////
      class DT_CORE_EXPORT SingleISector : public osg::Referenced
      {
         friend class BatchIsector;
      public:
         SingleISector(const unsigned idForISector, const std::string& nameForISector, bool checkClosestDrawables = false);

         SingleISector(const unsigned idForISector, bool checkClosestDrawables = false);

      protected:
         ~SingleISector();

      public:

         ///////////////////////////////////////////////////////////////////////////////
         void SetSectorAsRay(const osg::Vec3d& startPos, osg::Vec3d& direction, const double lineLength);

         ///////////////////////////////////////////////////////////////////////////////
         void SetSectorAsLineSegment(const osg::Vec3d& startPos, const osg::Vec3d& endPos);

         ///////////////////////////////////////////////////////////////////////////////
         void ResetSingleISector();

         ///@return the intersected point
         void GetHitPoint( osg::Vec3& xyz, unsigned pointNum = 0 ) const;

         ///@return the normal at the intersected point
         void GetHitPointNormal( osg::Vec3& normal, unsigned pointNum = 0 ) const;

         ///Get the number of intersected items
         unsigned GetNumberOfHits() const;

         /// get the hit information...
         Hit GetIntersectionHit(unsigned num) const;

         ////////////////////////////////////////////////////////////////////
         /// gets for the class.
         ////////////////////////////////////////////////////////////////////
         osgUtil::LineSegmentIntersector& GetLineSegment()  {return *mLineSegment;}
         DeltaDrawable* GetClosestDrawable() {return mClosestDrawable.get();}
         HitList& GetHitList()               {return mHitList;}
         bool CheckForClosestDrawable()      {return mCheckClosestDrawables;}
         std::string GetReferenceName()      {return mNameForReference;}
         int GetReferenceID()                {return mIDForReference;}
         bool GetIsOn()                      {return mIsOn;}
         ////////////////////////////////////////////////////////////////////

         ////////////////////////////////////////////////////////////////////
         // sets
         void SetToCheckForClosestDrawable(bool value) {mCheckClosestDrawables = value;}
         void ToggleIsOn(bool value)         {mIsOn = value;}
         // other sets are purposely not here, you should not touch them
         ////////////////////////////////////////////////////////////////////

         /**
         * Assigns the hitlist for this single isector.
         */
         void SetHitList(HitList& newList);

      protected:

         dtCore::RefPtr<osgUtil::LineSegmentIntersector>    mLineSegment;
         dtCore::ObserverPtr<DeltaDrawable>  mClosestDrawable;
         HitList                             mHitList;
         std::string                         mNameForReference;
         int                                 mIDForReference;
         bool                                mIsOn;
         bool                                mCheckClosestDrawables;
      };


      /**
      * Sets a drawable as the root of the intersection tests.  If this is specified,
      * it will take precedence over the currently assigned Delta3D scene.
      * @param drawable The drawable to intersect.
      */
      void SetQueryRoot(DeltaDrawable *drawable) { mQueryRoot = drawable; }

      ///Clears the currently assigned root drawable of the intersection tests.
      void ClearQueryRoot(){ mQueryRoot = NULL; }

      ///@return the root of the scene to query.  It will return if this is using the entire scene.
      DeltaDrawable* GetQueryRoot(){ return mQueryRoot.get(); }

      ///@return the root of the scene to query.  It will return if this is using the entire scene.
      const DeltaDrawable* GetQueryRoot() const { return mQueryRoot.get(); }

      ///Sets the scene to use as the base for the scene query.
      void SetScene(Scene* newScene){mScene = newScene;}

      ///@return the scene being queried.
      Scene* GetScene(){ return mScene; }

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
      * @return True if any intersections were detected.
      * @note If the query root has been set, only the query root drawable and its
      *  children are candidates for intersection.  If not, all drawables in the scene
      *  are possibilities.
      */
      bool Update(const osg::Vec3& cameraEyePoint, bool useHighestLvlOfDetail = true);

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
      BatchIsector& operator=( const BatchIsector& );
      BatchIsector( const BatchIsector& );

      Scene*                              mScene;           // the scene in which we start at
      dtCore::ObserverPtr<DeltaDrawable>  mQueryRoot;
      std::vector<dtCore::RefPtr<SingleISector> > mISectors;    // all the isectors to be sent down in one batch call.

   };
} // namespace

#endif // DELTA_BATCHISECTOR
