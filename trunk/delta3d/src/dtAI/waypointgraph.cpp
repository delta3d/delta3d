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

#include <dtAI/waypointgraph.h>
#include <dtAI/waypointcollection.h>
#include <dtAI/aiplugininterface.h>
#include <dtAI/tree.h>
#include <dtAI/navmesh.h>
#include <dtAI/waypointpair.h>
#include <dtAI/aidebugdrawable.h>


#include <dtCore/observerptr.h>

#include <dtUtil/log.h>

#include <algorithm>
#include <map>


namespace dtAI
{
   struct WaypointGraphImpl
   {
      struct WaypointHolder
      {
         WaypointHolder()
            : mParent(0)
            , mWaypoint(0){}
                  
         dtCore::ObserverPtr<WaypointCollection> mParent;
         const WaypointInterface* mWaypoint;
      };

      typedef std::map<WaypointID, WaypointHolder> WaypointMap;
      typedef std::vector<dtCore::RefPtr<WaypointCollection> > WaypointCollectionArray;
      
      WaypointGraphImpl()
         : mNavMesh(new NavMesh())
      {

      }

      void CleanUp()
      {
         mNavMesh->Clear();
         mWaypointOwnership.clear();

         WaypointCollectionArray::iterator iter = mRootNode.begin();
         WaypointCollectionArray::iterator iterEnd = mRootNode.end();

         for(;iter != iterEnd; ++iter)
         {
            dtCore::RefPtr<WaypointCollection> wc = *iter;
            //I guess this is a sanity check, since we are cleaning
            //up, it is possible this is being deleted elsewhere 
            if(wc.valid())
            {
               //perform any cleanup type duties
               wc->CleanUp();

               //delete the object
               wc = NULL;
            }
            else
            {
               LOG_ERROR("Found NULL waypoint nodes in the waypoint tree array.");
            }
         }

         //clear the vector of NULL/deleted pointers
         mRootNode.clear();
      }

      WaypointCollection* CastToCollection(WaypointCollection::WaypointTree* wt)
      {
         const WaypointCollection* col = dynamic_cast<const WaypointCollection*>(wt);
         if(col != NULL)
         {
            return const_cast<WaypointCollection*>(col);
         }

         return NULL;
      }

      const WaypointCollection* CastToCollection(const WaypointCollection::WaypointTree* wt) const 
      {
         const WaypointCollection* col = dynamic_cast<const WaypointCollection*>(wt);

         return col;
      }


      //wc can be NULL, if so it will make a new collection and add it
      void Insert(const WaypointInterface& waypoint, WaypointCollection* wcParent)
      {
         dtCore::RefPtr<WaypointCollection> wc = wcParent;

         if(!wc.valid())
         {
            //make new one
            wc = new WaypointCollection(waypoint.GetPosition());
            Insert(wc);
         }

         //add to the waypoint map
         WaypointHolder wh;
         wh.mWaypoint = &waypoint;
         wh.mParent = wc.get();

         mWaypointOwnership.insert(std::make_pair(waypoint.GetID(), wh));

         //Add to collection
         wc->InsertWaypoint(&waypoint);
      }


      //returns the top most node for a specific child
      WaypointCollection* GetRoot(WaypointCollection* wc)
      {
         WaypointCollection::WaypointTree* rootNode = wc;
         
         while(rootNode->parent() != NULL)
         {
            rootNode = rootNode->parent();
         }
            
         return CastToCollection(rootNode);
      }

      //returns the top most node for a specific child
      const WaypointCollection* GetRoot(const WaypointCollection* wc) const
      {
         const WaypointCollection::WaypointTree* rootNode = wc;

         while(rootNode->parent() != NULL)
         {
            rootNode = rootNode->parent();
         }

         return CastToCollection(rootNode);
      }


      void RemoveAndAddWithNewParent(dtCore::RefPtr<WaypointCollection> lhs, dtCore::RefPtr<WaypointCollection> rhs)
      {
         dtCore::RefPtr<WaypointCollection> newParent = new WaypointCollection();

         //remove existing WaypointNodes from array
         WaypointCollectionArray::iterator root_iter = std::find(mRootNode.begin(), mRootNode.end(), lhs.get());
         if(root_iter != mRootNode.end())
         {
            mRootNode.erase(root_iter);
         }

         root_iter = std::find(mRootNode.begin(), mRootNode.end(), rhs.get());
         if(root_iter != mRootNode.end())
         {
            mRootNode.erase(root_iter);
         }

         if(lhs->parent() != NULL)
         {
            lhs->parent()->remove_subtree(lhs);
         }
         if(rhs->parent() != NULL)
         {
            rhs->parent()->remove_subtree(rhs);
         }

         newParent->insert_subtree(lhs.get(), NULL);
         newParent->insert_subtree(rhs.get(), NULL);

         Insert(newParent.get());
      }

