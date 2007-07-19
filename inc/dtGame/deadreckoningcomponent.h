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
#include <dtGame/export.h>
#include <dtGame/gmcomponent.h>
#include <dtCore/nodecollector.h>
#include <dtGame/deadreckoninghelper.h>

namespace dtUtil
{
   class Log;
}

namespace dtCore
{
   class BatchIsector;
}

namespace dtGame
{
   class Message;
   class TickMessage;
   class GameActorProxy;
   
   
   class DT_GAME_EXPORT DeadReckoningComponent : public dtGame::GMComponent
   {
      public:
         ///The default component name, used when looking it up on the GM. 
         static const std::string DEFAULT_NAME;
      
         DeadReckoningComponent(const std::string& name = DEFAULT_NAME);
         
         /**
          * handles a processed a message
          * @see dtGame::GMComponent#ProcessMessage
          * @param The message
          */
         virtual void ProcessMessage(const dtGame::Message& message);
         
         /**
          * Gets the helper registered for an actor
          * @param proxy The proxy to get the helper for
          * @return A pointer to the helper, or NULL if the proxy is not registered
          */
         const DeadReckoningHelper* GetHelperForProxy(dtGame::GameActorProxy &proxy) const;

         /**
          * Registers an actor with this component.  To simplify coding in the actor, specifically when it comes
          * to setting properties on the helper, the actor should create it's own helper and pass it in when registering.
          * @param toRegister the actor to register.
          * @param helper the preconfigured helper object to use.
          * @throws dtUtil::Exception if this actor is already registered with the component.
          */
         void RegisterActor(dtGame::GameActorProxy& toRegister, DeadReckoningHelper& helper);

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
         dtCore::Transformable* GetTerrainActor() { return mTerrainActor.get(); }

         ///@return the terrain actor using the given name.  If it has not yet been queried, the query will run when this is called.
         const dtCore::Transformable* GetTerrainActor() const { return mTerrainActor.get(); };
         
         ///changes the actor to use for the terrain.
         void SetTerrainActor(dtCore::Transformable* newTerrain);
         
         ///@return the actor to use as an eye point for ground clamping.  This determines which LOD to clamp to. 
         dtCore::Transformable* GetEyePointActor() { return mEyePointActor.get(); };

         ///@return the actor to use as an eye point for ground clamping.  This determines which LOD to clamp to. 
         const dtCore::Transformable* GetEyePointActor() const { return mEyePointActor.get(); };
         
         ///changes the actor to use for the terrain.
         void SetEyePointActor(dtCore::Transformable* newEyePointActor);
         
         const osg::Vec3& GetLastEyePoint() const { return mCurrentEyePointABSPos; }
         
         /**
          * Sets the maximum distance from the player that three intersection point clamping will be used.  
          * After this, one intersection will be used.
          */
         void SetHighResGroundClampingRange(float range) 
         { 
            mHighResClampRange = range; 
            mHighResClampRange2 = range * range; 
         }

         /**
          * @return the maximum distance from the player that three intersection point clamping will be used.  
          */
         float GetHighResGroundClampingRange() const { return mHighResClampRange; }

         /**
          * Typically actors are only ground clamped when their positions are updated
          * but with this time, the actors are reclamped every so often in case LOD's have changed
          * on the terrain.
          * 
          * @param newTime the new time in seconds.  It defaults to 3.  Setting it to 0 disables force clamping.
          */
         void SetForceClampInterval(float newTime) { mForceClampInterval = newTime; }            
         
         /// @return the interval at which entities will be re-clamped.
         float GetForceClampInterval() const { return mForceClampInterval; }
         
      protected:
         virtual ~DeadReckoningComponent();

         /// apply the articulation support
         /// @param helper the instance containing the articulation data.
         /// @param gameActor the instance to be articulated.
         /// @param tickMessage the time data to be used when interpolating.
         void DoArticulation(dtGame::DeadReckoningHelper& helper, const dtGame::GameActor& gameActor, const dtGame::TickMessage& tickMessage) const;

         /// modifies the scene graph node by smoothing the articulation data.
         void DoArticulationSmooth(osgSim::DOFTransform& dofxform, const osg::Vec3& currLocation, const osg::Vec3& nextLocation, float currentTimeStep) const;

         /// modifies the scene graph node by predicting the articulation data.
         void DoArticulationPrediction(osgSim::DOFTransform& dofxform, const osg::Vec3& currLocation, const osg::Vec3& currentRate, float currentTimeStep) const;

         /// Calculates the bounding box for the given proxy, stores it in the helper, and populates the Vec3.
         void CalculateAndSetBoundingBox(osg::Vec3& modelDimensions,
               dtGame::GameActorProxy& gameActorProxy, DeadReckoningHelper& helper);
         
         /**
          * Clamps an actor to the ground.  This doesn't actually move an actor, it just outputs the position and rotation.
          * @param timeSinceUpdate the amount of time since the last actor update.
          * @param xform the current absolute transform of the actor.
          * @param gameActorProxy the actual actor.  This is passed case collision geometry is needed.
          * @param helper the deadreckoning helper for the actor
          */
         void ClampToGround(float timeSinceUpdate, dtCore::Transform& xform, 
            dtGame::GameActorProxy& gameActorProxy, DeadReckoningHelper& helper);
         
         ///Version of clamping that uses three intersection points to calculate the height and the rotation.
         void ClampToGroundThreePoint(float timeSinceUpdate, dtCore::Transform& xform,
            dtGame::GameActorProxy& gameActorProxy, DeadReckoningHelper& helper);
         
         void RunClampBatch();

         /// @return true if the entity should be force clamped based on saved values.
         bool ShouldForceClamp(DeadReckoningHelper& helper, float deltaRealTime, bool bTransformChanged);
         
         dtCore::BatchIsector& GetGroundClampIsector();

         std::map<dtCore::UniqueId, dtCore::RefPtr<DeadReckoningHelper> > mRegisteredActors;
         std::vector<std::pair<dtCore::Transform, dtGame::GameActorProxy*> > mGroundClampBatch;
         osg::Vec3 mCurrentEyePointABSPos;
         dtUtil::Log* mLogger;
         dtCore::RefPtr<dtCore::Transformable> mEyePointActor;
         dtCore::RefPtr<dtCore::Transformable> mTerrainActor;

         dtCore::RefPtr<dtCore::BatchIsector> mTripleIsector, mIsector;
         
         float mHighResClampRange, mHighResClampRange2;
         float mForceClampInterval;
         
         void TickRemote(const dtGame::TickMessage& tickMessage);
                       
   };
   
}

#endif 
