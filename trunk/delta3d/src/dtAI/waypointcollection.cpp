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

#include <dtAI/waypointcollection.h>
#include <dtAI/waypointpair.h>
#include <dtUtil/log.h>

#include <dtAI/waypointtypes.h>

namespace dtAI
{


   /////////////////////////////////////////////////////////////////////////////
   WaypointCollection::WaypointCollection()
      : WaypointTree(this)
      , WaypointInterface(WaypointTypes::WAYPOINT_COLLECTION.get())
      , mLeaf (false)
      , mRadius(0.0f)
      , mPosition()
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointCollection::WaypointCollection(const osg::Vec3& pos)
      : WaypointTree(this)
      , WaypointInterface(WaypointTypes::WAYPOINT_COLLECTION.get())
      , mLeaf (false)
      , mRadius(0.0f)
      , mPosition(pos)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointCollection::WaypointCollection(const WaypointCollection& way)
      : WaypointTree(way)
      , WaypointInterface(WaypointTypes::WAYPOINT_COLLECTION.get())
      , mLeaf(way.mLeaf)
      , mRadius(way.mRadius)
      , mPosition(way.mPosition)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointCollection::~WaypointCollection()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::CleanUp()
   {
      if(!is_root() && parent() != NULL)
      {
         //todo- fix this
         parent()->remove_subtree(this);
      }

      //todo- verify this is called above on erase
      clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointCollection::WaypointTree::ref_pointer WaypointCollection::clone() const
   {
      return new WaypointCollection();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointCollection::IsLeaf()
   {
      return mLeaf;
   }

   /////////////////////////////////////////////////////////////////////////////
   float WaypointCollection::GetRadius()
   {
      return mRadius;
   }

   /////////////////////////////////////////////////////////////////////////////
   /*virtual*/ const osg::Vec3& WaypointCollection::GetPosition() const
   {
      return mPosition;
   }

   /////////////////////////////////////////////////////////////////////////////
   /*virtual*/ void WaypointCollection::SetPosition(const osg::Vec3& pVec)
   {
      mPosition = pVec;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::InsertWaypoint(const WaypointInterface* waypoint)
   {
      //if we were not flagged as a leaf but don't have any children
      //and we get a waypoint, then we become a leaf
      if(!mLeaf && (degree() == 0))
      {
         mLeaf = true;
      }

      //add to the waypoint tree
      if(mLeaf)
      {
         push_back(new WaypointTree(waypoint));
         Recalculate();
      }
      else 
      {
         WaypointCollection* closestChild = FindClosestChild(*waypoint);
         if(closestChild != NULL)
         {
            closestChild->InsertWaypoint(waypoint);
         }
         else
         {
            LOG_ERROR("A waypoint collection thinks it's not a leaf but FindClosestChild() returns NULL.");
         }
         
      }         
      
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::AddChild(dtCore::RefPtr<WaypointCollection> waypoint)
   {
      //add to the waypoint tree
      if(mLeaf)
      {
        LOG_ERROR("Should not be inserting WaypointCollection into leaf node.");
      }
      else
      {
         insert_subtree(waypoint.get(), 0);
      }         
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::RemoveChild(dtCore::RefPtr<WaypointCollection> waypoint)
   {         
      //a check to make sure we are indeed the parent
      if( (waypoint->parent() != NULL) && (waypoint->parent()->value->GetID() == GetID()) )
      {
         
         WaypointTree::remove_subtree(waypoint.get());
         Recalculate();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::RemoveWaypoint(const WaypointInterface* waypoint)
   {
      //NOTE: this only works if the waypoint is an immediate child
      //potentially add find child to Tree class to fix this

      WaypointTreeChildIterator iter = begin_child();
      WaypointTreeChildIterator iterEnd = end_child();
      for(; iter != iterEnd; ++iter)
      {
         if(waypoint->GetID() == iter->value->GetID())
         {
            iter = erase(iter);
            break;
         }            
      }
      
      Recalculate();
   }


   /////////////////////////////////////////////////////////////////////////////
   WaypointCollection* WaypointCollection::FindClosestChild(const WaypointInterface& waypoint)
   {
      //may return null if is leaf node
      WaypointTreeConstChildIterator iter = begin_child();
      WaypointTreeConstChildIterator iterEnd = end_child();

      float dist = 0.0f;

      WaypointInterface* wiPtr = NULL;
      //we dont set the collection ptr until the end so we dont have to cast for every child
      WaypointCollection* wcPtr = NULL;
      for(;iter != iterEnd; ++iter)
      {
         WaypointPair wp(this, iter->value);
         if(wp.Get3DDistance() >= dist)
         {
            wiPtr = const_cast<WaypointInterface*>(iter->value);
            dist = wp.Get3DDistance();
         }
      }   
      if(wiPtr != NULL)
      {
         wcPtr = dynamic_cast<WaypointCollection*>(wiPtr);
      }

      //may return NULL
      return wcPtr;
   }

   /////////////////////////////////////////////////////////////////////////////
   const WaypointCollection* WaypointCollection::FindClosestChild(const WaypointInterface& waypoint) const
   {
      //may return null if is leaf node
      WaypointTree::const_child_iterator iter = begin_child();
      WaypointTree::const_child_iterator iterEnd = end_child();

      float dist = 0.0f;
      
      const WaypointInterface* wiPtr = NULL;
      //we dont set the collection ptr until the end so we dont have to cast for every child
      const WaypointCollection* wcPtr = NULL;
      for(;iter != iterEnd; ++iter)
      {
         WaypointPair wp(this, iter->value);
         if(wp.Get3DDistance() >= dist)
         {
            wiPtr = iter->value;
            dist = wp.Get3DDistance();
         }
      }   
      if(wiPtr != NULL)
      {
         wcPtr = dynamic_cast<const WaypointCollection*>(wiPtr);
      }

      //may return NULL
      return wcPtr;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::Recalculate()
   {
      //calculate new bounds

      WaypointTreeConstChildIterator iter = begin_child();
      WaypointTreeConstChildIterator iterEnd = end_child();
      
      //average points to find new center
      osg::Vec3 newCenter;
      for(;iter != iterEnd; ++iter)
      {
         newCenter += iter->value->GetPosition();
      }       

      //reset position
      mPosition = newCenter;
      int numChildren = degree();
      if(numChildren > 1)
      {
         mPosition /= numChildren;
      }

      //reset radius
      mRadius = 0.0f;
      WaypointTreeConstChildIterator iter2 = begin_child();
      WaypointTreeConstChildIterator iterEnd2 = end_child();
      for(;iter2 != iterEnd2; ++iter2)
      {
         //increase bounds, potentially recenter
         WaypointPair wp(this, iter2->value);
         if(wp.Get3DDistance() > mRadius)
         {
            mRadius = wp.Get3DDistance();
         }
      }         

      //recursively recalculate up to root
      if(!is_root() && parent() != NULL)
      {
         const WaypointCollection* wc = dynamic_cast<const WaypointCollection*>(parent());
         if(wc != NULL)
         {
            const_cast<WaypointCollection*>(wc)->Recalculate();
         }
         else
         {
            LOG_ERROR("Invalid WaypointTree, node has no parent.");
         }

      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::CreateProperties( WaypointPropertyBase& container )
   {
      WaypointInterface::CreateProperties(container);
   }

} // namespace dtAI
