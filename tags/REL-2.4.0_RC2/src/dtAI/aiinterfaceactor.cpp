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


#include <dtAI/aiinterfaceactor.h>
#include <dtAI/aiplugininterface.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/project.h>
#include <dtAI/navmesh.h>
#include <dtAI/waypointcollection.h>
#include <dtAI/astarwaypointutils.h>
#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointtypes.h>
#include <dtAI/waypointgraphastar.h>
#include <dtAI/waypointreaderwriter.h>

#include <dtUtil/templateutility.h>
#include <dtUtil/kdtree.h>

namespace dtAI
{
   //////////////////////////////////////////////////////////////////////////
   //file saving and loading utils
   //////////////////////////////////////////////////////////////////////////
   namespace WaypointFileHeader
   {
      const unsigned FILE_IDENT = 5705313;
      
      const unsigned VERSION_MAJOR = 1;
      const unsigned VERSION_MINOR = 0;

      const char FILE_START_END_CHAR = '!';
   };


   //////////////////////////////////////////////////////////////////////////////////
   //This is the default AI plugin interface implementation
   //////////////////////////////////////////////////////////////////////////////////////////
   struct KDHolder
   {
      typedef float value_type;

      KDHolder(const osg::Vec3& pos)
      {
         d[0] = pos[0];
         d[1] = pos[1];
         d[2] = pos[2];
      }

      KDHolder(const osg::Vec3& pos, WaypointID id)
      {
         d[0] = pos[0];
         d[1] = pos[1];
         d[2] = pos[2];

         mID = id;
      }

      KDHolder(value_type a, value_type b, value_type c)
      {
         d[0] = a;
         d[1] = b;
         d[2] = c;
      }

      KDHolder(const KDHolder& x)
      {
         d[0] = x.d[0];
         d[1] = x.d[1];
         d[2] = x.d[2];
         mID = x.mID;
      }

      operator osg::Vec3()
      {
         return osg::Vec3(d[0], d[1], d[2]);
      }

      inline value_type operator[](size_t const N) const { return d[N]; }

      WaypointID mID;
      value_type d[3];      
   };

   inline float KDHolderIndexFunc(KDHolder t, size_t k ) { return t[k]; }

   typedef std::pointer_to_binary_function<KDHolder, size_t, float> tree_search_func;
   typedef dtUtil::KDTree<3, KDHolder, tree_search_func> WaypointKDTree;
   typedef std::pair<WaypointKDTree::const_iterator, float> find_result;

   class DeltaAIInterface: public AIPluginInterface
   {
   public:
      DeltaAIInterface()
         //: mWaypointManager(WaypointManager::GetInstance())         
         : mWaypointGraph(new WaypointGraph())
         , mAStar(*mWaypointGraph)
         , mKDTreeDirty(true)
         , mKDTree(new WaypointKDTree(std::ptr_fun(KDHolderIndexFunc)))
      {
      }

      void InsertWaypoint(WaypointInterface* waypoint)
      {
         if(GetWaypointById(waypoint->GetID()) == NULL)
         {
            mWaypoints.push_back(waypoint);
            mWaypointGraph->InsertWaypoint(waypoint);

            //if we have created a drawable then we must add and remove to it
            if(mDrawable.valid())
            {
               mDrawable->InsertWaypoint(*waypoint);
            }

            KDHolder node(waypoint->GetPosition(), waypoint->GetID());
            mKDTree->insert(node);

            mKDTreeDirty = true;
         }
      }

      void InsertCollection(WaypointCollection* waypoint, unsigned level)
      {
         if(GetWaypointById(waypoint->GetID()) == NULL)
         {
            mWaypoints.push_back(waypoint);
            mWaypointGraph->InsertCollection(waypoint, level);

            //if we have created a drawable then we must add and remove to it
            if(mDrawable.valid())
            {
               mDrawable->InsertWaypoint(*waypoint);
            }

            KDHolder node(waypoint->GetPosition(), waypoint->GetID());
            mKDTree->insert(node);

            mKDTreeDirty = true;
         }
      }

      WaypointGraph& GetWaypointGraph()
      {
         return *mWaypointGraph;
      }
      
      const WaypointGraph& GetWaypointGraph() const
      {
         return *mWaypointGraph;
      }

