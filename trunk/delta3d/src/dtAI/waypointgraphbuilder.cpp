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

#include <algorithm>


namespace dtAI
{
   template<class _Container>
   class insert_back
   {	
   public:
      typedef _Container container_type;
      typedef typename _Container::reference reference;

      explicit insert_back(_Container& _Cont)
         : container(&_Cont)
      {	// construct with container
      }

      //returns true if the value was added to the container
      void operator()(typename _Container::const_reference _Val)
      {	// push value into container
         container->push_back(_Val);
      }

   protected:
      _Container* container;	// pointer to container
   };

   template<class _Container>
   class insert_back_no_duplicates
   {	
   public:
      typedef _Container container_type;
      typedef typename _Container::reference reference;

      explicit insert_back_no_duplicates(_Container& _Cont)
         : container(&_Cont)
      {	// construct with container
      }

      //returns true if the value was added to the container
      bool operator()(typename _Container::const_reference _Val)
      {	// push value into container
         if(std::find(container->begin(), container->end(), _Val) == container->end())
         {
            container->push_back(_Val);
            return true;
         }

         return false;
      }

   protected:
      _Container* container;	// pointer to container
   };


   template<class _Container>
   class array_remove
   {	
   public:
      typedef _Container container_type;
      typedef typename _Container::reference reference;

      explicit array_remove(_Container& _Cont)
         : container(&_Cont)
      {	// construct with container
      }

      //returns true if the value was added to the container
      bool operator()(typename _Container::reference _Val)
      {	// push value into container
         typename _Container::iterator iter = std::find(container->begin(), container->end(), _Val);
         if(iter != container->end())
         {
            container->erase(iter);
            return true;
         }

         return false;
      }

   protected:
      _Container* container;	// pointer to container
   };

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

      CreateSearchLevelNavMesh(*(sl->mNavMesh), newSearchLevel);

      //return ConvertFromBuilderSearchLevel(newSearchLevel.get());
      return !mAssignedNodes.empty();
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
                  wc->Insert(wayTo);
                  mAssignedNodes.push_back(wayTo);
                  mWPGraph->SetParent(wayTo->GetID(), wc);
                                       
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
         FindAllMatches(canidates, mAssignedNodes, insert_back<ConstWaypointArray>(tmp));
         std::for_each(tmp.begin(), tmp.end(), array_remove<ConstWaypointArray>(canidates));      

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
      WaypointCollection* parentNode = CreateWaypointCollection(curWay->GetPosition());
      mWPGraph->InsertCollection(parentNode, mCurrentCreationLevel);

      parentNode->Insert(curWay);
      mWPGraph->SetParent(curWay->GetID(), parentNode);

      mAssignedNodes.push_back(curWay);

      int totalCount = dtUtil::Min(int(cliques.size()), numChildren);
      for(int childNum = 0; childNum < totalCount; ++childNum)
      {
         //we have a successful match
         const WaypointInterface* curMatch = cliques[childNum];
         Assign(curMatch, parentNode);
         mWPGraph->SetParent(curMatch->GetID(), parentNode);
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
               FindAllMatches(canidates, canidatesNext, insert_back_no_duplicates<ConstWaypointArray>(cliques));
               canidatesNext.clear();
            }
         }

         //all the nodes in the resulting cliques array are potential matches, we must 
         //eliminate all nodes that are already assigned
         canidates.clear();
         FindAllMatches(cliques, mAssignedNodes, insert_back<ConstWaypointArray>(canidates));
         
         std::for_each(canidates.begin(), canidates.end(), array_remove<ConstWaypointArray>(cliques));         
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
   WaypointCollection* WaypointGraphBuilder::CreateWaypointCollection(const osg::Vec3& pos)
   {
      return dynamic_cast<WaypointCollection*>(mAIInterface->CreateWaypoint(pos, *WaypointTypes::WAYPOINT_COLLECTION));
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

   //////////////////////////////////////////////////////////////////////////
   void WaypointGraphBuilder::CreateSearchLevelNavMesh(const NavMesh& nm, BuilderSearchLevel* sl)
   {
      BuilderSearchLevel::WaypointCollectionArray::iterator iter = sl->mNodes.begin();
      BuilderSearchLevel::WaypointCollectionArray::iterator iterEnd = sl->mNodes.end();

      for(; iter != iterEnd; ++iter)
      {
         WaypointCollection* wc = (*iter).get();
         
         WaypointCollection::WaypointTree::const_child_iterator children = wc->begin_child();
         WaypointCollection::WaypointTree::const_child_iterator childrenEnd = wc->end_child();
         
         for(; children != childrenEnd; ++children)
         {
            const WaypointInterface* childPtr = children->value;
            
            NavMesh::NavMeshContainer::const_iterator nm_iter = nm.begin(childPtr);
            NavMesh::NavMeshContainer::const_iterator nm_iterEnd = nm.end(childPtr);

            //the actual child edges are stored on the WaypointCollection itself
            wc->GetNavMesh().InsertCopy(nm_iter, nm_iterEnd);

            //the search level only contains edges relevant to the nodes in it
            for(;nm_iter != nm_iterEnd; ++nm_iter)
            {
               WaypointCollection* wpToParent = mWPGraph->GetParent((*nm_iter).second->GetWaypointTo()->GetID());

               //todo- why would wpToParent ever be NULL
               if((wpToParent != NULL) && wpToParent->GetID() != wc->GetID())
               {
                  //this effectively adds an edge from me to all my children's destination waypoint parents                  
                  //sl->mNavMesh->AddEdge(wc, wpToParent);
                  mWPGraph->AddEdge(wc->GetID(), wpToParent->GetID());
               }
            }

         }


      }
   }

   bool WaypointGraphBuilder::Assign(const WaypointInterface* wp, WaypointCollection* parentNode)
   {
      //TODO: its late and I need to refactor this below
      ConstWaypointArray::iterator tIter = std::find(mUnAssignedNodes.begin(), mUnAssignedNodes.end(), wp);
      if(tIter != mUnAssignedNodes.end())
      {
         //and finally add as a child of the common parent
         parentNode->Insert(wp);
         mUnAssignedNodes.erase(tIter);
         mAssignedNodes.push_back(wp);
         return true;
      }
      else
      {
         tIter = std::find(mNodesUnMatched.begin(), mNodesUnMatched.end(), wp);
         if(tIter != mNodesUnMatched.end())
         {               
            parentNode->Insert(wp);
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
