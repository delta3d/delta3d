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

#include <dtAI/waypointgraphbuilder.h>
#include <dtAI/waypointcollection.h>
#include <dtAI/aiplugininterface.h>
#include <dtAI/waypointtypes.h>
#include <dtAI/navmesh.h>
#include <dtAI/waypointpair.h>
#include <dtAI/aidebugdrawable.h>

#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/templateutility.h>

#include <algorithm>


namespace dtAI
{
   template<class _Container>
   class copy_navmesh_paths
   {	
   public:
      typedef _Container container_type;
      typedef typename _Container::reference reference;

      explicit copy_navmesh_paths(const NavMesh* nm_from, NavMesh* nm_to)
         : mNMFrom(nm_from)
         , mNMTo(nm_to)
      {	
      }

      void operator()(typename _Container::const_reference _Val)
      {	
         mNMTo->InsertCopy(mNMFrom->begin(_Val), mNMFrom->end(_Val));
      }

   protected:
      const NavMesh* mNMFrom;	
      NavMesh* mNMTo;	
   };

   template <typename _OutputIterator>
   void FindAllMatches(const WaypointGraphBuilder::ConstWaypointArray& set1, const WaypointGraphBuilder::ConstWaypointArray& set2, _OutputIterator out)
   {
      WaypointGraphBuilder::ConstWaypointArray::const_iterator wpIter = set1.begin();
      WaypointGraphBuilder::ConstWaypointArray::const_iterator wpIterEnd = set1.end();
      for(;wpIter != wpIterEnd; ++wpIter)
      {
         const WaypointInterface* canidateWaypoint = (*wpIter);
                  
         if(std::find(set2.begin(), set2.end(), canidateWaypoint) != set2.end())
         {
            out(canidateWaypoint);
         }
      }

   }


   //////////////////////////////////////////////////////////////////////////
   class BuilderSearchLevel: public osg::Referenced
   {
   public:
      typedef std::vector<dtCore::RefPtr<dtAI::WaypointCollection> > WaypointCollectionArray;

      BuilderSearchLevel()
         : mLevelNum(0)
         , mNodes()
         , mNavMesh(new NavMesh())
      {

      }

   protected:
      virtual ~BuilderSearchLevel(){}

   public:

      unsigned mLevelNum;
      WaypointCollectionArray mNodes;
      dtCore::RefPtr<NavMesh> mNavMesh;
   };


   //////////////////////////////////////////////////////////////////////////
   WaypointGraphBuilder::WaypointGraphBuilder(AIPluginInterface& aiInterface, WaypointGraph& wpGraph)
      : mCurrentCreationLevel(0)
      , mAIInterface(&aiInterface)
      , mWPGraph(&wpGraph)
   {

   }

   //////////////////////////////////////////////////////////////////////////
   WaypointGraphBuilder::~WaypointGraphBuilder()
   {

   }

   //////////////////////////////////////////////////////////////////////////
   //dtCore::RefPtr<WaypointGraph> WaypointGraphBuilder::CreateWaypointGraph(const NavMesh& nm)
   //{
   //   WaypointGraph* wpGraph = new WaypointGraph();
   //   return wpGraph;
   //}

   //////////////////////////////////////////////////////////////////////////
   bool WaypointGraphBuilder::CreateNextSearchLevel(WaypointGraph::SearchLevel* sl)
   {
      mUnAssignedNodes.clear();
      mAssignedNodes.clear();
      mNodesUnMatched.clear();

      mCurrentCreationLevel = sl->mLevelNum + 1;

      mUnAssignedNodes.insert(mUnAssignedNodes.begin(), sl->mNodes.begin(), sl->mNodes.end());

      dtCore::RefPtr<BuilderSearchLevel> newSearchLevel = new BuilderSearchLevel();

      CreateAll4Cliques(mUnAssignedNodes, mNodesUnMatched, *(sl->mNavMesh), newSearchLevel.get());
      CreateAll2Cliques(mNodesUnMatched, mUnAssignedNodes, *(sl->mNavMesh), newSearchLevel.get());

      AssignRemainingCliques(mUnAssignedNodes, *(sl->mNavMesh), newSearchLevel.get());

      mWPGraph->CreateAbstractEdgesAtLevel(mCurrentCreationLevel);

      return newSearchLevel->mNodes.size() > 1;
   }
         