      void Insert(dtCore::RefPtr<WaypointCollection> wc)
      {
         //add node to vector
         mRootNode.push_back(wc);
      }

      void Remove(dtCore::RefPtr<WaypointCollection> wc)
      {
         //remove all paths
         mNavMesh->RemoveAllPaths(wc.get());
         
         //remove from map
         WaypointMap::iterator iter = mWaypointOwnership.find(wc->GetID());
         if(iter != mWaypointOwnership.end())
         {
            mWaypointOwnership.erase(iter);
         }
         
         //if we are a top level node it must be removed from the array
         if(wc->is_root())
         {
            WaypointCollectionArray::iterator root_iter = std::find(mRootNode.begin(), mRootNode.end(), wc);
            if(root_iter != mRootNode.end())
            {
               mRootNode.erase(root_iter);
            }
         }

         wc->CleanUp();
         wc = NULL;
      }

      //they have a path if they share a common parent
      bool HasPath(const WaypointCollection& lhs, const WaypointCollection& rhs) const
      {
         const WaypointCollection* wcParent = GetRoot(&lhs);
         if(rhs.is_descendant_of(*wcParent))
         {
            return true;
         }
         return false;
      }


      dtCore::RefPtr<NavMesh> mNavMesh;
      WaypointMap mWaypointOwnership;
      WaypointCollectionArray mRootNode;
   };


