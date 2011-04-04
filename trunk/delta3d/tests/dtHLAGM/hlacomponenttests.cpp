/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
*
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* Olen A. Bruce
* David Guthrie
* William E. Johnson II
*/
#include <prefix/unittestprefix.h>

//Defined in the dtgame unit tests.
#include "../dtGame/testcomponent.h"

#include <dtABC/application.h>

#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>

#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/namedgroupparameter.inl>

#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/ddmcameracalculatorgeographic.h>
#include <dtHLAGM/ddmregiondata.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/hlacomponentconfig.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/parametertoparameter.h>

#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/gameactor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>

#include <dtUtil/coordinates.h>
#include <dtUtil/datapathutils.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

#include <osg/Endian>

#include <iostream>
#include <string>
#include <vector>

extern dtABC::Application& GetGlobalApplication();

class TestHLAComponent: public dtHLAGM::HLAComponent
{
   public:

      void TestDeleteActor(const dtCore::UniqueId& actorId)
      {
         DeleteActor(actorId);
      }

      void TestPrepareInteraction(const dtGame::Message& message,
            RTI::ParameterHandleValuePairSet& interactionParams,
            const dtHLAGM::InteractionToMessage& interactionToMessage)
      {
         PrepareInteraction(message, interactionParams, interactionToMessage);
      }

      void TestPrepareUpdate(const dtGame::ActorUpdateMessage& message,
         RTI::AttributeHandleValuePairSet& updateAttrs,
         const dtHLAGM::ObjectToActor& objectToActor, bool newObject)
      {
         PrepareUpdate(message, updateAttrs, objectToActor, newObject);
      }

      dtHLAGM::ObjectRuntimeMappingInfo& GetRuntimeMappings()
      {
         return GetRuntimeMappingInfo();
      }
};



////////////////////////////////////////////////////////////////////////////////
static const std::string TEST_ATTR_ENTITY_TYPE("Entity Type As String");
static const std::string TEST_ATTR_MAPPING_NAME("Object Mapping Name");
static const std::string TEST_PARAM_MAPPING_NAME("Mapping Name");

class HLAComponentTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(HLAComponentTests);

      //All the tests are run in one method
      //they all use the RTI, which takes a long time
      //to start and stop it setup and teardown.
      CPPUNIT_TEST(RunAllTests);

      // 06/15/09
      //Temporarily removing this test from the suite due to a failing test
      //caused by an unknown issue.  Until the root of the problem is fixed,
      //these tests will have to remain off-line.
      // "ObjectClassNotPublished (Object class 16 is not published)
      //    'BaseEntity.PhysicalEntity.CulturalFeature'"

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void RunAllTests();

      void TestReflectAttributes();
      void TestReflectAttributesEntityTypeMissing();
      void TestReflectAttributesNoEntityType();
      void TestDispatchUpdate();
      void TestPrepareUpdate();
      void TestPrepareInteraction();
      void TestReceiveInteraction();
      void TestRuntimeMappingInfo();
      void TestSubscription();
      void TestConfigurationLocking();
      void TestGMLookup();
      void TestMessageProcessing();

   private:

      void BetweenTestSetUp();
      void BetweenTestTearDown();

      void AddAttribute(const std::string& name,
                        RTI::ObjectClassHandle classHandle,
                        RTI::AttributeHandleValuePairSet& handleSet,
                        char* encodedValue,
                        size_t size)
      {
         try
         {
            RTI::AttributeHandle attrHandle =
            mHLAComponent->GetRTIAmbassador()->getAttributeHandle(name.c_str(),
                                               classHandle);

            handleSet.add(attrHandle,
                          encodedValue,
                          size);

         }
         catch (RTI::NameNotFound)
         {
            CPPUNIT_FAIL("Unable to get attribute handle for " + name + ".");
         }
      }

      void AddParameter(const std::string& name,
                        RTI::InteractionClassHandle classHandle,
                        RTI::ParameterHandleValuePairSet& handleSet,
                        char* encodedValue,
                        size_t size)
      {
         try
         {
            RTI::ParameterHandle paramHandle =
                  mHLAComponent->GetRTIAmbassador()->getParameterHandle(name.c_str(),
                  classHandle);

            handleSet.add(paramHandle,
                        encodedValue,
                        size);
         }
         catch (RTI::NameNotFound)
         {
            CPPUNIT_FAIL("Unable to get parameter handle for " + name + ".");
         }
      }

      void PopulateTestActorUpdate(dtGame::ActorUpdateMessage& testMsg)
      {
         dtCore::UniqueId fakeActorId;
         testMsg.SetSendingActorId(fakeActorId);
         testMsg.SetAboutActorId(fakeActorId);
         testMsg.SetName("Rubber Chicken");
         testMsg.SetActorTypeCategory("TestHLA");
         testMsg.SetActorTypeName("Tank");

         testMsg.AddUpdateParameter(dtDAL::TransformableActorProxy::PROPERTY_ROTATION, dtDAL::DataType::VEC3);
      }

      dtCore::RefPtr<dtGame::GameManager> mGameManager;
      dtCore::RefPtr<TestHLAComponent> mHLAComponent;
      dtCore::RefPtr<TestComponent> mTestComponent;

      RTI::ObjectClassHandle mClassHandle1, mClassHandle2, mClassHandle3;
      RTI::ObjectHandle mObjectHandle1, mObjectHandle2, mObjectHandle3;

      static const std::string mTestGameActorLibrary;

      dtUtil::Log* logger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(HLAComponentTests);

const std::string HLAComponentTests::mTestGameActorLibrary = "testGameActorLibrary";

// Called implicitly by CPPUNIT when the app starts
void HLAComponentTests::setUp()
{
   try
   {
      dtDAL::Project::GetInstance().CreateContext("data/ProjectContext");
      dtDAL::Project::GetInstance().SetContext("data/ProjectContext");
      dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList() + ":" + dtUtil::GetDeltaRootPath() + "/tests/data");
      std::string logName("HLAComponentTests");
      logger = &dtUtil::Log::GetInstance(logName);
      dtUtil::Log::GetInstance("hlacomponent.cpp").SetLogLevel(dtUtil::Log::LOG_DEBUG);
      mGameManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
      mGameManager->SetApplication(GetGlobalApplication());
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);

      dtCore::RefPtr<dtGame::DefaultMessageProcessor> defMsgComp = new dtGame::DefaultMessageProcessor();
      mGameManager->AddComponent(*defMsgComp, dtGame::GameManager::ComponentPriority::HIGHEST);
      mTestComponent = new TestComponent("name");
      mGameManager->AddComponent(*mTestComponent, dtGame::GameManager::ComponentPriority::NORMAL);
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }

   mHLAComponent = new TestHLAComponent();

   try
   {
      dtCore::RefPtr<dtHLAGM::DDMCameraCalculatorGeographic> cameraCalculator = new dtHLAGM::DDMCameraCalculatorGeographic;
      cameraCalculator->SetCamera(new dtCore::Camera("Geographic"));
      cameraCalculator->SetName("Geographic");

      mHLAComponent->GetDDMSubscriptionCalculators().AddCalculator(*cameraCalculator);
      mGameManager->AddComponent(*mHLAComponent, dtGame::GameManager::ComponentPriority::NORMAL);
      dtHLAGM::HLAComponentConfig config;
      config.LoadConfiguration(*mHLAComponent, "Federations/HLAMappingExample.xml");
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }

   try
   {
      const std::string fom = "RPR-FOM.fed";
      const std::string fedFile = dtUtil::FindFileInPathList(fom);
      const std::string ridFile = dtUtil::FindFileInPathList("testRID.rid");
      CPPUNIT_ASSERT_MESSAGE("Couldn't find \"" + fom +
                             "\", make sure you install the Delta3D data package and set the DELTA_DATA environment var.",
                             !fedFile.empty());
      CPPUNIT_ASSERT(mHLAComponent->GetRTIAmbassador() == NULL);
      mHLAComponent->JoinFederationExecution("hla", fedFile, "delta3d", ridFile);
      CPPUNIT_ASSERT(mHLAComponent->GetRTIAmbassador() != NULL);
   }
   catch (const RTI::Exception& ex)
   {
      std::ostringstream ss;
      ss << ex;
      CPPUNIT_FAIL(std::string("Error joining federation : ") + ss.str());
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(std::string("Error joining federation : ") + ex.ToString());
   }

}

