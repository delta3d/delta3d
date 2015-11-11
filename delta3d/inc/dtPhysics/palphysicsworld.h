/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008, Alion Science and Technology.
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
 * Allen Danklefsen
 * David Guthrie
 */

#ifndef PALPHYSICSWORLD_H_
#define PALPHYSICSWORLD_H_

#include <dtUtil/configproperties.h>
#include <dtCore/base.h>
#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/action.h>

#include <dtPhysics/raycast.h>

class palRayHitCallback;
class palSolver;
class palFactory;

namespace dtPhysics
{
   class PhysicsObject;
   class CollisionContact;
   class PhysicsMaterials;
   class PhysicsWorldImpl;

   // Instead of using 9.81 everywhere, use a constant.
   const Real DEFAULT_GRAVITY_X = 0.0f;
   const Real DEFAULT_GRAVITY_Y = 0.0f;
   const Real DEFAULT_GRAVITY_Z = -9.80665f; //!< Standard gravity, according to NIST Special Publication 330, p. 39

   class DT_PHYSICS_EXPORT SolverWrapper: public dtCore::Base
   {
   public:
      SolverWrapper(palSolver& solver);

      /**
       * Sets the accuracy of the solver
       * @param fAccuracy Ranges from 0 to infinity, 0 indicates fast and inaccurate, higher indicates more accurate and slower.
       */
      void SetSolverAccuracy(Real accuracy);

      /// @return the solver accuracy.
      float GetSolverAccuracy();

      /**
       * Set the number of concurrent physics processing elements the physics simulation may use.
       * eg: Threads.
       */
      void SetProcessingElements(int n);

//      /// @return the number of processing elements, e.g. threads, cores, etc.
//      int GetProcessingElements() const;

      /**
       * Sets the number of substeps the solver may use.
       */
      void SetSubsteps(int n);

      /**
       * Sets the fixed time step for the physics engine to use.
       * Set it to 0.0 or less to disable the use of a fixed timestep.
       * It's best to set this via the call on the physics world because then the world knows what value you set.
       */
      void SetFixedTimeStep(Real fixedTimeStep) const;

      /**
       * Sets whether to take advantage of special hardware
       * eg: GPU, or CELL acceleration. This may cause restrictions on what and how much can be simulated
       */
      void SetHardware(bool status);

      /**
       * Queries whether the physics is running on special hardware
       */
      bool GetHardware();

      /// @return the pal solver this wraps.
      palSolver& GetPALSolver();
   private:
      palSolver& mSolver;
   };

   /**
    * @brief The PAL physics world.
    *
    */
   class DT_PHYSICS_EXPORT PhysicsWorld: public dtCore::Base
   {
   public:
      static const std::string DIRECTORY_NAME;
      static const std::string PHYSX_ENGINE;
      static const std::string BULLET_ENGINE;
      static const std::string ODE_ENGINE;
      static const std::string NEWTON_ENGINE;
      static const std::string JIGGLE_ENGINE;
      static const std::string TRUEAXIS_ENGINE;
      static const std::string TOKAMAK_ENGINE;

      static const std::string CONFIG_PHYSICS_ENGINE;
      static const std::string CONFIG_PHYSICS_ENGINE_DEFAULT;
      static const std::string CONFIG_PAL_PLUGIN_PATH;
      static const std::string CONFIG_ENABLE_HARDWARE_PHYSICS;
      static const std::string CONFIG_NUM_PROCESSING_ELEMENTS;
      static const std::string CONFIG_SOLVER_ITERATION_COUNT;
      static const std::string CONFIG_TICKS_PER_SECOND;
      static const std::string CONFIG_DEBUG_DRAW_RANGE;
      static const std::string CONFIG_PRINT_ENGINE_PROPERTY_DOCUMENTATION;

   public:
      /**
       * There can only be one physics world at a time.  Sad but true.  You can access the instance of it by calling this
       * method.  One must be created using the constructor.  If none exists, it will throw an exception.
       *
       * @note This class is referenced, but the static instance is NOT held onto with a ref ptr, so someone else
       * must manage its lifecycle.
       * @return The static instance to the physics world.
       */
      static PhysicsWorld& GetInstance();

      /**
       * Creates a new world object.  You must call Init to create the internal engine.
       *
       * @param engineToLoad The name of the engine to load as a plugin.  See the constants.
       * @param basePath The path to plugins.  If you don't set this, it will use DIRECTORY_NAME
       */
      PhysicsWorld(const std::string& engineToLoad, const std::string& basePath = "");

      /**
       * Creates a new world object.  You must call Init to create the internal engine.
       * It will set the plugin path and the engine to use based on config properties
       */
      PhysicsWorld(const dtUtil::ConfigProperties& config);

      /**
       * Creates, or reintializes the physics engine.   This will let you set gravity before
       * you create the world.
       *
       * If you call it again, it will delete the old world and re-create it.  Doing this
       * will invalidate all physics objects.
       */
      void Init();

      /**
       * Flags whether the world has been initialized.
       * Since the world can be accessed as a singleton, one may need
       * to check if it has been initialized before it can be used.
       * If the world is not initialized before use, a program crash may result.
       */
      static bool IsInitialized();

      /**
       * Deletes and cleans up the physics engine
       */
      static void Shutdown();

      /// Do a closest hit ray cast and call the given callback if it hits something.
      void TraceRay(RayCast& ray, dtPhysics::RayCast::RayCastCallback callback);
      /// Do a closest hit ray cast return true if it there is a hit.  The report will be filled in with closest hit.
      bool TraceRay(RayCast& ray, dtPhysics::RayCast::Report& report);

