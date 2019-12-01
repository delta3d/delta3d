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
#include <prefix/dtcoreprefix.h>

#include <dtCore/exceptionenum.h>
#include <dtCore/batchisector.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/cullmask.h>

#include <osg/Group>
#include <osg/Version>

#include <stack>
#include <algorithm>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::BatchIsector(dtCore::Scene* scene)
      : mScene(scene)
      , mFixedArraySize(32)
      //, mTraversalMask(dtUtil::CullMask::SCENE_INTERSECT_MASK)
   {
      for (int i = 0 ; i < mFixedArraySize; ++i)
      {
         mISectors[i] = new SingleISector(i);
      }

      // initialize traversal mask with default OSG value
      {
         osgUtil::IntersectVisitor intersectVisitor;
         mTraversalMask = intersectVisitor.getTraversalMask();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::~BatchIsector()
   {
      for (int i = 0 ; i < mFixedArraySize; ++i)
      {
         mISectors[i] = NULL;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool BatchIsector::Update(const osg::Vec3& cameraEyePoint, bool useHighestLvlOfDetail)
   {
      if (!mQueryRoot.valid() && mScene == NULL)
      {
         LOG_DEBUG("Went to update BatchIsector, however the queryroot and scene are not valid.");
         return false;
      }

      osgUtil::IntersectVisitor intersectVisitor;

      if (useHighestLvlOfDetail)
      {
         intersectVisitor.setLODSelectionMode(osgUtil::IntersectVisitor::USE_HIGHEST_LEVEL_OF_DETAIL);
      }
      else
      {
         intersectVisitor.setLODSelectionMode(osgUtil::IntersectVisitor::USE_SEGMENT_START_POINT_AS_EYE_POINT_FOR_LOD_LEVEL_SELECTION);
      }

      for (int i = 0 ; i < mFixedArraySize; ++i)
      {
         if (mISectors[i]->GetIsOn())
         {
            intersectVisitor.addLineSegment(mISectors[i]->mLineSegment.get());
         }
      }

      intersectVisitor.setEyePoint(cameraEyePoint);
      intersectVisitor.setTraversalMask(mTraversalMask);

      if (mQueryRoot.valid())
      {
         mQueryRoot->GetOSGNode()->accept(intersectVisitor);
      }
      else if (mScene != NULL)
      {
         mScene->GetSceneNode()->accept(intersectVisitor);
      }

      if (intersectVisitor.hits())
      {
         for (int i = 0 ; i < mFixedArraySize; ++i)
         {
            if (mISectors[i]->GetIsOn())
            {
               mISectors[i]->SetHitList(intersectVisitor.getHitList(mISectors[i]->mLineSegment.get()));
               if (mISectors[i]->mCheckClosestDrawables == true)
               {
                  osg::NodePath& nodePath = mISectors[i]->GetHitList()[0].getNodePath();
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
      if ((!mQueryRoot.valid() && mScene == NULL) || nodePath.empty())
      {
         return NULL;
      }

      std::set<osg::Node*> nodeCache;
      std::stack<dtCore::DeltaDrawable*> drawables;

      // Create a cache of the nodepath for quicker lookups since we are doing
      // quite a few.
      for (osg::NodePath::const_iterator itor = nodePath.begin();
           itor != nodePath.end();
           ++itor)
      {
         nodeCache.insert(*itor);
      }

      // In order to find the DeltaDrawable we first check the drawables at the
      // top level of the scene. Then, keep on going though the hierarchy, in order
      // to find the Delta Drawable deeper in the graph (the closest to the 
      // intersection point)
      dtCore::DeltaDrawable* pCurrClosest = NULL;

      if (mQueryRoot.valid())
      {
         drawables.push(mQueryRoot.get());
      }
      else if (mScene != NULL)
      {
         for (unsigned i = 0; i < mScene->GetNumberOfAddedDrawable(); ++i)
         {
            drawables.push(mScene->GetChild(i));
         }
      }
      else
      {
         return NULL;
      }

      while (!drawables.empty())
      {
         dtCore::DeltaDrawable* d = drawables.top();
         drawables.pop();

         if (nodeCache.find(d->GetOSGNode()) != nodeCache.end())
         {
            // save the current result
            pCurrClosest = d;
            // iterate through the children
            drawables.empty();
            for (unsigned i = 0; i < d->GetNumChildren(); ++i)
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
         throw dtCore::InvalidParameterException(
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
         throw dtCore::InvalidParameterException(
                  "EnableAndGetISector sent in bad index", __FILE__, __LINE__);
      }
      return *mISectors[nIndexID].get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::StopUsingSingleISector(int nIndexID)
   {
      if (CheckBoundsOnArray(nIndexID) == false)
      {
         return;
      }

      if (mISectors[nIndexID]->GetIsOn())
      {
         mISectors[nIndexID]->ResetSingleISector();
      }
      mISectors[nIndexID]->ToggleIsOn(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::StopUsingAllISectors()
   {
      for (int i = 0 ; i < mFixedArraySize; ++i)
      {
         StopUsingSingleISector(i);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool BatchIsector::CheckBoundsOnArray(int index)
   {
      if (index < 0 || index > mFixedArraySize - 1)
      {
         LOG_ERROR("You sent in a bad index to the batchISector");
         return false;
      }
      return true;
   }

   //////////////////////////////////////////////////////////////////////////////////////
   //                         Functions in the SingleISector                           //
   //////////////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::SingleISector::SingleISector(const int idForISector, const std::string& nameForISector, bool checkClosestDrawables)
   : mIsOn()
   {
      mCheckClosestDrawables = checkClosestDrawables;
      mNameForReference = nameForISector;
      mIDForReference = idForISector;
      mClosestDrawable = NULL;
      mLineSegment = new osg::LineSegment();
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::SingleISector::SingleISector(const int idForISector, bool checkClosestDrawables)
   : mIsOn()
   {
      mCheckClosestDrawables = checkClosestDrawables;
      mIDForReference = idForISector;
      mClosestDrawable = NULL;
      mLineSegment = new osg::LineSegment();
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::SingleISector::~SingleISector() {} 

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::SetToCheckForClosestDrawable(bool value)
   {
      mCheckClosestDrawables = value;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::ToggleIsOn(bool value)
   {
      mIsOn = value;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::GetHitPoint(osg::Vec3& xyz, int pointNum) const
   {
      if (pointNum >= (int) GetNumberOfHits()) { return; }

      xyz = mHitList[pointNum].getWorldIntersectPoint();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::GetHitPointNormal(osg::Vec3& normal, int pointNum) const
   {
      if (pointNum >= (int) GetNumberOfHits()) { return; }

      normal = mHitList[pointNum].getWorldIntersectNormal();
   }

   ///////////////////////////////////////////////////////////////////////////////
   unsigned BatchIsector::SingleISector::GetNumberOfHits() const
   {
      return mHitList.size();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::SetSectorAsRay(const osg::Vec3& startPos, osg::Vec3& direction, const float lineLength)
   {
      direction.normalize();
      if (dtUtil::IsFiniteVec(startPos) && dtUtil::IsFiniteVec(direction) && dtUtil::IsFinite(lineLength))
      {
         mLineSegment->set(startPos, startPos + (direction * lineLength));
      }
      else
      {
         LOG_ERROR("Non-finite ray assigned to the Isector.  Put a breakpoint here to debug.");
      }
      ResetSingleISector();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::SetSectorAsLineSegment(const osg::Vec3& startPos, const osg::Vec3& endPos)
   {
      if (dtUtil::IsFiniteVec(startPos) && dtUtil::IsFiniteVec(endPos))
      {
         mLineSegment->set(startPos, endPos);
      }
      else
      {
         LOG_ERROR("Non-finite ray assigned to the Isector.  Put a breakpoint here to debug.");
      }
      ResetSingleISector();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::ResetSingleISector()
   {
      mHitList.clear();
      mClosestDrawable = NULL;
   }

   struct DeleteNonFinite
   {
      bool operator()(BatchIsector::Hit& hit)
      {
         bool result = !dtUtil::IsFiniteVec(hit._intersectPoint) || !dtUtil::IsFiniteVec(hit._intersectNormal);

         if (!result && hit._matrix.valid())
         {
            result = !dtUtil::IsFiniteVec(hit._matrix->getTrans());
         }
         static bool mNotifiedAboutNAN = false;
         if (result && !mNotifiedAboutNAN)
         {
            LOGN_ERROR("batchisector.cpp", "Invalid collision point found in isector.  You only get this error once each time you run the application to avoid spamming the console.");
            mNotifiedAboutNAN = true;
         }
         return result;
      }
   };

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::SetHitList(osgUtil::IntersectVisitor::HitList& newList)
   {
      mHitList = newList;
      DeleteNonFinite deleteFunc;
      mHitList.erase(std::remove_if(mHitList.begin(), mHitList.end(), deleteFunc), mHitList.end());
   }

} // end namespace