// Called implicitly by CPPUNIT when the app terminates
void HLAComponentTests::tearDown()
{
   if (mHLAComponent.valid())
   {
      try
      {
         mHLAComponent->LeaveFederationExecution();
         CPPUNIT_ASSERT(mHLAComponent->GetRTIAmbassador() == NULL);
      }
      catch (...)
      {
         //failed leaving execution.      	
      }
   }

   dtCore::System::GetInstance().Stop();
   if (mGameManager.valid())
   {
      mGameManager->RemoveComponent(*mHLAComponent);
      mGameManager->RemoveComponent(*mTestComponent);
      
      mHLAComponent  = NULL;
      mTestComponent = NULL;
      mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
      mGameManager   = NULL;
   }
}

void HLAComponentTests::BetweenTestSetUp()
{
   RTI::RTIambassador* rtiamb = mHLAComponent->GetRTIAmbassador();
   CPPUNIT_ASSERT(rtiamb != NULL);

   try
   {
      mClassHandle1 = rtiamb->getObjectClassHandle(
                     "BaseEntity.PhysicalEntity.Platform.GroundVehicle");
      mObjectHandle1 = rtiamb->registerObjectInstance(mClassHandle1,
                                                      "TestObject1");
   }
   catch (const RTI::Exception& e)
   {
      std::ostringstream ss;
      ss << e << " '" << rtiamb->getObjectClassName(mClassHandle1) << "'";
      CPPUNIT_FAIL(ss.str());
   }

   try
   {
      mClassHandle2 = rtiamb->getObjectClassHandle(
                      "BaseEntity.PhysicalEntity.Platform.Aircraft");
      mObjectHandle2 = rtiamb->registerObjectInstance(mClassHandle2,
                                                     "TestObject2");
   }
   catch (const RTI::Exception& e)
   {
      std::ostringstream ss;
      ss << e << " '" << rtiamb->getObjectClassName(mClassHandle2) << "'";
      CPPUNIT_FAIL(ss.str());
   }

   try
   {
      mClassHandle3 = rtiamb->getObjectClassHandle(
                      "BaseEntity.PhysicalEntity.CulturalFeature");
      mObjectHandle3 = rtiamb->registerObjectInstance(mClassHandle3,
                                                      "TestObject3");
   }
   catch (const RTI::Exception& e)
   {
      std::ostringstream ss;
      ss << e << " '" << rtiamb->getObjectClassName(mClassHandle3) << "'";
      CPPUNIT_FAIL(ss.str());
   }

   mTestComponent->reset();
}

void HLAComponentTests::BetweenTestTearDown()
{
   mHLAComponent->GetRuntimeMappings().Clear();
   RTI::RTIambassador* rtiamb = mHLAComponent->GetRTIAmbassador();
   CPPUNIT_ASSERT(rtiamb != NULL);

   try
   {
      rtiamb->deleteObjectInstance(mObjectHandle1, "TestObject1");
      rtiamb->deleteObjectInstance(mObjectHandle2, "TestObject2");
      rtiamb->deleteObjectInstance(mObjectHandle3, "TestObject3");
   }
   catch (const RTI::Exception &e)
   {
      std::ostringstream ss;
      ss << e;
      CPPUNIT_FAIL(ss.str());
   }
}

void HLAComponentTests::RunAllTests()
{
   try
   {
      //This is a quick, read-only test.
      TestSubscription();

      BetweenTestSetUp();
      TestConfigurationLocking();
      BetweenTestTearDown();

      BetweenTestSetUp();
      TestReflectAttributes();
      BetweenTestTearDown();

      BetweenTestSetUp();
      TestReflectAttributesNoEntityType();
      BetweenTestTearDown();

      BetweenTestSetUp();
      TestReflectAttributesEntityTypeMissing();
      BetweenTestTearDown();

      BetweenTestSetUp();
      TestDispatchUpdate();
      BetweenTestTearDown();

      BetweenTestSetUp();
      TestPrepareUpdate();
      BetweenTestTearDown();

      BetweenTestSetUp();
      TestPrepareInteraction();
      BetweenTestTearDown();

      BetweenTestSetUp();
      TestReceiveInteraction();
      BetweenTestTearDown();
      BetweenTestSetUp();
      TestRuntimeMappingInfo();
      BetweenTestTearDown();

      BetweenTestSetUp();
      TestMessageProcessing();
      BetweenTestTearDown();

      BetweenTestSetUp();
      TestGMLookup();
      BetweenTestTearDown();

      //Test disconnect delete.

      dtHLAGM::ObjectRuntimeMappingInfo& mappings = mHLAComponent->GetRuntimeMappings();
      mappings.Clear();
      mappings.Put(0, dtCore::UniqueId());
      mappings.Put(1, dtCore::UniqueId());
      mappings.Put(2, dtCore::UniqueId());
      std::vector<dtCore::UniqueId> actorIdList;
      mappings.GetAllActorIds(actorIdList);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("3 mappings should be in the mapping list.", unsigned(3), unsigned(actorIdList.size()));

      mHLAComponent->LeaveFederationExecution();

      mTestComponent->reset();

      dtCore::System::GetInstance().Step();

      std::vector<dtCore::RefPtr<const dtGame::Message> >& messages = mTestComponent->GetReceivedProcessMessages();

      unsigned deleteCount = 0;
      for (unsigned i = 0; i < messages.size(); ++i)
      {
         if (messages[i]->GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
         {
            deleteCount++;
         }
      }
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The number of messages in the process list should equal the number of deletes",
         unsigned(deleteCount), unsigned(actorIdList.size()));
   }
   catch (RTI::ObjectNotKnown &)
   {
      CPPUNIT_FAIL("Object is unknown!");
   }
   catch (RTI::FederateNotExecutionMember &)
   {
      CPPUNIT_FAIL("Federate not Execution Member");
   }
   catch (RTI::ConcurrentAccessAttempted &)
   {
      CPPUNIT_FAIL("Concurrent Access Attempted");
   }
   catch (RTI::RTIinternalError &)
   {
      CPPUNIT_FAIL("RTIinternal Error");
   }
}