      /// Do a raycast and return all hits.
      void TraceRay(RayCast& ray, std::vector<RayCast::Report>& hits, bool sortResults = true);

      /// Does a complex raycast using a pal callback to allow a closest, all, any, or custom algorithm to be performed.
      void TraceRay(RayCast& ray, palRayHitCallback& rayHitCallback);

      /**
       * Steps the physics engine.
       * @param elapsedTime The step time.
       */
      void UpdateStep(float elapsedTime);

      void StartBackgroundUpdateStep(float elapsedTime);
      bool IsBackgroundUpdateStepRunning() const;
      void WaitForUpdateStepToComplete() const;


      /// so the component can communicate with this.
      Real GetStepTime() const;
      void SetStepTime(Real sa);

      /**
       * Since the physics updates on a fixed time step, the time simulated won't exactly match rest of the app
       * This is the amount of time the physics engine is behind the simulation.  To get the effective
       * SimTimeSinceStartup for the physics, subtract this from dtCore::System::GetInstance().GetSimTimeSinceStartup().
       * @see dtCore::System::GetSimTimeSinceStartup
       */
      double GetSimulationLagTime() const;

      /**
       * @return the number of times the physics has been stepped since startup.
       * This time the number of times this physics world has been stepped since it was created.  Since it can be
       * created and destroyed, and the step time can be changed, this value can only be directly compared to
       * dtCore::System::GetInstance().GetSimTimeSinceStartup if the physics system is created before the first step
       * and the step time has been constant since the first dtCore::System::Step.
       */
      unsigned GetNumStepsSinceStartup() const;

      /**
       * There are settings that may be configured for each engine.  Once the engine has been configured, calling this
       * function will make it print out the internal documentation for the settings available to LOG_ALWAYS.
       * An setting printed can be configured by adding them to the dtUtil::Configuration that is passed into the physics
       * world constructor BUT PREFIXED with "pal." .
       */
      void PrintEnginePropertyDocumentation();

      /**
       * After the physics interface was been assigned an engine, the solver interface
       * can be accessed here.  Not all engines support the solver interface, so this will return
       * NULL in those cases.
       */
      SolverWrapper* GetSolver();

      /**
       * @return the physics materials management interface.
       */
      PhysicsMaterials& GetMaterials();

      /**
       * @return The gravity amount last set.
       */
      VectorType GetGravity() const;

      /**
       * Changes the gravity.  It doesn't actually change unless you call Init()
       * @param g the new gravity vector.
       */
      void SetGravity(const VectorType& g);

      /// Enables or disables collisions between groups.
      void SetGroupCollision(CollisionGroup one, CollisionGroup two, bool enabled);

      /**
       * Enables listening for a collision between two objects.
       * @param obj1 The first object
       * @param obj2 The second object
       * @param enabled Enable/disable listening for a collision
       */
      void NotifyCollision(PhysicsObject& obj1, PhysicsObject& obj2, bool enabled);

      /**
       * Enables listening for a collision involving a single object.
       * @param obj The object which listens for all collisions
       * @param enabled Enable/disable listening for a collision
       */
      void NotifyCollision(PhysicsObject& obj, bool enabled);

      /**
       * Adds a new action to the physics system.  If the action has already been added, it does nothing.
       * @see Action
       */
      void AddAction(Action& action);

      /// Removes an action from the physics system, or does nothing if it has already been registered.
      void RemoveAction(Action& action);

      /// Checks to see if an action has been registered
      bool HasAction(Action& action);

      /**
       * Returns the collision contact points for the last full physics step.
       * Physics objects must be flagged to want collision notifications or you must call NotifyCollisions
       * directly for this to return anything.
       *
       * @note The results of this call during a dtPhysics::Action callback are undefined
       */
      void GetContacts(PhysicsObject& obj, std::vector<CollisionContact>& contacts);

      /**
       * Returns the collision contact points for the last full physics step.
       * Physics objects must be flagged to want collision notifications or you must call NotifyCollisions
       * directly for this to return anything.
       *
       * @note The results of this call during a dtPhysics::Action callback are undefined
       */
      void GetContacts(PhysicsObject& obj1, PhysicsObject& obj2, std::vector<CollisionContact>& contacts);

      /**
       * Clears all cached contacts.
       */
      void ClearContacts();

      /// @return the name of the physics engine plugin being used.
      const std::string& GetEngineName() const;

      /**
       *  @return the path to the plugins directory,
       *  the engine name will be added the end as a subdirec
       */
      const std::string GetPluginPath() const;

      /**
       * Use this function to create objects using the PAL Factory
       * rather than using the singleton declaration of it because of some dll linkage issues.
       */
      palFactory* GetPalFactory();


      /// Enables or disables physics debug drawing.  This only fills the debug draw object. Actually rendering it requires a separate drawable.
      void SetDebugDrawEnabled(bool enable);
      /// @return true if debug drawing is enabled.
      bool GetDebugDrawEnabled() const;

      /// @return a valid debug draw object if debug drawing is enabled, and the system has filled a debug draw object. otherwise NULL
      dtPhysics::DebugDraw* GetDebugDraw();

   private:
      ~PhysicsWorld();

      void Ctor();

      PhysicsWorldImpl* mImpl;
   };
}
#endif /* PALPHYSICSWORLD_H_ */
