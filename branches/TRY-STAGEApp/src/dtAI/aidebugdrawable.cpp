/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2009 Alion Science and Technology
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
* Bradley Anderegg
*/


#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointinterface.h>
#include <dtAI/waypointrenderinfo.h>
#include <dtAI/waypointpair.h>
#include <dtAI/navmesh.h>

#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/mathdefines.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>

#include <osg/Point>
#include <osg/LineWidth>

#include <osgText/Text>

namespace dtAI
{

   //////////////////////////////////////////////////////////////////////////////
   //AIDebugDrawable
   //////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::AIDebugDrawable()
      : mRenderInfo(new WaypointRenderInfo())
   {
      Init();
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::AIDebugDrawable(WaypointRenderInfo& pRenderInfo)
      : mRenderInfo(&pRenderInfo)      
   {
      Init();
   }

   ///////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::~AIDebugDrawable()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetRenderInfo(WaypointRenderInfo& pRenderInfo)
   {
      mRenderInfo = &pRenderInfo;
      
      OnRenderInfoChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   WaypointRenderInfo& AIDebugDrawable::GetRenderInfo(const WaypointRenderInfo& pRenderInfo)
   {
      return *mRenderInfo;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const WaypointRenderInfo& AIDebugDrawable::GetRenderInfo(const WaypointRenderInfo& pRenderInfo) const
   {
      return *mRenderInfo;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::OnRenderInfoChanged()
   {
      /**TODO: RE-CREATE ALL GEOMETRY
      if(mRenderInfo->mRenderWaypointID)
      {
      if(!mNode->containsNode(mGeodeIDs.get()))
      {
      mNode->addChild(mGeodeIDs.get());
      }
      else
      {
      mNode->removeChild(mGeodeIDs.get());
      }
      }*/
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::Init()
   {
      mNode = new osg::Group();
      mGeode = new osg::Geode();
      mGeodeIDs = new osg::Geode();
      mWaypointGeometry = new osg::Geometry();
      mNavMeshGeometry = new osg::Geometry();

      mWaypointIDs = new osg::IntArray();
      mWaypointPairs = new osg::UIntArray();
      mVerts = new osg::Vec3Array();

      mWaypointGeometry->setVertexArray(mVerts.get());
      mNavMeshGeometry->setVertexArray(mVerts.get());
      
      mNode->addChild(mGeode.get());
      mNode->addChild(mGeodeIDs.get());
      
      mGeode->addDrawable(mWaypointGeometry.get());
      mGeode->addDrawable(mNavMeshGeometry.get());

      //set the default color of the waypoints here so a derivative class can override it
      osg::Vec4Array* waypointColors = new osg::Vec4Array(1);
      (*waypointColors)[0] = mRenderInfo->mWaypointColor;
      mWaypointGeometry->setColorArray(waypointColors);
      mWaypointGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

      osg::Vec4Array* navmeshColors = new osg::Vec4Array(1);
      (*navmeshColors)[0] = mRenderInfo->mNavMeshColor;
      mNavMeshGeometry->setColorArray(navmeshColors);
      mNavMeshGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::ClearMemory()
   {
      //todo-
      ClearWaypointGraph();
   }


   ///////////////////////////////////////////////////////////////////////////////
   osg::Geode* AIDebugDrawable::GetGeode()
   {
      return mGeode.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetGeode( osg::Geode* geode )
   {
      mGeode = geode;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Node* AIDebugDrawable::GetOSGNode()
   {
      return mNode.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const osg::Node* AIDebugDrawable::GetOSGNode() const
   {
      return mNode.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetOSGNode( osg::Group* grp )
   {
      mNode = grp;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Geometry* AIDebugDrawable::GetGeometry()
   {
      return mWaypointGeometry.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   int AIDebugDrawable::FindWaypoint(unsigned id)
   {
      unsigned numWaypoints = mWaypointIDs->size();
      for(unsigned count = 0; count < numWaypoints; ++count)
      {
         if((*mWaypointIDs)[count] == id)
         {
            return count;
         }
      }

      return -1;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::InsertWaypoint(const WaypointInterface& wp)
   {
      int loc = FindWaypoint(wp.GetID());
      if(loc > -1)
      {
         //we already have this waypoint so lets make sure its in the right place
         (*mVerts)[loc].set(wp.GetPosition());
      }
      else //lets add it to the existing waypoints
      {
         mWaypointIDs->push_back(wp.GetID());
         mVerts->push_back(wp.GetPosition());

         if(mRenderInfo->mRenderWaypointText)
         {
            CreateWaypointIDText(wp);
         }
      }

      OnGeometryChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::RemoveWaypoint(unsigned id)
   {
      int loc = FindWaypoint(id);
      if(loc > -1)
      {
         //since this data is easily copied we can perform a faster erase
         //we simply copy the last element to the place of the element to be removed
         //and then we pop off the last element
         //it should be noted that order is not preserved.... if this matters we will need to revist this
         (*mVerts)[loc].set((*mVerts)[mVerts->size() - 1]);
         (*mWaypointIDs)[loc] = (*mWaypointIDs)[mWaypointIDs->size() - 1];

         mVerts->pop_back();
         mWaypointIDs->pop_back();

         OnGeometryChanged();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::OnGeometryChanged()
   {
      //seems like there should be a generic dirty()- does this only work with display lists?
      mWaypointGeometry->dirtyDisplayList();
      mWaypointGeometry->setVertexArray(mVerts.get());
      
      mWaypointGeometry->removePrimitiveSet(0);
      osg::PrimitiveSet* ps = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, mVerts->size());
      mWaypointGeometry->addPrimitiveSet(ps);
      //setting it back to zero will ensure any user data does not get removed when this function is called again
      mWaypointGeometry->setPrimitiveSet(0, ps);

      mNavMeshGeometry->removePrimitiveSet(0);
      osg::PrimitiveSet* psLines = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, mWaypointPairs->begin(), mWaypointPairs->end());
      mNavMeshGeometry->addPrimitiveSet(psLines);
      //setting it back to zero will ensure any user data does not get removed when this function is called again
      mNavMeshGeometry->setPrimitiveSet(0, psLines);

      osg::Point* p = new osg::Point(mRenderInfo->mWaypointSize);
      mWaypointGeometry->getOrCreateStateSet()->setAttribute(p, osg::StateAttribute::ON);

      osg::LineWidth* lw = new osg::LineWidth(mRenderInfo->mNavMeshWidth);
      mNavMeshGeometry->getOrCreateStateSet()->setAttribute(lw, osg::StateAttribute::ON);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::CreateWaypointIDText(const WaypointInterface& wp)
   {
      //note: we either create a render info in the constructor
      //or we take one by reference so should never be NULL

      osgText::Text* text = new osgText::Text(); 
      text->setDrawMode(osgText::Text::TEXT);
      text->setAlignment(osgText::TextBase::CENTER_CENTER);
      text->setAxisAlignment(osgText::TextBase::SCREEN);
      text->setAutoRotateToScreen(true);
      text->setFont(mRenderInfo->mWaypointFontFile);
      text->setCharacterSize(mRenderInfo->mWaypointFontSizeScalar);
      text->setColor(mRenderInfo->mWaypointFontColor);
      text->setPosition(wp.GetPosition() + mRenderInfo->mWaypointTextOffset);
      text->setText(dtUtil::ToString(wp.GetID()));
      mGeodeIDs->addDrawable(text);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::UpdateWaypointGraph( const NavMesh& nm )
   {
      //just clear and re-add them all, this should only happen often during editing
      ClearWaypointGraph();

      NavMesh::NavMeshContainer::const_iterator iter = nm.GetNavMesh().begin();
      NavMesh::NavMeshContainer::const_iterator iterEnd = nm.GetNavMesh().end();

      for(;iter != iterEnd; ++iter)
      {
         const WaypointPair* wp = (*iter).second;
         AddPathSegment(wp->GetWaypointFrom(), wp->GetWaypointTo());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::AddPathSegment( const WaypointInterface* pFrom, const WaypointInterface* pTo )
   {
      int indexFrom = FindWaypoint(pFrom->GetID());
      int indexTo = FindWaypoint(pTo->GetID());

      if(indexFrom > -1 && indexTo > -1)
      {
         mWaypointPairs->push_back(indexFrom);
         mWaypointPairs->push_back(indexTo);
         OnGeometryChanged();
      }
      else
      {
         LOG_ERROR("Invalid path segment");
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::ClearWaypointGraph()
   {
      mWaypointPairs->clear();
   }
}
