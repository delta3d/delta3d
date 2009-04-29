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


namespace dtAI
{
   //////////////////////////////////////////////////////////////////////////////////
   //This is the default AI plugin interface implementation
   //////////////////////////////////////////////////////////////////////////////////////////

   class DeltaAIInterface: public AIPluginInterface
   {
   public:
   /*   typedef Loki::ConcreteFactory< AIPluginInterface::AIFactory,
                                     Loki::OpNewFactoryUnit,
                                     LOKI_TYPELIST_1(Waypoint)> DeltaAIFactory;*/

      DeltaAIInterface()
         //: AIPluginInterface(new DeltaAIFactory())
         : mWaypointManager(WaypointManager::GetInstance())
      {

      }

      WaypointID InsertWaypoint(const osg::Vec3& pos)
      {
         return mWaypointManager.AddWaypoint(pos);
      }

      bool RemoveWaypoint(WaypointID id)
      {
         mWaypointManager.RemoveWaypoint(id);
         return true;
      }

      WaypointInterface* GetClosestWaypoint(const osg::Vec3& pos)
      {
         //un-implemented-- Placeholder for waypoint manager replacement
         return NULL;
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
         }
      }

      bool RemovePathSegment(WaypointID pFrom, WaypointID pTo)
      {
         const Waypoint* wayFrom = mWaypointManager.GetWaypoint(pFrom);
         const Waypoint* wayTo = mWaypointManager.GetWaypoint(pTo);

         if(wayFrom != NULL && wayTo != NULL)
         {
            mWaypointManager.GetNavMesh().RemovePathSegment(wayFrom, wayTo);
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
         mWaypointManager.GetNavMesh().Clear();
         mWaypointManager.Clear();
      }

      bool LoadWaypointFile(const std::string& filename)
      {
         return mWaypointManager.ReadFile(filename);
      }

      bool SaveWaypointFile(const std::string& filename)
      {
         return mWaypointManager.WriteFile(filename);
      }


   protected:

      virtual ~DeltaAIInterface()
      {
         ClearMemory();
      }


   private:

      WaypointAStar mAStar;
      WaypointManager& mWaypointManager;

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
    }

    ///////////////////////////////////////////////////////////////////////
    AIPluginInterface* AIInterfaceActorProxy::CreateAIInterface()
    {
       return new DeltaAIInterface();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void AIInterfaceActorProxy::BuildPropertyMap()
    {
        const dtUtil::RefString& GROUPNAME = "AIInterface";

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


    //////////////////////////////////////////////////////////////////////////////////////////
    //Entry Point
    //////////////////////////////////////////////////////////////////////////////////////////
    extern "C" DT_AI_EXPORT dtAI::AIPluginInterface* CreateAIInterface()
    {
       return new DeltaAIInterface;
    }

    extern "C" DT_AI_EXPORT void DestroyAIInterface(dtAI::AIPluginInterface* aiInterface)
    {
       delete aiInterface;
    }
}
