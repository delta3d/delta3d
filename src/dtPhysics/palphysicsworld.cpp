/* -*-c++-*-
 * dtPhysics
 * Copyright 2007-2008, Alion Science and Technology
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

#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/physicsobject.h>
#include <dtPhysics/collisioncontact.h>
#include <dtPhysics/bodywrapper.h>
#include <dtPhysics/palutil.h>
#include <dtPhysics/physicsmaterials.h>
#include <dtPhysics/customraycastcallbacks.h>

#include <pal/palFactory.h>
#include <pal/palCollision.h>
#include <pal/palSolver.h>

#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/threadpool.h>
#include <dtUtil/datapathutils.h>

#include <dtCore/system.h>
#include <dtCore/observerptr.h>

#include <OpenThreads/Thread>
#include <OpenThreads/Block>
#include <OpenThreads/Atomic>

#include <algorithm>
#include <set>

namespace dtPhysics
{
   class StepTask: public dtUtil::ThreadPoolTask
   {
   public:
      virtual void operator () ()
      {
         mWorld->UpdateStep(mUpdateTime);
      }

      float mUpdateTime;
      PhysicsWorld* mWorld;
   };

   class PhysicsWorldImpl
   {
   public:
      PhysicsWorldImpl(const std::string& engineToLoad, const std::string& basePath)
      : mInitialized(false)
      , mEngineName(engineToLoad)
      , mBasePath(basePath)
      , mPalPhysicsScene(NULL)
      , mPalCollisionDetection(NULL)
      , mPalCollisionDetectionEx(NULL)
      , mCompleteDebugDraw(NULL)
      , mRenderingDebugDraw(NULL)
      , mConfig(NULL)
      , mStepping(0)
      , mStepTime(1/60.0f)
      , mStepTimeAccum(0.0f)
      , mTotalStepCount(0U)
      // Standard Gravity according to NIST special publication 330, p. 39
      , mGravity(DEFAULT_GRAVITY_X,DEFAULT_GRAVITY_Y,DEFAULT_GRAVITY_Z)
      {
#ifdef DELTA_WIN32
#ifdef _DEBUG
         mPathSuffix = "debug";
#else
         mPathSuffix = "release";
#endif
#endif
      }

      ~PhysicsWorldImpl()
      {
         Shutdown();
      }

      /**
       * Cleans up the pal physics stuff.
       */
      inline void Shutdown()
      {
         palFactory* palFactory = palFactory::GetInstance();

         mSolver = NULL;
         mMaterials = NULL;

         // Pal factory cleanup is supposed to delete the physics.
         mPalPhysicsScene = NULL;

         if (palFactory != NULL)
         {
            palFactory->Cleanup();
         }
      }

      inline bool UpdateStep(float elapsedTime)
      {
         if (elapsedTime > FLT_EPSILON)
         {

            dtCore::Timer_t startTime = dtCore::Timer::Instance()->Tick();
            if (mStepTime > FLT_EPSILON)
            {
               mStepTimeAccum += elapsedTime;
               float steps = std::floor(mStepTimeAccum / mStepTime);

               if (mSolver != NULL)
               {
                  // If it has a solver, then we assume the physics engine handles fixed time step itself
                  // and we just need to call with the full time.  It will do nice things like clear
                  // all the forces even if no step happens, and it will do interpolation to make the positions
                  // of objects appear to match the real elapsed time even though the simulated time
                  // is equal to mStepTime * mTotalStepCount
                  mPalPhysicsScene->Update(elapsedTime);
               }
               else if (steps > 0.0f)
               {
                  // If it has no solver interface (like ODE), we have to feed it the fixed time step.
                  // this means no interpolation, which can cause weird rendering artifacts.
                  // Unfortunately, this will only step in multiples of the fixed time step
                  // so it really doesn't fix anything.  Pal will need to solve this problem.
                  mPalPhysicsScene->Update(mStepTime * steps);
               }
               else
               {
                  // TODO clear all the forces.  Pal needs an api for this, but physics engines may not
                  // solve for it.
               }

               mStepTimeAccum -= mStepTime * steps;
               mTotalStepCount+= int(steps);
            }
            else
            {
               mPalPhysicsScene->Update(elapsedTime);
               mStepTimeAccum = 0.0f;
               mTotalStepCount++;
            }
            LOGN_DEBUG("palphysicsworld.cpp", "Time to update physics ms: " + dtUtil::ToString(dtCore::Timer::Instance()->DeltaMil(startTime, dtCore::Timer::Instance()->Tick())));

            if (mRenderingDebugDraw != NULL)
            {
               palDebugDraw* temp = mCompleteDebugDraw;
               mCompleteDebugDraw = mRenderingDebugDraw;
               //temp will be NULL the first time, so setting to the second debugdraw object will just work.
               if (temp == NULL)
               {
                  temp = &mDebugDraws[1];
               }
               mRenderingDebugDraw = temp;
               mPalPhysicsScene->SetDebugDraw(mRenderingDebugDraw);
            }
         }
         --mStepping;
         return true;
      }

      // Only exists to hold references so they don't get deleted until they are cleaned up from pal.
      std::set<dtCore::RefPtr<Action> > mActions;
      dtCore::RefPtr<StepTask> mBackgroundStepTask;

      bool mInitialized;
      std::string mEngineName;
      std::string mBasePath;

      std::string mPathSuffix;

      palPhysics* mPalPhysicsScene;
      palCollisionDetection* mPalCollisionDetection;
      palCollisionDetectionExtended* mPalCollisionDetectionEx;

      dtPhysics::DebugDraw* mCompleteDebugDraw;
      dtPhysics::DebugDraw* mRenderingDebugDraw;
      dtPhysics::DebugDraw mDebugDraws[2];

      dtCore::RefPtr<PhysicsMaterials> mMaterials;
      dtCore::RefPtr<SolverWrapper> mSolver;
      const dtUtil::ConfigProperties* mConfig;
      //dtCore::RefPtr<osg::OperationThread> mOperationThread;
      OpenThreads::Atomic mStepping;

      Real mStepTime;
      Real mStepTimeAccum;
      unsigned mTotalStepCount;
      VectorType mGravity;
   };

   //////////////////////////////////////////////////////////////////////////
   SolverWrapper::SolverWrapper(palSolver& solver)
   : mSolver(solver)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void SolverWrapper::SetSolverAccuracy(Real accuracy)
   {
      mSolver.SetSolverAccuracy(accuracy);
   }

   //////////////////////////////////////////////////////////////////////////
   float SolverWrapper::GetSolverAccuracy()
   {
      return mSolver.GetSolverAccuracy();
   }

   //////////////////////////////////////////////////////////////////////////
   void SolverWrapper::SetProcessingElements(int n)
   {
      mSolver.SetPE(n);
   }

   //////////////////////////////////////////////////////////////////////////
   void SolverWrapper::SetSubsteps(int n)
   {
      mSolver.SetSubsteps(n);
   }

   //////////////////////////////////////////////////////////////////////////
   void SolverWrapper::SetFixedTimeStep(Real fixedStep) const
   {
      mSolver.SetFixedTimeStep(fixedStep);
   }

   //////////////////////////////////////////////////////////////////////////
   void SolverWrapper::SetHardware(bool status)
   {
      mSolver.SetHardware(status);
   }

   //////////////////////////////////////////////////////////////////////////
   bool SolverWrapper::GetHardware()
   {
      return mSolver.GetHardware();
   }

   //////////////////////////////////////////////////////////////////////////
   palSolver& SolverWrapper::GetPALSolver()
   {
      return mSolver;
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   static dtCore::ObserverPtr<PhysicsWorld> TheWorld;

   const std::string PhysicsWorld::DIRECTORY_NAME = "PalPlugins";
   const std::string PhysicsWorld::PHYSX_ENGINE   = "Novodex";
   const std::string PhysicsWorld::BULLET_ENGINE  = "Bullet";
   const std::string PhysicsWorld::ODE_ENGINE     = "ODE";
   const std::string PhysicsWorld::NEWTON_ENGINE  = "Newton";
   const std::string PhysicsWorld::JIGGLE_ENGINE  = "Jiggle";
   const std::string PhysicsWorld::TRUEAXIS_ENGINE= "TrueAxis";
   const std::string PhysicsWorld::TOKAMAK_ENGINE = "Tokamak";

   const std::string PhysicsWorld::CONFIG_PHYSICS_ENGINE("dtPhysics.PhysicsEngine");
   const std::string PhysicsWorld::CONFIG_PHYSICS_ENGINE_DEFAULT(dtPhysics::PhysicsWorld::BULLET_ENGINE);
   const std::string PhysicsWorld::CONFIG_PAL_PLUGIN_PATH("dtPhysics.PalPluginPath");
   const std::string PhysicsWorld::CONFIG_ENABLE_HARDWARE_PHYSICS("dtPhysics.EnableHardwarePhysics");
   const std::string PhysicsWorld::CONFIG_NUM_PROCESSING_ELEMENTS("dtPhysics.NumProcessingElements");
   const std::string PhysicsWorld::CONFIG_SOLVER_ITERATION_COUNT("dtPhysics.SolverIterationCount");
   const std::string PhysicsWorld::CONFIG_TICKS_PER_SECOND("dtPhysics.TicksPerSecond");
   const std::string PhysicsWorld::CONFIG_DEBUG_DRAW_RANGE("dtPhysics.DebugDrawRange");
   const std::string PhysicsWorld::CONFIG_PRINT_ENGINE_PROPERTY_DOCUMENTATION("dtPhysics.PrintEnginePropertyDocumentation");


   //////////////////////////////////////////////////////////////////////////
   PhysicsWorld& PhysicsWorld::GetInstance()
   {
      if (!TheWorld.valid())
         throw dtUtil::Exception("No PhysicsWorld has been created.", __FILE__, __LINE__);

      return *TheWorld;
   }

   //////////////////////////////////////////////////////////////////////////
   PhysicsWorld::PhysicsWorld(const std::string& engineToLoad, const std::string& basePath)
   : mImpl(new PhysicsWorldImpl(engineToLoad, basePath))
   {
      Ctor();
   }

   //////////////////////////////////////////////////////////////////////////
   PhysicsWorld::PhysicsWorld(const dtUtil::ConfigProperties& config)
   : mImpl(NULL)
   {
      const std::string engineToLoad = config.GetConfigPropertyValue(
               CONFIG_PHYSICS_ENGINE, CONFIG_PHYSICS_ENGINE_DEFAULT);
      const std::string basePath = config.GetConfigPropertyValue(CONFIG_PAL_PLUGIN_PATH);

      mImpl = new PhysicsWorldImpl(engineToLoad, basePath);
      mImpl->mConfig = &config;
      Ctor();
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::PrintEnginePropertyDocumentation()
   {
      PAL_MAP<PAL_STRING, PAL_STRING> propertyDocs;
      mImpl->mPalPhysicsScene->GetPropertyDocumentation(propertyDocs);

      dtUtil::Log& logger = dtUtil::Log::GetInstance("palphysicsworld.cpp");
      logger.LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, "Engine \"%s\" accepted engine properties (prefix with pal. in config.xml):", mImpl->mEngineName.c_str());

      PAL_MAP<PAL_STRING, PAL_STRING>::const_iterator i,iend;
      i = propertyDocs.begin();
      iend = propertyDocs.end();
      for (; i != iend; ++i)
      {
         logger.LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, "\t%s\t%s", i->first.c_str(), i->second.c_str());
      }
   }
   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::Ctor()
   {
      dtCore::ObserverPtr<PhysicsWorld> oldWorld = TheWorld.get();
      TheWorld = this;
      if (oldWorld.valid())
      {
         throw dtUtil::Exception("Unable to create a new PhysicsWorld.  The old one still has a reference.",
                  __FILE__, __LINE__);
      }

      // load pal dll interface
      palFactory* palFactory = palFactory::GetInstance();
      std::string pathString;

      // the method takes a char*, but doesn't change it, so we just const cast it. eew.
      palFactory->LoadPALfromDLL(const_cast<char*>(GetPluginPath().c_str()));

      // load specific physics
      if ( ! palFactory->SelectEngine(mImpl->mEngineName))
      {
         std::string error = "Failure to select physics engine [" + 
            mImpl->mEngineName + "] from path [" + GetPluginPath() + "]. Make sure: working dir is right; config.xml has correct paths; dependent DLLS are available; and paths are setup for all relevant physics engines.";
         throw dtUtil::Exception(error, __FUNCTION__, __LINE__);
      }

      mImpl->mPalPhysicsScene = palFactory->CreatePhysics();

      // test to see if it was created correctly
      if (mImpl->mPalPhysicsScene == NULL)
      {
         std::string error = "Pal Physics could not create the specified library [" + 
            mImpl->mEngineName + "] from path [" + GetPluginPath() + "]. Make sure: working dir is right; config.xml has correct paths; dependent DLLS are available; and paths are setup for all relevant physics engines.";
         throw dtUtil::Exception(error, __FUNCTION__, __LINE__);
      }

      mImpl->mPalCollisionDetection = mImpl->mPalPhysicsScene->asCollisionDetection();
      mImpl->mPalCollisionDetectionEx = dynamic_cast<palCollisionDetectionExtended*>(mImpl->mPalPhysicsScene);

      palSolver* tempSolver = dynamic_cast<palSolver*>(mImpl->mPalPhysicsScene);

      if (mImpl->mPalCollisionDetection == NULL)
      {
         throw dtUtil::Exception("Pal Physics implementation does not support collision detection interface.", __FUNCTION__, __LINE__);
      }

      if (tempSolver != NULL)
      {
         unsigned processingElements = OpenThreads::GetNumberOfProcessors();
         bool useHardware = true;
         bool printPalEngineSettingsHelp = false;
         float iterCount = 10.0f;

         mImpl->mStepTime = 1.0 / dtCore::System::GetInstance().GetFrameRate();

         if (mImpl->mConfig != NULL)
         {
            const std::string numProcessingElements = mImpl->mConfig->GetConfigPropertyValue(
                     CONFIG_NUM_PROCESSING_ELEMENTS);

            const std::string useHardwareStr = mImpl->mConfig->GetConfigPropertyValue(
                     CONFIG_ENABLE_HARDWARE_PHYSICS, "true");

            printPalEngineSettingsHelp = dtUtil::ToType<bool>(mImpl->mConfig->GetConfigPropertyValue(CONFIG_PRINT_ENGINE_PROPERTY_DOCUMENTATION, "false"));

            useHardware = dtUtil::ToType<bool>(useHardwareStr);

            processingElements = numProcessingElements.empty() ?
                      processingElements : dtUtil::ToType<unsigned>(numProcessingElements);

            std::string ticksPerSec = mImpl->mConfig->GetConfigPropertyValue(CONFIG_TICKS_PER_SECOND);
            if (!ticksPerSec.empty())
            {
               float ticksPerSecFloat = dtUtil::ToType<float>(ticksPerSec);
               if (ticksPerSecFloat > FLT_EPSILON)
               {
                  mImpl->mStepTime = 1.0f / ticksPerSecFloat;
                  // The physics can't step less than the system update rate or there are problems, plus is someone
                  // wants to step it more than 10000 times a second, I guess they can change this function..
                  dtUtil::Clamp(mImpl->mStepTime, 0.0001f , float(1.0f / dtCore::System::GetInstance().GetFrameRate()));
               }
               else
               {
                  mImpl->mStepTime = 0.0f;
               }
            }

            std::string iterCountStr = mImpl->mConfig->GetConfigPropertyValue(CONFIG_SOLVER_ITERATION_COUNT);
            if (!iterCountStr.empty())
            {
                iterCount = dtUtil::ToType<float>(iterCountStr);
                dtUtil::ClampMin(iterCount, 1.0f);
            }

            std::string rangeStr = mImpl->mConfig->GetConfigPropertyValue(CONFIG_DEBUG_DRAW_RANGE);
            if (!rangeStr.empty())
            {
               float range = dtUtil::ToType<float>(rangeStr);
               mImpl->mDebugDraws[0].SetRange(range);
               mImpl->mDebugDraws[1].SetRange(range);
            }
         }

         if (printPalEngineSettingsHelp)
         {
            PrintEnginePropertyDocumentation();
         }

         mImpl->mSolver = new SolverWrapper(*tempSolver);
         mImpl->mSolver->SetProcessingElements(processingElements);
         mImpl->mSolver->SetHardware(useHardware);
         if (mImpl->mStepTime > FLT_EPSILON)
         {
            // Set the substeps to be the max number of steps the physics engine could POSSIBLY need to do in 2 seconds.
            // it can still be changed in code, but we don't want the default to give confusing results.
            mImpl->mSolver->SetSubsteps(int(2.0f / mImpl->mStepTime));
         }
         mImpl->mSolver->SetSolverAccuracy(iterCount);
         SetStepTime(mImpl->mStepTime);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::Init()
   {
      palPhysicsDesc desc;

      // init with gravity
      for (unsigned i = 0; i < 3; ++i)
      {
         desc.m_vGravity._vec[i] = mImpl->mGravity[i];
      }

      desc.m_nUpAxis = PAL_Z_AXIS;
      //desc.m_Properties["ODE_NoInitOrShutdown"] = "true";
      desc.m_Properties["Bullet_UseInternalEdgeUtility"] = "true";

      if (mImpl->mConfig != NULL)
      {
         std::vector<std::pair<std::string, std::string> > props;
         mImpl->mConfig->GetConfigPropertiesWithPrefix("pal.", props);

         std::vector<std::pair<std::string, std::string> >::const_iterator i,iend;
         i = props.begin();
         iend = props.end();
         for (; i != iend; ++i)
         {
            desc.m_Properties[i->first] = i->second;
         }
      }


      mImpl->mPalPhysicsScene->Init(desc);

      palMaterials* materials = mImpl->mPalPhysicsScene->GetMaterials(); //palFactory->CreateMaterials();

      if (materials == NULL)
      {
         throw dtUtil::Exception("Pal Physics was unable to create a materials interface.", __FUNCTION__, __LINE__);
      }

      mImpl->mMaterials = new PhysicsMaterials(*materials);

      /// Create the default material.
      if (mImpl->mMaterials->GetMaterial(dtPhysics::PhysicsMaterials::DEFAULT_MATERIAL_NAME) == NULL)
      {
         // use defaults
         MaterialDef def;

         mImpl->mMaterials->NewMaterial(dtPhysics::PhysicsMaterials::DEFAULT_MATERIAL_NAME, def);
      }

      mImpl->mInitialized = true;
   }

   //////////////////////////////////////////////////////////////////////////
   bool PhysicsWorld::IsInitialized()
   {
      return TheWorld.valid() && TheWorld->mImpl->mInitialized;
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::Shutdown()
   {
      TheWorld = nullptr;
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::TraceRay(RayCast& ray, RayCast::RayCastCallback callback)
   {
      RayCast::Report report;
      if (TraceRay(ray, report))
      {
         callback(report);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool PhysicsWorld::TraceRay(RayCast& ray, RayCast::Report& report)
   {
      const VectorType& pos = ray.GetOrigin();
      VectorType dir = ray.GetDirection();
      Float dirLength = dir.normalize();
      palRayHit rayHit;
      mImpl->mPalCollisionDetection->RayCast(pos.x(), pos.y(), pos.z(), dir.x(), dir.y(), dir.z(), dirLength, rayHit);

      PalRayHitToRayCastReport(report, rayHit);
      return rayHit.m_bHit;
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::TraceRay(RayCast& ray, std::vector<RayCast::Report>& hits, bool sortResults)
   {
      FindAllHitsCallback callback(ray.GetDirection().length());
      PhysicsWorld::GetInstance().TraceRay(ray, callback);
      hits = callback.mHits;
      if (sortResults)
      {
         std::sort(hits.begin(), hits.end());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::TraceRay(RayCast& ray, palRayHitCallback& rayHitCallback)
   {
      if (mImpl->mPalCollisionDetectionEx == NULL)
      {
         throw new dtUtil::Exception("Ray casting with a callback unsupported by the physics backend.",
                  __FILE__, __LINE__);
      }

      const VectorType& pos = ray.GetOrigin();
      VectorType dir = ray.GetDirection();
      Float dirLength = dir.normalize();
      mImpl->mPalCollisionDetectionEx->RayCast(pos.x(), pos.y(), pos.z(), dir.x(), dir.y(), dir.z(),
               dirLength, rayHitCallback, ray.GetCollisionGroupFilter());
   }

   //////////////////////////////////////////////////////////////////////////
   PhysicsWorld::~PhysicsWorld()
   {
      //to make sure it's NOT ode so the ode hack won't prevent proper shutdown.
      mImpl->mEngineName.clear();
      delete mImpl;
      mImpl = nullptr;
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::UpdateStep(float elapsedTime)
   {
      mImpl->UpdateStep(elapsedTime);
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::StartBackgroundUpdateStep(float elapsedTime)
   {
      if (elapsedTime < FLT_EPSILON)
      {
         return;
      }

      bool canStep = ((++mImpl->mStepping) == 1);

      if (canStep)
      {
         if (mImpl->mBackgroundStepTask == NULL)
         {
            mImpl->mBackgroundStepTask = new StepTask;
            mImpl->mBackgroundStepTask->mWorld = this;
         }
         else
         {
            //Let's just verify it has stopped.
            if (!mImpl->mBackgroundStepTask->WaitUntilComplete(1))
            {
               --mImpl->mStepping;
               LOG_ERROR("Attempted to step the physics, and the canStep variable allowed it, but the task is blocked!");
               return;
            }
         }
         mImpl->mBackgroundStepTask->mUpdateTime = elapsedTime;
         dtUtil::ThreadPool::AddTask(*mImpl->mBackgroundStepTask, dtUtil::ThreadPool::BACKGROUND);
      }
      else
      {
         // If I can't step it, then decrement, otherwise the physics on the other thread will
         // decrement it when it's done.
         --mImpl->mStepping;
         LOGN_WARNING("palphysicsworld.cpp", "Attempted to step the physics, but it is already running.");
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool PhysicsWorld::IsBackgroundUpdateStepRunning() const
   {
      return mImpl->mStepping > 0;
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::WaitForUpdateStepToComplete() const
   {
      if (mImpl->mBackgroundStepTask.valid())
      {
         if (!mImpl->mBackgroundStepTask->WaitUntilComplete(100000))
         {
            LOGN_ERROR("palphysicsworld.cpp", "Waited for the physics step to complete for 100 seconds, but it never completed.");
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   Real PhysicsWorld::GetStepTime() const
   {
      return mImpl->mStepTime;
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::SetStepTime(Real sa)
   {
      mImpl->mStepTime = sa;
      if (mImpl->mSolver.valid())
      {
      	mImpl->mSolver->SetFixedTimeStep(sa);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   double PhysicsWorld::GetSimulationLagTime() const
   {
      return mImpl->mStepTimeAccum;
   }

   //////////////////////////////////////////////////////////////////////////
   unsigned PhysicsWorld::GetNumStepsSinceStartup() const
   {
      return mImpl->mTotalStepCount;
   }

   //////////////////////////////////////////////////////////////////////////
   SolverWrapper* PhysicsWorld::GetSolver()
   {
      return mImpl->mSolver.get();
   }

   //////////////////////////////////////////////////////////////////////////
   PhysicsMaterials& PhysicsWorld::GetMaterials()
   {
      return *mImpl->mMaterials;
   }

   //////////////////////////////////////////////////////////////////////////
   VectorType PhysicsWorld::GetGravity() const
   {
      return mImpl->mGravity;
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::SetGravity(const VectorType& g)
   {
      mImpl->mGravity = g;
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::SetGroupCollision(CollisionGroup one, CollisionGroup two, bool enabled)
   {
      mImpl->mPalPhysicsScene->SetGroupCollision(one, two, enabled);
   }

   //////////////////////////////////////////////////////////////////////////
   static inline void CheckBody(const BaseBodyWrapper* body, int lineNum)
   {
      if (body == NULL)
      {
         throw dtUtil::Exception("Body may not be NULL when requesting collision notification. "
                  "The PhysicsObject may not be initialized",
                  __FILE__, __LINE__);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::NotifyCollision(PhysicsObject& obj1, PhysicsObject& obj2, bool enabled)
   {
      CheckBody(obj1.GetBodyWrapper(), __LINE__);
      CheckBody(obj2.GetBodyWrapper(), __LINE__);

      palBodyBase& body1 = obj1.GetBodyWrapper()->GetPalBodyBase();
      palBodyBase& body2 = obj2.GetBodyWrapper()->GetPalBodyBase();

      mImpl->mPalCollisionDetection->NotifyCollision(&body1, &body2, enabled);
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::NotifyCollision(PhysicsObject& obj, bool enabled)
   {
      CheckBody(obj.GetBodyWrapper(), __LINE__);
      palBodyBase& body = obj.GetBodyWrapper()->GetPalBodyBase();

      mImpl->mPalCollisionDetection->NotifyCollision(&body, enabled);
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::AddAction(Action& action)
   {
      if (mImpl->mPalPhysicsScene == NULL)
      {
         throw dtUtil::Exception("The Physics system has not been initialized, actions may not be added or removed",
                  __FILE__, __LINE__);
      }
      if (!HasAction(action))
      {
         mImpl->mPalPhysicsScene->AddAction(action.GetPalAction());
         mImpl->mActions.insert(&action);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::RemoveAction(Action& action)
   {
      if (mImpl->mPalPhysicsScene == NULL)
      {
         throw dtUtil::Exception("The Physics system has not been initialized, actions may not be added or removed",
                  __FILE__, __LINE__);
      }

      mImpl->mPalPhysicsScene->RemoveAction(action.GetPalAction());
      mImpl->mActions.erase(&action);
   }

   //////////////////////////////////////////////////////////////////////////
   bool PhysicsWorld::HasAction(Action& action)
   {
      return mImpl->mActions.find(&action) != mImpl->mActions.end();
   }

#if defined(PAL_VERSION_GREATER_OR_EQUAL) && PAL_VERSION_GREATER_OR_EQUAL(0,7,0)
   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::GetContacts(PhysicsObject& obj, std::vector<CollisionContact>& contacts)
   {
      CheckBody(obj.GetBodyWrapper(), __LINE__);
      palBodyBase& body = obj.GetBodyWrapper()->GetPalBodyBase();

      palContact palContact;

      ContactConverter cc(contacts);

      mImpl->mPalCollisionDetection->ForEachContact(
            [&cc, &body](const palContactPoint& curContact)
            {
               if (curContact.m_pBody1 == &body || curContact.m_pBody2 == &body)
               {
                  cc(curContact);
               }
            }
            );
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::GetContacts(PhysicsObject& obj1, PhysicsObject& obj2, std::vector<CollisionContact>& contacts)
   {
      CheckBody(obj1.GetBodyWrapper(), __LINE__);
      CheckBody(obj2.GetBodyWrapper(), __LINE__);

      palBodyBase& body1 = obj1.GetBodyWrapper()->GetPalBodyBase();
      palBodyBase& body2 = obj2.GetBodyWrapper()->GetPalBodyBase();

      palContact palContact;

      ContactConverter cc(contacts);

      mImpl->mPalCollisionDetection->ForEachContact(
            [&cc, &body1, &body2](const palContactPoint& curContact)
            {
               if ((curContact.m_pBody1 == &body1 && curContact.m_pBody2 == &body2) ||
                     (curContact.m_pBody2 == &body1 && curContact.m_pBody1 == &body2))
               {
                  cc(curContact);
               }
            }
            );
   }
#else
   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::GetContacts(PhysicsObject& obj, std::vector<CollisionContact>& contacts)
   {
      CheckBody(obj.GetBodyWrapper(), __LINE__);
      palBodyBase& body = obj.GetBodyWrapper()->GetPalBodyBase();

      palContact palContact;

      mImpl->mPalCollisionDetection->GetContacts(&body, palContact);

      contacts.reserve(palContact.m_ContactPoints.size() + contacts.size());
      std::for_each(palContact.m_ContactPoints.begin(), palContact.m_ContactPoints.end(), ContactConverter(contacts));
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::GetContacts(PhysicsObject& obj1, PhysicsObject& obj2, std::vector<CollisionContact>& contacts)
   {
      CheckBody(obj1.GetBodyWrapper(), __LINE__);
      CheckBody(obj2.GetBodyWrapper(), __LINE__);

      palBodyBase& body1 = obj1.GetBodyWrapper()->GetPalBodyBase();
      palBodyBase& body2 = obj2.GetBodyWrapper()->GetPalBodyBase();

      palContact palContact;

      mImpl->mPalCollisionDetection->GetContacts(&body1, &body2, palContact);

      contacts.reserve(palContact.m_ContactPoints.size() + contacts.size());
      std::for_each(palContact.m_ContactPoints.begin(), palContact.m_ContactPoints.end(), ContactConverter(contacts));
   }
#endif

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::ClearContacts()
   {
      mImpl->mPalCollisionDetection->ClearContacts();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& PhysicsWorld::GetEngineName() const
   {
      return mImpl->mEngineName;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string PhysicsWorld::GetPluginPath() const
   {
      std::string finalPath;
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      if (mImpl->mBasePath.empty() || !fileUtils.DirExists(mImpl->mBasePath))
      {
      
         if (!dtUtil::GetDeltaRootPath().empty())
         {
            finalPath = dtUtil::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR + "ext" + dtUtil::FileUtils::PATH_SEPARATOR + DIRECTORY_NAME;
         }

         if (finalPath.empty() || !fileUtils.DirExists(finalPath)) 
         {
            finalPath = fileUtils.CurrentDirectory() + dtUtil::FileUtils::PATH_SEPARATOR + "ext" + dtUtil::FileUtils::PATH_SEPARATOR
                             + DIRECTORY_NAME;
         }
#ifdef __APPLE__
         if (finalPath.empty() || !fileUtils.DirExists(finalPath))
         {
            finalPath = dtUtil::GetBundlePlugInsPath() + "/" + DIRECTORY_NAME;
         }
#endif
      }
      else
      {
         finalPath = mImpl->mBasePath;
      }

      std::string engineName = GetEngineName();
      std::locale loc;
      for (unsigned i = 0; i < engineName.length(); ++i)
      {
         engineName[i] = std::tolower(engineName[i], loc);
      }
#ifndef PAL_PLUGIN_ARCH_PATH
      finalPath += "/" + engineName;
#else
      finalPath += "/" + engineName + "/" + PAL_PLUGIN_ARCH_PATH;
#endif
      if (!mImpl->mPathSuffix.empty())
      {
         finalPath.append("/");
         finalPath.append(mImpl->mPathSuffix);
      }
      return finalPath + "/";
   }

   //////////////////////////////////////////////////////////////////////////
   palFactory* PhysicsWorld::GetPalFactory()
   {
      return palFactory::GetInstance();
   }

   //////////////////////////////////////////////////////////////////////////
   void PhysicsWorld::SetDebugDrawEnabled(bool enable)
   {
      if (GetDebugDrawEnabled() != enable)
      {
         mImpl->mCompleteDebugDraw = NULL;
         if (enable)
         {
            mImpl->mRenderingDebugDraw = &mImpl->mDebugDraws[0];
            mImpl->mPalPhysicsScene->SetDebugDraw(&mImpl->mDebugDraws[0]);
         }
         else
         {
            mImpl->mRenderingDebugDraw = NULL;
            mImpl->mPalPhysicsScene->SetDebugDraw(NULL);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool PhysicsWorld::GetDebugDrawEnabled() const
   {
      return mImpl->mPalPhysicsScene->GetDebugDraw() != NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   dtPhysics::DebugDraw* PhysicsWorld::GetDebugDraw()
   {
      return mImpl->mCompleteDebugDraw;
   }

}
