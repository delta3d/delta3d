/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007-2008, Alion Science and Technology
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
 * @author David Guthrie
 */
#include <prefix/unittestprefix.h>

#include <cppunit/extensions/HelperMacros.h>

#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/physicsmaterials.h>
#include <dtPhysics/palutil.h>
#include <pal/palCollision.h>
#include <pal/palSolver.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/mathdefines.h>
#include <dtCore/system.h>

#include <cctype>
#include <cmath>

static std::vector<std::string> PhysicsEngines;

const std::vector<std::string>& GetPhysicsEngineList()
{
   if (PhysicsEngines.empty())
   {
      PhysicsEngines.push_back(dtPhysics::PhysicsWorld::BULLET_ENGINE);
      PhysicsEngines.push_back(dtPhysics::PhysicsWorld::ODE_ENGINE);   
      //PhysicsEngines.push_back(dtPhysics::PhysicsWorld::PHYSX_ENGINE);
      //PhysicsEngines.push_back(dtPhysics::PhysicsWorld::JIGGLE_ENGINE);
      //PhysicsEngines.push_back(dtPhysics::PhysicsWorld::NEWTON_ENGINE);
   }
   return PhysicsEngines;
}



namespace dtPhysics
{
   class TestCustomRaycastCallback : public palRayHitCallback
   {
   public:
      /////////////////////////////////////////////////////////////////////////////////////////////
      TestCustomRaycastCallback()
      : mGotAHit(false)
      , mHitCount(0)
      , mClosestHitsObject(nullptr)
      , mRayLength(0.0f)
      {
      }

      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ~TestCustomRaycastCallback(){}

      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Float AddHit(palRayHit& hit)
      {
         ++mHitCount;
         dtPhysics::PhysicsObject* physObject = reinterpret_cast<dtPhysics::PhysicsObject*>(hit.m_pBody->GetUserData());


         if (!mGotAHit || mClosestHit.m_fDistance > hit.m_fDistance)
         {
            mClosestHitsObject = physObject;
            mGotAHit = true;
            mClosestHit = hit;
         }

         //return mClosestHit.m_fDistance;
         return mRayLength;
      }

   public:
      bool mGotAHit;
      palRayHit mClosestHit;
      unsigned mHitCount;
      dtPhysics::PhysicsObject* mClosestHitsObject;
      float mRayLength;
   };

   class PhysicsWorldTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(PhysicsWorldTests);
      CPPUNIT_TEST(TestWorldPerEngine);
      CPPUNIT_TEST(TestSolverPerEngine);
      CPPUNIT_TEST(TestActionsPerEngine);
      CPPUNIT_TEST(TestnullptrStaticInstance);
      CPPUNIT_TEST(TestMaterialsPerEngine);
      CPPUNIT_TEST(TestMaterialInteractionsPerEngine);
      CPPUNIT_TEST(TestMaterialAliasesPerEngine);
      CPPUNIT_TEST_SUITE_END();

   public:
      PhysicsWorldTests() {}
      ~PhysicsWorldTests() {}

      void setUp();
      void tearDown();

      void TestWorldPerEngine();
      void TestSolverPerEngine();
      void TestActionsPerEngine();
      void TestnullptrStaticInstance();
      void TestMaterialsPerEngine();
      void TestMaterialInteractionsPerEngine();
      void TestMaterialAliasesPerEngine();

   private:
      //Sub Tests
      void TestPhysicsBasics();
      void TestPhysicsStep();
      void TestRayCast();
      void TestRayCastSorted();

      void TestSolver();
      void TestActions();

      void TestMaterials();
      void TestMaterialInteractions();
      void TestMaterialAliases();

      bool RayCallbackTest(const RayCast::Report& report);
      bool RayCallbackThrow(const RayCast::Report& report);

      void ChangeEngine()
      {
         try
         {
            dtPhysics::PhysicsWorld::Shutdown();
            mPhysWorld = nullptr;
            mPhysWorld = new PhysicsWorld(mCurrentEngine);

            CPPUNIT_ASSERT( ! PhysicsWorld::IsInitialized());
            mPhysWorld->Init();
            CPPUNIT_ASSERT(PhysicsWorld::IsInitialized());
         }
         catch (dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }

      dtCore::RefPtr<PhysicsWorld> mPhysWorld;
      dtCore::RefPtr<dtUtil::Log> mLogger;
      std::string mCurrentEngine;

   };

