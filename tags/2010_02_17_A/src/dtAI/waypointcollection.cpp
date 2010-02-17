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
#include <dtAI/waypointtypes.h>


#include <dtUtil/log.h>


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

      mChildEdges.clear();
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
   WaypointCollection* WaypointCollection::GetParent()
   {
      WaypointTree* parentNode = WaypointTree::parent();
      if(parentNode != NULL)
      {
         return dynamic_cast<WaypointCollection*>(parentNode);
      }

      return NULL;
   }
      
   /////////////////////////////////////////////////////////////////////////////
   const WaypointCollection* WaypointCollection::GetParent() const
   {
      const WaypointTree* parentNode = WaypointTree::parent();
      if(parentNode != NULL)
      {
         return dynamic_cast<const WaypointCollection*>(parentNode);
      }

      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::Insert(const WaypointInterface* waypoint)
   {
      if(!HasChild(waypoint->GetID()))
      {
         if(waypoint->GetWaypointType() == *WaypointTypes::WAYPOINT_COLLECTION)
         {
            const WaypointCollection* wc = dynamic_cast<const WaypointCollection*>(waypoint);
            
            if(wc != NULL)
            {
               AddChild(wc);
            }
            else
            {
               LOG_ERROR("Error adding WaypointCollection '" + waypoint->ToString() + "' as child of WaypointCollection '" + ToString() + "'.");
            }
         }
         else
         {
            InsertWaypoint(waypoint);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::Remove(const WaypointInterface* waypoint)
   {
      if(waypoint->GetWaypointType() == *WaypointTypes::WAYPOINT_COLLECTION)
      {
         const WaypointCollection* wc = dynamic_cast<const WaypointCollection*>(waypoint);

         if(wc != NULL)
         {
            RemoveChild(wc);
         }
         else
         {
            LOG_ERROR("Error removing WaypointCollection '" + waypoint->ToString() + "' from WaypointCollection '" + ToString() + "'.");
         }
      }
      else
      {
         RemoveWaypoint(waypoint);
      }
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
         LOG_ERROR("Only leaf nodes may hold concrete waypoints, if this is a WaypointCollection use AddChild().");
      }         
      
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::AddChild(dtCore::RefPtr<const WaypointCollection> waypoint)
   {
      //add to the waypoint tree, it only becomes a leaf by inserting a waypoint into it
      if(mLeaf)
      {
        LOG_ERROR("WaypointCollection leaf nodes may only hold concrete waypoints, at some point a concrete waypoint was childed to this node.");
      }
      else
      {
         WaypointCollection* wc = const_cast<WaypointCollection*>(waypoint.get());
         WaypointTree::insert_subtree(wc, 0);
         Recalculate();
      }         
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::RemoveChild(dtCore::RefPtr<const WaypointCollection> waypoint)
   {         
      //a check to make sure we are indeed the parent
      if( (waypoint->parent() != NULL) && (waypoint->parent()->value->GetID() == GetID()) )
      {         
         WaypointCollection* wc = const_cast<WaypointCollection*>(waypoint.get());
         WaypointTree::remove_subtree(wc);
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
   void WaypointCollection::AddEdge(WaypointID sibling, const ChildEdge& edge)
   {
      if(!ContainsEdge(sibling, edge))
      {
         mChildEdges.insert(std::make_pair(sibling, edge));
      }
   }


   /////////////////////////////////////////////////////////////////////////////
   bool WaypointCollection::RemoveEdge(WaypointID sibling, const ChildEdge& edge)
   {    
      ChildEdgeMap::iterator iter = mChildEdges.lower_bound(sibling);
      ChildEdgeMap::iterator iterEnd = mChildEdges.upper_bound(sibling);
      for (;iter != iterEnd; ++iter)
      {
         if((*iter).second == edge)
         {
            mChildEdges.erase(iter);
            return true;
         }
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::ClearEdges()
   {
      mChildEdges.clear();
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::GetEdges(WaypointID sibling, ChildEdgeArray& result) const
   {
      ChildEdgeMap::const_iterator iter = mChildEdges.lower_bound(sibling);
      ChildEdgeMap::const_iterator iterEnd = mChildEdges.upper_bound(sibling);
      for (;iter != iterEnd; ++iter)
      {
         result.push_back((*iter).second);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointCollection::CreateProperties( WaypointPropertyBase& container )
   {
      WaypointInterface::CreateProperties(container);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointCollection::ContainsEdge(WaypointID sibling, const ChildEdge& edge)
   {
      ChildEdgeMap::const_iterator iter = mChildEdges.lower_bound(sibling);
      ChildEdgeMap::const_iterator iterEnd = mChildEdges.upper_bound(sibling);
      for (;iter != iterEnd; ++iter)
      {
         if((*iter).second == edge)
         {
            return true;
         }
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointCollection::HasChild( WaypointID id )
   {
      WaypointTreeConstChildIterator iter = begin_child();
      WaypointTreeConstChildIterator iterEnd = end_child();

      for(;iter != iterEnd; ++iter)
      {
         if(iter->value->GetID() == id)
         {
            return true;
         }
      }  

      return false;
   }
} // namespace dtAI
