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
#include <dtCore/batchisector.h>

#include <dtCore/deltadrawable.h>
#include <dtCore/scene.h>
#include <dtUtil/log.h>

#include <osg/Group>
#include <osg/Version>

#include <stack>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::BatchIsector(dtCore::Scene *scene) :
      mScene(scene)
   {
      // TODO Set nodemask on query
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::~BatchIsector()
   {
      
      mISectors.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool BatchIsector::Update(const osg::Vec3& cameraEyePoint, bool useHighestLvlOfDetail)
   {
      if( !mQueryRoot.valid() && mScene == NULL )
      {
         LOG_DEBUG("Went to update BatchIsector, however the queryroot and scene are not valid.");
         return false;
      }

      osgUtil::IntersectVisitor intersectVisitor;

      if(useHighestLvlOfDetail)
         intersectVisitor.setLODSelectionMode(osgUtil::IntersectVisitor::USE_HIGHEST_LEVEL_OF_DETAIL);
      else
         intersectVisitor.setLODSelectionMode(osgUtil::IntersectVisitor::USE_SEGMENT_START_POINT_AS_EYE_POINT_FOR_LOD_LEVEL_SELECTION);

      std::vector<dtCore::RefPtr<SingleISector> >::iterator iter = mISectors.begin();
      for(; iter != mISectors.end(); ++iter)
      {
         intersectVisitor.addLineSegment((*iter)->mLineSegment.get());
      }

      intersectVisitor.setEyePoint(cameraEyePoint);

      if(mQueryRoot.valid())
      {
         mQueryRoot->GetOSGNode()->accept(intersectVisitor);
      }
      else if( mScene != NULL )
      {
         mScene->GetSceneNode()->accept(intersectVisitor);
      }

      if(intersectVisitor.hits())
      {
         for(iter = mISectors.begin(); iter != mISectors.end(); ++iter)
         {
            (*iter)->mHitList = intersectVisitor.getHitList((*iter)->mLineSegment.get());
            if((*iter)->mCheckClosestDrawables == true)
            {
               osg::NodePath &nodePath = (*iter)->mHitList[0].getNodePath();
               (*iter)->mClosestDrawable = MapNodePathToDrawable(nodePath);
            }
         }
         return true;
      }
      return false;
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable *BatchIsector::MapNodePathToDrawable(osg::NodePath &nodePath)
   {
      if( ( !mQueryRoot.valid() && mScene == NULL ) || nodePath.empty() )
      {
         return NULL;
      }
   
      std::set<osg::Node *> nodeCache;
      osg::NodePath::iterator itor;
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
      std::vector<dtCore::RefPtr<SingleISector> >::iterator iter = mISectors.begin();
      for(; iter != mISectors.end(); ++iter)
      {
         (*iter)->ResetSingleISector();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::SingleISector& BatchIsector::CreateOrGetISector(int nID)
   {
      std::vector<dtCore::RefPtr<SingleISector> >::iterator iter = mISectors.begin();
      for(; iter != mISectors.end(); ++iter)
      {
         if((*iter)->mIDForReference == nID)
         {
            return *(*iter).get();
         }
      }
      
      dtCore::RefPtr<SingleISector> tempSector = new SingleISector(nID);
      mISectors.push_back(tempSector);
      return *mISectors.back().get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   BatchIsector::SingleISector& BatchIsector::CreateOrGetISector(const std::string& nameOfISector)
   {
      std::vector<dtCore::RefPtr<SingleISector> >::iterator iter = mISectors.begin();
      for(; iter != mISectors.end(); ++iter)
      {
         if((*iter)->mNameForReference == nameOfISector)
         {
            return *(*iter).get();
         }
      }

      dtCore::RefPtr<SingleISector> tempSector = new SingleISector(nameOfISector);
      mISectors.push_back(tempSector);
      return *mISectors.back().get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const BatchIsector::SingleISector& BatchIsector::GetSingleISector(int nID)
   {
      return CreateOrGetISector(nID);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const BatchIsector::SingleISector& BatchIsector::GetSingleISector(const std::string& nameOfISector)
   {
      return CreateOrGetISector(nameOfISector);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::DeleteSingleISector(int nID)
   {
      std::vector<dtCore::RefPtr<SingleISector> >::iterator iter = mISectors.begin();
      for(; iter != mISectors.end(); ++iter)
      {
         if((*iter)->mIDForReference == nID)
         {
            mISectors.erase(iter);
            return;
         }
      }
   }
   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::DeleteSingleISector(const std::string& nameOfISector)
   {
      std::vector<dtCore::RefPtr<SingleISector> >::iterator iter = mISectors.begin();
      for(; iter != mISectors.end(); ++iter)
      {
         if((*iter)->mNameForReference == nameOfISector)
         {
            mISectors.erase(iter);
            return;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////
   //                         Functions in the SingleISector                                    //
   ///////////////////////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::GetHitPoint( osg::Vec3& xyz, int pointNum ) const
   {
      if (pointNum >= GetNumberOfHits()) return;
   
      xyz = mHitList[pointNum].getWorldIntersectPoint();
   }
   ///////////////////////////////////////////////////////////////////////////////
   void BatchIsector::SingleISector::GetHitPointNormal( osg::Vec3& normal, int pointNum ) const
   {
      if (pointNum >= GetNumberOfHits()) return;
   
      normal = mHitList[pointNum].getWorldIntersectNormal();
   }

   ///////////////////////////////////////////////////////////////////////////////
   int BatchIsector::SingleISector::GetNumberOfHits() const
   {
      return mHitList.size();
   }
} // end namespace