      bool Assign(WaypointID childWp, WaypointCollection* parentWp)
      {
         //only the parent can be added through this function
         bool containsWp = GetWaypointById(parentWp->GetID()) != NULL;

         if(mWaypointGraph->Assign(childWp, parentWp))
         {
            if(!containsWp)
            {
               mWaypoints.push_back(parentWp);

               //if we have created a drawable then we must add and remove to it
               if(mDrawable.valid())
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

      bool MoveWaypoint(WaypointInterface* wi, const osg::Vec3& newPos)
      {         
         //the kd-tree cannot move :( for now remove and re-insert
         osg::Vec3 pos = wi->GetPosition();        

         find_result found = mKDTree->find_nearest(pos, 1.0f);
         if(found.first != mKDTree->end() && (*found.first).mID == wi->GetID())
         {            
            mKDTree->erase(found.first);
            
            KDHolder node(newPos, wi->GetID());
            mKDTree->insert(node);

            wi->SetPosition(newPos);

            //re-insert to move
            mWaypointGraph->InsertWaypoint(wi);

            //the drawable allows re-inserting to move
            if(mDrawable.valid())
            {
               mDrawable->InsertWaypoint(*wi);
            }

            mKDTreeDirty = true;
            return true;
         }

         return false;
      }

      bool RemoveWaypoint(WaypointInterface* wi)
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
            if(wpPtr != NULL && wpPtr->GetID() == wi->GetID())
            {            

               //remove from current drawable
               if(mDrawable.valid())
               {
                  RemoveAllEdges(wpPtr->GetID());
                  mDrawable->RemoveWaypoint(wpPtr->GetID());
               }

               //remove from kd-tree
               mKDTree->erase(*iter);
               mKDTreeDirty = true;

               //remove from waypoint graph
               mWaypointGraph->RemoveWaypoint(wpPtr->GetID());

               //finally remove it from internal array
               dtUtil::array_remove<WaypointRefArray> rm(mWaypoints);
               dtCore::RefPtr<WaypointInterface> wpRef = wpPtr;
               result = rm(wpRef);
               break;
            }
         }
      
         return result;
      }

      WaypointInterface* GetWaypointById(WaypointID id)
      {
         //todo- fix this const cast
         return const_cast<WaypointInterface*>(mWaypointGraph->FindWaypoint(id));
      }

      const WaypointInterface* GetWaypointById(WaypointID id) const
      {
         //todo- fix this const cast
         return const_cast<WaypointInterface*>(mWaypointGraph->FindWaypoint(id));
      }

      WaypointInterface* GetWaypointByName(const std::string& name)
      {
         WaypointRefArray::const_iterator iter = mWaypoints.begin();
         WaypointRefArray::const_iterator iterEnd = mWaypoints.end();

         for(;iter != iterEnd; ++iter)
         {
            if((*iter)->ToString() == name)
            {
               return (*iter).get();
            }
         }

         return NULL;
      }

      void GetWaypointsByName(const std::string& name, WaypointArray& arrayToFill)
      {
         WaypointRefArray::const_iterator iter = mWaypoints.begin();
         WaypointRefArray::const_iterator iterEnd = mWaypoints.end();

         for(;iter != iterEnd; ++iter)
         {
            if((*iter)->ToString() == name)
            {
               arrayToFill.push_back((*iter).get());
            }
         }
      }

      void AddEdge(WaypointID pFrom, WaypointID pTo)
      {
         mWaypointGraph->AddEdge(pFrom, pTo);

         /*if(mDrawable.valid())
         {
            mDrawable->UpdateWaypointGraph(mWaypointManager.GetNavMesh());
         }*/
      }

      bool RemoveEdge(WaypointID pFrom, WaypointID pTo)
      {
         bool result = mWaypointGraph->RemoveEdge(pFrom, pTo);

         /*if(result && mDrawable.valid())
         {
            mDrawable->UpdateWaypointGraph(mWaypointManager.GetNavMesh());
         }*/
         return result;
      }

      void RemoveAllEdges(WaypointID pFrom)
      {
         mWaypointGraph->RemoveAllEdgesFromWaypoint(pFrom);
         if(mDrawable.valid())
         {
            NavMesh* nm = mWaypointGraph->GetNavMeshAtSearchLevel(0);
            if(nm != NULL)
            {
               mDrawable->UpdateWaypointGraph(*nm);
            }
         }
      }

      void GetAllEdgesFromWaypoint(WaypointID pFrom, ConstWaypointArray& result)
      {
         mWaypointGraph->GetAllEdgesFromWaypoint(pFrom, result);
      }

      PathFindResult FindPath(WaypointID pFrom, WaypointID pTo, ConstWaypointArray& result)
      {
         WaypointGraphAStar astar(*mWaypointGraph); 

         return astar.FindSingleLevelPath(pFrom, pTo, result);
      }

      PathFindResult HierarchicalFindPath(WaypointID pFrom, WaypointID pTo, ConstWaypointArray& result)
      {
         WaypointGraphAStar astar(*mWaypointGraph); 

         return astar.HierarchicalFindPath(pFrom, pTo, result);
      }

      void ClearMemory()
      {
         mKDTree->clear();

         if(mDrawable.valid())
         {
            mDrawable->ClearMemory();
         }

         mWaypointGraph->Clear();
      }

      bool LoadLegacyWaypointFile(const std::string& filename)
      {
         WaypointManager& wm = WaypointManager::GetInstance();
         bool result = wm.ReadFile(filename);
         if(result)
         {
            NavMesh::NavMeshContainer::const_iterator nm_iter = wm.GetNavMesh().GetNavMesh().begin();
            NavMesh::NavMeshContainer::const_iterator nm_iterEnd = wm.GetNavMesh().GetNavMesh().end();

            for(;nm_iter != nm_iterEnd; ++nm_iter)
            {
               const WaypointPair* wp = (*nm_iter).second;

               if(GetWaypointById(wp->GetWaypointFrom()->GetID()) == NULL)
               {
                  InsertWaypoint(const_cast<WaypointInterface*>(wp->GetWaypointFrom()));
               }

               if(GetWaypointById(wp->GetWaypointTo()->GetID()) == NULL)
               {
                  InsertWaypoint(const_cast<WaypointInterface*>(wp->GetWaypointTo()));
               }
               
               AddEdge(wp->GetWaypointFrom()->GetID(), wp->GetWaypointTo()->GetID());
            }
            
            //wm.SetDeleteOnClear(false);
         }
         return result;
      }


      bool LoadWaypointFile(const std::string& filename)
      {
         dtCore::RefPtr<WaypointReaderWriter> reader = new WaypointReaderWriter(*this);
         bool result = reader->LoadWaypointFile(filename);

         if(!result)
         {
            //this is temporary to support the old waypoint file
            result = LoadLegacyWaypointFile(filename);
         }

         mLastFileLoaded = filename;
         return result;
      }

      bool SaveWaypointFile(const std::string& filename)
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

      AIDebugDrawable* GetDebugDrawable()
      {
         if(!mDrawable.valid())
         {
            mDrawable = new AIDebugDrawable();

            //now we must add all current waypoints
            WaypointRefArray::const_iterator iter = mWaypoints.begin();
            WaypointRefArray::const_iterator iterEnd = mWaypoints.end();

            for(;iter != iterEnd; ++iter)
            {
               mDrawable->InsertWaypoint(**iter);
            }

            NavMesh* nm = mWaypointGraph->GetNavMeshAtSearchLevel(0);
            if(nm != NULL)
            {
               mDrawable->UpdateWaypointGraph(*nm);
            }
         }

         return mDrawable.get();
      }

      void GetWaypoints(WaypointArray& toFill)
      {
         std::for_each(mWaypoints.begin(), mWaypoints.end(), dtUtil::insert_back<WaypointArray, dtCore::RefPtr<WaypointInterface> >(toFill));
      }

      void GetWaypoints(ConstWaypointArray& toFill) const
      {
         std::for_each(mWaypoints.begin(), mWaypoints.end(), dtUtil::insert_back<ConstWaypointArray, dtCore::RefPtr<WaypointInterface> >(toFill));
      }

      void GetWaypointsByType(const dtDAL::ObjectType& type, WaypointArray& toFill)
      {
         WaypointRefArray::const_iterator iter = mWaypoints.begin();
         WaypointRefArray::const_iterator iterEnd = mWaypoints.end();

         for(;iter != iterEnd; ++iter)
         {
            if((*iter)->GetWaypointType() == type)
            {
               toFill.push_back((*iter).get());
            }
         }
      }


      WaypointInterface* GetClosestWaypoint(const osg::Vec3& pos, float maxRadius)
      {
         if(mKDTreeDirty)
         {
            Optimize();
         }

         WaypointInterface* result = NULL;

         find_result found = mKDTree->find_nearest(pos, maxRadius);
         if(found.first != mKDTree->end())
         {              
            result = GetWaypointById(found.first->mID);            
         }

         return result;
      }


      bool GetWaypointsAtRadius(const osg::Vec3& pos, float radius, WaypointArray& arrayToFill)
      {
         if(mKDTreeDirty)
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
            if(wi != NULL)
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

   protected:

      virtual ~DeltaAIInterface()
      {
         ClearMemory();
         delete mKDTree;
      }

      void Optimize()
      {
         mKDTree->optimize();
         mKDTreeDirty = false;
      }

   private:

      dtCore::RefPtr<AIDebugDrawable> mDrawable;

      dtCore::RefPtr<WaypointGraph> mWaypointGraph;

      WaypointGraphAStar mAStar;

      typedef std::vector<dtCore::RefPtr<dtAI::WaypointInterface> > WaypointRefArray;
      WaypointRefArray mWaypoints;

      bool mKDTreeDirty;
      WaypointKDTree* mKDTree;

      std::string mLastFileLoaded;
   };


   ///////////////////////////////////////////////////////////////////////////////
   // CLASS CONSTANTS
   ///////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString AIInterfaceActorProxy::CLASS_NAME("dtAI::AIInterface");
   const dtUtil::RefString AIInterfaceActorProxy::PROPERTY_WAYPOINT_FILE_NAME("dtAI::WaypointFilename");


   //////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   //////////////////////////////////////////////////////////////////////////////
   AIInterfaceActor::AIInterfaceActor()
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   AIInterfaceActor::~AIInterfaceActor()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Node* AIInterfaceActor::GetOSGNode()
   {
      return dtAI::WaypointManager::GetInstance().GetOSGNode();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const osg::Node* AIInterfaceActor::GetOSGNode() const
   {
      return dtAI::WaypointManager::GetInstance().GetOSGNode();
   }


   ///////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   ///////////////////////////////////////////////////////////////////////////////
   AIInterfaceActorProxy::AIInterfaceActorProxy()
   {
      SetClassName(AIInterfaceActorProxy::CLASS_NAME.Get());
   }

   ///////////////////////////////////////////////////////////////////////////////
    AIInterfaceActorProxy::~AIInterfaceActorProxy()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    void AIInterfaceActorProxy::CreateActor()
    {
       AIInterfaceActor* actor = new AIInterfaceActor();
       SetActor(*actor);

       mAIInterface = CreateAIInterface();

       mAIInterface->RegisterWaypointType<Waypoint>(WaypointTypes::DEFAULT_WAYPOINT.get());
       mAIInterface->RegisterWaypointType<NamedWaypoint>(WaypointTypes::NAMED_WAYPOINT.get());
       mAIInterface->RegisterWaypointType<TacticalWaypoint>(WaypointTypes::TACTICAL_WAYPOINT.get());
       mAIInterface->RegisterWaypointType<WaypointCollection>(WaypointTypes::WAYPOINT_COLLECTION.get());
    }

    ///////////////////////////////////////////////////////////////////////
    AIPluginInterface* AIInterfaceActorProxy::CreateAIInterface()
    {
       return new DeltaAIInterface();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void AIInterfaceActorProxy::BuildPropertyMap()
    {
       dtDAL::ActorProxy::BuildPropertyMap();

       const dtUtil::RefString GROUPNAME = "AIInterface";

       AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::STATIC_MESH,
                   PROPERTY_WAYPOINT_FILE_NAME,
                   PROPERTY_WAYPOINT_FILE_NAME,
                   dtDAL::MakeFunctor(*this, &AIInterfaceActorProxy::LoadFile),
                   "Loads the waypoint and connectivity graph.", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void AIInterfaceActorProxy::LoadFile(const std::string& fileName)
    {
         mAIInterface->ClearMemory();
         if (!fileName.empty())
         {
            std::string res = dtDAL::Project::GetInstance().GetContext() + '/'+ fileName;

            bool success = mAIInterface->LoadWaypointFile(res);

            if(!success)
            {
               LOG_ERROR("Unable to load Waypoint File '" + fileName + "'")
            }
         }
    }

    ///////////////////////////////////////////////////////////////////////////////
    bool AIInterfaceActorProxy::IsPlaceable() const
    {
       return false;
    }

    AIPluginInterface* AIInterfaceActorProxy::GetAIInterface()
    {
      return mAIInterface.get();
    }

    const AIPluginInterface* AIInterfaceActorProxy::GetAIInterface() const
    {
       return mAIInterface.get();
    }
}