void HLAComponentTests::TestRuntimeMappingInfo()
{
   dtHLAGM::ObjectRuntimeMappingInfo mappingInfo;
   try
   {
      std::vector<dtCore::UniqueId> actorIdList;

      dtCore::UniqueId id1;
      dtCore::UniqueId id2;
      std::string rtiid1 = "TestRTIID01";
      std::string rtiid2 = "TestRTIID02";

      dtHLAGM::EntityIdentifier eid1(1,1,1), eid2(2,2,2);

      dtCore::RefPtr<dtHLAGM::ObjectToActor> ota1 = new dtHLAGM::ObjectToActor();
      ota1->SetObjectClassName("BaseEntity.PhysicalEntity.Platform.GroundVehicle");
      dtCore::RefPtr<dtHLAGM::ObjectToActor> ota2 = new dtHLAGM::ObjectToActor();
      ota2->SetObjectClassName("BaseEntity.PhysicalEntity.Platform.Aircraft");

      CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle1) == NULL);
      CPPUNIT_ASSERT(static_cast<const dtHLAGM::ObjectRuntimeMappingInfo*>(&mappingInfo)->GetObjectToActor(mObjectHandle1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetId(eid1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetEntityId(id1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetHandle(id1)  == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid1)  == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetRTIId(id1)  == NULL);

      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle1, id1));
      CPPUNIT_ASSERT_MESSAGE("Adding a second mapping for an object handle should fail.",
                              !mappingInfo.Put(mObjectHandle1, id2));
      CPPUNIT_ASSERT_MESSAGE("Adding a second mapping for an id should fail.",
                              !mappingInfo.Put(mObjectHandle2, id1));

      CPPUNIT_ASSERT(mappingInfo.Put(rtiid1, id1));
      CPPUNIT_ASSERT_MESSAGE("Adding a second mapping for an RTI id should fail.",
                              !mappingInfo.Put(rtiid1, id2));
      CPPUNIT_ASSERT_MESSAGE("Adding a second mapping for an id should fail.",
                              !mappingInfo.Put(rtiid2, id1));

      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle2, id2));
      CPPUNIT_ASSERT(mappingInfo.Put(rtiid2, id2));

      mappingInfo.GetAllActorIds(actorIdList);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The mapping info object should return two actor ids.", unsigned(2), unsigned(actorIdList.size()));

      CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetId(mObjectHandle1) == id1);
      CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle2) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetId(mObjectHandle2) == id2);

      CPPUNIT_ASSERT(mappingInfo.GetHandle(id1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetHandle(id1) == mObjectHandle1);
      CPPUNIT_ASSERT(mappingInfo.GetHandle(id2) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetHandle(id2) == mObjectHandle2);

      CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetIdByRTIId(rtiid1) == id1);
      CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid2) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetIdByRTIId(rtiid2) == id2);

      CPPUNIT_ASSERT(mappingInfo.GetRTIId(id1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetRTIId(id1) == rtiid1);
      CPPUNIT_ASSERT(mappingInfo.GetRTIId(id2) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetRTIId(id2) == rtiid2);

      //The order of the addition lines differs from the object to id and entity to id in this
      //case because the mapping is not bi-directional.
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle1, *ota1));
      CPPUNIT_ASSERT_MESSAGE("Adding a second mapping for an object handle should fail",
                             !mappingInfo.Put(mObjectHandle1, *ota2));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle2, *ota2));
      CPPUNIT_ASSERT_MESSAGE("Adding a second mapping for an object to actor mapping should fail",
                             !mappingInfo.Put(mObjectHandle2, *ota1));

      CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetObjectToActor(mObjectHandle1) == *ota1);
      CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle2) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetObjectToActor(mObjectHandle2) == *ota2);

      CPPUNIT_ASSERT(mappingInfo.Put(eid1, id1));
      CPPUNIT_ASSERT_MESSAGE("Adding a second mapping for an entity id should fail.",
                             !mappingInfo.Put(eid1, id2));
      CPPUNIT_ASSERT_MESSAGE("Adding a second mapping for an id should fail.",
                             !mappingInfo.Put(eid2, id1));
      CPPUNIT_ASSERT(mappingInfo.Put(eid2, id2));

      CPPUNIT_ASSERT(mappingInfo.GetId(eid1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetId(eid1) == id1);
      CPPUNIT_ASSERT(mappingInfo.GetId(eid2) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetId(eid2) == id2);

      CPPUNIT_ASSERT(mappingInfo.GetEntityId(id1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetEntityId(id1) == eid1);
      CPPUNIT_ASSERT(mappingInfo.GetEntityId(id2) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetEntityId(id2) == eid2);


      //removing object handle 2 should remove both the object id and object to actor mappings
      //which should, in turn, remove the entity id mapped to the id.
      mappingInfo.Remove(mObjectHandle2);

      CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetId(mObjectHandle1) == id1);
      CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle2) == NULL);

      CPPUNIT_ASSERT(mappingInfo.GetHandle(id1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetHandle(id1) == mObjectHandle1);
      CPPUNIT_ASSERT(mappingInfo.GetHandle(id2) == NULL);

      CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetObjectToActor(mObjectHandle1) == *ota1);
      CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle2) == NULL);

      CPPUNIT_ASSERT(mappingInfo.GetId(eid1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetId(eid1) == id1);
      CPPUNIT_ASSERT(mappingInfo.GetId(eid2) == NULL);

      CPPUNIT_ASSERT(mappingInfo.GetEntityId(id1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetEntityId(id1) == eid1);
      CPPUNIT_ASSERT(mappingInfo.GetEntityId(id2) == NULL);

      CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetIdByRTIId(rtiid1) == id1);
      CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid2) == NULL);

      CPPUNIT_ASSERT(mappingInfo.GetRTIId(id1) != NULL);
      CPPUNIT_ASSERT(*mappingInfo.GetRTIId(id1) == rtiid1);
      CPPUNIT_ASSERT(mappingInfo.GetRTIId(id2) == NULL);

      //removing object id 1 should remove both the object handle and entity id mappings
      //which should, in turn, remove the object to actor mapped to the handle.
      mappingInfo.Remove(id1);

      CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetHandle(id1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetId(eid1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetEntityId(id1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetRTIId(id1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid1) == NULL);

      CPPUNIT_ASSERT(mappingInfo.Put(eid1, id1));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle1, id1));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle1, *ota1));
      CPPUNIT_ASSERT(mappingInfo.Put(rtiid1, id1));

      //removing eid1 should remove both the object handle and actor id mappings
      //which should, in turn, remove the object to actor mapped to the handle.
      mappingInfo.Remove(eid1);

      CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetHandle(id1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetId(eid1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetEntityId(id1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetRTIId(id1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid1) == NULL);

      mappingInfo.GetAllActorIds(actorIdList);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The mapping info object has be cleared out, so it should have no actor id's.",
         unsigned(0), unsigned(actorIdList.size()));

      //Fill the mapping again to make sure it can be cleared.
      CPPUNIT_ASSERT(mappingInfo.Put(eid1, id1));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle1, id1));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle1, *ota1));
      CPPUNIT_ASSERT(mappingInfo.Put(rtiid1, id1));
      CPPUNIT_ASSERT(mappingInfo.Put(eid2, id2));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle2, id2));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle2, *ota2));
      CPPUNIT_ASSERT(mappingInfo.Put(rtiid2, id2));

      mappingInfo.GetAllActorIds(actorIdList);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The mapping info object should return two actor ids.",
         unsigned(2), unsigned(actorIdList.size()));

      mappingInfo.Clear();

      mappingInfo.GetAllActorIds(actorIdList);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The mapping info object has be cleared out, so it should have no actor id's.",
         unsigned(0), unsigned(actorIdList.size()));

      CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle1) == NULL);
      CPPUNIT_ASSERT(static_cast<const dtHLAGM::ObjectRuntimeMappingInfo*>(&mappingInfo)->GetObjectToActor(mObjectHandle1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetId(eid1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetEntityId(id1) == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetHandle(id1)  == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid1)  == NULL);
      CPPUNIT_ASSERT(mappingInfo.GetRTIId(id1)  == NULL);
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}

void HLAComponentTests::TestSubscription()
{
   const std::string message("No Handle should be 0 after connecting to the RTI: ");

   {
      std::vector<const dtHLAGM::ObjectToActor*> toFillOta;
      mHLAComponent->GetAllObjectToActorMappings(toFillOta);
      unsigned sizeota = toFillOta.size();
      CPPUNIT_ASSERT(sizeota > 0);

      for (std::vector<const dtHLAGM::ObjectToActor*>::iterator i = toFillOta.begin();
         i != toFillOta.end(); ++i)
      {
         const dtHLAGM::ObjectToActor& ota = **i;

         std::ostringstream ss;
         ss << message << " \"" << ota.GetObjectClassName() << "\" \"" << ota.GetActorType().GetCategory() << "."
            << ota.GetActorType().GetName()
            << "  IsRemoteOnly " << ota.IsRemoteOnly() << " IsLocalOnly " << ota.IsLocalOnly() << "\"";

         CPPUNIT_ASSERT_MESSAGE(ss.str(), ota.GetObjectClassHandle() != 0);

         if (!ota.GetEntityIdAttributeName().empty())
            CPPUNIT_ASSERT_MESSAGE(ss.str(), ota.GetEntityIdAttributeHandle() != 0);
         else
            CPPUNIT_ASSERT_MESSAGE("The entity id attribute should be null if it is not being used.",
                  ota.GetEntityIdAttributeHandle() == 0);

         if (ota.GetEntityType() != NULL)
            CPPUNIT_ASSERT_MESSAGE(ss.str(), ota.GetEntityTypeAttributeHandle() != 0);
         else
            CPPUNIT_ASSERT_MESSAGE("The DIS ID attribute should be null if it is not being used.",
                  ota.GetEntityTypeAttributeHandle() == 0);

         for (std::vector<dtHLAGM::AttributeToPropertyList>::const_iterator j = ota.GetOneToManyMappingVector().begin();
            j != ota.GetOneToManyMappingVector().end(); ++j)
         {
            const dtHLAGM::AttributeToPropertyList& atpl = *j;

            // Test for parameter mappings that should be marked as special.
            // --- Get the HLA name of the mapping.
            const std::string& hlaName = atpl.GetHLAName();

            // --- Begin the assert message...
            std::ostringstream oss;
            oss << "HLA attribute \"" << hlaName.c_str() << "\"";

            // --- Check the property mapping for its special state.
            if( hlaName == dtHLAGM::HLAComponent::ATTR_NAME_ENTITY_TYPE
               || hlaName == dtHLAGM::HLAComponent::ATTR_NAME_MAPPING_NAME )
            {
               oss << " MUST be marked as special" << std::endl;
               CPPUNIT_ASSERT_MESSAGE( oss.str(), atpl.IsSpecial() );
            }
            else // Normal property mapping that is not special.
            {
               oss << " should NOT be marked as special" << std::endl;
               CPPUNIT_ASSERT_MESSAGE( oss.str(), ! atpl.IsSpecial() );
               if (!hlaName.empty())
                  CPPUNIT_ASSERT_MESSAGE(ss.str(), atpl.GetAttributeHandle() != 0);
            }
         }
      }
   }

   {
      std::vector<const dtHLAGM::InteractionToMessage*> toFillItm;
      mHLAComponent->GetAllInteractionToMessageMappings(toFillItm);

      unsigned sizeitm = toFillItm.size();
      CPPUNIT_ASSERT(sizeitm > 0);

      for (std::vector<const dtHLAGM::InteractionToMessage*>::iterator i = toFillItm.begin();
         i != toFillItm.end(); ++i)
      {
         const dtHLAGM::InteractionToMessage& itm = **i;

         CPPUNIT_ASSERT_MESSAGE(message, itm.GetInteractionClassHandle() != 0);

         for (std::vector<dtHLAGM::ParameterToParameterList>::const_iterator j = itm.GetOneToManyMappingVector().begin();
            j != itm.GetOneToManyMappingVector().end(); ++j)
         {
            const dtHLAGM::ParameterToParameterList& atpl = *j;
            if (atpl.GetHLAName() == dtHLAGM::HLAComponent::ATTR_NAME_MAPPING_NAME)
            {
               CPPUNIT_ASSERT_MESSAGE("The parameter handle for the MAPPING_NAME should be 0",
                        atpl.GetParameterHandle() == 0);
            }
            else
            {
               CPPUNIT_ASSERT_MESSAGE(message, atpl.GetParameterHandle() != 0);
            }
         }
      }
   }

   std::vector<std::vector<const dtHLAGM::DDMRegionData*> > toFill;
   mHLAComponent->GetDDMSubscriptionCalculatorRegions(toFill);
   CPPUNIT_ASSERT(!toFill.empty());
   CPPUNIT_ASSERT(toFill.size() == mHLAComponent->GetDDMSubscriptionCalculators().GetSize());

   dtHLAGM::DDMCameraCalculatorGeographic* camCalc = dynamic_cast<dtHLAGM::DDMCameraCalculatorGeographic*>(mHLAComponent->GetDDMSubscriptionCalculators()[0]);
   CPPUNIT_ASSERT(camCalc != NULL);


   const dtHLAGM::DDMRegionData* data = toFill[0][0];

   CPPUNIT_ASSERT_EQUAL(3U, data->GetNumberOfExtents());

   const dtHLAGM::DDMRegionData::DimensionValues* dv = data->GetDimensionValue(0);
   CPPUNIT_ASSERT_EQUAL(camCalc->GetFirstDimensionName(), dv->mName);

   dv = data->GetDimensionValue(1);
   CPPUNIT_ASSERT_EQUAL(camCalc->GetSecondDimensionName(), dv->mName);

   dv = data->GetDimensionValue(2);
   CPPUNIT_ASSERT_EQUAL(camCalc->GetThirdDimensionName(), dv->mName);

}

