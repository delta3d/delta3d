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

#include <dtAI/deltaaiinterface.h>
#include <dtAI/navmesh.h>
#include <dtAI/waypointcollection.h>
#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointreaderwriter.h>
#include <dtAI/waypointrenderinfo.h>
#include <dtUtil/templateutility.h>
#include <iterator>

namespace dtAI
{
   /////////////////////////////////////////////////////////////////////////////
   // file saving and loading utils
   /////////////////////////////////////////////////////////////////////////////
   namespace WaypointFileHeader
   {
/*      const unsigned FILE_IDENT = 5705313;

      const unsigned VERSION_MAJOR = 1;
      const unsigned VERSION_MINOR = 0;

      const char FILE_START_END_CHAR = '!';
*/
   };

   /////////////////////////////////////////////////////////////////////////////
   DeltaAIInterface::DeltaAIInterface()
      : mWaypointGraph(new WaypointGraph())
      , mAStar(*mWaypointGraph)
      , mKDTreeDirty(true)
      , mKDTree(new WaypointKDTree(std::ptr_fun(KDHolderIndexFunc)))
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::InsertWaypoint(WaypointInterface* waypoint)
   {
      if (GetWaypointById(waypoint->GetID()) == NULL)
      {
         mWaypoints.push_back(waypoint);
         mWaypointGraph->InsertWaypoint(waypoint);

         // if we have created a drawable then we must add and remove to it
         if (mDrawable.valid())
         {
            mDrawable->InsertWaypoint(*waypoint);
         }

         KDHolder node(waypoint->GetPosition(), waypoint->GetID());
         mKDTree->insert(node);

         mKDTreeDirty = true;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::InsertCollection(WaypointCollection* waypoint, unsigned level)
   {
      if (GetWaypointById(waypoint->GetID()) == NULL)
      {
         mWaypoints.push_back(waypoint);
         mWaypointGraph->InsertCollection(waypoint, level);

         // if we have created a drawable then we must add and remove to it
         if (mDrawable.valid())
         {
            mDrawable->InsertWaypoint(*waypoint);
         }

         KDHolder node(waypoint->GetPosition(), waypoint->GetID());
         mKDTree->insert(node);

         mKDTreeDirty = true;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointGraph& DeltaAIInterface::GetWaypointGraph()
   {
      return *mWaypointGraph;
   }

   /////////////////////////////////////////////////////////////////////////////
   const WaypointGraph& DeltaAIInterface::GetWaypointGraph() const
   {
      return *mWaypointGraph;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeltaAIInterface::Assign(WaypointID childWp, WaypointCollection* parentWp)
   {
      // only the parent can be added through this function
      bool containsWp = GetWaypointById(parentWp->GetID()) != NULL;

      if (mWaypointGraph->Assign(childWp, parentWp))
      {
         if (!containsWp)
         {
            mWaypoints.push_back(parentWp);

            // if we have created a drawable then we must add and remove to it
            if (mDrawable.valid())
            {
               mDrawable->InsertWaypoint(*parentWp);
            }

            KDHolder node(parentWp->GetPosition(), parentWp->GetID());
            mKDTree->insert(node);

            mKDTreeDirty = true;
         }

         return true;
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeltaAIInterface::MoveWaypoint(WaypointInterface* wi, const osg::Vec3& newPos)
   {
      // the kd-tree cannot move :( for now remove and re-insert
      osg::Vec3 pos = wi->GetPosition();

      find_result found = mKDTree->find_nearest(pos, 1.0f);
      if (found.first != mKDTree->end() && (*found.first).mID == wi->GetID())
      {
         mKDTree->erase(found.first);

         KDHolder node(newPos, wi->GetID());
         mKDTree->insert(node);

         wi->SetPosition(newPos);

         // re-insert to move
         mWaypointGraph->InsertWaypoint(wi);

         // the drawable allows re-inserting to move
         if (mDrawable.valid())
         {
            mDrawable->InsertWaypoint(*wi);
         }

         mKDTreeDirty = true;
         return true;
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeltaAIInterface::RemoveWaypoint(WaypointInterface* wi)
   {
      bool result = false;

      osg::Vec3 pos = wi->GetPosition();
      std::vector<KDHolder> v;

      mKDTree->find_within_range(pos, 0.1f, std::back_inserter(v));

      std::vector<KDHolder>::const_iterator iter = v.begin();
      std::vector<KDHolder>::const_iterator iterEnd = v.end();
      for (; iter != iterEnd; ++iter)
      {
         WaypointInterface* wpPtr = GetWaypointById(iter->mID);
         if (wpPtr != NULL && wpPtr->GetID() == wi->GetID())
         {
            // remove from current drawable
            if (mDrawable.valid())
            {
               RemoveAllEdges(wpPtr->GetID());
               mDrawable->RemoveWaypoint(wpPtr->GetID());
            }

            // remove from kd-tree
            mKDTree->erase(*iter);
            mKDTreeDirty = true;

            // remove from waypoint graph
            mWaypointGraph->RemoveWaypoint(wpPtr->GetID());

            // finally remove it from internal array
            dtUtil::array_remove<WaypointRefArray> rm(mWaypoints);
            dtCore::RefPtr<WaypointInterface> wpRef = wpPtr;
            result = rm(wpRef);
            break;
         }
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointInterface* DeltaAIInterface::GetWaypointById(WaypointID id)
   {
      // todo- fix this const cast
      return const_cast<WaypointInterface*>(mWaypointGraph->FindWaypoint(id));
   }

   /////////////////////////////////////////////////////////////////////////////
   const WaypointInterface* DeltaAIInterface::GetWaypointById(WaypointID id) const
   {
      // todo- fix this const cast
      return const_cast<WaypointInterface*>(mWaypointGraph->FindWaypoint(id));
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointInterface* DeltaAIInterface::GetWaypointByName(const std::string& name)
   {
      WaypointRefArray::const_iterator iter = mWaypoints.begin();
      WaypointRefArray::const_iterator iterEnd = mWaypoints.end();

      for (;iter != iterEnd; ++iter)
      {
         if ((*iter)->ToString() == name)
         {
            return (*iter).get();
         }
      }

      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::GetWaypointsByName(const std::string& name, WaypointArray& arrayToFill)
   {
      WaypointRefArray::const_iterator iter = mWaypoints.begin();
      WaypointRefArray::const_iterator iterEnd = mWaypoints.end();

      for (;iter != iterEnd; ++iter)
      {
         if ((*iter)->ToString() == name)
         {
            arrayToFill.push_back((*iter).get());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeltaAIInterface::HasEdge(WaypointID from, WaypointID to)
   {
      bool hasEdge = false;

      ConstWaypointArray toArray;
      GetAllEdgesFromWaypoint(from, toArray);

      // Is the "to" waypoint accessible from "from"?
      for (size_t toIndex = 0; toIndex < toArray.size(); ++toIndex)
      {
         if (toArray[toIndex]->GetID() == to)
         {
            hasEdge = true;
            break;
         }
      }

      return hasEdge;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::AddEdge(WaypointID pFrom, WaypointID pTo)
   {
      mWaypointGraph->AddEdge(pFrom, pTo);

      /*if (mDrawable.valid())
      {
      mDrawable->UpdateWaypointGraph(mWaypointManager.GetNavMesh());
      }*/
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeltaAIInterface::RemoveEdge(WaypointID pFrom, WaypointID pTo)
   {
      bool result = mWaypointGraph->RemoveEdge(pFrom, pTo);

      /*if (result && mDrawable.valid())
      {
      mDrawable->UpdateWaypointGraph(mWaypointManager.GetNavMesh());
      }*/
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::RemoveAllEdges(WaypointID pFrom)
   {
      dtAI::WaypointGraph::ConstWaypointArray edgePoints;
      mWaypointGraph->GetAllEdgesFromWaypoint(pFrom, edgePoints);

      if (mDrawable.valid())
      {
         WaypointInterface* wi = GetWaypointById(pFrom);
         mDrawable->RemoveAllEdges(wi);

         // This brute force method should no longer be necessary
         // but is intentionally left here in case things go awry.
         // A side effect of the code below is causing the debug
         // drawable to draw all edges, even in selection based mode.

         //NavMesh* nm = mWaypointGraph->GetNavMeshAtSearchLevel(0);
         //if (nm != NULL)
         //{
         //   mDrawable->UpdateWaypointGraph(*nm);
         //}
      }

      mWaypointGraph->RemoveAllEdgesFromWaypoint(pFrom);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::GetAllEdgesFromWaypoint(WaypointID pFrom, ConstWaypointArray& result) const
   {
      mWaypointGraph->GetAllEdgesFromWaypoint(pFrom, result);
   }

   /////////////////////////////////////////////////////////////////////////////
   PathFindResult DeltaAIInterface::FindPath(WaypointID pFrom, WaypointID pTo, ConstWaypointArray& result)
   {
      WaypointGraphAStar astar(*mWaypointGraph);

      return astar.FindSingleLevelPath(pFrom, pTo, result);
   }

   /////////////////////////////////////////////////////////////////////////////
   PathFindResult DeltaAIInterface::HierarchicalFindPath(WaypointID pFrom, WaypointID pTo, ConstWaypointArray& result)
   {
      WaypointGraphAStar astar(*mWaypointGraph);

      return astar.HierarchicalFindPath(pFrom, pTo, result);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::ClearMemory()
   {
      mKDTree->clear();

      if (mDrawable.valid())
      {
         mDrawable->ClearMemory();
      }

      mWaypointGraph->Clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeltaAIInterface::LoadLegacyWaypointFile(const std::string& filename)
   {
      //this will use the WaypointManager to load the file, leaving the WM
      //fully reconstituted.  Then we can copy the data back into the
      //AiInterfaceActor.  If memory allocation is a concern and the WaypointManager
      //isn't being used in the application, then WaypointManager::Clear() can be
      //called after this to free up some memory.
      WaypointManager& wm = WaypointManager::GetInstance();
      bool result = wm.ReadFile(filename);
      if (result)
      {
         for (WaypointManager::WaypointMap::const_iterator i = wm.GetWaypoints().begin();
            i != wm.GetWaypoints().end();
            ++i)
         {
            InsertWaypoint(i->second);
         }

         for (NavMesh::NavMeshContainer::const_iterator nm_iter = wm.GetNavMesh().GetNavMesh().begin();
            nm_iter != wm.GetNavMesh().GetNavMesh().end();
            ++nm_iter)
         {
            const WaypointPair* wp = (*nm_iter).second;

            AddEdge(wp->GetWaypointFrom()->GetID(), wp->GetWaypointTo()->GetID());
         }

         // Waypoints are deleted in ClearMemory.  The WaypointManager is legacy
         // and should not be used for memory management if at all possible.
         wm.SetDeleteOnClear(false);

         // The WaypointManager doesn't need to hang on to this anymore
         ///Note: doing this will prevent existing WaypointManager apps, which use the old .ai files, from working
         //wm.Clear();
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeltaAIInterface::LoadWaypointFile(const std::string& filename)
   {
      dtCore::RefPtr<WaypointReaderWriter> reader = new WaypointReaderWriter(*this);
      // Hide the drawable for a moment so that it doesn't repeatedly call the geometry changed code.
      dtCore::RefPtr<AIDebugDrawable> tempDrawable = mDrawable;
      mDrawable = NULL;
      bool result = reader->LoadWaypointFile(filename);
      mDrawable = tempDrawable;

      if (!result)
      {
         //this is temporary to support the old waypoint file
         result = LoadLegacyWaypointFile(filename);
      }
      else
      {
         if (mDrawable.valid())
         {
            UpdateDebugDrawable();
         }
      }

      mLastFileLoaded = filename;
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeltaAIInterface::SaveWaypointFile(const std::string& filename)
   {
      dtCore::RefPtr<WaypointReaderWriter> reader = new WaypointReaderWriter(*this);
      if (filename.empty() && !mLastFileLoaded.empty())
      {
         return reader->SaveWaypointFile(mLastFileLoaded);
      }
      else
      {
         return reader->SaveWaypointFile(filename);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::SetDebugDrawable(AIDebugDrawable* debugDrawable)
   {
      if (mDrawable.valid())
      {
         mDrawable->ClearMemory();
      }

      mDrawable = debugDrawable;
      if (mDrawable.valid())
      {
         UpdateDebugDrawable();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable* DeltaAIInterface::GetDebugDrawable()
   {
      if (!mDrawable.valid())
      {
         mDrawable = new AIDebugDrawable();
         UpdateDebugDrawable();
      }

      return mDrawable.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::UpdateDebugDrawable()
   {
      if (mDrawable->GetRenderInfo()->IsAnyRenderingEnabled())
      {
         std::vector<dtAI::WaypointInterface*> waypointList;
         GetWaypoints(waypointList);

         mDrawable->SetWaypoints(waypointList);

         NavMesh* nm = mWaypointGraph->GetNavMeshAtSearchLevel(0);
         if (nm != NULL)
         {
            mDrawable->UpdateWaypointGraph(*nm);
         }
      }
      else
      {
         mDrawable->OnRenderInfoChanged();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::GetWaypoints(WaypointArray& toFill)
   {
      std::for_each(mWaypoints.begin(), mWaypoints.end(), dtUtil::insert_back< WaypointArray, dtCore::RefPtr<WaypointInterface> >(toFill));
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::GetWaypoints(ConstWaypointArray& toFill) const
   {
      std::for_each(mWaypoints.begin(), mWaypoints.end(), dtUtil::insert_back< ConstWaypointArray, dtCore::RefPtr<WaypointInterface> >(toFill));
   }

   /////////////////////////////////////////////////////////////////////////////
   size_t DeltaAIInterface::GetNumWaypoints() const
   {
      return mWaypoints.size();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAI::WaypointID DeltaAIInterface::GetMaxWaypointID() const
   {
      dtAI::WaypointID maxID = 0;
      for (size_t i = 0; i < GetNumWaypoints(); ++i)
      {
         maxID = std::max(maxID, mWaypoints[i]->GetID());
      }
      return maxID;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::GetWaypointsByType(const dtCore::ObjectType& type, WaypointArray& toFill)
   {
      WaypointRefArray::const_iterator iter = mWaypoints.begin();
      WaypointRefArray::const_iterator iterEnd = mWaypoints.end();

      for (;iter != iterEnd; ++iter)
      {
         if ((*iter)->GetWaypointType() == type)
         {
            toFill.push_back((*iter).get());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointInterface* DeltaAIInterface::GetClosestWaypoint(const osg::Vec3& pos, float maxRadius)
   {
      if (mKDTreeDirty)
      {
         Optimize();
      }

      WaypointInterface* result = NULL;

      find_result found = mKDTree->find_nearest(pos, maxRadius);
      if (found.first != mKDTree->end())
      {
         result = GetWaypointById(found.first->mID);
      }

      return result;
   }


   /////////////////////////////////////////////////////////////////////////////
   WaypointInterface* DeltaAIInterface::GetClosestNamedWaypoint(const std::string& name, const osg::Vec3& pos, float maxRadius)
   {
      WaypointInterface* closestPoint = NULL;

      WaypointRefArray::const_iterator iter = mWaypoints.begin();
      WaypointRefArray::const_iterator iterEnd = mWaypoints.end();

      float minDistance = FLT_MAX;

      for (;iter != iterEnd; ++iter)
      {
         if ((*iter)->ToString() == name)
         {
            float distanceSquared = ((*iter)->GetPosition() - pos).length2();
            if (distanceSquared < minDistance && distanceSquared < osg::square(maxRadius))
            {
               minDistance = distanceSquared;
               closestPoint = *iter;
            }
         }
      }

      return closestPoint;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeltaAIInterface::GetWaypointsAtRadius(const osg::Vec3& pos, float radius, WaypointArray& arrayToFill)
   {
      if (mKDTreeDirty)
      {
         Optimize();
      }

      std::vector<KDHolder> v;

      mKDTree->find_within_range(pos, radius, std::back_inserter(v));

      std::vector<KDHolder>::const_iterator iter = v.begin();
      std::vector<KDHolder>::const_iterator iterEnd = v.end();
      for (; iter != iterEnd; ++iter)
      {
         WaypointInterface* wi = GetWaypointById(iter->mID);
         if (wi != NULL)
         {
            arrayToFill.push_back(wi);
         }
         else
         {
            LOG_ERROR("Error searching for waypoints.");
         }
      }

      return !arrayToFill.empty();
   }

   /////////////////////////////////////////////////////////////////////////////
   DeltaAIInterface::~DeltaAIInterface()
   {
      ClearMemory();
      delete mKDTree;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaAIInterface::Optimize()
   {
      mKDTree->optimize();
      mKDTreeDirty = false;
   }
}
