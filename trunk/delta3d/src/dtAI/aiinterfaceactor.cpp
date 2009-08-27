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
#include <dtAI/astarwaypointutils.h>
#include <dtAI/aidebugdrawable.h>
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
   typedef dtUtil::KDTree<3, KDHolder, tree_search_func> WaypointTree;
   typedef std::pair<WaypointTree::const_iterator, float> find_result;

   class DeltaAIInterface: public AIPluginInterface
   {

   public:

      DeltaAIInterface()
         : mWaypointManager(WaypointManager::GetInstance())
         , mKDTreeDirty(true)
         , mTree(new WaypointTree(std::ptr_fun(KDHolderIndexFunc)))
      {

      }

      void InsertWaypoint(WaypointInterface* waypoint)
      {
         ///note- this currently only supports backwards compatability with the 
         ///waypoint manager which only supports Waypoints
         Waypoint* waypointDerivativeTemp = dynamic_cast<Waypoint*>(waypoint);
         
         if(waypointDerivativeTemp != NULL)
         {
            mWaypointManager.AddWaypoint(waypointDerivativeTemp);

            //if we have created a drawable then we must add and remove to it
            if(mDrawable.valid())
            {
               mDrawable->InsertWaypoint(*waypoint);
            }

            KDHolder node(waypoint->GetPosition(), waypoint->GetID());
            mTree->insert(node);

            mKDTreeDirty = true;
         }
         else
         {
            LOG_ERROR("The current version of the AIInterfaceActor only supports the legacy Waypoint type 'dtAI::Waypoint'.");
         }
      }

      bool MoveWaypoint(WaypointInterface* wi, const osg::Vec3& newPos)
      {         
         //the kd-tree cannot move :( for now remove and re-insert
         osg::Vec3 pos = wi->GetPosition();        

         find_result found = mTree->find_nearest(pos, 1.0f);
         if(found.first != mTree->end() && (*found.first).mID == wi->GetID())
         {            
            mTree->erase(found.first);
            
            KDHolder node(newPos, wi->GetID());
            mTree->insert(node);

            wi->SetPosition(newPos);

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
         find_result found = mTree->find_nearest(pos, 1.0f);
         if(found.first != mTree->end() && found.first->mID == wi->GetID())
         {            
            mTree->erase(found.first);
            
            if(mDrawable.valid())
            {
               mDrawable->RemoveWaypoint(wi->GetID());
            }

            mWaypointManager.RemoveWaypoint(wi->GetID());

            mKDTreeDirty = true;

            result = true;
         }
      
         return result;
      }

      WaypointInterface* GetWaypointById(WaypointID id)
      {
         return mWaypointManager.GetWaypoint(id);
      }

      WaypointInterface* GetWaypointByName(const std::string& name)
      {
         //un-implemented-- Placeholder for waypoint manager replacement
         return NULL;
      }

      void AddPathSegment(WaypointID pFrom, WaypointID pTo)
      {
         const Waypoint* wayFrom = mWaypointManager.GetWaypoint(pFrom);
         const Waypoint* wayTo = mWaypointManager.GetWaypoint(pTo);

         if(wayFrom != NULL && wayTo != NULL)
         {
            mWaypointManager.GetNavMesh().AddPathSegment(wayFrom, wayTo);
            if(mDrawable.valid())
            {
               mDrawable->UpdateWaypointGraph(mWaypointManager.GetNavMesh());
            }
         }
      }

      bool RemovePathSegment(WaypointID pFrom, WaypointID pTo)
      {
         const Waypoint* wayFrom = mWaypointManager.GetWaypoint(pFrom);
         const Waypoint* wayTo = mWaypointManager.GetWaypoint(pTo);

         if(wayFrom != NULL && wayTo != NULL)
         {
            mWaypointManager.GetNavMesh().RemovePathSegment(wayFrom, wayTo);
            if(mDrawable.valid())
            {
               mDrawable->UpdateWaypointGraph(mWaypointManager.GetNavMesh());
            }
            return true;
         }

         return false;
      }

      void RemoveAllPaths(WaypointID pFrom)
      {
         const Waypoint* wayFrom = mWaypointManager.GetWaypoint(pFrom);

         if(wayFrom != NULL)
         {
            mWaypointManager.GetNavMesh().RemoveAllPaths(wayFrom);
            if(mDrawable.valid())
            {
               mDrawable->UpdateWaypointGraph(mWaypointManager.GetNavMesh());
            }
         }
      }

      void GetAllPathsFromWaypoint(WaypointID pFrom, WaypointArray& result)
      {
         const Waypoint* wayFrom = mWaypointManager.GetWaypoint(pFrom);

         if(wayFrom != NULL)
         {
            NavMesh::NavMeshContainer::iterator iter = mWaypointManager.GetNavMesh().begin(wayFrom);
            NavMesh::NavMeshContainer::iterator iterEnd = mWaypointManager.GetNavMesh().end(wayFrom);

            for(;iter != iterEnd; ++iter)
            {
               //this yucky const cast will go away when we refactor the navmesh and waypoint manager
               result.push_back(const_cast<dtAI::WaypointInterface*>((*iter).second->GetWaypointTo()));
            }
         }
      }

      bool FindPath(WaypointID pFrom, WaypointID pTo, WaypointArray& result)
      {
         const Waypoint* wayFrom = mWaypointManager.GetWaypoint(pFrom);
         const Waypoint* wayTo = mWaypointManager.GetWaypoint(pTo);

         if(wayFrom != NULL && wayTo != NULL)
         {
            mAStar.Reset(wayFrom, wayTo);
            if(mAStar.FindPath() == PATH_FOUND)
            {
               //we should be able to do a simple assign here however we have a
               //const array of waypoints so we have to const cast each one
               //this code should go away with the refactor
               WaypointAStar::container_type::iterator iter = mAStar.GetPath().begin();
               WaypointAStar::container_type::iterator iterEnd = mAStar.GetPath().end();

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
         mWaypointManager.Clear();
         mTree->clear();
         
         if(mDrawable.valid())
         {
            mDrawable->ClearMemory();
         }
      }

      bool LoadWaypointFile(const std::string& filename)
      {
         return mWaypointManager.ReadFile(filename);
      }

      bool SaveWaypointFile(const std::string& filename)
      {
         return mWaypointManager.WriteFile(filename);
      }

      //note: if you ever call this function it will require additional maintenance
      //adding and removing waypoints
      AIDebugDrawable* GetDebugDrawable()
      {
         if(!mDrawable.valid())
         {
            mDrawable = new AIDebugDrawable();

            //now we must add all current waypoints
            dtAI::WaypointManager::WaypointMap::const_iterator iter = mWaypointManager.GetWaypoints().begin();
            dtAI::WaypointManager::WaypointMap::const_iterator iterEnd = mWaypointManager.GetWaypoints().end();

            for(;iter != iterEnd; ++iter)
            {
               mDrawable->InsertWaypoint(*(iter->second));
            }

            mDrawable->UpdateWaypointGraph(mWaypointManager.GetNavMesh());
         }

         return mDrawable.get();
      }

      void GetWaypoints(WaypointArray& toFill)
      {
         dtAI::WaypointManager::WaypointMap::const_iterator iter = mWaypointManager.GetWaypoints().begin();
         dtAI::WaypointManager::WaypointMap::const_iterator iterEnd = mWaypointManager.GetWaypoints().end();

         for(;iter != iterEnd; ++iter)
         {
            toFill.push_back((*iter).second);
         }
      }

      WaypointInterface* GetClosestWaypoint(const osg::Vec3& pos, float maxRadius)
      {
         if(mKDTreeDirty)
         {
            Optimize();
         }

         WaypointInterface* result = NULL;

         find_result found = mTree->find_nearest(pos, maxRadius);
         if(found.first != mTree->end())
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

         mTree->find_within_range(pos, radius, std::back_inserter(v));

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
         delete mTree;
      }

      void Optimize()
      {
         mTree->optimize();
         mKDTreeDirty = false;
      }

   private:

      dtCore::RefPtr<AIDebugDrawable> mDrawable;
      WaypointAStar mAStar;
      WaypointManager& mWaypointManager;

      bool mKDTreeDirty;
      WaypointTree* mTree;      

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

       mAIInterface->RegisterWaypointType<Waypoint>(new dtDAL::ObjectType("Waypoint"));
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
