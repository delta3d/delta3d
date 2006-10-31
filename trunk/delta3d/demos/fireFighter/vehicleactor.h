/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * William E. Johnson II
 */
#ifndef DELTA_FIRE_FIGHTER_VEHICLE_ACTOR
#define DELTA_FIRE_FIGHTER_VEHICLE_ACTOR

#include <dtGame/gameactor.h>
#include <dtUtil/enumeration.h>
#include <fireFighter/export.h>

namespace dtCore
{
   class Object;
}

namespace dtDAL
{
   class ActorProxyIcon;
}

class FIRE_FIGHTER_EXPORT VehicleActor : public dtGame::GameActor
{
   public:

      class FIRE_FIGHTER_EXPORT CoordSys : public dtUtil::Enumeration
      {
         DECLARE_ENUM(CoordSys);

         public:

            static CoordSys SYS_ABS;
            static CoordSys SYS_REL;
            
         private:

            CoordSys(const std::string &name) : dtUtil::Enumeration(name)
            {
               AddInstance(this);
            }
      };

      /// Constructor
      VehicleActor(dtGame::GameActorProxy &proxy);

      virtual void TimeUpdate(double deltaTime) { }
      void StartEngines();
      void ShutDownEngines();
      void SetCoordSys(CoordSys &sys);
      CoordSys& GetCoordSys();

   protected:
      
      bool mEngineRunning;
      CoordSys *mCoordSys;


      /// Destructor
      virtual ~VehicleActor();

   private:
};

class FIRE_FIGHTER_EXPORT VehicleActorProxy : public dtGame::GameActorProxy
{
   public:

      /// Constructor
      VehicleActorProxy();

      /// Builds the properties of this actor
      virtual void BuildPropertyMap();

      /// Builds the invokables of this actor
      virtual void BuildInvokables();

      /// Instantiates the actor itself
      virtual void CreateActor() { mActor = new VehicleActor(*this); }

      /**
       * Gets the billboard used to represent static meshes if this proxy's
       * render mode is RenderMode::DRAW_BILLBOARD_ICON.
       * @return
       */
      dtDAL::ActorProxyIcon* GetBillBoardIcon();

      virtual const dtDAL::ActorProxy::RenderMode& GetRenderMode()
      {
         return dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
      }

   protected:

      /// Destructor
      virtual ~VehicleActorProxy();

   private:

      dtCore::RefPtr<dtDAL::ActorProxyIcon> mBillboardIcon;
};

#endif