void HLAComponentTests::TestConfigurationLocking()
{
   CPPUNIT_ASSERT_THROW_MESSAGE("One may not enable or disable DDM while connected.", mHLAComponent->SetDDMEnabled(!mHLAComponent->IsDDMEnabled()), dtUtil::Exception);
   CPPUNIT_ASSERT_THROW_MESSAGE("One may not clear the configuration which connected.", mHLAComponent->ClearConfiguration(), dtUtil::Exception);

   dtCore::RefPtr<dtDAL::ActorType> at = new dtDAL::ActorType("a", "b");
   dtHLAGM::EntityType et(1,2,3,4,5,6,7);
   std::vector<dtHLAGM::AttributeToPropertyList> testVec;
   CPPUNIT_ASSERT_THROW_MESSAGE("One may not register an actor mapping while it's connected.",
         mHLAComponent->RegisterActorMapping(*at, "", &et, testVec), dtUtil::Exception);

   dtCore::RefPtr<dtHLAGM::ObjectToActor> ota = new dtHLAGM::ObjectToActor;
   ota->SetActorType(*at);
   ota->SetObjectClassName("test1");
   ota->SetEntityType(&et);
   CPPUNIT_ASSERT_THROW_MESSAGE("One may not register an actor mapping while it's connected.",
         mHLAComponent->RegisterActorMapping(*ota), dtUtil::Exception);

   CPPUNIT_ASSERT_THROW_MESSAGE("One may not Unregister an actor mapping while it's connected.",
         mHLAComponent->UnregisterActorMapping(*at), dtUtil::Exception);

   CPPUNIT_ASSERT_THROW_MESSAGE("One may not Unregister an actor mapping while it's connected.",
         mHLAComponent->UnregisterObjectMapping("test1", &et), dtUtil::Exception);

   CPPUNIT_ASSERT_THROW_MESSAGE("One may not Unregister an actor mapping while it's connected.",
         mHLAComponent->UnregisterObjectMapping("test1", &et), dtUtil::Exception);

   dtCore::RefPtr<dtHLAGM::InteractionToMessage> itm = new dtHLAGM::InteractionToMessage;

   CPPUNIT_ASSERT_THROW_MESSAGE("One may not register an interaction mapping while it's connected.",
         mHLAComponent->RegisterMessageMapping(*itm), dtUtil::Exception);

   std::string interactionTypeName("");
   std::vector<dtHLAGM::ParameterToParameterList> testOneToOneMessageVector;

   CPPUNIT_ASSERT_THROW_MESSAGE("One may not register an interaction mapping while it's connected.",
         mHLAComponent->RegisterMessageMapping(dtGame::MessageType::INFO_ACTOR_UPDATED,
               interactionTypeName, testOneToOneMessageVector), dtUtil::Exception);

   CPPUNIT_ASSERT_THROW_MESSAGE("One may not Unregister an message mapping while it's connected.",
         mHLAComponent->UnregisterMessageMapping(dtGame::MessageType::INFO_ACTOR_UPDATED), dtUtil::Exception);

   CPPUNIT_ASSERT_THROW_MESSAGE("One may not Unregister an interaction mapping while it's connected.",
         mHLAComponent->UnregisterInteractionMapping(interactionTypeName), dtUtil::Exception);

   CPPUNIT_ASSERT_MESSAGE("The default entity type attribute should show up as a valid entity type",
            mHLAComponent->IsEntityTypeAttribute(mHLAComponent->GetHLAEntityTypeAttributeName()));

   CPPUNIT_ASSERT_MESSAGE("The AlternateEntityType on Physical Entity is used on an object to actor mapping in the loaded config.",
            mHLAComponent->IsEntityTypeAttribute("AlternateEntityType"));

   CPPUNIT_ASSERT_MESSAGE("This string is not a valid entity type attribute.",
            !mHLAComponent->IsEntityTypeAttribute("some junk"));
}

void HLAComponentTests::TestReflectAttributesNoEntityType()
{
   try
   {
      RTI::AttributeHandleValuePairSet* ahs =
         RTI::AttributeSetFactory::create(1);

      char encodedEulerAngles[sizeof(float) * 3];
      dtHLAGM::EulerAngles rotation(2.0f, 1.1f, 3.14f);
      rotation.Encode(encodedEulerAngles);
      AddAttribute("Orientation",
                   mClassHandle3,
                   *ahs,
                   encodedEulerAngles,
                   rotation.EncodedLength());

      mHLAComponent->discoverObjectInstance(mObjectHandle3, mClassHandle3, "testMapping");
      const dtCore::UniqueId* id = mHLAComponent->GetRuntimeMappings().GetId(mObjectHandle3);
      CPPUNIT_ASSERT(id != NULL);

      mHLAComponent->reflectAttributeValues(mObjectHandle3, *ahs, "");

      dtCore::System::GetInstance().Step();

      id = mHLAComponent->GetRuntimeMappings().GetId(mObjectHandle3);
      CPPUNIT_ASSERT_MESSAGE("It should have mapped in the new actor id, even with a NULL EntityType.",
                     id != NULL);

      //Check the actual message to see if it was a create message.
      dtCore::RefPtr<const dtGame::Message> msg =
            mTestComponent->FindProcessMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);

      CPPUNIT_ASSERT_MESSAGE("A create message should have been sent.", msg.valid());

      dtCore::RefPtr<const dtGame::ActorUpdateMessage> aum = static_cast<const dtGame::ActorUpdateMessage*>(msg.get());

      CPPUNIT_ASSERT_MESSAGE("The actor type should not be NULL.", aum->GetActorType() != NULL);
      CPPUNIT_ASSERT_MESSAGE("The actor type should be the CulturalFeature.", aum->GetActorType()->GetName() == "CulturalFeature");

   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}

