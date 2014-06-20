/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 05/11/2007
 */

#ifndef __DELTA_ANIMATIONCOMPONENT_H__
#define __DELTA_ANIMATIONCOMPONENT_H__

#include <string>
#include <map>

#include <dtAnim/export.h>

#include <dtCore/refptr.h>

#include <dtGame/datacentricgmcomponent.h>

#include <dtAnim/animationhelper.h>
#include <dtUtil/threadpool.h>


namespace dtGame
{
   class Message;
   class TickMessage;
   class GameActorProxy;
   class BaseGroundClamper;
}

namespace dtAnim
{
class AnimationHelper;

class DT_ANIM_EXPORT AnimationComponent: public dtGame::DataCentricGMComponent<AnimationHelper>
{
public:

   typedef dtGame::DataCentricGMComponent<AnimationHelper> BaseClass;

   static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;

   ///The default component name, used when looking it up on the GM.
   static const std::string DEFAULT_NAME;

   AnimationComponent(dtCore::SystemComponentType& type = *TYPE);

   /**
    * Overridden to handle clearing the terrain actor is some cases.
    * @see dtGame::GMComponent#ProcessMessage
    * @param The message
    */
   /*virtual*/ void ProcessMessage(const dtGame::Message& message);

   /**
    * Overridden to handle callback functions
    */
   /*virtual*/ bool RegisterActor(dtGame::GameActorProxy& actor, dtAnim::AnimationHelper& helper);

   /**
    * Overridden to clear callback functions
    */
   /*virtual*/ bool UnregisterActor(const dtCore::UniqueId& actorId);

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

   /// Set the ground clamper responsible for clamping animated objects.
   void SetGroundClamper( dtGame::BaseGroundClamper& clamper );

   /// Get the ground clamper responsible for clamping animated objects.
   dtGame::BaseGroundClamper& GetGroundClamper();
   const dtGame::BaseGroundClamper& GetGroundClamper() const;

   /**
    * Called from helpers when an animatable has reached a point when
    * an event should be fired, if one is specified.
    * This method is registered as a callback to Animation Helpers.
    * @param eventName The name of the event to be fired.
    */
   virtual void OnAnimationEvent(const std::string& eventName);

protected:
   virtual ~AnimationComponent();

   virtual void TickLocal(float dt);
   void BuildThreadWorkerTasks();
   // creates batches of isector queries
   void GroundClamp(BaseClass::ActorCompMapping&);
   void ExecuteCommands(BaseClass::ActorCompMapping&);

private:
   AnimationComponent(const AnimationComponent&);               //not implemented
   AnimationComponent& operator=(const AnimationComponent&);    //not implemented

   dtCore::RefPtr<dtGame::BaseGroundClamper> mGroundClamper;

   // A field used exclusively for the event sending code.
   // This tracks the current actor that whose helper's commands
   // are currently being executed. This information is important
   // for any Game Event Messages fired as a result of command
   // executions.
   dtCore::UniqueId mCurrentSendingActorId;
};

} // namespace dtGame

#endif // __DELTA_ANIMATIONCOMPONENT_H__