   ///////////////////////////////////////////////////////////////////////////////
   WaypointGraph::WaypointGraph()
      : mImpl(new WaypointGraphImpl())
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointGraph::~WaypointGraph()
   {      
      Clear();

      delete mImpl;
      mImpl = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointGraph::Clear()
   {
      OnClear();
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void WaypointGraph::OnClear()
   {
      mImpl->CleanUp();
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointGraph::CreateGraph(NavMesh& waypointData, WaypointGraphBuilderInterface* wb)
   {
      //adding edge data will make a flat graph, and insert the waypoints
      NavMesh::NavMeshContainer::const_iterator iter = waypointData.GetNavMesh().begin();
      NavMesh::NavMeshContainer::const_iterator iterEnd = waypointData.GetNavMesh().end();

      //add all edges, the resulting graph will be a single WaypointCollections for all inter-traversable waypoints
      for(;iter != iterEnd; ++iter)
      {
         const WaypointInterface* wayFrom = (*iter).second->GetWaypointFrom();
         const WaypointInterface* wayTo = (*iter).second->GetWaypointTo();
         
         int idFrom = wayFrom->GetID();
         int idTo = wayTo->GetID();

         if(!Contains(idFrom))
         {
            WaypointCollection* col = wb->SelectBestCollection(*wayFrom, *this);
            mImpl->Insert(*wayFrom, col);
         }

         if(!Contains(idTo))
         {
            WaypointCollection* col = wb->SelectBestCollection(*wayTo, *this);
            mImpl->Insert(*wayTo, col);
         }                  

         AddEdge(wayFrom, wayTo); 
      }     

   }


   ///////////////////////////////////////////////////////////////////////////////
   //bool WaypointGraph::LoadWaypointFile( const std::string& filename )
   //{
   //   return false;
   //}

   ///////////////////////////////////////////////////////////////////////////////
   //bool WaypointGraph::SaveWaypointFile( const std::string& filename )
   //{
   //   return false;
   //}

   /////////////////////////////////////////////////////////////////////////////
   void WaypointGraph::InsertWaypoint(WaypointInterface* waypoint)
   {
      if(!Contains(waypoint->GetID()))
      {
         //passing in null will create a new one
         mImpl->Insert(*waypoint, NULL);         
      }
      else //since we already have the waypoint, assume we are moving it,
           //a valid operation and noted in the header
      {
         //recalculate up the tree
         WaypointCollection* wc = FindCollection(waypoint->GetID());
         if(wc != NULL)
         {            
            wc->Recalculate();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointGraph::RemoveWaypoint(WaypointInterface* waypoint)
   {
      WaypointGraphImpl::WaypointMap::iterator iter = mImpl->mWaypointOwnership.find(waypoint->GetID());
      if(iter != mImpl->mWaypointOwnership.end())
      {
         //remove the edges before you remove the waypoint
         RemoveAllEdgesFromWaypoint(waypoint);

         //remove the waypoint from the graph
         WaypointGraphImpl::WaypointHolder& wh = (*iter).second;
         if(wh.mParent == NULL)
         {
            LOG_ERROR("All waypoints must have a non NULL parent");
         }
         else
         {
            wh.mParent->RemoveWaypoint(waypoint);
            //if this waypoint is the only child of this collection
            //then remove the collection
            if(wh.mParent->degree() == 0)
            {               
               //remove immediate parent
               mImpl->Remove(wh.mParent.get());
            }
         }
         
         //erase the waypoint from the map
         mImpl->mWaypointOwnership.erase(iter);
         return true;
      }
      
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   const WaypointInterface* WaypointGraph::FindWaypoint(WaypointID id) const
   {
      WaypointGraphImpl::WaypointMap::const_iterator iter = mImpl->mWaypointOwnership.find(id);
      if(iter != mImpl->mWaypointOwnership.end())
      {
         return (*iter).second.mWaypoint;
      }

      return NULL;
   }

   //if waypoint is a collection returns it, else returns parent of actual waypoint
   //may return NULL
   /////////////////////////////////////////////////////////////////////////////
   WaypointCollection* WaypointGraph::FindCollection( WaypointID id )
   {
      WaypointGraphImpl::WaypointMap::iterator iter = mImpl->mWaypointOwnership.find(id);
      if(iter != mImpl->mWaypointOwnership.end())
      {
         WaypointGraphImpl::WaypointHolder& wh = (*iter).second;
         const WaypointCollection* wc = dynamic_cast<const WaypointCollection*>(wh.mWaypoint);
         if(wc != NULL)
         {
            return const_cast<WaypointCollection*>(wc);
         }
         else
         {
            return wh.mParent.get();
         }
      }

      return NULL;
   }

   //if waypoint is a collection returns it, else returns parent of actual waypoint
   //may return NULL
   /////////////////////////////////////////////////////////////////////////////
   const WaypointCollection* WaypointGraph::FindCollection(WaypointID id) const
   {
      WaypointGraphImpl::WaypointMap::const_iterator iter = mImpl->mWaypointOwnership.find(id);
      if(iter != mImpl->mWaypointOwnership.end())
      {
         const WaypointGraphImpl::WaypointHolder& wh = (*iter).second;
         const WaypointCollection* wc = dynamic_cast<const WaypointCollection*>(wh.mWaypoint);
         if(wc != NULL)
         {
            return wc;
         }
         else
         {
            return wh.mParent.get();
         }
      }

      return NULL;
   }

   //returns the top most node for a specific child
   /////////////////////////////////////////////////////////////////////////////
   WaypointCollection* WaypointGraph::GetRootParent(WaypointID id)
   {
      WaypointCollection* wc = FindCollection(id);
      if(wc != NULL)
      {
         wc = mImpl->GetRoot(wc);
      }

      return wc;
   }

   //returns the top most node for a specific child
   /////////////////////////////////////////////////////////////////////////////
   const WaypointCollection* WaypointGraph::GetRootParent(WaypointID id) const
   {
      const WaypointCollection* wc = FindCollection(id);
      if(wc != NULL)
      {
         wc = mImpl->GetRoot(wc);
      }

      return wc;
   }


   /////////////////////////////////////////////////////////////////////////////
   void WaypointGraph::AddEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo)
   {
      const WaypointInterface* from = FindWaypoint(pFrom->GetID());
      const WaypointInterface* to = FindWaypoint(pTo->GetID());

      //it is ok to add waypoints with an edge
      if (from == NULL || to == NULL)
      {
         if(from == NULL)
         {
            WaypointCollection* wc = FindCollection(pTo->GetID());
            //if wc is NULL thats fine, this will make a new one
            mImpl->Insert(*pFrom, wc);
         }

         if(to == NULL)
         {
            WaypointCollection* wc = FindCollection(pFrom->GetID());
            //if wc is NULL thats fine, this will make a new one
            mImpl->Insert(*pTo, wc);
         }
      }
      else if(!HasPath(*pFrom, *pTo))
      {
         //if no hierarchical path between them, they are disjoint
         //add the collections as children to a new collection
         WaypointCollection* fromRoot = mImpl->GetRoot(FindCollection(pFrom->GetID()));
         WaypointCollection* toRoot = mImpl->GetRoot(FindCollection(pTo->GetID()));
 
         mImpl->RemoveAndAddWithNewParent(fromRoot, toRoot);         
      }
      

      mImpl->mNavMesh->AddPathSegment(pFrom, pTo);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointGraph::RemoveEdge(const WaypointInterface* wayFrom, const WaypointInterface* wayTo)
   {
      //TODO-

      mImpl->mNavMesh->RemovePathSegment(wayFrom, wayTo);     

      //restructure graph if critical edges are removed

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointGraph::RemoveAllEdgesFromWaypoint(const WaypointInterface* pFrom)
   {
      const WaypointInterface* way = FindWaypoint(pFrom->GetID());

      if(way != NULL)
      {
         NavMesh::NavMeshContainer::iterator iter = mImpl->mNavMesh->begin(pFrom);
         for(;iter != mImpl->mNavMesh->end(pFrom); iter = mImpl->mNavMesh->begin(pFrom))
         {
            RemoveEdge(iter->second->GetWaypointFrom(), iter->second->GetWaypointTo());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointGraph::GetAllEdgesFromWaypoint(const WaypointInterface& pFrom, ConstWaypointArray& result) const
   {
      const WaypointInterface* way = FindWaypoint(pFrom.GetID());

      if(way != NULL)
      {
         NavMesh::NavMeshContainer::iterator iter = mImpl->mNavMesh->begin(way);
         NavMesh::NavMeshContainer::iterator iterEnd = mImpl->mNavMesh->end(way);
         
         for(;iter != iterEnd; ++iter)
         {
            result.push_back( (*iter).second->GetWaypointTo() );
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   //WaypointInterface* WaypointGraph::FindClosest(const osg::Vec3& point3d) const
   //{
   //   //TODO:
   //   return NULL;
   //}

   ///////////////////////////////////////////////////////////////////////////////
   //void WaypointGraph::FindRadial(const osg::Vec3& pos, float radius, WaypointArray& vectorIn) const
   //{

   //}

   ///////////////////////////////////////////////////////////////////////////////
   //int WaypointGraph::GetNumLevels() const
   //{
   //   //todo
   //   return 0;
   //}

   ///////////////////////////////////////////////////////////////////////////////
   //int WaypointGraph::GetWaypointLevel(WaypointInterface& id)
   //{
   //   int treeLevel = -1;
   //   WaypointGraphImpl::WaypointMap::const_iterator iter = mImpl->mWaypoints.find(waypoint->GetID());
   //   if(iter != mImpl->mWaypoints.end())
   //   {
   //      return treeLevel;
   //   }

   //   return treeLevel;
   //}

   ///////////////////////////////////////////////////////////////////////////////
   //void WaypointGraph::SetCurrentSearchLevel(int l)
   //{
   //   mImpl->mCurrentSearchLevel = l;
   //}

   ///////////////////////////////////////////////////////////////////////////////
   //int WaypointGraph::GetCurrentSearchLevel() const
   //{
   //   return mImpl->mCurrentSearchLevel;
   //}

   ///////////////////////////////////////////////////////////////////////////////
   //WaypointID WaypointGraph::MapWaypointToLevel(int atLevel, WaypointInterface& point, int outLevel)
   //{
   //   return 0;
   //}

   /////////////////////////////////////////////////////////////////////////////
   //bool WaypointGraph::FindPath(WaypointInterface& from, WaypointInterface& to, WaypointArray& result) const
   //{
   //   //todo
   //   return false;
   //}

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointGraph::HasPath(const WaypointInterface& rhs, const WaypointInterface& lhs) const
   {
      const WaypointCollection* lhsC = FindCollection(lhs.GetID());
      const WaypointCollection* rhsC = FindCollection(rhs.GetID());

      bool result = false;
      if(lhsC != NULL && rhsC != NULL)
      {
         result = mImpl->HasPath(*lhsC, *rhsC);
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointGraph::Contains(WaypointID id) const
   {
      return (mImpl->mWaypointOwnership.find(id) != mImpl->mWaypointOwnership.end());
   }

   void WaypointGraph::GetNavMeshAtLevel(const WaypointCollection* root, unsigned level, NavMesh& result) const
   {
      const WaypointCollection* wcAtLevel = root;
      int numLevels = root->level();
      for(int currentLevel = 0; currentLevel < numLevels; ++currentLevel)
      {
         //wcAtLevel = root->first_child();

         //if(wcAtLevel != NULL && currentLevel == level)
         //{
         //   
         //}
      }
   }

   AIDebugDrawable* WaypointGraph::GetDebugDrawableAtLevel(const WaypointCollection* root, unsigned level) const
   {
      AIDebugDrawable* dd = new AIDebugDrawable();

      dtCore::RefPtr<NavMesh> nm = new NavMesh();

      GetNavMeshAtLevel(root, level, *nm);
      dd->UpdateWaypointGraph(*nm);
      return dd;
   }

} // namespace dtAI