   //////////////////////////////////////////////////////////////////////////
   void WaypointGraphBuilder::AssignRemainingCliques(ConstWaypointArray& nodesToBuild, const NavMesh& nm, BuilderSearchLevel* wl)
   {      
      while(!nodesToBuild.empty())
      {         
         const WaypointInterface* curWp = nodesToBuild.back();
         nodesToBuild.pop_back();

         bool unassigned = true;

         NavMesh::NavMeshContainer::const_iterator nm_iter = nm.begin(curWp);
         NavMesh::NavMeshContainer::const_iterator nm_iterEnd = nm.end(curWp);

         //the search level only contains edges relevant to the nodes in it
         for(;nm_iter != nm_iterEnd; ++nm_iter)
         {
            const WaypointPair* wpPair = (*nm_iter).second;
            const WaypointInterface* wayTo = wpPair->GetWaypointTo();

            //make sure we have a bidirectional edge
            if(nm.ContainsEdge(wayTo, curWp))
            {
               WaypointCollection* wc = mWPGraph->GetParent(wayTo->GetID());
               if(wc != NULL)
               {
                  mWPGraph->Assign(wayTo->GetID(), wc);

                  mAssignedNodes.push_back(wayTo);
                                       
                  unassigned = false;
                  break;               
               }
            }
         }

         if(unassigned)
         {
            LOG_ERROR("Cannot find valid parent for waypoint '" + curWp->ToString() + "'.");            
         }

         unassigned = true;
      }
   }      

   //////////////////////////////////////////////////////////////////////////
   void WaypointGraphBuilder::CreateAll2Cliques(ConstWaypointArray& nodesToBuild, ConstWaypointArray& unassignedNodes, const NavMesh& nm, BuilderSearchLevel* wl)
   {      
      while(!nodesToBuild.empty())
      {         
         const WaypointInterface* curWp = nodesToBuild.back();
         nodesToBuild.pop_back();

         ConstWaypointArray canidates, tmp;
         //find all bidirectional paths
         FindCanidates(curWp, nm, canidates);

         //remove all nodes already assigned
         FindAllMatches(canidates, mAssignedNodes, dtUtil::insert_back<ConstWaypointArray>(tmp));
         std::for_each(tmp.begin(), tmp.end(), dtUtil::array_remove<ConstWaypointArray>(canidates));      

         if(!canidates.empty())
         {  
            WaypointCollection* wpColl = CreateClique(1, curWp, canidates, nm);

            //store the condensed node
            wl->mNodes.push_back(wpColl);

            //for each node in the array add all child paths to the navmesh
            //std::for_each(cliques.begin(), cliques.begin() + total, copy_navmesh_paths<ConstWaypointArray>(&nm, &wpColl->GetNavMesh()));
         }
         else
         {
            unassignedNodes.push_back(curWp);
         }   

      }
   }      


   //////////////////////////////////////////////////////////////////////////
   void WaypointGraphBuilder::CreateAll4Cliques(ConstWaypointArray& nodesToBuild, ConstWaypointArray& unassignedNodes, const NavMesh& nm, BuilderSearchLevel* wl)
   {      
      while(!nodesToBuild.empty())
      {         
         const WaypointInterface* curWp = nodesToBuild.back();
         nodesToBuild.pop_back();

         ConstWaypointArray cliques;
         FindCliques(curWp, nm, cliques);

         if(cliques.size() >= 2)
         {
            int total = dtUtil::Min(3, int(cliques.size()));
            WaypointCollection* wpColl = CreateClique(total, curWp, cliques, nm);
            
            //store the condensed node
            wl->mNodes.push_back(wpColl);

            //for each node in the array add all child paths to the navmesh
            //std::for_each(cliques.begin(), cliques.begin() + total, copy_navmesh_paths<ConstWaypointArray>(&nm, &wpColl->GetNavMesh()));
         }
         else
         {
            unassignedNodes.push_back(curWp);
         }   

      }
   }      


   //////////////////////////////////////////////////////////////////////////
   WaypointCollection* WaypointGraphBuilder::CreateClique(int numChildren, const WaypointInterface* curWay, const ConstWaypointArray& cliques, const NavMesh& nm)
   {     
      //create a new hold to contain this waypoint, and potentially 4 siblings
      WaypointCollection* parentNode = CreateWaypointCollection(curWay->GetPosition(), mCurrentCreationLevel);
      
      mWPGraph->Assign(curWay->GetID(), parentNode);

      mAssignedNodes.push_back(curWay);

      int totalCount = dtUtil::Min(int(cliques.size()), numChildren);
      for(int childNum = 0; childNum < totalCount; ++childNum)
      {
         //we have a successful match
         const WaypointInterface* curMatch = cliques[childNum];
         Assign(curMatch, parentNode);
      }

      return parentNode;
   }     


