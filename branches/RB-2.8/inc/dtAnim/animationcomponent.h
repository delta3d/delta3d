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

#include <dtGame/gmcomponent.h>

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

class DT_ANIM_EXPORT AnimationComponent: public dtGame::GMComponent
{
public:

   class AnimCompUpdateTask: public dtUtil::ThreadPoolTask
   {
   public:
      AnimCompUpdateTask();
      virtual void operator()();
      float mUpdateDT;
      std::vector<dtCore::RefPtr<dtAnim::AnimationHelper> > mAnimActorComps;
   };

   struct AnimCompData
   {
      dtCore::RefPtr<dtAnim::AnimationHelper> mAnimActorComp;
   };

   typedef dtGame::GMComponent BaseClass;
   typedef std::map<dtCore::UniqueId, AnimCompData > AnimCompMap;
   typedef AnimCompMap::allocator_type::value_type AnimCompMapping;
   typedef AnimCompMap::iterator AnimCompIter;

public:
   ///The default component name, used when looking it up on the GM.
   static const std::string DEFAULT_NAME;

   AnimationComponent(const std::string& name = DEFAULT_NAME);

   /**
    * handles a processed a message
    * @see dtGame::GMComponent#ProcessMessage
    * @param The message
    */
   /*virtual*/ void ProcessMessage(const dtGame::Message &message);

   /**
    * Gets the helper registered for an actor
    * @param proxy The proxy to get the helper for
    * @return A pointer to the helper, or NULL if the proxy is not registered
    */
   const dtAnim::AnimationHelper* GetHelperForProxy(dtGame::GameActorProxy &proxy) const;

   /**
    * Registers an actor with this component.  To simplify coding in the actor, specifically when it comes
    * to setting properties on the helper, the actor should create it's own helper and pass it in when registering.
    * @param toRegister the actor to register.
    * @param helper the preconfigured helper object to use.
    * @throws dtUtil::Exception if this actor is already registered with the component.
    */
   void RegisterActor(dtGame::GameActorProxy& toRegister, dtAnim::AnimationHelper& helper);

   /**
    * Registers an actor with this component.  To simplify coding in the actor, specifically when it comes
    * to setting properties on the helper, the actor should create it's own helper and pass it in when registering.
    * @param toRegister the actor to register.
    * @param helper the preconfigured helper object to use.
    */
   void UnregisterActor(dtGame::GameActorProxy& toRegister);

   /// alternate unregister that just tasks the actor id
   void UnregisterActor(const dtCore::UniqueId& actorId);

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

   /// Set the ground clamper responsible for clamping animated objects.
   void SetGroundClamper( dtGame::BaseGroundClamper& clamper );

   /// Get the ground clamper responsible for clamping animated objects.
   dtGame::BaseGroundClamper& GetGroundClamper();
   const dtGame::BaseGroundClamper& GetGroundClamper() const;

   /**
    * Called from helpers when an animatable has reached a point when
    * an event should be fire, if one is specified.
    * This method is registered as a callback to Animation Helpers.
    * @param eventName The name of the event to be fired.
    */
   virtual void OnAnimationEvent(const std::string& eventName);

protected:
   virtual ~AnimationComponent();

   virtual void TickLocal(float dt);
   void BuildThreadWorkerTasks();
   // creates batches of isector queries
   void GroundClamp();

private:
   AnimationComponent(const AnimationComponent&);               //not implemented
   AnimationComponent& operator=(const AnimationComponent&);    //not implemented

   AnimCompMap mRegisteredActors;

   dtCore::RefPtr<dtGame::BaseGroundClamper> mGroundClamper;
   std::vector<dtCore::RefPtr<AnimCompUpdateTask> > mThreadTasks;

   // A field used exclusively for the event sending code.
   // This tracks the current actor that whose helper's commands
   // are currently being executed. This information is important
   // for any Game Event Messages fired as a result of command
   // executions.
   dtCore::UniqueId mCurrentSendingActorId;
};

} // namespace dtGame

#endif // __DELTA_ANIMATIONCOMPONENT_H__

