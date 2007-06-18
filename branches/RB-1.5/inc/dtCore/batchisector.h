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

#include <osg/Vec3>
#include <osgUtil/IntersectVisitor>

namespace dtCore
{
   class DeltaDrawable;
   class Scene;

   /**
      To search the whole Scene for the height of terrain at a given xyz:
   \code
      BatchIsector *isect = new BatchIsector( mScene );
      osg::Vec3 queryPoint( 500.0f, 500.0f, 1000.0f );
      isect->EnableAndGetISector(0).SetSectorAsLineSegment(const osg::Vec3& startPos, const osg::Vec3& endPos);
      isect->Update(queryPoint, true);
      osg::Vec3 hitPt;
      isect->GetSingleISector(0).GetHitPoint( hitPt );
   \endcode
    */

   class DT_CORE_EXPORT BatchIsector : public osg::Referenced
   {
      public:

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
               SingleISector(const int idForISector, const std::string& nameForISector, bool checkClosestDrawables = false)
               {
                  mCheckClosestDrawables = checkClosestDrawables;
                  mNameForReference = nameForISector;
                  mIDForReference = -1;
                  mClosestDrawable = NULL;
                  mLineSegment = new osg::LineSegment();
                  mIsOn = false;
               }

               SingleISector(const int idForISector, bool checkClosestDrawables = false)
               {
                  mCheckClosestDrawables = checkClosestDrawables;
                  mIDForReference = idForISector;
                  mClosestDrawable = NULL;
                  mLineSegment = new osg::LineSegment();
                  mIsOn = false;
               }

            protected:
               ~SingleISector(){}

            public:

               ///////////////////////////////////////////////////////////////////////////////
               void SetSectorAsRay(const osg::Vec3& startPos, osg::Vec3& direction, const float lineLength)
               {
                  direction.normalize();
                  mLineSegment->set(startPos, startPos + (direction*lineLength));
                  ResetSingleISector();
               }

               ///////////////////////////////////////////////////////////////////////////////
               void SetSectorAsLineSegment(const osg::Vec3& startPos, const osg::Vec3& endPos)
               {
                  mLineSegment->set(startPos, endPos);
                  ResetSingleISector();
               }

               ///////////////////////////////////////////////////////////////////////////////
               void ResetSingleISector()
               {
                  mHitList.clear();
                  mClosestDrawable = NULL;
               }

               ///@return the intersected point
               void GetHitPoint( osg::Vec3& xyz, int pointNum = 0 ) const;

               ///@return the normal at the intersected point
               void GetHitPointNormal( osg::Vec3& normal, int pointNum = 0 ) const;

               ///Get the number of intersected items
               int GetNumberOfHits() const;

               /// get the hit information...
               osgUtil::Hit GetIntersectionHit(int num) const
               {
                  // error check this....
                  return mHitList[num];
               }

               ////////////////////////////////////////////////////////////////////
               /// gets for the class.
               ////////////////////////////////////////////////////////////////////
               osg::LineSegment* GetLineSegment()  {return mLineSegment.get();}
               DeltaDrawable* GetClosestDrawable() {return mClosestDrawable.get();}
               osgUtil::IntersectVisitor::HitList& GetHitList() {return mHitList;}
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

            protected:

               dtCore::RefPtr<osg::LineSegment>    mLineSegment;
               dtCore::ObserverPtr<DeltaDrawable>  mClosestDrawable;
               osgUtil::IntersectVisitor::HitList  mHitList; 
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
         DeltaDrawable *MapNodePathToDrawable(osg::NodePath &geode);

         private:

            // Disallowed to prevent compile errors on VS2003. It apparently
            // creates this functions even if they are not used, and if
            // this class is forward declared, these implicit functions will
            // cause compiler errors for missing calls to "ref".
            BatchIsector& operator=( const BatchIsector& ); 
            BatchIsector( const BatchIsector& );

            Scene*                              mScene;           // the scene in which we start at
            dtCore::ObserverPtr<DeltaDrawable>  mQueryRoot;
            dtCore::RefPtr<SingleISector>       mISectors[32];    // all the isectors to be sent down in one batch call.
   };
} // namespace

#endif // DELTA_BATCHISECTOR