   CPPUNIT_TEST_SUITE_REGISTRATION(PhysicsWorldTests);

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::setUp()
   {
      mLogger = &dtUtil::Log::GetInstance("PhysicsWorldTests.cpp");

      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();

   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::tearDown()
   {
      dtCore::System::GetInstance().Stop();
      mPhysWorld = nullptr;
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestnullptrStaticInstance()
   {
      CPPUNIT_ASSERT_THROW(PhysicsWorld::GetInstance(), dtUtil::Exception);
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestWorldPerEngine()
   {
      for (unsigned i = 0; i < GetPhysicsEngineList().size(); ++i)
      {
         mCurrentEngine = GetPhysicsEngineList()[i];
         try
         {
            TestPhysicsBasics();
            TestPhysicsStep();
            TestRayCast();
            TestRayCastSorted();
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(mCurrentEngine  +": "+ ex.ToString());
         }
      }
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestSolverPerEngine()
   {
      for (unsigned i = 0; i < GetPhysicsEngineList().size(); ++i)
      {
         mCurrentEngine = GetPhysicsEngineList()[i];
         try
         {
            TestSolver();
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(mCurrentEngine  +": "+ ex.ToString());
         }
      }
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestActionsPerEngine()
   {
      for (unsigned i = 0; i < GetPhysicsEngineList().size(); ++i)
      {
         mCurrentEngine = GetPhysicsEngineList()[i];
         try
         {
            TestActions();
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(mCurrentEngine  +": "+ ex.ToString());
         }
      }
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestPhysicsBasics()
   {
      ChangeEngine();

      PhysicsWorld& world = PhysicsWorld::GetInstance();

      // test the gravity on the component, this kinda uses the physics world as well.
      VectorType grav(rand() % 10, rand() %10, rand() %10);
      world.SetGravity(grav);
      CPPUNIT_ASSERT_MESSAGE("Gravity Doesnt match up!", world.GetGravity() == grav);

      CPPUNIT_ASSERT_EQUAL(mCurrentEngine, world.GetEngineName());
      //TODO test the path.
      //NOTE: this does not seem to work on windows, it returns file not found even though the directory is valid -bga
      //dtUtil::FileInfo info = dtUtil::FileUtils::GetInstance().GetFileInfo(world.GetPluginPath());
      //CPPUNIT_ASSERT(info.fileType == dtUtil::DIRECTORY);

      std::string engineLower = world.GetEngineName();
      for (unsigned i = 0; i < engineLower.length(); ++i)
      {
        engineLower[i] = std::tolower(engineLower[i]);
      }

      std::string basePath = dtUtil::GetDeltaRootPath();
      if (!dtUtil::FileUtils::GetInstance().DirExists(basePath))
      {
         basePath = dtUtil::FileUtils::GetInstance().CurrentDirectory();
      }
      std::string expectedPath = basePath + "/ext/" + PhysicsWorld::DIRECTORY_NAME
               + "/" + engineLower + "/";
#ifdef PAL_PLUGIN_ARCH_PATH
      expectedPath += PAL_PLUGIN_ARCH_PATH + "/";
#endif
#if defined(DELTA_WIN32) && defined(_DEBUG)
      expectedPath += "debug/";
#elif defined(DELTA_WIN32)
      expectedPath += "release/";
#endif

      CPPUNIT_ASSERT_EQUAL(expectedPath, world.GetPluginPath());

      world.SetStepTime(300.0f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(300.0f, world.GetStepTime(), 0.01);
   }

   void PhysicsWorldTests::TestPhysicsStep()
   {
      ChangeEngine();

      PhysicsWorld& world = PhysicsWorld::GetInstance();

      world.SetStepTime(Real(0.010));
      world.Init();

      CPPUNIT_ASSERT_EQUAL(0U, world.GetNumStepsSinceStartup());
      world.UpdateStep(Real(0.042));
      CPPUNIT_ASSERT_EQUAL(4U, world.GetNumStepsSinceStartup());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.002, world.GetSimulationLagTime(), 0.0001);
      world.UpdateStep(Real(0.039));
      CPPUNIT_ASSERT_EQUAL(8U, world.GetNumStepsSinceStartup());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.001, world.GetSimulationLagTime(), 0.0001);
   }

   /////////////////////////////////////////////////////////
   bool PhysicsWorldTests::RayCallbackTest(const RayCast::Report& report)
   {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(7.0f, float(report.mDistance), 0.5f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, float(report.mHitNormal.x()), 0.1f);
      //CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0f, float(report.mHitNormal.y()), 0.1f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, float(report.mHitNormal.z()), 0.1f);

      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, float(report.mHitPos.x()), 0.1f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(7.0f, float(report.mHitPos.y()), 0.1f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.17f, float(report.mHitPos.z()), 0.01f);
      return true;
   }

   /////////////////////////////////////////////////////////
   bool PhysicsWorldTests::RayCallbackThrow(const RayCast::Report& report)
   {
      throw dtUtil::Exception("This should not have been called", __FILE__, __LINE__);
      return true;
   }

   static dtCore::RefPtr<PhysicsObject> CreateTestPhysObject(const std::string& name, PrimitiveType& type, const VectorType& extents, const VectorType& pos, CollisionGroup g)
   {
      dtCore::RefPtr<PhysicsObject> obj = PhysicsObject::CreateNew(name);
      obj->SetPrimitiveType(type);
      obj->SetExtents(extents);
      obj->Create();
      obj->SetCollisionGroup(g);
      dtCore::Transform xform;
      xform.SetTranslation(pos);
      obj->SetTransform(xform);
      return obj;
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestRayCast()
   {
      PhysicsWorld& world = PhysicsWorld::GetInstance();
      RayCast ray;
      ray.SetOrigin(VectorType(0.0, 0.0, 0.0));
      ray.SetDirection(VectorType(0.0, 3.0, 1.0));

      dtCore::RefPtr<PhysicsObject> obj = CreateTestPhysObject("Jo", PrimitiveType::BOX, VectorType(10.0, 10.0, 10.0),
            VectorType(0.0, 12.0, 0.0), 4);

      dtCore::RefPtr<PhysicsObject> obj2 = CreateTestPhysObject("Bo", PrimitiveType::CYLINDER, VectorType(5.0, 5.0, 10.0),
            VectorType(0.0, 20.0, 0.0), 9);

      RayCast::Report report;
      CPPUNIT_ASSERT_MESSAGE("The ray should NOT be long enough to hit.", !world.TraceRay(ray, report));
      CPPUNIT_ASSERT_MESSAGE("The ray should NOT be long enough to hit.", !report.mHasHitObject);
      CPPUNIT_ASSERT_NO_THROW(world.TraceRay(ray, RayCast::RayCastCallback(this, &PhysicsWorldTests::RayCallbackThrow)));

      ray.SetDirection(VectorType(0.0, 40.0, 1.0));
      CPPUNIT_ASSERT_MESSAGE("The ray should be long enough to hit.", world.TraceRay(ray, report));
      CPPUNIT_ASSERT_MESSAGE("The ray should be long enough to hit.", report.mHasHitObject);
      //verify the callback is hit.
      CPPUNIT_ASSERT_THROW(world.TraceRay(ray, RayCast::RayCastCallback(this, &PhysicsWorldTests::RayCallbackThrow)), dtUtil::Exception);
      //re-run the ray trace with callback to test the results
      world.TraceRay(ray, RayCast::RayCastCallback(this, &PhysicsWorldTests::RayCallbackTest));

      CPPUNIT_ASSERT(report.mHitObject.get() == obj.get());
      RayCallbackTest(report);

      TestCustomRaycastCallback testCustomCallback0;
      testCustomCallback0.mRayLength = ray.GetDirection().length();

      TestCustomRaycastCallback testCustomCallback1;
      testCustomCallback1.mRayLength = ray.GetDirection().length();

      TestCustomRaycastCallback testCustomCallback2;
      testCustomCallback2.mRayLength = ray.GetDirection().length();

      ray.SetCollisionGroupFilter(1 << 9);
      world.TraceRay(ray, testCustomCallback0);
      CPPUNIT_ASSERT(testCustomCallback0.mGotAHit);
      CPPUNIT_ASSERT_EQUAL(1U, testCustomCallback0.mHitCount);
      PalRayHitToRayCastReport(report, testCustomCallback0.mClosestHit);
      CPPUNIT_ASSERT(testCustomCallback0.mClosestHitsObject == obj2.get());

      ray.SetCollisionGroupFilter(1 << 4);
      world.TraceRay(ray, testCustomCallback1);
      CPPUNIT_ASSERT(testCustomCallback1.mGotAHit);
      CPPUNIT_ASSERT_EQUAL(1U, testCustomCallback1.mHitCount);
      PalRayHitToRayCastReport(report, testCustomCallback1.mClosestHit);
      CPPUNIT_ASSERT(testCustomCallback1.mClosestHitsObject == obj.get());


      ray.SetCollisionGroupFilter(~0);
      world.TraceRay(ray, testCustomCallback2);
      CPPUNIT_ASSERT(testCustomCallback2.mGotAHit);
      CPPUNIT_ASSERT_EQUAL(2U, testCustomCallback2.mHitCount);
      PalRayHitToRayCastReport(report, testCustomCallback2.mClosestHit);
      RayCallbackTest(report);
      CPPUNIT_ASSERT(testCustomCallback2.mClosestHitsObject == obj.get());
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestRayCastSorted()
   {
      PhysicsWorld& world = PhysicsWorld::GetInstance();
      std::vector<RayCast::Report> hits;

      // Three spheres one inside another, so the results are not sorted propertly (probably).
      dtCore::RefPtr<PhysicsObject> obj = CreateTestPhysObject("Jo", PrimitiveType::SPHERE, VectorType(10.0, 0.0, 0.0),
            VectorType(20.0, 20.0, 20.0), 4);

      dtCore::RefPtr<PhysicsObject> obj2 = CreateTestPhysObject("Bo", PrimitiveType::SPHERE, VectorType(5.0, 0.0, 0.0),
            VectorType(22.0, 22.0, 22.0), 9);

      dtCore::RefPtr<PhysicsObject> obj3 = CreateTestPhysObject("Mo", PrimitiveType::SPHERE, VectorType(2.0, 0.0, 0.0),
            VectorType(23.0, 22.0, 24.0), 10);

      // Ray to go clean through
      RayCast ray;
      ray.SetOrigin(VectorType(0.0, 0.0, 0.0));
      ray.SetDirection(VectorType(46.0, 44.0, 48.0));

      world.TraceRay(ray, hits, false);

      CPPUNIT_ASSERT_EQUAL(size_t(3), hits.size());

#if !defined(_MSC_VER) || _MSC_VER >= 1600
      CPPUNIT_ASSERT(!std::is_sorted(hits.begin(), hits.end()));
      hits.clear();
      world.TraceRay(ray, hits, true);
      CPPUNIT_ASSERT_EQUAL(size_t(3), hits.size());
      CPPUNIT_ASSERT(std::is_sorted(hits.begin(), hits.end()));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(std::sqrt(20.0f * 20.0f * 3.0f) - 10.0f, hits[0].mDistance, 0.5f);
#endif
   }


   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestSolver()
   {
      ChangeEngine();

      dtPhysics::SolverWrapper* solver = PhysicsWorld::GetInstance().GetSolver();
      if (mCurrentEngine == dtPhysics::PhysicsWorld::ODE_ENGINE)
      {
         CPPUNIT_ASSERT(solver == nullptr);
         return;
      }

      CPPUNIT_ASSERT(solver != nullptr);
      solver->SetSubsteps(3);

      // Accuracy should default to 10.0f;
      CPPUNIT_ASSERT_EQUAL(solver->GetSolverAccuracy(), 10.0f);

      solver->SetSolverAccuracy(15.0f);

      CPPUNIT_ASSERT_EQUAL(solver->GetSolverAccuracy(), 15.0f);

      solver->SetSolverAccuracy(Real(-31.4));

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The solver value won't set less that 0.0", 0.0f, solver->GetSolverAccuracy());

      CPPUNIT_ASSERT(!solver->GetHardware());
      //solver->SetHardware(true);
      //CPPUNIT_ASSERT(solver->GetHardware());

      solver->SetProcessingElements(2);

      PhysicsWorld::GetInstance().Init();
      solver->GetPALSolver().StartIterate(Real(0.016));
      //There is no way to know if this will return true of false.
      solver->GetPALSolver().QueryIterationComplete();
      solver->GetPALSolver().WaitForIteration();
      // Strangely, physx seems to hang if you ask if it's done after it's done.
      if (mCurrentEngine != dtPhysics::PhysicsWorld::PHYSX_ENGINE)
         CPPUNIT_ASSERT(solver->GetPALSolver().QueryIterationComplete());
   }

   class TestAction: public dtPhysics::Action
   {
   public:
      TestAction()
      :mCalled(false)
      {}

      virtual ~TestAction() {}

      virtual void operator()(float dt)
      {
         mCalled = true;
      }

      bool WasCalled() const
      {
         return mCalled;
      }

      void ResetCalled()
      {
         mCalled = false;
      }

   private:
      bool mCalled;
   };

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestActions()
   {
      ChangeEngine();
      dtCore::RefPtr<TestAction> action = new TestAction();
      CPPUNIT_ASSERT(!action->WasCalled());
      dtPhysics::PhysicsWorld& world = dtPhysics::PhysicsWorld::GetInstance();

      world.AddAction(*action);

      // The world may not step if there are no object in it.
      dtCore::RefPtr<PhysicsObject> obj = CreateTestPhysObject("Jo", PrimitiveType::BOX, VectorType(10.0, 10.0, 10.0),
            VectorType(0.0, 12.0, 0.0), 4);

      world.UpdateStep(1.0f/60.0f);

      CPPUNIT_ASSERT_MESSAGE("The action should have been added to the world, so it should have been called.",
               action->WasCalled());
      action->ResetCalled();
      CPPUNIT_ASSERT(!action->WasCalled());

      dtCore::RefPtr<TestAction> action2 = new TestAction();
      CPPUNIT_ASSERT(!action2->WasCalled());

      world.AddAction(*action2);

      world.UpdateStep(1.0f/60.0f);
      CPPUNIT_ASSERT_MESSAGE("The action is still in the world, so it should have been called.",
               action->WasCalled());
      CPPUNIT_ASSERT_MESSAGE("The action2 should have been added to the world, so it should have been called.",
               action2->WasCalled());
      action->ResetCalled();
      action2->ResetCalled();
      CPPUNIT_ASSERT(!action->WasCalled());
      CPPUNIT_ASSERT(!action2->WasCalled());

      world.RemoveAction(*action);

      world.UpdateStep(1.0f/60.0f);
      CPPUNIT_ASSERT_MESSAGE("The first action has been removed, so it should not have been called.",
               !action->WasCalled());
      CPPUNIT_ASSERT_MESSAGE("The action2 is still in the world, so it should have been called.",
               action2->WasCalled());

      action2->ResetCalled();

      world.RemoveAction(*action2);
      world.UpdateStep(1.0f/60.0f);

      CPPUNIT_ASSERT_MESSAGE("The second action has been removed, so it should not have been called.",
               !action->WasCalled());
      CPPUNIT_ASSERT_MESSAGE("The second action has been removed, so it should not have been called.",
               !action2->WasCalled());

      dtCore::ObserverPtr<dtPhysics::Action> actionOb = action.get();
      dtCore::ObserverPtr<dtPhysics::Action> action2Ob = action2.get();

      action = nullptr;
      action2 = nullptr;

      CPPUNIT_ASSERT_MESSAGE("The first action should have been deleted.", !actionOb.valid());
      CPPUNIT_ASSERT_MESSAGE("The second action should have been deleted.", !action2Ob.valid());
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestMaterialsPerEngine()
   {
      for (unsigned i = 0; i < GetPhysicsEngineList().size(); ++i)
      {
         mCurrentEngine = GetPhysicsEngineList()[i];
         ChangeEngine();
         TestMaterials();
      }
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestMaterialInteractionsPerEngine()
   {
      for (unsigned i = 0; i < GetPhysicsEngineList().size(); ++i)
      {
         mCurrentEngine = GetPhysicsEngineList()[i];
         ChangeEngine();
         TestMaterialInteractions();
      }
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestMaterialAliasesPerEngine()
   {
      for (unsigned i = 0; i < GetPhysicsEngineList().size(); ++i)
      {
         mCurrentEngine = GetPhysicsEngineList()[i];
         ChangeEngine();
         TestMaterialAliases();
      }
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestMaterials()
   {
      PhysicsMaterials& materials = mPhysWorld->GetMaterials();
      // The default material should be the first registered material.
      CPPUNIT_ASSERT(materials.GetMaterialCount() == 1);

      CPPUNIT_ASSERT(materials.GetMaterial("Dumb") == nullptr);

      Material* defaultMat = materials.GetMaterial(PhysicsMaterials::DEFAULT_MATERIAL_NAME);
      CPPUNIT_ASSERT(defaultMat != nullptr);
      MaterialIndex index = defaultMat->GetId();
      CPPUNIT_ASSERT(defaultMat == materials.GetMaterialByIndex(index));

      const std::string testMaterialName("cheese");
      MaterialDef def;
      def.SetKineticFriction(Real(0.55));
      def.SetStaticFriction(Real(0.34));
      def.SetRestitution(Real(0.91));

      MaterialDef def2;
      def2.SetKineticFriction(Real(0.97));
      def2.SetStaticFriction(Real(0.44));
      def2.SetRestitution(Real(0.01));
      def2.SetDisableStrongFriction(!def.GetDisableStrongFriction());
      def2.SetDirOfAnisotropy(dtPhysics::VectorType(0.3, 0.2, 0.9));
      def2.SetEnableAnisotropicFriction(true);
      def2.SetKineticAnisotropicFriction(dtPhysics::VectorType(0.8, 0.2, 1.1));
      def2.SetStaticAnisotropicFriction(dtPhysics::VectorType(0.2, 0.8, 1.0));

      Material* cheeseMat = materials.NewMaterial(testMaterialName, def);
      CPPUNIT_ASSERT(cheeseMat != nullptr);
      CPPUNIT_ASSERT(materials.GetMaterialCount() == 2);
      CPPUNIT_ASSERT(cheeseMat == materials.CreateOrUpdateMaterial(testMaterialName, def));
      CPPUNIT_ASSERT(materials.GetMaterialCount() == 2);
      CPPUNIT_ASSERT(cheeseMat->GetId() == def.GetMaterialIndex());
      index = cheeseMat->GetId();
      CPPUNIT_ASSERT(cheeseMat == materials.GetMaterialByIndex(index));

      Material* uniqueMaterial = materials.GetMaterial(testMaterialName);
      CPPUNIT_ASSERT(uniqueMaterial != nullptr);
      CPPUNIT_ASSERT(uniqueMaterial == cheeseMat);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetKineticFriction(), uniqueMaterial->m_fKinetic, 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetStaticFriction(), uniqueMaterial->m_fStatic, 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetRestitution(), uniqueMaterial->m_fRestitution, 0.01f);

      def.SetKineticFriction(Real(6.3));
      def.SetStaticFriction(Real(4.2));
      def.SetRestitution(Real(0.43));
      CPPUNIT_ASSERT(materials.SetMaterialDef(testMaterialName, def));
      
      // Ensure that set fails for an unrecognized material
      CPPUNIT_ASSERT( ! materials.SetMaterialDef("fake", def));

      uniqueMaterial = materials.GetMaterial(testMaterialName);
      CPPUNIT_ASSERT(uniqueMaterial != nullptr);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetKineticFriction(), uniqueMaterial->m_fKinetic, 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetStaticFriction(), uniqueMaterial->m_fStatic, 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetRestitution(), uniqueMaterial->m_fRestitution, 0.01f);

      def.SetKineticFriction(0.001f);
      Material* test1 = materials.NewMaterial(testMaterialName +"1", def);
      CPPUNIT_ASSERT_MESSAGE("Recreating the same named material doesn't work.", materials.NewMaterial(testMaterialName +"1", def) == nullptr);
      CPPUNIT_ASSERT(test1 == materials.GetMaterial(testMaterialName +"1"));
      CPPUNIT_ASSERT(test1->GetId() == def.GetMaterialIndex());
      
      def.SetStaticFriction(3.77f);
      Material* test2 = materials.CreateOrUpdateMaterial(testMaterialName +"2", def);
      CPPUNIT_ASSERT(test2 == materials.GetMaterial(testMaterialName +"2"));
      CPPUNIT_ASSERT(test2->GetId() == def.GetMaterialIndex());
      
      Material* test3 = materials.CreateOrUpdateMaterial(testMaterialName +"3", def);
      CPPUNIT_ASSERT(test3 == materials.GetMaterial(testMaterialName +"3"));
      CPPUNIT_ASSERT(test3->GetId() == def.GetMaterialIndex());
      CPPUNIT_ASSERT(test3 == materials.CreateOrUpdateMaterial(testMaterialName +"3", def2));
      
      // 3 more materials were added, so there should be a total of 4.
      CPPUNIT_ASSERT(materials.GetMaterialCount() == 5);

      CPPUNIT_ASSERT_DOUBLES_EQUAL(float(test1->m_fKinetic), float(def.GetKineticFriction()), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(float(test1->m_fStatic), 4.2f, 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(float(test1->m_fRestitution), 0.43f, 0.01f);
      CPPUNIT_ASSERT_EQUAL(2U, test1->GetId());
      CPPUNIT_ASSERT(test1 == materials.GetMaterialByIndex(2U));

      CPPUNIT_ASSERT_DOUBLES_EQUAL(float(test2->m_fKinetic), float(def.GetKineticFriction()), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(float(test2->m_fStatic), float(def.GetStaticFriction()), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(float(test2->m_fRestitution), 0.43f, 0.01f);
      CPPUNIT_ASSERT_EQUAL(3U, test2->GetId());
      CPPUNIT_ASSERT(test2 == materials.GetMaterialByIndex(3U));

      CPPUNIT_ASSERT_DOUBLES_EQUAL(float(test3->m_fKinetic), float(def2.GetKineticFriction()), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(float(test3->m_fStatic), float(def2.GetStaticFriction()), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(float(test3->m_fRestitution), float(def2.GetRestitution()), 0.01f);
      CPPUNIT_ASSERT_EQUAL(test3->m_bDisableStrongFriction, def2.GetDisableStrongFriction());
      CPPUNIT_ASSERT_EQUAL(test3->m_bEnableAnisotropicFriction, def2.GetEnableAnisotropicFriction());
      dtPhysics::VectorType tempVec;
      PalVecToVectorType(tempVec, test3->m_vDirAnisotropy);
      CPPUNIT_ASSERT(dtUtil::Equivalent(tempVec, def2.GetDirOfAnisotropy(), 0.01f));
      PalVecToVectorType(tempVec, test3->m_vKineticAnisotropic);
      CPPUNIT_ASSERT(dtUtil::Equivalent(tempVec, def2.GetKineticAnisotropicFriction(), 0.01f));
      PalVecToVectorType(tempVec, test3->m_vStaticAnisotropic);
      CPPUNIT_ASSERT(dtUtil::Equivalent(tempVec, def2.GetStaticAnisotropicFriction(), 0.01f));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(float(test3->m_fRestitution), float(def2.GetRestitution()), 0.01f);
      CPPUNIT_ASSERT_EQUAL(4U, test3->GetId());
      CPPUNIT_ASSERT(test3 == materials.GetMaterialByIndex(4U));


      // Test accessing the same def by index.
      // --- Retrieve the original again.
      CPPUNIT_ASSERT(materials.GetMaterialDef(testMaterialName, def));
      // --- Retrieve the same information by index with another struct.
      CPPUNIT_ASSERT(materials.GetMaterialDefByIndex(index, def2));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetKineticFriction(), def2.GetKineticFriction(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetStaticFriction(), def2.GetStaticFriction(), 0.01f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetRestitution(), def2.GetRestitution(), 0.01f);
      CPPUNIT_ASSERT(def.GetMaterialIndex() == def2.GetMaterialIndex());
      CPPUNIT_ASSERT(def.GetMaterialIndex() == index);
      CPPUNIT_ASSERT(def2.GetMaterialIndex() == index);

      // Test that accessing an unrecognized material should fail.
      CPPUNIT_ASSERT(nullptr == materials.GetMaterialByIndex(12345));
      CPPUNIT_ASSERT( ! materials.GetMaterialDefByIndex(12345, def));
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestMaterialAliases()
   {
      PhysicsMaterials& materials = mPhysWorld->GetMaterials();

      const std::string cheeseMaterialName("cheese");
      const std::string cheddarMaterialAlias("chedder");
      const std::string brieMaterialAlias("brie");
      const std::string steelMaterialName("steel");
      const std::string ironMaterialAlias("iron");

      MaterialDef def;
      def.SetKineticFriction(Real(0.55));
      def.SetStaticFriction(Real(0.34));
      def.SetRestitution(Real(0.97));
      dtPhysics::Material* mat1 = materials.NewMaterial(cheeseMaterialName, def);

      def.SetKineticFriction(Real(0.8));
      def.SetStaticFriction(Real(0.9));
      def.SetRestitution(Real(0.78));
      dtPhysics::Material* mat2 = materials.NewMaterial(steelMaterialName, def);

      materials.SetMaterialAlias(cheeseMaterialName, cheddarMaterialAlias);
      materials.SetMaterialAlias(cheeseMaterialName, brieMaterialAlias);
      materials.SetMaterialAlias(steelMaterialName, ironMaterialAlias);

      CPPUNIT_ASSERT(materials.GetMaterial(cheddarMaterialAlias) == mat1);
      CPPUNIT_ASSERT(materials.GetMaterial(brieMaterialAlias) == mat1);
      CPPUNIT_ASSERT(materials.GetMaterial(ironMaterialAlias) == mat2);

      def.SetKineticFriction(Real(1.55));
      def.SetStaticFriction(Real(1.34));
      def.SetRestitution(Real(0.97));

      MaterialDef defRead;
      materials.SetMaterialInteraction(cheeseMaterialName, steelMaterialName, def);
      materials.GetMaterialInteraction(brieMaterialAlias, ironMaterialAlias, defRead);
      CPPUNIT_ASSERT_EQUAL(def.GetKineticFriction(), defRead.GetKineticFriction());
      CPPUNIT_ASSERT_EQUAL(def.GetStaticFriction(), defRead.GetStaticFriction());
      CPPUNIT_ASSERT_EQUAL(def.GetRestitution(), defRead.GetRestitution());


      materials.CreateOrUpdateMaterial(cheddarMaterialAlias, def);
      materials.RemoveAlias(ironMaterialAlias);
      CPPUNIT_ASSERT(materials.GetMaterial(cheddarMaterialAlias) != nullptr);
      CPPUNIT_ASSERT(materials.GetMaterial(cheddarMaterialAlias) != mat1);
      CPPUNIT_ASSERT(materials.GetMaterial(brieMaterialAlias) == mat1);
      CPPUNIT_ASSERT(materials.GetMaterial(ironMaterialAlias) == nullptr);

      materials.ClearAliases();
      CPPUNIT_ASSERT_MESSAGE("Cheddar is no longer an alias. it should still exist after aliases are cleared", materials.GetMaterial(cheddarMaterialAlias) != nullptr);
      CPPUNIT_ASSERT(materials.GetMaterial(brieMaterialAlias) == nullptr);
      CPPUNIT_ASSERT(materials.GetMaterial(ironMaterialAlias) == nullptr);
   }

   /////////////////////////////////////////////////////////
   void PhysicsWorldTests::TestMaterialInteractions()
   {
      PhysicsMaterials& materials = mPhysWorld->GetMaterials();

      const std::string cheeseMaterialName("cheese");
      const std::string steelMaterialName("steel");

      MaterialDef def;
      def.SetKineticFriction(Real(0.55));
      def.SetStaticFriction(Real(0.34));
      def.SetRestitution(Real(0.97));
      dtPhysics::Material* mat1 = materials.NewMaterial(cheeseMaterialName, def);

      def.SetKineticFriction(Real(0.8));
      def.SetStaticFriction(Real(0.9));
      def.SetRestitution(Real(0.78));
      dtPhysics::Material* mat2 = materials.NewMaterial(steelMaterialName, def);

      def.SetKineticFriction(Real(0.3));
      def.SetStaticFriction(Real(0.2));
      def.SetRestitution(Real(0.1));

      materials.SetMaterialInteraction(cheeseMaterialName, steelMaterialName, def);
      MaterialDef actualDef;
      //Test looking up the materials in both orders.
      materials.GetMaterialInteraction(steelMaterialName, cheeseMaterialName, actualDef);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetKineticFriction(), actualDef.GetKineticFriction(), 0.01);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetStaticFriction(), actualDef.GetStaticFriction(), 0.01);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetRestitution(), actualDef.GetRestitution(), 0.01);

      materials.GetMaterialInteraction(cheeseMaterialName, steelMaterialName, actualDef);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetKineticFriction(), actualDef.GetKineticFriction(), 0.01);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetStaticFriction(), actualDef.GetStaticFriction(), 0.01);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetRestitution(), actualDef.GetRestitution(), 0.01);


      def.SetKineticFriction(Real(0.7));
      def.SetStaticFriction(Real(0.4));
      def.SetRestitution(Real(0.92));

      materials.SetMaterialInteraction(*mat1, *mat1, def);
      materials.GetMaterialInteraction(cheeseMaterialName, cheeseMaterialName, actualDef);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetKineticFriction(), actualDef.GetKineticFriction(), 0.01);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetStaticFriction(), actualDef.GetStaticFriction(), 0.01);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetRestitution(), actualDef.GetRestitution(), 0.01);

      def.SetKineticFriction(Real(0.2));
      def.SetStaticFriction(Real(0.56));
      def.SetRestitution(Real(0.97));

      materials.SetMaterialInteraction(*mat2, *mat2, def);
      materials.GetMaterialInteraction(*mat2, *mat2, actualDef);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetKineticFriction(), actualDef.GetKineticFriction(), 0.01);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetStaticFriction(), actualDef.GetStaticFriction(), 0.01);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(def.GetRestitution(), actualDef.GetRestitution(), 0.01);

      CPPUNIT_ASSERT_EQUAL(mat1->GetName(), cheeseMaterialName);
      CPPUNIT_ASSERT_EQUAL(mat2->GetName(), steelMaterialName);

   }
}
