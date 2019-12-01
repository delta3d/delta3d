/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtABC/application.h>
#include <dtCore/system.h>
#include <dtGame/gamemanager.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/palutil.h>
#include <dtPhysics/physicscompiler.h>
#include <dtPhysics/physicscomponent.h>
#include <dtPhysics/physicsactorregistry.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/physicsmaterials.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>



using namespace dtPhysics;

static std::vector<std::string> PhysicsEngines;
extern dtABC::Application& GetGlobalApplication();



namespace dtPhysics
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const dtUtil::RefString DTPHYSICS_REGISTRY("dtPhysics");
   static const dtUtil::RefString MODEL_DEFAULT("StaticMeshes/tests/test_planes.osg");
   static const dtUtil::RefString MODEL_HIGH_RES("StaticMeshes/tests/test_dodecahedrons.osg");
   static const dtUtil::RefString DESCRIPTION_A("PhysicsMat = Mat_A");
   static const dtUtil::RefString DESCRIPTION_B("PhysicsMat = Mat_B");
   static const dtUtil::RefString DESCRIPTION_C("PhysicsMat = Mat_C");
   static const dtUtil::RefString NAME_A("Mat_A");
   static const dtUtil::RefString NAME_B("Mat_B");
   static const dtUtil::RefString NAME_C("Mat_C");
   static const dtUtil::RefString NAME_DEFAULT(PhysicsMaterials::DEFAULT_MATERIAL_NAME);
   static const int INDEX_A = 1;
   static const int INDEX_B = 2;
   static const int INDEX_C = 3;
   static const int INDEX_NULL = 0;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class PhysicsCompilerTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(PhysicsCompilerTests);
      CPPUNIT_TEST(TestNodeDescriptionCollector);
      CPPUNIT_TEST(TestNodeDescriptionFilter);
      CPPUNIT_TEST(TestGetNodeDescriptions);
      CPPUNIT_TEST(TestDefaultMaterial);
      CPPUNIT_TEST(TestMaterialSearchByActor);
      CPPUNIT_TEST(TestMaterialSearchByPhysicsWorld);
      CPPUNIT_TEST(TestCompileSingleMaterial);
      CPPUNIT_TEST(TestCompileSingleMaterialByActors);
      CPPUNIT_TEST(TestCompileAllMaterials);
      CPPUNIT_TEST(TestCompileAllMaterialsByActors);
      CPPUNIT_TEST(TestCompileMaxVertsPerMesh);
      CPPUNIT_TEST(TestCompileMaxEdgeLength);
      CPPUNIT_TEST(TestCompileNoDefaultMaterial);
      CPPUNIT_TEST(TestMaterialAssignment);
      CPPUNIT_TEST(TestCreateGeometry);
      CPPUNIT_TEST(TestCreatePhysicsObjectsForGeometry);
      CPPUNIT_TEST_SUITE_END();

   public:
      PhysicsCompilerTests()
         : mCompileCount(0)
         , mCurrentEngine("Bullet")
      {}

      ~PhysicsCompilerTests()
      {}

      void setUp();
      void tearDown();

      osg::Node* LoadTestModel(const std::string& modelPath = MODEL_DEFAULT.Get());

      void SetupTestActors();
      void SetupTestPhysicsWorld();

      std::string GetNodeDescription(const osg::Node& node) const;
      std::string GetNodeDescription_Explicit(const osg::Node& node) const;

      std::string GetStringNonFiltered(const std::string& str) const;

      void OnGeometryCompiled(PhysicsCompileResult& result);

      void TestNodeDescriptionCollector();
      void TestNodeDescriptionFilter();
      void TestGetNodeDescriptions();
      void TestDefaultMaterial();
      void TestMaterialSearchByActor();
      void TestMaterialSearchByPhysicsWorld();
      void TestCompileSingleMaterial();
      void TestCompileSingleMaterialByActors();
      void TestCompileAllMaterials();
      void TestCompileAllMaterialsByActors();
      void TestCompileMaxVertsPerMesh();
      void TestCompilePerGeode();
      void TestCompileMaxEdgeLength();
      void TestCompileNoDefaultMaterial();
      void TestMaterialAssignment();
      void TestCreateGeometry();
      void TestCreatePhysicsObjectsForGeometry();

   private:
      void TestMaterialAssignment(bool overrideMaterials);

      int mCompileCount;
      std::string mCurrentEngine;
      std::string mNodeDescriptionToFind;
      dtCore::RefPtr<PhysicsWorld> mPhysicsWorld;
      dtCore::RefPtr<PhysicsCompiler> mCompiler;
      dtCore::RefPtr<PhysicsCompileResult> mCompileResult;
      dtCore::RefPtr<dtGame::GameManager> mGM;
      dtCore::RefPtr<osg::Node> mTestNode;
      dtCore::RefPtr<dtUtil::Log> mLogger;
      
      dtPhysics::Material* mMatA; // Quadrants 1 & 3
      dtPhysics::Material* mMatB; // Quadrant 2
      dtPhysics::Material* mMatC; // Quadrant 4
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(PhysicsCompilerTests);

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::setUp()
   {
      try
      {
         mLogger = &dtUtil::Log::GetInstance("PhysicsCompilerTests.cpp");

         dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
         dtCore::System::GetInstance().Start();

         mGM = new dtGame::GameManager(*GetGlobalApplication().GetScene());
         mGM->SetApplication(GetGlobalApplication());
         mGM->LoadActorRegistry(DTPHYSICS_REGISTRY);

         GeometryCompiledCallback geomCompiledCallback(this, &PhysicsCompilerTests::OnGeometryCompiled);

         mCompiler = new PhysicsCompiler;
         mCompiler->SetGeometryCompiledCallback(geomCompiledCallback);

         // Setup the test node.
         mTestNode = LoadTestModel();
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::tearDown()
   {
      mTestNode = NULL;

      dtCore::System::GetInstance().Stop();

      mPhysicsWorld = NULL;
      mMatA = NULL;
      mMatB = NULL;
      mMatC = NULL;

      mCompiler = NULL;

      mCompileCount = 0;
      mCompileResult = NULL;

      mGM->DeleteAllActors(true);
      mGM->UnloadActorRegistry(DTPHYSICS_REGISTRY);
      mGM = NULL;
      dtPhysics::PhysicsWorld::Shutdown();
      
      VertexData::ClearAllCachedData();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Node* PhysicsCompilerTests::LoadTestModel(const std::string& modelPath)
   {
      dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList() + ";" + dtUtil::GetDeltaRootPath() + "/examples/data/;");
      std::string filepath = dtUtil::FindFileInPathList(modelPath);
      CPPUNIT_ASSERT( ! filepath.empty());
      
      osg::Node* model = dtUtil::FileUtils::GetInstance().ReadNode(filepath);
      CPPUNIT_ASSERT(model != NULL);

      return model;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::SetupTestActors()
   {
      // NOTE: Term "Proxy" is being removed from class names
      // and what are currently "Actors" will be renamed "Drawables";
      // In turn "Proxy" will be renamed to "Actor"
      // These typdefs are here to prevent confusion.
      typedef dtPhysics::MaterialActor PhysicsMaterialActor;

      if (!dtPhysics::PhysicsWorld::IsInitialized())
      {
         mPhysicsWorld = new PhysicsWorld(mCurrentEngine);
         mPhysicsWorld->Init();
      }

      mGM->AddComponent(*new dtPhysics::PhysicsComponent(dtPhysics::PhysicsWorld::GetInstance(), false));

      dtCore::RefPtr<PhysicsMaterialActor> matA;
      dtCore::RefPtr<PhysicsMaterialActor> matB;
      dtCore::RefPtr<PhysicsMaterialActor> matC;

      mGM->CreateActor(*dtPhysics::PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE, matA);
      mGM->CreateActor(*dtPhysics::PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE, matB);
      mGM->CreateActor(*dtPhysics::PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE, matC);

      matA->SetName(NAME_A);
      matB->SetName(NAME_B);
      matC->SetName(NAME_C);

      // Set the material indices directly since it is not automatic for MaterialDef.
      matA->GetMaterialDef().SetMaterialIndex(INDEX_A);
      matB->GetMaterialDef().SetMaterialIndex(INDEX_B);
      matC->GetMaterialDef().SetMaterialIndex(INDEX_C);

      mGM->AddActor(*matA);
      mGM->AddActor(*matB);
      mGM->AddActor(*matC);
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::SetupTestPhysicsWorld()
   {
      try
      {
         mPhysicsWorld = new PhysicsWorld(mCurrentEngine);
         mPhysicsWorld->Init();

         PhysicsMaterials& materials = mPhysicsWorld->GetMaterials();

         MaterialDef def;

         def.SetKineticFriction(Real(0.33));
         def.SetStaticFriction(Real(0.33));
         def.SetRestitution(Real(0.33));
         mMatA = materials.NewMaterial(NAME_A, def);

         def.SetKineticFriction(Real(0.66));
         def.SetStaticFriction(Real(0.66));
         def.SetRestitution(Real(0.66));
         mMatB = materials.NewMaterial(NAME_B, def);

         def.SetKineticFriction(Real(0.99));
         def.SetStaticFriction(Real(0.99));
         def.SetRestitution(Real(0.99));
         mMatC = materials.NewMaterial(NAME_C, def);
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   // HELPER STRUCT
   struct FindDescriptionPred
   {
      bool operator() (const std::string& desc) const
      {
         return mTargetDesc.empty() ? true
            : mTargetDesc == desc;
      }

      std::string mTargetDesc;
   };

   /////////////////////////////////////////////////////////////////////////////
   std::string PhysicsCompilerTests::GetNodeDescription(const osg::Node& node) const
   {
      typedef osg::Node::DescriptionList DescList;
      const DescList& descs = node.getDescriptions();

      FindDescriptionPred pred;

      DescList::const_iterator foundIter = std::find_if(descs.begin(), descs.end(), pred);
      return foundIter != descs.end() ? *foundIter : "";
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string PhysicsCompilerTests::GetNodeDescription_Explicit(const osg::Node& node) const
   {
      typedef osg::Node::DescriptionList DescList;
      const DescList& descs = node.getDescriptions();

      // Set the explicit description to find.
      FindDescriptionPred pred;
      pred.mTargetDesc = mNodeDescriptionToFind;

      DescList::const_iterator foundIter = std::find_if(descs.begin(), descs.end(), pred);
      return foundIter != descs.end() ? *foundIter : "";
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string PhysicsCompilerTests::GetStringNonFiltered(const std::string& str) const
   {
      return str;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::OnGeometryCompiled(PhysicsCompileResult& result)
   {
      mCompileResult = &result;
      ++mCompileCount;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestNodeDescriptionCollector()
   {
      NodeDescriptionSearchFunc nodeDescSearchFunc(this, &PhysicsCompilerTests::GetNodeDescription);

      NodeDescriptionCollector visitor(nodeDescSearchFunc);
      
      mTestNode->accept(visitor);

      // 4 Geometries, 3 unique descriptions.
      std::set<std::string>& strArray = visitor.mDescriptionList;
      CPPUNIT_ASSERT(visitor.mNodesWithDescriptionsCount == 4);
      CPPUNIT_ASSERT(strArray.size() == 3);
      CPPUNIT_ASSERT(strArray.end() != strArray.find(DESCRIPTION_A.c_str()));
      CPPUNIT_ASSERT(strArray.end() != strArray.find(DESCRIPTION_B.c_str()));
      CPPUNIT_ASSERT(strArray.end() != strArray.find(DESCRIPTION_C.c_str()));


      // Test finding explicit descriptions.
      nodeDescSearchFunc = NodeDescriptionSearchFunc(this, &PhysicsCompilerTests::GetNodeDescription_Explicit);
      
      visitor.mNodeDescSearchFunc = nodeDescSearchFunc;
      
      strArray.clear();
      visitor.mNodesWithDescriptionsCount = 0;
      mNodeDescriptionToFind = DESCRIPTION_A; // For nodes Q1 & Q3
      mTestNode->accept(visitor);
      CPPUNIT_ASSERT(strArray.end() != strArray.find(DESCRIPTION_A.c_str()));
      CPPUNIT_ASSERT(strArray.size() == 1);
      CPPUNIT_ASSERT(visitor.mNodesWithDescriptionsCount == 2);

      strArray.clear();
      visitor.mNodesWithDescriptionsCount = 0;
      mNodeDescriptionToFind = DESCRIPTION_B; // Q2
      mTestNode->accept(visitor);
      CPPUNIT_ASSERT(strArray.end() != strArray.find(DESCRIPTION_B.c_str()));
      CPPUNIT_ASSERT(strArray.size() == 1);
      CPPUNIT_ASSERT(visitor.mNodesWithDescriptionsCount == 1);

      strArray.clear();
      visitor.mNodesWithDescriptionsCount = 0;
      mNodeDescriptionToFind = DESCRIPTION_C; // Q4
      mTestNode->accept(visitor);
      CPPUNIT_ASSERT(strArray.end() != strArray.find(DESCRIPTION_C.c_str()));
      CPPUNIT_ASSERT(strArray.size() == 1);
      CPPUNIT_ASSERT(visitor.mNodesWithDescriptionsCount == 1);
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestNodeDescriptionFilter()
   {
      // Test default functionality.
      std::string testStr("PhysicsMat = SomeName");

      std::string result = mCompiler->GetMaterialNameFiltered(testStr);
      CPPUNIT_ASSERT(result == "SomeName");


      // Test a custom filter
      FilterStringFunc filterFunc(this, &PhysicsCompilerTests::GetStringNonFiltered);
      mCompiler->SetNodeDescriptionFilter(filterFunc);

      result = mCompiler->GetMaterialNameFiltered(testStr);
      CPPUNIT_ASSERT(result == testStr);


      // Test reseting the default functionality.
      FilterStringFunc nullFunc;
      mCompiler->SetNodeDescriptionFilter(nullFunc);

      result = mCompiler->GetMaterialNameFiltered(testStr);
      CPPUNIT_ASSERT(result == "SomeName");
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestGetNodeDescriptions()
   {
      typedef std::set<std::string> StrSet;
      StrSet strs;
      CPPUNIT_ASSERT(3 == mCompiler->GetNodeDescriptions(*mTestNode, strs));
      CPPUNIT_ASSERT(strs.size() == 3);
      CPPUNIT_ASSERT(strs.end() != strs.find(DESCRIPTION_A.c_str()));
      CPPUNIT_ASSERT(strs.end() != strs.find(DESCRIPTION_B.c_str()));
      CPPUNIT_ASSERT(strs.end() != strs.find(DESCRIPTION_C.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestDefaultMaterial()
   {
      try
      {
         // Test initial values.
         CPPUNIT_ASSERT(NAME_DEFAULT == mCompiler->GetDefaultMaterialName());
         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetDefaultMaterialIndex());

         SetupTestPhysicsWorld();

         // Test setting a value.
         mCompiler->SetDefaultMaterialName(NAME_B);
         CPPUNIT_ASSERT(NAME_B == mCompiler->GetDefaultMaterialName());
         CPPUNIT_ASSERT(INDEX_B == mCompiler->GetDefaultMaterialIndex());

         // Test setting back the system default.
         mCompiler->SetDefaultMaterialName(NAME_DEFAULT);
         CPPUNIT_ASSERT(NAME_DEFAULT == mCompiler->GetDefaultMaterialName());
         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetDefaultMaterialIndex());
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestMaterialSearchByActor()
   {
      SetupTestPhysicsWorld();
      SetupTestActors();

      try
      {
         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetMaterialIndexByMaterialActor(NAME_A));
         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetMaterialIndexByMaterialActor(NAME_B));
         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetMaterialIndexByMaterialActor(NAME_C));
         // --- Ensure that the compiler resorts to a default search method.
         CPPUNIT_ASSERT(INDEX_A == mCompiler->GetMaterialIndex(NAME_A));
         CPPUNIT_ASSERT(INDEX_B == mCompiler->GetMaterialIndex(NAME_B));
         CPPUNIT_ASSERT(INDEX_C == mCompiler->GetMaterialIndex(NAME_C));

         // Enable search-by-actor
         CPPUNIT_ASSERT( ! mCompiler->IsMaterialSearchByActor());
         mCompiler->SetMaterialSearchByActor(mGM.get());
         CPPUNIT_ASSERT(mCompiler->IsMaterialSearchByActor());
         
         CPPUNIT_ASSERT(INDEX_A == mCompiler->GetMaterialIndexByMaterialActor(NAME_A));
         CPPUNIT_ASSERT(INDEX_B == mCompiler->GetMaterialIndexByMaterialActor(NAME_B));
         CPPUNIT_ASSERT(INDEX_C == mCompiler->GetMaterialIndexByMaterialActor(NAME_C));
         // --- Ensure that the compiler's higher level search method still works.
         CPPUNIT_ASSERT(INDEX_A == mCompiler->GetMaterialIndex(NAME_A));
         CPPUNIT_ASSERT(INDEX_B == mCompiler->GetMaterialIndex(NAME_B));
         CPPUNIT_ASSERT(INDEX_C == mCompiler->GetMaterialIndex(NAME_C));
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestMaterialSearchByPhysicsWorld()
   {
      // Setup actors only first to ensure the world search methods
      // are not affected by their presence.
      try
      {
         CPPUNIT_ASSERT( ! mCompiler->IsMaterialSearchByActor());

         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetMaterialIndexByPhysicsWorld(NAME_A));
         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetMaterialIndexByPhysicsWorld(NAME_B));
         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetMaterialIndexByPhysicsWorld(NAME_C));
         // --- Ensure that the compiler resorts to a default search method.
         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetMaterialIndex(NAME_A));
         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetMaterialIndex(NAME_B));
         CPPUNIT_ASSERT(INDEX_NULL == mCompiler->GetMaterialIndex(NAME_C));

         // Enable search-by-physics-world by creating the world instance.
         SetupTestPhysicsWorld();
         SetupTestActors();
         
         CPPUNIT_ASSERT(INDEX_A == mCompiler->GetMaterialIndexByPhysicsWorld(NAME_A));
         CPPUNIT_ASSERT(INDEX_B == mCompiler->GetMaterialIndexByPhysicsWorld(NAME_B));
         CPPUNIT_ASSERT(INDEX_C == mCompiler->GetMaterialIndexByPhysicsWorld(NAME_C));
         // --- Ensure that the compiler's higher level search method still works.
         CPPUNIT_ASSERT(INDEX_A == mCompiler->GetMaterialIndex(NAME_A));
         CPPUNIT_ASSERT(INDEX_B == mCompiler->GetMaterialIndex(NAME_B));
         CPPUNIT_ASSERT(INDEX_C == mCompiler->GetMaterialIndex(NAME_C));
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestCompileSingleMaterial()
   {
      SetupTestPhysicsWorld();

      VertexDataTable data;
      PhysicsCompileOptions options; // use defaults

      try
      {
         CPPUNIT_ASSERT( ! mCompiler->IsMaterialSearchByActor());

         CPPUNIT_ASSERT(1 == mCompiler->CompilePhysicsForNodeMaterial(*mTestNode, options, DESCRIPTION_A, data));
         CPPUNIT_ASSERT(1 == mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT(1 == data.size());
         CPPUNIT_ASSERT(1 == data[NAME_A].size());
         CPPUNIT_ASSERT(1 == mCompileResult->mPartTotalInProgress);
         CPPUNIT_ASSERT(NAME_A == mCompileResult->mMaterialName);
         data.clear();
         mCompileCount = 0;
         mCompileResult = NULL;
         
         CPPUNIT_ASSERT(1 == mCompiler->CompilePhysicsForNodeMaterial(*mTestNode, options, DESCRIPTION_B, data));
         CPPUNIT_ASSERT(1 == mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT(1 == data.size());
         CPPUNIT_ASSERT(1 == data[NAME_B].size());
         CPPUNIT_ASSERT(1 == mCompileResult->mPartTotalInProgress);
         CPPUNIT_ASSERT(NAME_B == mCompileResult->mMaterialName);
         data.clear();
         mCompileCount = 0;
         mCompileResult = NULL;
         
         CPPUNIT_ASSERT(1 == mCompiler->CompilePhysicsForNodeMaterial(*mTestNode, options, DESCRIPTION_C, data));
         CPPUNIT_ASSERT(1 == mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT(1 == data.size());
         CPPUNIT_ASSERT(1 == data[NAME_C].size());
         CPPUNIT_ASSERT(1 == mCompileResult->mPartTotalInProgress);
         CPPUNIT_ASSERT(NAME_C == mCompileResult->mMaterialName);
         data.clear();
         mCompileCount = 0;
         mCompileResult = NULL;
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestCompileSingleMaterialByActors()
   {
      SetupTestActors();

      VertexDataTable data;
      PhysicsCompileOptions options; // use defaults

      try
      {
         CPPUNIT_ASSERT( ! mCompiler->IsMaterialSearchByActor());
         mCompiler->SetMaterialSearchByActor(mGM.get());
         CPPUNIT_ASSERT(mCompiler->IsMaterialSearchByActor());

         CPPUNIT_ASSERT(1 == mCompiler->CompilePhysicsForNodeMaterial(*mTestNode, options, DESCRIPTION_A, data));
         CPPUNIT_ASSERT(1 == mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT(1 == data.size());
         CPPUNIT_ASSERT(1 == data[NAME_A].size());
         CPPUNIT_ASSERT(1 == mCompileResult->mPartTotalInProgress);
         CPPUNIT_ASSERT(NAME_A == mCompileResult->mMaterialName);
         data.clear();
         mCompileCount = 0;
         mCompileResult = NULL;
         
         CPPUNIT_ASSERT(1 == mCompiler->CompilePhysicsForNodeMaterial(*mTestNode, options, DESCRIPTION_B, data));
         CPPUNIT_ASSERT(1 == mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT(1 == data.size());
         CPPUNIT_ASSERT(1 == data[NAME_B].size());
         CPPUNIT_ASSERT(1 == mCompileResult->mPartTotalInProgress);
         CPPUNIT_ASSERT(NAME_B == mCompileResult->mMaterialName);
         data.clear();
         mCompileCount = 0;
         mCompileResult = NULL;
         
         CPPUNIT_ASSERT(1 == mCompiler->CompilePhysicsForNodeMaterial(*mTestNode, options, DESCRIPTION_C, data));
         CPPUNIT_ASSERT(1 == mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT(1 == data.size());
         CPPUNIT_ASSERT(1 == data[NAME_C].size());
         CPPUNIT_ASSERT(1 == mCompileResult->mPartTotalInProgress);
         CPPUNIT_ASSERT(NAME_C == mCompileResult->mMaterialName);
         data.clear();
         mCompileCount = 0;
         mCompileResult = NULL;
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestCompileAllMaterials()
   {
      SetupTestPhysicsWorld();
      
      VertexDataTable data;
      PhysicsCompileOptions options; // use defaults

      try
      {
         CPPUNIT_ASSERT( ! mCompiler->IsMaterialSearchByActor());

         CPPUNIT_ASSERT(3 == mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT(3 == mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT(3 == data.size());
         CPPUNIT_ASSERT(1 == data[NAME_A].size());
         CPPUNIT_ASSERT(1 == data[NAME_B].size());
         CPPUNIT_ASSERT(1 == data[NAME_C].size());
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestCompileAllMaterialsByActors()
   {
      SetupTestActors();
      
      VertexDataTable data;
      PhysicsCompileOptions options; // use defaults

      try
      {
         CPPUNIT_ASSERT( ! mCompiler->IsMaterialSearchByActor());
         mCompiler->SetMaterialSearchByActor(mGM.get());
         CPPUNIT_ASSERT(mCompiler->IsMaterialSearchByActor());

         CPPUNIT_ASSERT(3 == mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT(3 == mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT(3 == data.size());
         CPPUNIT_ASSERT(1 == data[NAME_A].size());
         CPPUNIT_ASSERT(1 == data[NAME_B].size());
         CPPUNIT_ASSERT(1 == data[NAME_C].size());
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestCompileMaxVertsPerMesh()
   {
      SetupTestPhysicsWorld();

      VertexDataTable data;
      PhysicsCompileOptions options;

      try
      {
         // Change the model.
         mTestNode = LoadTestModel(MODEL_HIGH_RES.Get());
         
         CPPUNIT_ASSERT(NAME_DEFAULT == mCompiler->GetDefaultMaterialName());

         // Test that all geometry can be combined to one mesh.
         CPPUNIT_ASSERT_EQUAL(1, mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT_EQUAL(1, mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT_EQUAL(size_t(1), data.size()); // one default material
         CPPUNIT_ASSERT_EQUAL(size_t(1), data[NAME_DEFAULT].size());
         mCompileCount = 0;
         data.clear();

         // Test a splt result using an option.
         // Has 180 verts, 60 for each of 3 dodecahedrons (3 geodes).
         options.mMaxVertsPerMesh = 15;

         // Split should not go below geode level.
         // Result should be 3 for the 3 geodes versus 12 as math would suggest.
         CPPUNIT_ASSERT_EQUAL(3, mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT_EQUAL(3, mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT_EQUAL(size_t(1), data.size()); // one default material
         CPPUNIT_ASSERT_EQUAL(size_t(3), data[NAME_DEFAULT].size());
         mCompileCount = 0;
         data.clear();


         // Test an arbitrary split that is larger than geodes, where
         // the resulting parts have more verts than a single geode.
         options.mMaxVertsPerMesh = 120; // One part may have 120 verts while the other has 60

         CPPUNIT_ASSERT_EQUAL(2, mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT_EQUAL(2, mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT_EQUAL(size_t(1), data.size()); // one default material
         CPPUNIT_ASSERT_EQUAL(size_t(2), data[NAME_DEFAULT].size());
         mCompileCount = 0;
         data.clear();


         // Test a limit larger than a geode but less than 2 combined.
         options.mMaxVertsPerMesh = 90; // One part may have 120 verts while the other has 60

         CPPUNIT_ASSERT_EQUAL(2, mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT_EQUAL(2, mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT_EQUAL(size_t(1), data.size()); // one default material
         CPPUNIT_ASSERT_EQUAL(size_t(2), data[NAME_DEFAULT].size());
      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestCompileMaxEdgeLength()
   {
      // TODO:
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestCompilePerGeode()
   {
      SetupTestPhysicsWorld();

      VertexDataTable data;
      PhysicsCompileOptions options;

      try
      {
         // Change the model.
         mTestNode = LoadTestModel(MODEL_HIGH_RES.Get());
         options.mSplitUpGeodes = true;

         CPPUNIT_ASSERT(NAME_DEFAULT == mCompiler->GetDefaultMaterialName());

         // We should get three even without a max number of verts.
         CPPUNIT_ASSERT_EQUAL(3, mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT_EQUAL(3, mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT_EQUAL(size_t(1), data.size()); // one default material
         CPPUNIT_ASSERT_EQUAL(size_t(1), data[NAME_DEFAULT].size());
      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestCompileNoDefaultMaterial()
   {
      SetupTestPhysicsWorld();

      VertexDataTable data;
      PhysicsCompileOptions options;

      try
      {
         // Change the model.
         mTestNode = LoadTestModel(MODEL_HIGH_RES.Get());

         // Ensure that the default material use is enabled by default.
         CPPUNIT_ASSERT(options.mAllowDefaultMaterial);

         CPPUNIT_ASSERT(1 == mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT(1 == mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT(1 == data.size()); // one default material
         CPPUNIT_ASSERT(1 == data[NAME_DEFAULT].size());
         mCompileCount = 0;
         data.clear();

         // Test disabling the use of the default material.
         options.mAllowDefaultMaterial = false;
         
         CPPUNIT_ASSERT(0 == mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT(0 == mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT(data.empty()); // one default material

         // Test re-enabling the default material to ensure
         // that the failed compile in the previous section
         // was not just because the compiler works once.
         // This will clarify that the compiler is still in
         // a working state.
         options.mAllowDefaultMaterial = true;

         CPPUNIT_ASSERT_EQUAL(1, mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT_EQUAL(1, mCompileCount); // Number of time callback called
         CPPUNIT_ASSERT_EQUAL(size_t(1), data.size()); // one default material
         CPPUNIT_ASSERT_EQUAL(size_t(1), data[NAME_DEFAULT].size());
         mCompileCount = 0;
         data.clear();
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   void PhysicsCompilerTests::TestMaterialAssignment()
   {
      SetupTestPhysicsWorld();
      SetupTestActors();
      TestMaterialAssignment(false);
      TestMaterialAssignment(true);
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestMaterialAssignment(bool overrideMaterials)
   {

      VertexDataTable data;
      PhysicsCompileOptions options; // use defaults

      try
      {
         CPPUNIT_ASSERT(3 == mCompiler->CompilePhysicsForNode(*mTestNode, options, data));
         CPPUNIT_ASSERT(3 == data.size());
         CPPUNIT_ASSERT(1 == data[NAME_A].size());
         CPPUNIT_ASSERT(1 == data[NAME_B].size());
         CPPUNIT_ASSERT(1 == data[NAME_C].size());

         // Ensure data references the proper material indices.
         CPPUNIT_ASSERT(data[NAME_A][0]->mMaterialFlags[0] == mMatA->GetId());
         CPPUNIT_ASSERT(data[NAME_B][0]->mMaterialFlags[0] == mMatB->GetId());
         CPPUNIT_ASSERT(data[NAME_C][0]->mMaterialFlags[0] == mMatC->GetId());
         CPPUNIT_ASSERT(data[NAME_A][0]->GetFirstMaterialIndex() == mMatA->GetId());
         CPPUNIT_ASSERT(data[NAME_B][0]->GetFirstMaterialIndex() == mMatB->GetId());
         CPPUNIT_ASSERT(data[NAME_C][0]->GetFirstMaterialIndex() == mMatC->GetId());

         RayCast rayA; // Quadrant 1
         rayA.SetOrigin(VectorType(5.0, 5.0, 1.0));
         rayA.SetDirection(VectorType(0.0, 0.0, -10.0));

         RayCast rayB; // Quadrant 2
         rayB.SetOrigin(VectorType(-5.0, 5.0, 1.0));
         rayB.SetDirection(VectorType(0.0, 0.0, -10.0));

         RayCast rayC; // Quadrant 3
         rayC.SetOrigin(VectorType(-5.0, -5.0, 1.0));
         rayC.SetDirection(VectorType(0.0, 0.0, -10.0));

         RayCast rayD; // Quadrant 4
         rayD.SetOrigin(VectorType(5.0, -5.0, 1.0));
         rayD.SetDirection(VectorType(0.0, 0.0, -10.0));


         // Setup collidable physics objects
         dtCore::Transform xform;
         dtCore::RefPtr<dtPhysics::Geometry> geomA = dtPhysics::Geometry::CreateConcaveGeometry(xform, *data[NAME_A][0], 0.0f);
         dtCore::RefPtr<dtPhysics::Geometry> geomB = dtPhysics::Geometry::CreateConcaveGeometry(xform, *data[NAME_B][0], 0.0f);
         dtCore::RefPtr<dtPhysics::Geometry> geomC = dtPhysics::Geometry::CreateConcaveGeometry(xform, *data[NAME_C][0], 0.0f);

         // --- Ensure the geometry has a reference back to the original vertex data.
         CPPUNIT_ASSERT(geomA->GetVertexData() == data[NAME_A][0].get());
         CPPUNIT_ASSERT(geomB->GetVertexData() == data[NAME_B][0].get());
         CPPUNIT_ASSERT(geomC->GetVertexData() == data[NAME_C][0].get());

         dtCore::RefPtr<PhysicsObject> poA = PhysicsObject::CreateNew("TestPlanesA");
         dtCore::RefPtr<PhysicsObject> poB = PhysicsObject::CreateNew("TestPlanesB");
         dtCore::RefPtr<PhysicsObject> poC = PhysicsObject::CreateNew("TestPlanesC");

         PrimitiveType& primType = PrimitiveType::TRIANGLE_MESH;
         MechanicsType& mechType = MechanicsType::STATIC;
         poA->SetPrimitiveType(primType);
         poB->SetPrimitiveType(primType);
         poC->SetPrimitiveType(primType);
         poA->SetMechanicsType(mechType);
         poB->SetMechanicsType(mechType);
         poC->SetMechanicsType(mechType);

         dtPhysics::MaterialActorPtr mat;
         mGM->CreateActor(*dtPhysics::PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE, mat);

         // the physics objects and physics actor component have to be added to an actor, so override material actor is as good as any.
         mat->AddComponent(*new dtPhysics::PhysicsActComp);
         mat->GetComponent<dtPhysics::PhysicsActComp>()->AddPhysicsObject(*poA);
         mat->GetComponent<dtPhysics::PhysicsActComp>()->AddPhysicsObject(*poB);
         mat->GetComponent<dtPhysics::PhysicsActComp>()->AddPhysicsObject(*poC);


         const std::string overrideMaterialName("boola");
         if (overrideMaterials)
         {
            mat->SetName(overrideMaterialName);
            mat->GetMaterialDef().SetRestitution(0.125);
            mat->GetMaterialDef().SetStaticFriction(0.25);
            mGM->AddActor(*mat);
            poA->SetMaterialId(mat->GetId());
            poB->SetMaterialId(mat->GetId());
            poC->SetMaterialId(mat->GetId());
         }
         poA->CreateFromGeometry(*geomA);
         poB->CreateFromGeometry(*geomB);
         poC->CreateFromGeometry(*geomC);

         if (overrideMaterials)
         {
            // --- Ensure that the appropriate materials have been set by the geometry.
            CPPUNIT_ASSERT_EQUAL(overrideMaterialName, poA->GetMaterial()->GetName());
            CPPUNIT_ASSERT_EQUAL(overrideMaterialName, poB->GetMaterial()->GetName());
            CPPUNIT_ASSERT_EQUAL(overrideMaterialName, poC->GetMaterial()->GetName());
         }
         else
         {
            // --- Ensure that the appropriate materials have been set by the geometry.
            CPPUNIT_ASSERT(poA->GetMaterial() == mMatA);
            dtPhysics::MaterialActor* matTemp = nullptr;
            mGM->FindActorByName(mMatA->GetName(), matTemp);
            CPPUNIT_ASSERT(matTemp != nullptr);
            // I know these actors exist because the setup created them.
            CPPUNIT_ASSERT_EQUAL(matTemp->GetId(), poA->GetMaterialId());

            CPPUNIT_ASSERT(poB->GetMaterial() == mMatB);
            mGM->FindActorByName(mMatB->GetName(), matTemp);
            CPPUNIT_ASSERT(matTemp != nullptr);
            CPPUNIT_ASSERT_EQUAL(matTemp->GetId(), poB->GetMaterialId());

            CPPUNIT_ASSERT(poC->GetMaterial() == mMatC);
            mGM->FindActorByName(mMatC->GetName(), matTemp);
            CPPUNIT_ASSERT(matTemp != nullptr);
            CPPUNIT_ASSERT_EQUAL(matTemp->GetId(), poC->GetMaterialId());
         }


         // Cast rays for material detection.
         RayCast::Report reportA;
         RayCast::Report reportB;
         RayCast::Report reportC;
         RayCast::Report reportD;
         CPPUNIT_ASSERT(mPhysicsWorld->TraceRay(rayA, reportA));
         CPPUNIT_ASSERT(mPhysicsWorld->TraceRay(rayB, reportB));
         CPPUNIT_ASSERT(mPhysicsWorld->TraceRay(rayC, reportC));
         CPPUNIT_ASSERT(mPhysicsWorld->TraceRay(rayD, reportD));
         CPPUNIT_ASSERT(reportA.mHitObject.valid());
         CPPUNIT_ASSERT(reportB.mHitObject.valid());
         CPPUNIT_ASSERT(reportC.mHitObject.valid());
         CPPUNIT_ASSERT(reportD.mHitObject.valid());
         if (overrideMaterials)
         {
            CPPUNIT_ASSERT_EQUAL(overrideMaterialName, reportA.mHitObject->GetMaterial()->GetName());
            CPPUNIT_ASSERT_EQUAL(overrideMaterialName, reportB.mHitObject->GetMaterial()->GetName());
            CPPUNIT_ASSERT_EQUAL(overrideMaterialName, reportC.mHitObject->GetMaterial()->GetName());
            CPPUNIT_ASSERT_EQUAL(overrideMaterialName, reportD.mHitObject->GetMaterial()->GetName());
         }
         else
         {
            CPPUNIT_ASSERT(reportA.mHitObject->GetMaterial() == mMatA); // Material A assigned to Q1 & Q3
            CPPUNIT_ASSERT(reportB.mHitObject->GetMaterial() == mMatB); // Q2
            CPPUNIT_ASSERT(reportC.mHitObject->GetMaterial() == mMatA); // Material A assigned to Q1 & Q3
            CPPUNIT_ASSERT(reportD.mHitObject->GetMaterial() == mMatC); // Q4
         }
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestCreateGeometry()
   {
      SetupTestPhysicsWorld();

      typedef PhysicsObjectOptions PO;

      PhysicsObjectOptions options;
      dtCore::Transform xform;
      dtCore::RefPtr<VertexData> vertData;
      dtCore::RefPtr<Geometry> geom;
      dtPhysics::VectorType dims(1.0, 1.0, 1.0);
      Real massIncrement = 0.5;
      Real marginIncrement = 0.001;

      options.mDimensions = dims;

      try
      {
         VertexDataTable data;
         PhysicsCompileOptions compileOptions; // use defaults

         // Change the model.
         mTestNode = LoadTestModel(MODEL_HIGH_RES.Get());
         CPPUNIT_ASSERT(1 == mCompiler->CompilePhysicsForNode(*mTestNode, compileOptions, data));
         CPPUNIT_ASSERT(1 == data.size()); // one default material
         CPPUNIT_ASSERT(1 == data.begin()->second.size()); // one vertex databuffer
         vertData = data.begin()->second.front();
         CPPUNIT_ASSERT(vertData.valid());


         CPPUNIT_ASSERT(options.mPrimitiveType == PO::DEFAULT_PRIMITIVE_TYPE);
         CPPUNIT_ASSERT(options.mMechanicsType == PO::DEFAULT_MECHANICS_TYPE);
         CPPUNIT_ASSERT(options.mIsPolytope);
         CPPUNIT_ASSERT(options.mClearExistingObjects);
         CPPUNIT_ASSERT(options.mMass == PO::DEFAULT_MASS);
         CPPUNIT_ASSERT(options.mCollisionMargin == PO::DEFAULT_COLLISION_MARGIN);


         // --- SIMPLE TYPES --- //
         // BOX
         options.mPrimitiveType = &PrimitiveType::BOX;
         geom = mCompiler->CreateGeometry(options, xform, NULL);

         CPPUNIT_ASSERT(geom.valid());
         CPPUNIT_ASSERT(geom->GetMass() == options.mMass);
         CPPUNIT_ASSERT(geom->GetMargin() == options.mCollisionMargin);
         options.mMass += massIncrement;
         options.mCollisionMargin += marginIncrement;


         // SPHERE
         options.mPrimitiveType = &PrimitiveType::SPHERE;
         geom = mCompiler->CreateGeometry(options, xform, NULL);

         CPPUNIT_ASSERT(geom.valid());
         CPPUNIT_ASSERT(geom->GetMass() == options.mMass);
// For some reason this does not work for spheres.
//         CPPUNIT_ASSERT(geom->GetMargin() == options.mCollisionMargin);
         options.mMass += massIncrement;
         options.mCollisionMargin += marginIncrement;


         // CYLINDER
         options.mPrimitiveType = &PrimitiveType::CYLINDER;
         geom = mCompiler->CreateGeometry(options, xform, NULL);

         CPPUNIT_ASSERT(geom.valid());
         CPPUNIT_ASSERT(geom->GetMass() == options.mMass);
         CPPUNIT_ASSERT(geom->GetMargin() == options.mCollisionMargin);
         options.mMass += massIncrement;
         options.mCollisionMargin += marginIncrement;


         // CAPSULE
         options.mPrimitiveType = &PrimitiveType::CAPSULE;
         geom = mCompiler->CreateGeometry(options, xform, NULL);

         CPPUNIT_ASSERT(geom.valid());
         CPPUNIT_ASSERT(geom->GetMass() == options.mMass);
         CPPUNIT_ASSERT(geom->GetMargin() == options.mCollisionMargin);
         options.mMass += massIncrement;
         options.mCollisionMargin += marginIncrement;


         // --- COMPLEX TYPES --- //
         // TRIANGLE_MESH
         options.mPrimitiveType = &PrimitiveType::TRIANGLE_MESH;
         geom = mCompiler->CreateGeometry(options, xform, NULL);
         CPPUNIT_ASSERT( ! geom.valid());
         geom = mCompiler->CreateGeometry(options, xform, vertData.get());

         CPPUNIT_ASSERT(geom.valid());
         CPPUNIT_ASSERT(geom->GetMass() == options.mMass);
         CPPUNIT_ASSERT(geom->GetMargin() == options.mCollisionMargin);
         options.mMass += massIncrement;
         options.mCollisionMargin += marginIncrement;


         // TERRAIN_MESH
         options.mPrimitiveType = &PrimitiveType::TERRAIN_MESH;
         geom = mCompiler->CreateGeometry(options, xform, NULL);
         CPPUNIT_ASSERT( ! geom.valid());
         geom = mCompiler->CreateGeometry(options, xform, vertData.get());

         CPPUNIT_ASSERT(geom.valid());
         CPPUNIT_ASSERT(geom->GetMass() == options.mMass);
         CPPUNIT_ASSERT(geom->GetMargin() == options.mCollisionMargin);
         options.mMass += massIncrement;
         options.mCollisionMargin += marginIncrement;


         // CONVEX_HULL (POLYTOPE)
         options.mPrimitiveType = &PrimitiveType::CONVEX_HULL;
         geom = mCompiler->CreateGeometry(options, xform, NULL);
         CPPUNIT_ASSERT( ! geom.valid());
         geom = mCompiler->CreateGeometry(options, xform, vertData.get());

         CPPUNIT_ASSERT(geom.valid());
         CPPUNIT_ASSERT(geom->GetMass() == options.mMass);
         CPPUNIT_ASSERT(geom->GetMargin() == options.mCollisionMargin);
         options.mMass += massIncrement;
         options.mCollisionMargin += marginIncrement;


         // CONVEX_HULL (NON_POLYTOPE)
         // --- Test the polytope flag relevant only to convex hull types.
         options.mIsPolytope = false;
         geom = mCompiler->CreateGeometry(options, xform, vertData.get());

         CPPUNIT_ASSERT(geom.valid());
         CPPUNIT_ASSERT(geom->GetMass() == options.mMass);
         CPPUNIT_ASSERT(geom->GetMargin() == options.mCollisionMargin);
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsCompilerTests::TestCreatePhysicsObjectsForGeometry()
   {
      SetupTestPhysicsWorld();

      Real testMass = 0.5 + PhysicsObjectOptions::DEFAULT_MASS;

      PhysicsObjectOptions options;
      PhysicsObjectArray objects;
      
      options.mMass = testMass;

      try
      {
         VertexDataTable data;
         PhysicsCompileOptions compileOptions; // use defaults

         // Change the model.
         CPPUNIT_ASSERT(3 == mCompiler->CompilePhysicsForNode(*mTestNode, compileOptions, data));
         CPPUNIT_ASSERT(3 == data.size()); // one default material

         
         // Transfer the items from the 3 separate buckets to a single
         // vector for the sake of the following tests.
         VertexDataTable::iterator curIter = data.begin();
         TriangleRecorder::VertexDataArray vertArray;
         vertArray.push_back(curIter->second.front()); ++curIter;
         vertArray.push_back(curIter->second.front()); ++curIter;
         vertArray.push_back(curIter->second.front());

         CPPUNIT_ASSERT(3 == mCompiler->CreatePhysicsObjectsForGeometry(options, vertArray, objects));
         CPPUNIT_ASSERT(3 == objects.size());

         PhysicsObject* poA = objects[0].get();
         PhysicsObject* poB = objects[1].get();
         PhysicsObject* poC = objects[2].get();

         // Check the object material references
         CPPUNIT_ASSERT(poA != NULL);
         CPPUNIT_ASSERT(poA->GetMaterial() == mMatA);
         CPPUNIT_ASSERT(&poA->GetPrimitiveType() == options.mPrimitiveType);
         CPPUNIT_ASSERT(&poA->GetMechanicsType() == options.mMechanicsType);
         CPPUNIT_ASSERT(poA->GetGeometry(0) != NULL);
         CPPUNIT_ASSERT(poA->GetGeometry(0)->GetMass() == options.mMass);
         CPPUNIT_ASSERT(poA->GetGeometry(0)->GetMargin() == options.mCollisionMargin);
         
         CPPUNIT_ASSERT(poB != NULL);
         CPPUNIT_ASSERT(poB->GetMaterial() == mMatB);
         CPPUNIT_ASSERT(&poB->GetPrimitiveType() == options.mPrimitiveType);
         CPPUNIT_ASSERT(&poB->GetMechanicsType() == options.mMechanicsType);
         CPPUNIT_ASSERT(poB->GetGeometry(0) != NULL);
         CPPUNIT_ASSERT(poB->GetGeometry(0)->GetMass() == options.mMass);
         CPPUNIT_ASSERT(poB->GetGeometry(0)->GetMargin() == options.mCollisionMargin);

         CPPUNIT_ASSERT(poC != NULL);
         CPPUNIT_ASSERT(poC->GetMaterial() == mMatC);
         CPPUNIT_ASSERT(&poC->GetPrimitiveType() == options.mPrimitiveType);
         CPPUNIT_ASSERT(&poC->GetMechanicsType() == options.mMechanicsType);
         CPPUNIT_ASSERT(poC->GetGeometry(0) != NULL);
         CPPUNIT_ASSERT(poC->GetGeometry(0)->GetMass() == options.mMass);
         CPPUNIT_ASSERT(poC->GetGeometry(0)->GetMargin() == options.mCollisionMargin);
      }
      catch (dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

}
