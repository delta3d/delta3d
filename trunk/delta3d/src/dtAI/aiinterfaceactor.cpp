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
#include <dtAI/waypointmanager.h>
#include <dtAI/waypointcollection.h>
#include <dtAI/astarwaypointutils.h>
#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointtypes.h>
#include <dtAI/waypointgraphastar.h>
#include <dtUtil/templateutility.h>
#include <dtUtil/kdtree.h>


namespace dtAI
{
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
               if(mDrawable.valid())
               {
                  RemoveAllEdges(wpPtr->GetID());
                  mDrawable->RemoveWaypoint(wpPtr->GetID());                  
               }

               mKDTree->erase(*iter);
               mWaypointGraph->RemoveWaypoint(wpPtr->GetID());
               
               mKDTreeDirty = true;
               //dtUtil::array_remove rm(mWaypoints);               
               //dtCore::RefPtr<WaypointInterface*> wpRef = wpPtr;
               //result = rm(wpRef);
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

      bool FindPath(WaypointID pFrom, WaypointID pTo, WaypointArray& result)
      {
         const WaypointInterface* wayFrom = mWaypointGraph->FindWaypoint(pFrom);
         const WaypointInterface* wayTo = mWaypointGraph->FindWaypoint(pTo);

         if(wayFrom != NULL && wayTo != NULL)
         {
            mAStar.Reset(wayFrom, wayTo);
            if(mAStar.FindPath() == PATH_FOUND)
            {
               //we should be able to do a simple assign here however we have a
               //const array of waypoints so we have to const cast each one
               //this code should go away with the refactor
               WaypointGraphAStar::container_type::iterator iter = mAStar.GetPath().begin();
               WaypointGraphAStar::container_type::iterator iterEnd = mAStar.GetPath().end();

               for(;iter != iterEnd; ++iter)
               {
                  result.push_back(const_cast<dtAI::WaypointInterface*>(*iter));
               }

               return true;
            }
         }

         return false;
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

      bool LoadWaypointFile(const std::string& filename)
      {
         //temporarily uses the waypoint manager
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

      bool SaveWaypointFile(const std::string& filename)
      {
         //return mWaypointManager.WriteFile(filename);
         return true;
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

         std::string res = dtDAL::Project::GetInstance().GetContext() + '/'+ fileName;

         bool success = mAIInterface->LoadWaypointFile(res);

         if(!success)
         {
            LOG_ERROR("Unable to load Waypoint File '" + fileName + "'")
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
