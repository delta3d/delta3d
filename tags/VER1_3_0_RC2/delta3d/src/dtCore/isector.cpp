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
 * @author Matthew W. Campbell
 */

#include <dtCore/isector.h>

#include <dtCore/deltadrawable.h>
#include <dtCore/scene.h>

#include <osg/Group>

#include <stack>

namespace dtCore
{

   IMPLEMENT_MANAGEMENT_LAYER(Isector)
      
   ///////////////////////////////////////////////////////////////////////////////
   Isector::Isector(dtCore::Scene *scene) :
      mStart(0,0,0), mDirection(0,1,0), mLineLength(1000000.0f), mUpdateLineSegment(true), mScene(scene), mLineSegment(new osg::LineSegment()), mClosestDrawable(0)
   {
      // Default collision category = 4
      SetCollisionCategoryBits( UNSIGNED_BIT(4) );
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   Isector::Isector(const osg::Vec3 &start, const osg::Vec3 &dir,dtCore::Scene *scene):
      mStart(start), mDirection(dir), mLineLength(1000000.0f), mUpdateLineSegment(true), mScene(scene), mLineSegment(new osg::LineSegment()), mClosestDrawable(0)
   {
      // Default collision category = 4
      SetCollisionCategoryBits( UNSIGNED_BIT(4) );
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   Isector::Isector(dtCore::Scene *scene, const osg::Vec3 &start, const osg::Vec3 &end):
      mStart(start), mDirection(), mLineLength(1000000.0f), mUpdateLineSegment(true), mScene(scene), mLineSegment(new osg::LineSegment()), mClosestDrawable(0)
   {
      mDirection = end-start;
      mLineLength = mDirection.length();
   
      // Default collision category = 4
      SetCollisionCategoryBits( UNSIGNED_BIT(4) );
   }

   ///////////////////////////////////////////////////////////////////////////////
   Isector::~Isector()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Isector::SetScene(dtCore::Scene* newScene)
   {
      mScene = newScene;
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void Isector::SetUseEyePoint(bool newValue)
   {
      if (newValue)
      {
         return mIntersectVisitor.setLODSelectionMode(osgUtil::IntersectVisitor::USE_SEGMENT_START_POINT_AS_EYE_POINT_FOR_LOD_LEVEL_SELECTION);
      }
      else
      {
         return mIntersectVisitor.setLODSelectionMode(osgUtil::IntersectVisitor::USE_HEIGHEST_LEVEL_OF_DETAIL);
      }
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   bool Isector::GetUseEyePoint() const
   {
      return mIntersectVisitor.getLODSelectionMode() == osgUtil::IntersectVisitor::USE_SEGMENT_START_POINT_AS_EYE_POINT_FOR_LOD_LEVEL_SELECTION;
   }
   
   
   ///////////////////////////////////////////////////////////////////////////////
   bool Isector::Update()
   {
      if( !mSceneRoot.valid() && mScene == 0 )
      {
         return false;
      }
   
      //Make sure our line segment is correct.
      if(mUpdateLineSegment)
      {
         CalcLineSegment();
      }
   
      if(mSceneRoot.valid())
      {
         mSceneRoot->GetOSGNode()->accept(mIntersectVisitor);
      }
      else if( mScene != 0 )
      {
         mScene->GetSceneNode()->accept(mIntersectVisitor);
      }
      else
      {
         return false;
      }
   
      if(mIntersectVisitor.hits())
      {
         mHitList = mIntersectVisitor.getHitList(mLineSegment.get());
         osg::NodePath &nodePath = mHitList[0].getNodePath();
         mClosestDrawable = MapNodePathToDrawable(nodePath);
         return true;
      }
      else
      {
         mClosestDrawable = 0;
         return false;
      }
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable *Isector::MapNodePathToDrawable(osg::NodePath &nodePath)
   {
      if( ( !mSceneRoot.valid() && mScene == 0 ) || nodePath.empty() )
      {
         return 0;
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
      //top level of the scene.  If not found, we check all children of the drawables
      //for a match.
      if( mSceneRoot.valid() )
      {
         drawables.push(mSceneRoot.get());
      }
      else if( mScene != 0 )
      {
         for( unsigned i = 0; i < mScene->GetNumberOfAddedDrawable(); ++i )
         {
            drawables.push( mScene->GetDrawable( i ) );
         }
      }
      else
      {
         return 0;
      }
   
      while (!drawables.empty())
      {
         dtCore::DeltaDrawable *d = drawables.top();
         drawables.pop();
   
         if (nodeCache.find(d->GetOSGNode()) != nodeCache.end())
         {
            return d;
         }
   
         for (unsigned i = 0; i < d->GetNumChildren(); i++)
         {
            drawables.push(d->GetChild(i));
         }
      }
   
      return NULL;
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void Isector::Reset()
   {
      mUpdateLineSegment = true;
      mIntersectVisitor.reset();
      mClosestDrawable = NULL;
      mHitList.clear();
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void Isector::CalcLineSegment()
   {
      //Make sure the current direction vector is normalized.
      mDirection.normalize();
   
      //Since we are working with line segments, we need to convert our ray
      //representation to a finite line.
      osg::Vec3 endPoint = mStart + (mDirection*mLineLength);
      mLineSegment->set(mStart,endPoint);
      //Add line segment sets the eye point to the beginning of the line segment.
      osg::Vec3 eyePoint = GetEyePoint();
      mIntersectVisitor.addLineSegment(mLineSegment.get());
      SetEyePoint(eyePoint);
      mUpdateLineSegment = false;
   }
   
   
   
   /**
    * Get the intersected point since the last call to Update().
    *
    * @param xyz : The xyz position to be filled out [in/out]
    * @param pointNum:  Which intersection point to return [0..GetNumberOfHits()]
    */
   void Isector::GetHitPoint( osg::Vec3& xyz, int pointNum/* =0  */) const
   {
      if (pointNum >= GetNumberOfHits()) return;
   
      osg::Vec3 ip = mHitList[pointNum].getWorldIntersectPoint();
      xyz = ip;
   }
   
   
   /** 
    * Get the number of items that were intersected by this Isector.  Note: 
    * Isector::Update() must be called prior to calling this method.
    * 
    * @return The number of intersected items
    */
   int Isector::GetNumberOfHits() const
   {
      return mHitList.size();
   }
   
}
