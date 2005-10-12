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


#include "dtCore/isector.h"
#include "dtCore/scene.h"
#include <stack>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Isector)


///////////////////////////////////////////////////////////////////////////////
Isector::Isector(dtCore::Scene *scene) :
   mStart(0,0,0), mDirection(0,1,0), mLineLength(1000000.0f), mUpdateLineSegment(true), mScene(scene), mLineSegment(new osg::LineSegment()), mClosestDrawable(0)
{
}

///////////////////////////////////////////////////////////////////////////////
Isector::Isector(const osg::Vec3 &start, const osg::Vec3 &dir,dtCore::Scene *scene):
   mStart(start), mDirection(dir), mLineLength(1000000.0f), mUpdateLineSegment(true), mScene(scene), mLineSegment(new osg::LineSegment()), mClosestDrawable(0)
{
}

///////////////////////////////////////////////////////////////////////////////
Isector::Isector(dtCore::Scene *scene, const osg::Vec3 &start, const osg::Vec3 &end):
   mStart(start), mDirection(), mLineLength(1000000.0f), mUpdateLineSegment(true), mScene(scene), mLineSegment(new osg::LineSegment()), mClosestDrawable(0)
{
   mDirection = end-start;
   mLineLength = mDirection.length();
}

///////////////////////////////////////////////////////////////////////////////
bool Isector::Update()
{
   if(!mSceneRoot.valid() && !mScene.valid())
   {
      return false;
   }

   //Make sure our line segment is correct.
   if(mUpdateLineSegment)
      CalcLineSegment();

   if (mSceneRoot.valid())
      mSceneRoot->GetOSGNode()->accept(mIntersectVisitor);
   else
      mScene->GetSceneNode()->accept(mIntersectVisitor);

   if (mIntersectVisitor.hits()) {
      mHitList = mIntersectVisitor.getHitList(mLineSegment.get());
         osg::NodePath &nodePath = mHitList[0].getNodePath();
      mClosestDrawable = MapNodePathToDrawable(nodePath);
      return true;
   }
   else {
      mClosestDrawable = NULL;
      return false;
   }
}

///////////////////////////////////////////////////////////////////////////////
dtCore::DeltaDrawable *Isector::MapNodePathToDrawable(osg::NodePath &nodePath)
{
   if ((!mScene.valid() && !mScene.valid()) || nodePath.empty())
      return NULL;

   int i;
   std::set<osg::Node *> nodeCache;
   osg::NodePath::iterator itor;
   std::stack<dtCore::DeltaDrawable *> drawables;

   //Create a cache of the nodepath for quicker lookups since we are doing
   //quite a few.
   for (itor=nodePath.begin(); itor!=nodePath.end(); ++itor)
      nodeCache.insert(*itor);

   //In order to find the DeltaDrawable we first check the drawables at the
   //top level of the scene.  If not found, we check all children of the drawables
   //for a match.
   if (mSceneRoot.valid()) {
      drawables.push(mSceneRoot.get());
   }
   else {
      for (i=0; i<mScene->GetNumberOfAddedDrawable(); i++)
         drawables.push(mScene->GetDrawable((unsigned)i));
   }

   while (!drawables.empty()) {
      dtCore::DeltaDrawable *d = drawables.top();
      drawables.pop();

      if (nodeCache.find(d->GetOSGNode()) != nodeCache.end())
         return d;

      for (i=0; (unsigned)i<d->GetNumChildren(); i++)
         drawables.push(d->GetChild((unsigned)i));
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
void Isector::Reset()
{
   mIntersectVisitor.reset();
   mClosestDrawable = NULL;
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
   mIntersectVisitor.addLineSegment(mLineSegment.get());
   mUpdateLineSegment = false;
}



/*!
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

   //osg::Geode *g = mHitList[pointNum]._geode.get();
}


/*! 
* Get the number of items that were intersected by this Isector.  Note: 
* Isector::Update() must be called prior to calling this method.
* 
* @return The number of intersected items
*/
int Isector::GetNumberOfHits() const
{
   return( mHitList.size() );
}