void HLAComponentTests::TestReflectAttributesEntityTypeMissing()
{
   try
   {
      RTI::AttributeHandleValuePairSet* ahs =
         RTI::AttributeSetFactory::create(4);

      char encodedEntityIdentifier[6];

      dtHLAGM::EntityIdentifier entityId(3,3,1);

      entityId.Encode(encodedEntityIdentifier);

      AddAttribute("EntityIdentifier", mClassHandle1, *ahs, encodedEntityIdentifier,
                   entityId.EncodedLength());


      char encodedInt[sizeof(unsigned)];

      *((unsigned*)encodedInt) = 1;
      if (osg::getCpuByteOrder() == osg::LittleEndian)
         osg::swapBytes(encodedInt, sizeof(unsigned));

      AddAttribute("DamageState", mClassHandle1, *ahs, encodedInt, sizeof(unsigned));

      char encodedEulerAngles[sizeof(float) * 3];

      dtHLAGM::EulerAngles rotation(2.0f, 1.1f, 3.14f);
      rotation.Encode(encodedEulerAngles);
      AddAttribute("Orientation",
                   mClassHandle1,
                   *ahs,
                   encodedEulerAngles,
                   rotation.EncodedLength());


      char encodedWorldCoordinate[sizeof(double) * 3];
      dtHLAGM::WorldCoordinate location(1.0, 1.0, 1.0);
      location.Encode(encodedWorldCoordinate);

      AddAttribute("WorldLocation",
                   mClassHandle1,
                   *ahs,
                   encodedWorldCoordinate,
                   location.EncodedLength());

      mHLAComponent->discoverObjectInstance(mObjectHandle1, mClassHandle1, "");
      const dtCore::UniqueId* id = mHLAComponent->GetRuntimeMappings().GetId(mObjectHandle1);
      CPPUNIT_ASSERT(id != NULL);


      mHLAComponent->reflectAttributeValues(mObjectHandle1, *ahs, "");

      dtCore::System::GetInstance().Step();

      //Check the actual message to see if it was a create message.
      dtCore::RefPtr<const dtGame::Message> msg =
            mTestComponent->FindProcessMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);

      CPPUNIT_ASSERT_MESSAGE("No message should have been sent.", !msg.valid());

      id = mHLAComponent->GetRuntimeMappings().GetId(mObjectHandle1);
      CPPUNIT_ASSERT_MESSAGE("It should not have removed the object id since no entity type was sent.  It should wait to see if it will get an entity type later.",
                     id != NULL);


      //add the entity type.
      char encodedEntityType[8];

      //some unknown entity type.
      dtHLAGM::EntityType entityType(9,9,9,2,4,6,0);

      entityType.Encode(encodedEntityType);

      AddAttribute("EntityType", mClassHandle1, *ahs, encodedEntityType,
                   entityType.EncodedLength());


      mHLAComponent->reflectAttributeValues(mObjectHandle1, *ahs, "");

      id = mHLAComponent->GetRuntimeMappings().GetId(mObjectHandle1);
      CPPUNIT_ASSERT_MESSAGE("It should have removed the object id since it had an entity type, but no matching mapping.",
                     id == NULL);
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}

void HLAComponentTests::TestReflectAttributes()
{
   try
   {
      RTI::AttributeHandleValuePairSet* ahs =
         RTI::AttributeSetFactory::create(4);

      // Add Entity Identifier
      char encodedEntityIdentifier[6];

      dtHLAGM::EntityIdentifier entityId(3,3,1);

      entityId.Encode(encodedEntityIdentifier);

      AddAttribute("EntityIdentifier", mClassHandle1, *ahs, encodedEntityIdentifier,
                   entityId.EncodedLength());


      // Add Entity Type
      char encodedEntityType[8];

      dtHLAGM::EntityType entityType(1,1,222,2,4,6,0);

      entityType.Encode(encodedEntityType);

      AddAttribute("AlternateEntityType", mClassHandle1, *ahs, encodedEntityType,
                   entityType.EncodedLength());


      // Add Damage State
      char encodedInt[sizeof(unsigned)];

      *((unsigned*)encodedInt) = 1;
      if (osg::getCpuByteOrder() == osg::LittleEndian)
         osg::swapBytes(encodedInt, sizeof(unsigned));

      AddAttribute("DamageState", mClassHandle1, *ahs, encodedInt, sizeof(unsigned));


      // Add Orientation
      char encodedEulerAngles[sizeof(float) * 3];

      dtHLAGM::EulerAngles rotation(2.0f, 1.1f, 3.14f);
      rotation.Encode(encodedEulerAngles);
      AddAttribute("Orientation",
                   mClassHandle1,
                   *ahs,
                   encodedEulerAngles,
                   rotation.EncodedLength());

      dtHLAGM::ArticulatedParameter ap[] =
      {
               dtHLAGM::ArticulatedParameter(4, 0, dtHLAGM::ParameterValue(dtHLAGM::ArticulatedParts(2048, 8, 3.7))),
               dtHLAGM::ArticulatedParameter(3, 2048, dtHLAGM::ParameterValue(dtHLAGM::ArticulatedParts(4416, 8, -3.7))),
               dtHLAGM::ArticulatedParameter(1, 2048, dtHLAGM::ParameterValue(dtHLAGM::AttachedParts(0, dtHLAGM::EntityType(1, 1, 88, 7, 4, 3, 3))))
      };
      char encodedArticulations[ap[0].EncodedLength() * 3];
      ap[0].Encode(encodedArticulations );
      ap[1].Encode(encodedArticulations + ap[0].EncodedLength());
      ap[2].Encode(encodedArticulations + (ap[0].EncodedLength() * 2));

      AddAttribute("ArticulatedParametersArray",
                   mClassHandle1,
                   *ahs,
                   encodedArticulations,
                   ap[0].EncodedLength() * 3);

      // Add World Location
      char encodedWorldCoordinate[sizeof(double) * 3];
      dtHLAGM::WorldCoordinate location(1.0, 1.0, 1.0);
      location.Encode(encodedWorldCoordinate);

      AddAttribute("WorldLocation",
                   mClassHandle1,
                   *ahs,
                   encodedWorldCoordinate,
                   location.EncodedLength());


      mHLAComponent->discoverObjectInstance(mObjectHandle1, mClassHandle1, "dumbstringname");


      const dtCore::UniqueId* id = mHLAComponent->GetRuntimeMappings().GetId(mObjectHandle1);
      CPPUNIT_ASSERT(id != NULL);

      //test deleting right after creating
      mHLAComponent->removeObjectInstance(mObjectHandle1, "");

      id = mHLAComponent->GetRuntimeMappings().GetId(mObjectHandle1);
      CPPUNIT_ASSERT(id == NULL);

      //recreate the object for the full test.
      mHLAComponent->discoverObjectInstance(mObjectHandle1, mClassHandle1, "dumbname");

      id = mHLAComponent->GetRuntimeMappings().GetId(mObjectHandle1);
      CPPUNIT_ASSERT(id != NULL);


      mHLAComponent->reflectAttributeValues(mObjectHandle1, *ahs, "");

      dtCore::System::GetInstance().Step();

      //Check the actual message to see if it was a create message.
      dtCore::RefPtr<const dtGame::Message> msg =
            mTestComponent->FindProcessMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);

      CPPUNIT_ASSERT(msg.valid());

      //There is a mapping to set the sending id to the new actor as well.
      CPPUNIT_ASSERT(msg->GetSendingActorId() == *id);

      const dtGame::ActorUpdateMessage& createMsg = static_cast<const dtGame::ActorUpdateMessage&>(*msg);
      CPPUNIT_ASSERT(createMsg.GetUpdateParameter("Mesh") != NULL);

      CPPUNIT_ASSERT(createMsg.GetUpdateParameter("Articulated Parameters Array") != NULL);
      CPPUNIT_ASSERT_EQUAL(dtDAL::DataType::ARRAY, createMsg.GetUpdateParameter("Articulated Parameters Array")->GetDataType());
      const dtDAL::NamedArrayParameter* articulatedParams = static_cast<const dtDAL::NamedArrayParameter*>(createMsg.GetUpdateParameter("Articulated Parameters Array"));
      CPPUNIT_ASSERT_EQUAL(size_t(3), articulatedParams->GetSize());

      // just checking the names here to as a cursory test of functionality.
      // Since the array feature just calls the single item in a loop, as long
      // it basically is correct, the parameter translator test is sufficient to test the overall
      // behavior.
      for (unsigned i = 0; i < articulatedParams->GetSize(); ++i)
      {
         CPPUNIT_ASSERT(articulatedParams->GetParameter(i)->GetDataType() == dtDAL::DataType::GROUP);
         const dtDAL::NamedGroupParameter* curGroup = static_cast<const dtDAL::NamedGroupParameter*>(articulatedParams->GetParameter(i));
         CPPUNIT_ASSERT_EQUAL_MESSAGE(curGroup->GetName(), size_t(0), curGroup->GetName()->find("Articulated", 0));
         CPPUNIT_ASSERT(curGroup->GetParameter("OurName") != NULL);
       }


      //check the entity id mapping.
      id = mHLAComponent->GetRuntimeMappings().GetId(entityId);
      CPPUNIT_ASSERT(id != NULL);

      dtCore::RefPtr<dtGame::GameActorProxy> proxy = mGameManager->FindGameActorById(*id);
      CPPUNIT_ASSERT(proxy.valid());

      const dtHLAGM::ObjectToActor* otoa = mHLAComponent->GetActorMapping(proxy->GetActorType());
      const std::string& objectMappingName = otoa->GetMappingName();

      // Test that the object name has been assigned to the update message if it
      // has been mapped.
      const dtGame::StringMessageParameter* mappingNameParam =
         dynamic_cast<const dtGame::StringMessageParameter*>(createMsg.GetUpdateParameter(TEST_ATTR_MAPPING_NAME));
      if( mappingNameParam != NULL )
      {
         CPPUNIT_ASSERT( mappingNameParam->GetValue() == objectMappingName );
      }

      // Test that the Entity Type can be mapped to a string if it has been mapped
      // as a string parameter to one of the object mappings.
      const dtGame::StringMessageParameter* entityTypeParam =
         dynamic_cast<const dtGame::StringMessageParameter*>(createMsg.GetUpdateParameter(TEST_ATTR_ENTITY_TYPE));
      if( entityTypeParam != NULL )
      {
         std::stringstream oss;
         oss << entityType;
         CPPUNIT_ASSERT( oss.str() == entityTypeParam->GetValue() );
      }

      osg::Vec3 expectedTranslation = mHLAComponent->GetCoordinateConverter().
         ConvertToLocalTranslation(osg::Vec3d(location.GetX(),
                                              location.GetY(),
                                              location.GetZ()));

      osg::Vec3 expectedRotation = mHLAComponent->GetCoordinateConverter().
         ConvertToLocalRotation(rotation.GetPsi(), rotation.GetTheta(), rotation.GetPhi());

      dtCore::Transform xform;
      proxy->GetGameActor().GetTransform(xform);
      osg::Vec3 actualTranslation;
      xform.GetTranslation(actualTranslation);
      osg::Vec3 actualRotation;
      xform.GetRotation(actualRotation);

      CPPUNIT_ASSERT(osg::equivalent(actualTranslation[0], expectedTranslation[0], 1e-3f) &&
                     osg::equivalent(actualTranslation[1], expectedTranslation[1], 1e-3f) &&
                     osg::equivalent(actualTranslation[2], expectedTranslation[2], 1e-3f));
      CPPUNIT_ASSERT(osg::equivalent(actualRotation[0], expectedRotation[0], 1e-3f) &&
                     osg::equivalent(actualRotation[1], expectedRotation[1], 1e-3f) &&
                     osg::equivalent(actualRotation[2], expectedRotation[2], 1e-3f));

      dtDAL::ResourceActorProperty* rap = dynamic_cast<dtDAL::ResourceActorProperty*>(proxy->GetProperty("Mesh"));
      CPPUNIT_ASSERT(rap != NULL);
      dtDAL::ResourceDescriptor rd = rap->GetValue();
      CPPUNIT_ASSERT_MESSAGE("the mesh resource should have been set.", rd.IsEmpty() == false);
      const std::string expectedMeshValue("StaticMeshes:articulation_test.ive");
      CPPUNIT_ASSERT_EQUAL(expectedMeshValue, rd.GetResourceIdentifier());

      //Clear the resource value.
      rap->SetValue(dtDAL::ResourceDescriptor::NULL_RESOURCE);

      //run the same reflect call again to make sure the mesh value is not sent the second time.
      mHLAComponent->reflectAttributeValues(mObjectHandle1, *ahs, "");

      mTestComponent->reset();
      dtCore::System::GetInstance().Step();

      //Check the actual message to see if it was a create message.
      msg = mTestComponent->FindProcessMessageOfType(dtGame::MessageType::INFO_ACTOR_UPDATED);

      CPPUNIT_ASSERT(msg.valid());

      //There is a mapping to set the sending id to the new actor as well.
      CPPUNIT_ASSERT(msg->GetSendingActorId() == *id);

      CPPUNIT_ASSERT_MESSAGE("Non-required default values should only be sent in the actor created message",
         static_cast<const dtGame::ActorUpdateMessage&>(*msg).GetUpdateParameter("Mesh") == NULL);

      CPPUNIT_ASSERT_MESSAGE("The value should still be null because it didn't send the property the second time.",
         rap->GetValue().IsEmpty());

      //now test deleting the object.
      mHLAComponent->removeObjectInstance(mObjectHandle1, "");

      dtCore::System::GetInstance().Step();

      //Check using the old id value to see if the actor is in the GM.
      proxy = mGameManager->FindGameActorById(*id);
      CPPUNIT_ASSERT(!proxy.valid());

      //Check to see that the mappings were cleared.
      id = mHLAComponent->GetRuntimeMappings().GetId(mObjectHandle1);
      CPPUNIT_ASSERT(id == NULL);
      id = mHLAComponent->GetRuntimeMappings().GetId(entityId);
      CPPUNIT_ASSERT(id == NULL);

   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}

