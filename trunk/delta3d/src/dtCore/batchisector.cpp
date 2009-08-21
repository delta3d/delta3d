/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
#include <prefix/dtcoreprefix-src.h>

#include <dtCore/exceptionenum.h>
#include <dtCore/batchisector.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <osg/Group>
#include <osg/Version>

#include <stack>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::BatchIsector(dtCore::Scene *scene, unsigned numIsectors) :
      mScene(scene)
   {
      mISectors.reserve(numIsectors);
      for(unsigned i = 0 ; i < numIsectors; ++i)
      {
         mISectors.push_back(new BatchIsector::SingleISector(i));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::~BatchIsector()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool BatchIsector::Update(const osg::Vec3& cameraEyePoint, bool useHighestLvlOfDetail)
   {
      if( !mQueryRoot.valid() && mScene == NULL )
      {
         LOG_DEBUG("Went to update BatchIsector, however the queryroot and scene are not valid.");
         return false;
      }

      osgUtil::IntersectionVisitor intersectionVisitor;
      dtCore::RefPtr<osgUtil::IntersectorGroup> intersectGroup = new osgUtil::IntersectorGroup;
      intersectionVisitor.setIntersector(intersectGroup.get());

      if (useHighestLvlOfDetail)
      {
         intersectionVisitor.setLODSelectionMode(osgUtil::IntersectionVisitor::USE_HIGHEST_LEVEL_OF_DETAIL);
      }
      else
      {
         intersectionVisitor.setLODSelectionMode(osgUtil::IntersectionVisitor::USE_EYE_POINT_FOR_LOD_LEVEL_SELECTION);
      }

      for (unsigned i = 0 ; i < mISectors.size(); ++i)
      {
         if (mISectors[i]->GetIsOn())
         {
            intersectGroup->addIntersector(mISectors[i]->mLineSegment.get());
         }
      }

      intersectionVisitor.setReferenceEyePoint(cameraEyePoint);

      if (mQueryRoot.valid())
      {
         mQueryRoot->GetOSGNode()->accept(intersectionVisitor);
      }
      else if (mScene != NULL)
      {
         mScene->GetSceneNode()->accept(intersectionVisitor);
      }

      if (intersectGroup->containsIntersections())
      {
         for (unsigned i = 0 ; i < mISectors.size(); ++i)
         {
            if (mISectors[i]->GetIsOn())
            {
               mISectors[i]->SetHitList(mISectors[i]->mLineSegment->getIntersections());
               if (mISectors[i]->mCheckClosestDrawables == true)
               {
                  const osg::NodePath& nodePath = (*mISectors[i]->mHitList.begin()).nodePath;
                  mISectors[i]->mClosestDrawable = MapNodePathToDrawable(nodePath);
               }
            }
         }

         return true;
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* BatchIsector::MapNodePathToDrawable(const osg::NodePath& nodePath)
   {
      if( ( !mQueryRoot.valid() && mScene == NULL ) || nodePath.empty() )
      {
         return NULL;
      }

      std::set<osg::Node *> nodeCache;
      osg::NodePath::const_iterator itor;
      std::stack<dtCore::DeltaDrawable *> drawables;

      //Create a cache of the nodepath for quicker lookups since we are doing
      //quite a few.
      for (itor = nodePath.begin(); itor != nodePath.end(); ++itor)
      {
         nodeCache.insert(*itor);
      }

      //In order to find the DeltaDrawable we first check the drawables at the
      //top level of the scene.  Then, keep on going though the hierarchy, in order
      //to find the Delta Drawable deeper in the graph (the closest to the 
      //intersection point)
      dtCore::DeltaDrawable* pCurrClosest = NULL;

      if( mQueryRoot.valid() )
      {
         drawables.push(mQueryRoot.get());
      }
      else if( mScene != NULL )
      {
         for( unsigned i = 0; i < mScene->GetNumberOfAddedDrawable(); ++i )
         {
            drawables.push( mScene->GetDrawable( i ) );
         }
      }
      else
      {
         return NULL;
      }

      while (!drawables.empty())
      {
         dtCore::DeltaDrawable *d = drawables.top();
         drawables.pop();

         if (nodeCache.find(d->GetOSGNode()) != nodeCache.end())
         {
            // save the current result
            pCurrClosest = d;
            // iterate through the children
            drawables.empty();
            for (unsigned i = 0; i < d->GetNumChildren(); i++)
            {
               drawables.push(d->GetChild(i));
            }
            //return d;
         }
      }

      return pCurrClosest;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::Reset()
   {
      StopUsingAllISectors();
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::SingleISector& BatchIsector::EnableAndGetISector(int nIndexID)
   {
      if (CheckBoundsOnArray(nIndexID) == false)
      {
         throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
                  "EnableAndGetISector sent in bad index", __FILE__, __LINE__);
      }
      mISectors[nIndexID]->ToggleIsOn(true);
      return *mISectors[nIndexID];
   }

   ///////////////////////////////////////////////////////////////////////////////
   const BatchIsector::SingleISector& BatchIsector::GetSingleISector(int nIndexID)
   {
      if (CheckBoundsOnArray(nIndexID) == false)
      {
         throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
                  "EnableAndGetISector sent in bad index", __FILE__, __LINE__);
      }
      return *mISectors[nIndexID].get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::StopUsingSingleISector(int nIndexID)
   {
      if (CheckBoundsOnArray(nIndexID) == false)
         return;  

      if (mISectors[nIndexID]->GetIsOn())
      {
         mISectors[nIndexID]->ResetSingleISector();
      }
      mISectors[nIndexID]->ToggleIsOn(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::StopUsingAllISectors()
   {
      for (unsigned i = 0 ; i < mISectors.size(); ++i)
      {
         StopUsingSingleISector(i);
      }
   }

   ///////////////////////////////////////////////////////////////////////////
   bool BatchIsector::CheckBoundsOnArray(unsigned index)
   {
      return index < mISectors.size();
   }

   //////////////////////////////////////////////////////////////////////////////////////
   //                         Functions in the SingleISector                                    //
   //////////////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::SingleISector::SingleISector(const unsigned idForISector, const std::string& nameForISector, bool checkClosestDrawables)
   : mLineSegment(new osgUtil::LineSegmentIntersector(osgUtil::Intersector::MODEL, osg::Vec3d(), osg::Vec3d()))
   , mNameForReference(nameForISector)
   , mIDForReference(-1)
   , mIsOn(false)
   , mCheckClosestDrawables(checkClosestDrawables)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::SingleISector::SingleISector(const unsigned idForISector, bool checkClosestDrawables)
   : mLineSegment(new osgUtil::LineSegmentIntersector(osgUtil::Intersector::MODEL, osg::Vec3d(), osg::Vec3d()))
   , mIDForReference(idForISector)
   , mIsOn(false)
   , mCheckClosestDrawables(checkClosestDrawables)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::SingleISector::~SingleISector()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::GetHitPoint( osg::Vec3& xyz, unsigned pointNum ) const
   {
      if (pointNum >= GetNumberOfHits()) return;

      xyz = GetIntersectionHit(pointNum).getWorldIntersectPoint();
   }
   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::GetHitPointNormal( osg::Vec3& normal, unsigned pointNum ) const
   {
      if (pointNum >= GetNumberOfHits()) return;

      normal = GetIntersectionHit(pointNum).getWorldIntersectNormal();
   }

   ///////////////////////////////////////////////////////////////////////////////
   unsigned BatchIsector::SingleISector::GetNumberOfHits() const
   {
      return mHitList.size();
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::Hit BatchIsector::SingleISector::GetIntersectionHit(unsigned num) const
   {
      unsigned count = 0;
      HitList::const_iterator i, iend;
      i = mHitList.begin();
      iend = mHitList.end();
      for (; i != iend; ++i, ++count)
      {
         if (count == num)
         {
            return *i;
         }
      }
      return Hit();
   }


   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::SetSectorAsRay(const osg::Vec3d& startPos, osg::Vec3d& direction, const double lineLength)
   {
      direction.normalize();
      SetSectorAsLineSegment(startPos, startPos + (direction*lineLength));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::SetSectorAsLineSegment(const osg::Vec3d& startPos, const osg::Vec3d& endPos)
   {
      mLineSegment->setStart(startPos);
      mLineSegment->setEnd(endPos);
      ResetSingleISector();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::ResetSingleISector()
   {
      mHitList.clear();
      mClosestDrawable = NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::SetHitList(BatchIsector::HitList& newList)
   {
      mHitList = newList;
   }

} // end namespace
