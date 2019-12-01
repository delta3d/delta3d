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
 * Bradley Anderegg
 * Allen Danklefsen
 */
#ifndef DELTA_PHYSICS_COMPONENT
#define DELTA_PHYSICS_COMPONENT

#include <dtPhysics/physicsexport.h>
#include <dtGame/gmcomponent.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/physicsactcomp.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/debugdrawable.h>

#include <dtUtil/getsetmacros.h>

namespace dtGame
{
   class TickMessage;
}

namespace dtPhysics
{
   ///////////////////////////
   // forward Declarations
   /////////////////////////////////////////////////////////////////////////////
   // Class:
   // Notes:
   /////////////////////////////////////////////////////////////////////////////
   class DT_PHYSICS_EXPORT PhysicsComponent: public dtGame::GMComponent
   {
   public:
      typedef dtGame::GMComponent BaseClass;
      typedef std::vector<PhysicsActCompPtr> PhysicsActCompVector;

      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
      // component name
      static const std::string DEFAULT_NAME;

      PhysicsComponent(dtCore::SystemComponentType& type = *TYPE);

      /**
       * @throw dtUtil::Exception if it could not load the engine
       */
      PhysicsComponent(dtPhysics::PhysicsWorld& world, bool debugPhysics,
            dtCore::SystemComponentType& type = *TYPE);

      /// @return the physics world.
      PhysicsWorld& GetPhysicsWorld();

      /// @return the physics world as const.
      const PhysicsWorld& GetPhysicsWorld() const;

      /*virtual*/ void ProcessMessage(const dtGame::Message& message);

      const PhysicsActComp* GetActorComp(const std::string& name) const;
      PhysicsActComp* GetActorComp(const std::string& name);
      void RegisterActorComp(PhysicsActComp& toRegister);
      void UnregisterActorComp(PhysicsActComp& toRemove);
      bool IsActorCompRegistered(const PhysicsActComp& pActComp);

      ///This doesn't work yet, but eventually it should shutdown and reload the physics world on map change.
      void SetClearOnMapChange(bool value) {mClearOnMapchange = value;}
      bool GetClearOnMapChange() const {return mClearOnMapchange;}

      /**
       * This allows you to change engines on the fly.
       * @throw dtUtil::Exception if it could not load the engine
       */
      void ChangePhysicsEngine(const std::string& physicsToLoad);

      /// remove all helpers and materials from the component
      void ClearAll();

      /**
       * Called immediately after a component is added to the GM. Override this
       * to do init type behavior that needs access to the GameManager.
       */
      virtual void OnAddedToGM();
      virtual void OnRemovedToGM();

      /// @see PhysicsWorld::SetGroupCollision
      void SetGroupCollision(CollisionGroup one, CollisionGroup two, bool enabled);

      /**
       * Set this to true if the physics should run in the background
       * while the app is rendering, false if it should step all in Preframe.
       */
      DT_DECLARE_ACCESSOR(bool, StepInBackground);

      /// Set this to false to disable stepping the physics engine altogether.
      DT_DECLARE_ACCESSOR(bool, SteppingEnabled);

      /**
       * Enables the next type of debug draw for the physics.  If the GM has an environment actor, this will do a
       * tri state of (rendered world only, physics world only, both).  If no environment actor exists in the GM,
       * it will simply toggle the physics world on and off.
       */
      void SetNextDebugDrawMode();

      /**
       * This is called each tick.  It starts the physics update.  If StepInBackGround is set to true, then
       * this will start the background threading update of the physics.  If it is false, then if it will block
       * until the physics step completes.
       *
       * If SteppingEnabled is false, this will simply cleanup any cached data and exit.
       */
      void BeginUpdate(const dtGame::TickMessage& tm);

      /**
       * Do a simple single step and block until it is done.  Useful for debugging.
       * It works the same as the update step on the physics world, but it calls all the callbacks.
       */
      void UpdateStep(float dt);

      /**
       * If Step in background is true, this will block if the physics running the background.  Otherwise
       * it will just return.
       */
      void WaitUntilUpdateCompletes(float dt);

   protected:
      virtual ~PhysicsComponent();

   private:
      PhysicsActCompVector  mRegisteredActorComps;
      std::string          mPhysicsLoaded;
      dtCore::RefPtr<PhysicsWorld> mImpl;
      dtCore::RefPtr<dtPhysics::DebugDrawable> mDebDraw;
      bool                 mClearOnMapchange;
      bool                 mOverrodeStepInBackground;
   };

} //end namespace

#endif //DELTA_PHYSICS_COMPONENT