void HLAComponentTests::TestDispatchUpdate()
{
   try
   {
      dtCore::RefPtr<dtGame::ActorUpdateMessage> testMsg;
      mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED, testMsg);

      PopulateTestActorUpdate(*testMsg);

      const dtCore::UniqueId& fakeActorId = testMsg->GetAboutActorId();

      dtHLAGM::EntityIdentifier entityId(3,3,2);

      //insert a bogus mapping to see if the object maps properly.
      mHLAComponent->GetRuntimeMappings().Put(entityId, fakeActorId);

      dtCore::RefPtr<const dtDAL::ActorType> testTankType = mGameManager->FindActorType(testMsg->GetActorTypeCategory(), testMsg->GetActorTypeName());

      CPPUNIT_ASSERT(testTankType.valid());

      mHLAComponent->DispatchNetworkMessage(*testMsg);
      const std::string* rtiID = mHLAComponent->GetRuntimeMappings().GetRTIId(fakeActorId);
      const RTI::ObjectHandle* ptrHandle = mHLAComponent->GetRuntimeMappings().GetHandle(fakeActorId);
      CPPUNIT_ASSERT_MESSAGE("The RTI Object ID  string should be set when an object is first sent out via HLA",
            rtiID != NULL);
      CPPUNIT_ASSERT_MESSAGE("The RTI Object handle string should be set when an object is first sent out via HLA",
            ptrHandle != NULL);
      //save off a copy
      //const RTI::ObjectHandle handle = *ptrHandle;

      dtCore::RefPtr<dtGame::Message> deletedMsg;
      mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_DELETED, deletedMsg);

      deletedMsg->SetAboutActorId(fakeActorId);
      mHLAComponent->DispatchNetworkMessage(*deletedMsg);

      rtiID = mHLAComponent->GetRuntimeMappings().GetRTIId(fakeActorId);
      ptrHandle = mHLAComponent->GetRuntimeMappings().GetHandle(fakeActorId);
      CPPUNIT_ASSERT_MESSAGE("The RTI Object ID  should be NULL",
            rtiID == NULL);
      CPPUNIT_ASSERT_MESSAGE("The RTI Object handle should be NULL",
            ptrHandle == NULL);

   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}

