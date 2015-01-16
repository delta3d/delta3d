/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * David Guthrie
 */
#ifndef DELTA_DEAD_RECKONING_COMPONENT
#define DELTA_DEAD_RECKONING_COMPONENT

#include <string>
#include <map>

#include <dtCore/refptr.h>
#include <dtUtil/nodecollector.h>

#include <dtGame/export.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/deadreckoninghelper.h>


namespace dtUtil
{
   class Log;
}

namespace dtGame
{
   class Message;
   class TickMessage;
   class GameActorProxy;
   class GroundClamper;

   class DT_GAME_EXPORT DeadReckoningComponent : public dtGame::GMComponent
   {
   public:
      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;

      static const std::string DEFAULT_NAME;

      DeadReckoningComponent(dtCore::SystemComponentType& type = *TYPE);

      /**
       * handles a processed a message
       * @see dtGame::GMComponent#ProcessMessage
       * @param The message
       */
      virtual void ProcessMessage(const dtGame::Message& message);

      /**
       * Registers an actor with this component.  To simplify coding in the actor, specifically when it comes
       * to setting properties on the helper, the actor should create it's own helper and pass it in when registering.
       * @param toRegister the actor to register.
       * @param helper the preconfigured helper object to use.
       * @throws dtUtil::Exception if this actor is already registered with the component.
       */
      void RegisterActor(dtGame::GameActorProxy& toRegister, DeadReckoningActorComponent& helper);

      /**
       * Registers an actor with this component.  To simplify coding in the actor, specifically when it comes
       * to setting properties on the helper, the actor should create it's own helper and pass it in when registering.
       * @param toRegister the actor to register.
       * @param helper the preconfigured helper object to use.
       */
      void UnregisterActor(dtGame::GameActorProxy& toRegister);

      /**
       * @return true if the given actor is registered with this component.
       */
      bool IsRegisteredActor(dtGame::GameActorProxy& gameActorProxy);

      ///@return the terrain actor using the given name.  If it has not yet been queried, the query will run when this is called.
      dtCore::Transformable* GetTerrainActor();

      ///@return the terrain actor using the given name.  If it has not yet been queried, the query will run when this is called.
      const dtCore::Transformable* GetTerrainActor() const;

      ///changes the actor to use for the terrain.
      void SetTerrainActor(dtCore::Transformable* newTerrain);

      ///@return the actor to use as an eye point for ground clamping.  This determines which LOD to clamp to.
      dtCore::Transformable* GetEyePointActor();

      ///@return the actor to use as an eye point for ground clamping.  This determines which LOD to clamp to.
      const dtCore::Transformable* GetEyePointActor() const;

      ///changes the actor to use for the terrain.
      void SetEyePointActor(dtCore::Transformable* newEyePointActor);

      /**
       * Set the time over which this component should smooth articulations between two DR DOF targets.
       */
      void SetArticulationSmoothTime( float smoothTime );

      float GetArticulationSmoothTime() const { return mArticSmoothTime; }

      /// Set the ground clamper responsible for clamping animated objects.
      void SetGroundClamper( dtGame::BaseGroundClamper& clamper );

      /// @return the ground clamping utility class
      const BaseGroundClamper& GetGroundClamper() const;

      /// @return the ground clamping utility class
      BaseGroundClamper& GetGroundClamper();

   protected:
      virtual ~DeadReckoningComponent();

      /**
       * Apply the articulation support
       * @param helper the instance containing the articulation data.
       * @param xformable the instance to be articulated.
       * @param tickMessage the time data to be used when interpolating.
       */
      void DoArticulation(dtGame::DeadReckoningActorComponent& helper,
            const dtCore::Transformable& xformable,
            const dtGame::TickMessage& tickMessage) const;

      /**
       * Move the articulation DOF between the current position and next
       * position over a certain time step.
       * @param dofxform DOF transform for the current articulation.
       * @param currLocation Current positional or rotational value on the DOF.
       * @param nextLocation The target positional or rotational value of the DOF.
       * @param simTimeDelta Simulation time step for the current frame.
       * @param isPositionChange Flag to differentiate the data from rotational to
       *        positional DOF value modifications. Most articulations are rotational.
       */
      void DoArticulationSmooth(osgSim::DOFTransform& dofxform,
            const osg::Vec3& currLocation, const osg::Vec3& nextLocation,
            float simTimeDelta, bool isPositionChange = false) const;

      /**
       * Move the articulation DOF from the current position to the next
       * based on the rate of movement and simulation time step.
       * @param dofxform DOF transform for the current articulation.
       * @param currLocation Current positional or rotational value on the DOF.
       * @param currentRate Rate of change in the DOF value per second.
       * @param simTimeDelta Simulation time step for the current frame.
       * @param isPositionChange Flag to differentiate the data from rotational to
       *        positional DOF value modifications. Most articulations are rotational.
       */
      void DoArticulationPrediction(osgSim::DOFTransform& dofxform,
            const osg::Vec3& currLocation, const osg::Vec3& currentRate,
            float simTimeDelta, bool isPositional = false) const;

      std::map<dtCore::UniqueId, dtCore::RefPtr<DeadReckoningActorComponent> > mRegisteredActors;
      dtCore::RefPtr<dtGame::BaseGroundClamper> mGroundClamper;

      dtUtil::Log* mLogger;

      float mArticSmoothTime;

      void TickRemote(const dtGame::TickMessage& tickMessage);

   };

}

#endif 
