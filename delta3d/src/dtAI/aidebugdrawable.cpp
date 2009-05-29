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

#include <dtUtil/stringutils.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>

#include <osgText/Text>

namespace dtAI
{

   //////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::AIDebugDrawable()
      : mRenderWaypointID(true)
   {
      Init();
   }

   ///////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::~AIDebugDrawable()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::Init()
   {
      mNode = new osg::Group();
      mGeode = new osg::Geode();
      mGeodeIDs = new osg::Geode();
      mGeometry = new osg::Geometry();
      
      mWaypointIDs = new osg::IntArray();
      mVerts = new osg::Vec3Array();
      mGeometry->setVertexArray(mVerts.get());

      mNode->addChild(mGeode.get());
      mNode->addChild(mGeodeIDs.get());
      mGeode->addDrawable(mGeometry.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::ClearMemory()
   {
      //todo-
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
      return mGeometry.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool AIDebugDrawable::GetRenderWaypointID() const
   {
      return mRenderWaypointID;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetRenderWaypointID(bool b)
   {
      mRenderWaypointID = b;
      
      if(mRenderWaypointID)
      {
         if(!mNode->containsNode(mGeodeIDs.get()))
         {
            mNode->addChild(mGeodeIDs.get());
         }
         else
         {
            mNode->removeChild(mGeodeIDs.get());
         }
      }
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
   void AIDebugDrawable::InsertWaypoint( const WaypointInterface& wp )
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

         if(mRenderWaypointID)
         {
            CreateWaypointIDText(wp);
         }
      }

      OnGeometryChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::RemoveWaypoint( unsigned id )
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
      mGeometry->dirtyDisplayList();
      mGeometry->setVertexArray(mVerts.get());
      mGeometry->removePrimitiveSet(0);
      osg::PrimitiveSet* ps = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, mVerts->size());
      mGeometry->addPrimitiveSet(ps);
      //setting it back to zero will ensure any user data does not get removed when this function is called again
      mGeometry->setPrimitiveSet(0, ps);

      //TODO- WHERE THE HECK DO I DO THIS IN OSG?????
      glPointSize(10.0f);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::CreateWaypointIDText(const WaypointInterface& wp)
   {
      osgText::Text* text = new osgText::Text(); 
      text->setDrawMode(osgText::Text::TEXT);
      text->setAlignment(osgText::TextBase::CENTER_CENTER);
      text->setAxisAlignment(osgText::TextBase::SCREEN);
      text->setAutoRotateToScreen(true);
      text->setFont("Arial.ttf");
      text->setCharacterSize(0.5f);
      text->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
      text->setBackdropColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
      text->setPosition(wp.GetPosition() + osg::Vec3(0.0f, 0.0f, 0.3f));
      text->setText(dtUtil::ToString(wp.GetID()));
      mGeodeIDs->addDrawable(text);
   }
}