void HLAComponentTests::TestPrepareUpdate()
{
   try
   {
      RTI::AttributeHandleValuePairSet* ahs =
         RTI::AttributeSetFactory::create(4);

      dtCore::RefPtr<dtGame::ActorUpdateMessage> testMsg;
      mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED, testMsg);

      PopulateTestActorUpdate(*testMsg);

      dtCore::RefPtr<dtDAL::NamedArrayParameter> articArray = new dtDAL::NamedArrayParameter("Articulated Parameters Array");
      testMsg->AddUpdateParameter(*articArray);

      // The sub group of parameters pertaining to either an
      // ArticulatedPart or an AttachPart
      dtCore::RefPtr<dtGame::GroupMessageParameter> subParam = NULL;

      // Create other articulated parameters to lengthen the array
      // --- Extra 0
      subParam = new dtGame::GroupMessageParameter( "ArticulatedPartMessageParam0" );
      subParam->AddValue( "Azimuth", 0.0f  );
      subParam->AddValue( "OurName", std::string("DOF1")  );
      subParam->AddValue( "OurParent", std::string("Parent_DOF0")  );
      subParam->AddValue( "Change", (unsigned short)(11) );
      articArray->AddParameter( *subParam );
      // --- Extra 1
      subParam = new dtGame::GroupMessageParameter( "ArticulatedPartMessageParam1" );
      subParam->AddValue( "Azimuth", 0.0f );
      subParam->AddValue( "OurName", std::string("DOF2") );
      subParam->AddValue( "OurParent", std::string("Parent_DOF1") );
      subParam->AddValue( "Change", (unsigned short)(84) );
      articArray->AddParameter( *subParam );
      // Attach Parts
      subParam = new dtGame::GroupMessageParameter( "AttachedPartMessageParam0" );
      subParam->AddValue( "DISInfo", std::string("3.4.225.6.8.9.4" ) );
      subParam->AddValue( "Station", 1U );
      subParam->AddValue( "OurParent", std::string("Parent_DOF2") );
      subParam->AddValue( "Change", (unsigned short)(123) );
      articArray->AddParameter( *subParam );

      const dtCore::UniqueId& fakeActorId = testMsg->GetAboutActorId();

      dtHLAGM::EntityIdentifier entityId(3,3,2);

      //insert a bogus mapping to see if the object maps properly.
      mHLAComponent->GetRuntimeMappings().Put(entityId, fakeActorId);


      dtCore::RefPtr<const dtDAL::ActorType> testTankType = mGameManager->FindActorType(testMsg->GetActorTypeCategory(), testMsg->GetActorTypeName());

      CPPUNIT_ASSERT(testTankType.valid());

      const dtHLAGM::ObjectToActor* oToA = mHLAComponent->GetActorMapping(*testTankType);

      if (oToA != NULL)
      {
         mHLAComponent->TestPrepareUpdate(*testMsg, *ahs, *oToA, true);
         const std::string* rtiID = mHLAComponent->GetRuntimeMappings().GetRTIId(fakeActorId);
         CPPUNIT_ASSERT_MESSAGE("The RTI Object ID  string should be set when an object is first sent out via HLA",
               rtiID != NULL);


         // Ensure that special incoming-only parameters are not being sent out.
         // The Object Mapping Name and Entity Type should be avoided.
         const dtGame::MessageParameter* paramMappingName = testMsg->GetParameter( "Object Mapping Name" );
         const dtGame::MessageParameter* paramEntityType = testMsg->GetParameter( "Entity Type As String" );
         if (paramMappingName != NULL || paramEntityType != NULL)
         {
            std::ostringstream oss;
            oss << "Outgoing Actor Updates should NOT contain outgoing parameter \""
               << ( paramMappingName != NULL ? paramMappingName->GetName().c_str() : paramEntityType->GetName().c_str() )
               << "\". This parameter has been mapped to a special incoming-only HLA type." << std::endl;
            CPPUNIT_ASSERT_MESSAGE( oss.str(), false );
         }


         bool foundEntityTypeAttr = false;
         //There are two entity id's to be mapped because
         //one is coming from the sendingActorId and the other
         //is coming from the aboutActorId.  This is just
         //part of the test.  It makes absolutely no sense and wouldn't
         //work if you did this at runtime.
         bool foundEntityIdAttr1 = false;
         bool foundEntityIdAttr2 = false;
         bool foundOrientationAttr = false;
         bool foundArrayAttr = false;
         bool foundDamageStateAttr = false;

         for (unsigned i = 0; i < ahs->size(); ++i)
         {
            RTI::AttributeHandle attrHandle = ahs->getHandle(i);

            if (attrHandle == oToA->GetEntityTypeAttributeHandle())
            {
               foundEntityTypeAttr = true;
               unsigned long length;
               char* buffer = ahs->getValuePointer(i, length);
               CPPUNIT_ASSERT_MESSAGE("The mapped parameter for the DISID should be of the proper length.",
                  length == oToA->GetEntityType()->EncodedLength());

               dtHLAGM::EntityType actual;
               actual.Decode(buffer);

               CPPUNIT_ASSERT_MESSAGE("The encoded entity type should match the value.",
                  *oToA->GetEntityType() == actual);
            }
            if (attrHandle == oToA->GetEntityIdAttributeHandle())
            {
               if (!foundEntityIdAttr1)
                  foundEntityIdAttr1 = true;
               else
               {
                  foundEntityIdAttr2 = true;
               }

               unsigned long length;
               char* buffer = ahs->getValuePointer(i, length);
               CPPUNIT_ASSERT_MESSAGE("The mapped parameter for the about actor id should be the length of an entity Id.",
                  length == entityId.EncodedLength());

               dtHLAGM::EntityIdentifier actual;
               actual.Decode(buffer);

               CPPUNIT_ASSERT_MESSAGE("The encoded entity id should match the test value.",
                  entityId == actual);
            }
            else
            {
               for (unsigned j = 0; j < oToA->GetOneToManyMappingVector().size(); ++j)
               {
                  const dtHLAGM::AttributeToPropertyList& aToPList = oToA->GetOneToManyMappingVector()[j];

                  const dtHLAGM::OneToManyMapping::ParameterDefinition& paramDef
                     = aToPList.GetParameterDefinitions()[0];

                  if (aToPList.GetAttributeHandle() == attrHandle)
                  {

                     if (paramDef.GetGameName() == "Damage State")
                     {
                        foundDamageStateAttr = true;
                        unsigned long length;
                        char* buffer = ahs->getValuePointer(i, length);
                        CPPUNIT_ASSERT_MESSAGE("The mapped parameter for the damage state should be the size of an unsigned int.",
                           length == aToPList.GetHLAType().GetEncodedLength() && length == sizeof(unsigned));

                        unsigned actual = *((unsigned*)buffer);
                        if (osg::getCpuByteOrder() == osg::LittleEndian)
                           osg::swapBytes((char*)&actual, sizeof(unsigned));

                        CPPUNIT_ASSERT_EQUAL_MESSAGE("The damage state value should be 3 (Destroyed)", unsigned(3), actual);

                     }
                     else if (dtDAL::TransformableActorProxy::PROPERTY_ROTATION == paramDef.GetGameName())
                     {
                        foundOrientationAttr = true;
                        unsigned long length;
                        //I just want the length.
                        ahs->getValuePointer(i, length);
                        CPPUNIT_ASSERT_MESSAGE("The mapped parameter for the orientation should be the size of three floats.",
                           length == aToPList.GetHLAType().GetEncodedLength() && length == 3 * sizeof(float));
                        //There are other tests that check the converter for rotation.
                     }
                     else if ("Articulated Parameters Array" == paramDef.GetGameName())
                     {
                        foundArrayAttr = true;
                        unsigned long length;
                        //I just want the length.
                        ahs->getValuePointer(i, length);
                        dtHLAGM::ArticulatedParameter artParam;
                        CPPUNIT_ASSERT_MESSAGE("The mapped parameter for the articulated parameters should be the size of three articulated parameters.",
                           length == 3 * aToPList.GetHLAType().GetEncodedLength() && length == 3 * artParam.EncodedLength());
                        //There are other tests that check the converter for rotation.
                     }
                     else if (dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION == paramDef.GetGameName())
                     {
                        CPPUNIT_FAIL("The world coordinate should not have ended up in the output.  It doesn't have a default value.");
                     }
                  }
               }
            }
         }

         delete ahs;
         ahs = NULL;

         CPPUNIT_ASSERT_MESSAGE("The entity id attribute based on the aboutActorId should have been found.",
            foundEntityIdAttr1);
         CPPUNIT_ASSERT_MESSAGE("The entity id attribute based on the sendingActorId should have been found.",
            foundEntityIdAttr2);
         CPPUNIT_ASSERT(foundEntityTypeAttr);
         CPPUNIT_ASSERT(foundDamageStateAttr);
         CPPUNIT_ASSERT(foundOrientationAttr);
      }
      else
      {
         delete ahs;
         ahs = NULL;

         CPPUNIT_FAIL("No object to actor mapping was found for actor type TestHLA.Tank.");
      }
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }

}