   //////////////////////////////////////////////////////////////////////////
   void WaypointGraphBuilder::FindCliques(const WaypointInterface* wp, const NavMesh& nm, ConstWaypointArray& cliques)
   {
      ConstWaypointArray canidates;
      ConstWaypointArray canidatesNext;

      const WaypointInterface* currentWaypoint = wp;

      NavMesh::NavMeshContainer::const_iterator iter = nm.begin(currentWaypoint);
      NavMesh::NavMeshContainer::const_iterator iterEnd = nm.end(currentWaypoint);

      //add all edges, the resulting graph will be a single WaypointCollections for all inter-traversable waypoints
      for(;iter != iterEnd; ++iter)
      {
         FindCanidates(currentWaypoint, nm, canidates);

         ConstWaypointArray::const_iterator wpIter = canidates.begin();
         ConstWaypointArray::const_iterator wpIterEnd = canidates.end();
         for(;wpIter != wpIterEnd; ++wpIter)
         {
            const WaypointInterface* canidateWaypoint = (*wpIter);

            if(canidateWaypoint->GetID() != currentWaypoint->GetID())
            {
               FindCanidates(canidateWaypoint, nm, canidatesNext);

               //a template function defined at the top of this file
               //insert_back_no_duplicates was a handy back inserter modification, at the top as well
               FindAllMatches(canidates, canidatesNext, dtUtil::insert_back_no_duplicates<ConstWaypointArray>(cliques));
               canidatesNext.clear();
            }
         }

         //all the nodes in the resulting cliques array are potential matches, we must 
         //eliminate all nodes that are already assigned
         canidates.clear();
         FindAllMatches(cliques, mAssignedNodes, dtUtil::insert_back<ConstWaypointArray>(canidates));
         
         std::for_each(canidates.begin(), canidates.end(), dtUtil::array_remove<ConstWaypointArray>(cliques));         
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void WaypointGraphBuilder::FindCanidates(const WaypointInterface* wp, const NavMesh& nm, ConstWaypointArray& result)
   {
      const WaypointInterface* currentWaypoint = wp;

      NavMesh::NavMeshContainer::const_iterator iter = nm.begin(currentWaypoint);
      NavMesh::NavMeshContainer::const_iterator iterEnd = nm.end(currentWaypoint);

      //add all edges, the resulting graph will be a single WaypointCollections for all inter-traversable waypoints
      for(;iter != iterEnd; ++iter)
      {
         WaypointPair* wpPair = (*iter).second;
         //const WaypointInterface* wayFrom = wpPair->GetWaypointFrom();
         const WaypointInterface* wayTo = wpPair->GetWaypointTo();

         if(!nm.IsOneWay(wpPair))
         {
            //add it to the canidate list
            result.push_back(wayTo);
         }
      }
   }


   //////////////////////////////////////////////////////////////////////////
   WaypointCollection* WaypointGraphBuilder::CreateWaypointCollection(const osg::Vec3& pos, unsigned searchLevel)
   {
      WaypointCollection* wc = dynamic_cast<WaypointCollection*>(mAIInterface->CreateNoInsert(*WaypointTypes::WAYPOINT_COLLECTION));
      wc->SetPosition(pos);
      mAIInterface->InsertCollection(wc, searchLevel);
      return wc;
   }


   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<WaypointGraph::SearchLevel> WaypointGraphBuilder::ConvertFromBuilderSearchLevel(BuilderSearchLevel* sl)
   {
      WaypointGraph::SearchLevel* newSearchLevel = new WaypointGraph::SearchLevel();
      newSearchLevel->mNavMesh = new NavMesh();

      newSearchLevel->mNavMesh->InsertCopy(sl->mNavMesh->GetNavMesh().begin(), sl->mNavMesh->GetNavMesh().end());

      BuilderSearchLevel::WaypointCollectionArray::iterator iter = sl->mNodes.begin();
      BuilderSearchLevel::WaypointCollectionArray::iterator iterEnd = sl->mNodes.end();
      for(;iter != iterEnd; ++iter)
      {
         //change from ref pointers to raw pointers 
         newSearchLevel->mNodes.push_back((*iter).get());
      }
      
      return newSearchLevel;
   }

   bool WaypointGraphBuilder::Assign(const WaypointInterface* wp, WaypointCollection* parentNode)
   {
      //TODO: this should be refactored so its not so ugly
      ConstWaypointArray::iterator tIter = std::find(mUnAssignedNodes.begin(), mUnAssignedNodes.end(), wp);
      if(tIter != mUnAssignedNodes.end())
      {
         //and finally add as a child of the common parent         
         mWPGraph->Assign(wp->GetID(), parentNode);
         mUnAssignedNodes.erase(tIter);
         mAssignedNodes.push_back(wp);
         return true;
      }
      else
      {
         tIter = std::find(mNodesUnMatched.begin(), mNodesUnMatched.end(), wp);
         if(tIter != mNodesUnMatched.end())
         {               
            mWPGraph->Assign(wp->GetID(), parentNode);
            mNodesUnMatched.erase(tIter);
            mAssignedNodes.push_back(wp);
            return true;
         }
         else
         {
            LOG_ERROR("Error creating WaypointGraph, waypoint '" + wp->ToString() + "' is part of clique but not in the unassigned array.");                  
         }
      }

      return false;
   }

} // namespace dtAI