void HLAComponentTests::TestPrepareInteraction()
{
   try
   {
      RTI::ParameterHandleValuePairSet* phs =
         RTI::ParameterSetFactory::create(4);

      dtCore::RefPtr<dtGame::TimerElapsedMessage> testMsg;
      mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_TIMER_ELAPSED, testMsg);

      dtHLAGM::EntityIdentifier entityId(3,3,2);
      dtCore::UniqueId fakeActorId;

      float testFloat = 0.0388f;

      //insert a bogus mapping to see if the interaction maps properly.
      mHLAComponent->GetRuntimeMappings().Put(entityId, fakeActorId);

      testMsg->SetAboutActorId(fakeActorId);
      testMsg->SetLateTime(testFloat);

      const dtHLAGM::InteractionToMessage* iToM = mHLAComponent->GetMessageMapping(dtGame::MessageType::INFO_TIMER_ELAPSED);

      if (iToM != NULL)
      {
         mHLAComponent->TestPrepareInteraction(*testMsg, *phs, *iToM);


         // Ensure that the interaction does not contain a parameter for the special
         // incoming-only parameter, Mapping Name.
         CPPUNIT_ASSERT_MESSAGE("Outgoing interactions should NOT be sending out the special incoming-only Mapping Name parameter.",
            testMsg->GetParameter("Mapping Name") == NULL );


         bool foundEntityIdParameter = false;
         bool foundLateTimeParameter = false;

         for (unsigned i = 0; i < phs->size(); ++i)
         {
            RTI::ParameterHandle paramHandle = phs->getHandle(i);
            for (unsigned j = 0; j < iToM->GetOneToManyMappingVector().size(); ++j)
            {
               const dtHLAGM::ParameterToParameterList& pToPList = iToM->GetOneToManyMappingVector()[j];
               if (pToPList.GetParameterHandle() == paramHandle)
               {
                  if (pToPList.GetParameterDefinitions()[0].GetGameName() == dtHLAGM::HLAComponent::ABOUT_ACTOR_ID)
                  {
                     foundEntityIdParameter = true;
                     unsigned long length;
                     char* buffer = phs->getValuePointer(i, length);
                     CPPUNIT_ASSERT_MESSAGE("The mapped parameter for the actor id should be the length of an entity Id.",
                        length == entityId.EncodedLength());

                     dtHLAGM::EntityIdentifier actual;
                     actual.Decode(buffer);

                     CPPUNIT_ASSERT_MESSAGE("The encoded entity id should match the value set.", entityId == actual);
                  }
                  else if (pToPList.GetParameterDefinitions()[0].GetGameName() == "LateTime")
                  {
                     foundLateTimeParameter = true;
                     unsigned long length;
                     char* buffer = phs->getValuePointer(i, length);
                     CPPUNIT_ASSERT_MESSAGE("The mapped parameter for the late time should be the size of float.",
                        length == pToPList.GetHLAType().GetEncodedLength() && length == sizeof(float));

                     float actual = *((float*)buffer);
                     if (osg::getCpuByteOrder() == osg::LittleEndian)
                        osg::swapBytes((char*)&actual, sizeof(float));

                     CPPUNIT_ASSERT_EQUAL_MESSAGE("The encoded float should match the value from the message.", testFloat, actual);

                  }
               }
            }
         }

         delete phs;
         phs = NULL;

         CPPUNIT_ASSERT(foundEntityIdParameter);
         CPPUNIT_ASSERT(foundLateTimeParameter);
      }
      else
      {
         delete phs;
         phs = NULL;

         CPPUNIT_FAIL("No interaction to message mapping was found for message INFO_TIMER_ELAPSED.");
      }
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }

}

void HLAComponentTests::TestReceiveInteraction()
{
   try
   {
      RTI::RTIambassador* rtiamb = mHLAComponent->GetRTIAmbassador();
      CPPUNIT_ASSERT(rtiamb != NULL);

      RTI::InteractionClassHandle classHandle1(0);//, classHandle2;
      try
      {
         classHandle1 = rtiamb->getInteractionClassHandle(
               "InteractionRoot.WeaponFire");
      }
      catch (RTI::NameNotFound &)
      {
         CPPUNIT_FAIL("Could not find Interaction Class Name");
      }
      catch (RTI::FederateNotExecutionMember &)
      {
         CPPUNIT_FAIL("Federate not Execution Member");
      }
      catch (RTI::ConcurrentAccessAttempted &)
      {
         CPPUNIT_FAIL("Concurrent Access Attempted");
      }
      catch (RTI::RTIinternalError &)
      {
         CPPUNIT_FAIL("RTIinternal Error");
      }

      RTI::ParameterHandleValuePairSet* phs =
            RTI::ParameterSetFactory::create(1);

      char encodedEntityIdentifier[6];

      dtHLAGM::EntityIdentifier entityId(3,3,1);
      entityId.Encode(encodedEntityIdentifier);

      dtCore::UniqueId fakeActorId;

      //insert a bogus mapping to see if the interaction maps properly.
      mHLAComponent->GetRuntimeMappings().Put(entityId, fakeActorId);

      AddParameter("FiringObjectIdentifier", classHandle1, *phs, encodedEntityIdentifier, 6);

      mHLAComponent->receiveInteraction(classHandle1, *phs, "");
      delete phs;
      phs = NULL;

      dtCore::System::GetInstance().Step();

      dtCore::RefPtr<const dtGame::Message> msg = mTestComponent->FindProcessMessageOfType(dtGame::MessageType::INFO_TIMER_ELAPSED);
      CPPUNIT_ASSERT(msg.valid());
      CPPUNIT_ASSERT(msg->GetAboutActorId() == fakeActorId);

      const dtHLAGM::InteractionToMessage* itom = mHLAComponent->GetMessageMapping(msg->GetMessageType());
      const std::string& interactionMappingName = itom->GetMappingName();

      // Test that the object name has been assigned to the update message if it
      // has been mapped.
      const dtGame::StringMessageParameter* mappingNameParam =
         dynamic_cast<const dtGame::StringMessageParameter*>(msg->GetParameter(TEST_PARAM_MAPPING_NAME));
      if( mappingNameParam != NULL )
      {
         CPPUNIT_ASSERT( mappingNameParam->GetValue() == interactionMappingName );
      }

   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}

void HLAComponentTests::TestGMLookup()
{
   try
   {
      CPPUNIT_ASSERT(mGameManager.valid());

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The default name should equal the constant",
         dtHLAGM::HLAComponent::DEFAULT_NAME, mHLAComponent->GetName());

      dtGame::GMComponent *component =
         mGameManager->GetComponentByName(dtHLAGM::HLAComponent::DEFAULT_NAME);

      CPPUNIT_ASSERT(component != NULL);
      CPPUNIT_ASSERT_MESSAGE("The component found should match the component added to the GM",
         component == mHLAComponent.get());
   }
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void HLAComponentTests::TestMessageProcessing()
{
   dtHLAGM::ObjectRuntimeMappingInfo& mappingInfo = mHLAComponent->GetRuntimeMappings();

   // Create some test mappings
   dtCore::UniqueId id1;
   std::string rtiid1 = "TestRTIID01";

   dtHLAGM::EntityIdentifier eid1(1,1,1);

   dtCore::RefPtr<dtHLAGM::ObjectToActor> ota1 = new dtHLAGM::ObjectToActor();
   ota1->SetObjectClassName("BaseEntity.PhysicalEntity.Platform.GroundVehicle");

   // Make sure the mappings start off empty
   CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle1) == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle1) == NULL);
   CPPUNIT_ASSERT(static_cast<const dtHLAGM::ObjectRuntimeMappingInfo*>(&mappingInfo)->GetObjectToActor(mObjectHandle1) == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetId(eid1) == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetEntityId(id1) == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetHandle(id1)  == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid1)  == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetRTIId(id1)  == NULL);

   // Create the mappings
   CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle1, id1));
   CPPUNIT_ASSERT(mappingInfo.Put(rtiid1, id1));
   CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle1, *ota1));
   CPPUNIT_ASSERT(mappingInfo.Put(eid1, id1));

   // Make sure the mappings are valid
   CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle1) != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle1) != NULL);
   CPPUNIT_ASSERT(static_cast<const dtHLAGM::ObjectRuntimeMappingInfo*>(&mappingInfo)->GetObjectToActor(mObjectHandle1) != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetId(eid1) != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetEntityId(id1) != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetHandle(id1)  != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid1)  != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetRTIId(id1)  != NULL);

   // Send a map loaded message
   dtCore::RefPtr<dtGame::MapMessage> msg;
   mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED, msg);
   mGameManager->SendMessage(*msg);
   dtCore::System::GetInstance().Step();


   // Make sure mappings have not been cleared
   CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle1) != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle1) != NULL);
   CPPUNIT_ASSERT(static_cast<const dtHLAGM::ObjectRuntimeMappingInfo*>(&mappingInfo)->GetObjectToActor(mObjectHandle1) != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetId(eid1) != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetEntityId(id1) != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetHandle(id1)  != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid1)  != NULL);
   CPPUNIT_ASSERT(mappingInfo.GetRTIId(id1)  != NULL);

   // Send a map unloaded message
   mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_UNLOADED, msg);
   mGameManager->SendMessage(*msg);
   dtCore::System::GetInstance().Step();

   // Make sure the mappings have been cleared
   CPPUNIT_ASSERT(mappingInfo.GetId(mObjectHandle1) == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetObjectToActor(mObjectHandle1) == NULL);
   CPPUNIT_ASSERT(static_cast<const dtHLAGM::ObjectRuntimeMappingInfo*>(&mappingInfo)->GetObjectToActor(mObjectHandle1) == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetId(eid1) == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetEntityId(id1) == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetHandle(id1)  == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetIdByRTIId(rtiid1)  == NULL);
   CPPUNIT_ASSERT(mappingInfo.GetRTIId(id1)  == NULL);

}
