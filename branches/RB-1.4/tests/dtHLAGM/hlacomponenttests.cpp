/* -*-c++-*-
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
 * @author Olen A. Bruce
 * @author David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <vector>
#include <string>
#include <osg/Endian>
#include <dtUtil/coordinates.h>
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/parametertoparameter.h>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/hlacomponentconfig.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/messagetype.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/basemessages.h>
#include <dtGame/actorupdatemessage.h>
#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtCore/transform.h>


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

class TestComponent: public dtGame::GMComponent
{
   public:
      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedProcessMessages()
      { return mReceivedProcessMessages; }
      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedDispatchNetworkMessages()
      { return mReceivedDispatchNetworkMessages; }

      virtual void ProcessMessage(const dtGame::Message& msg)
      {
         mReceivedProcessMessages.push_back(&msg);
      }
      virtual void DispatchNetworkMessage(const dtGame::Message& msg)
      {
         mReceivedDispatchNetworkMessages.push_back(&msg);
      }

      void reset()
      {
         mReceivedDispatchNetworkMessages.clear();
         mReceivedProcessMessages.clear();
      }

      dtCore::RefPtr<const dtGame::Message> FindProcessMessageOfType(const dtGame::MessageType& type)
      {
         for (unsigned i = 0; i < mReceivedProcessMessages.size(); ++i)
         {
            if (mReceivedProcessMessages[i]->GetMessageType() == type)
               return mReceivedProcessMessages[i];
         }
         return NULL;
      }
      dtCore::RefPtr<const dtGame::Message> FindDispatchNetworkMessageOfType(const dtGame::MessageType& type)
      {
         for (unsigned i = 0; i < mReceivedDispatchNetworkMessages.size(); ++i)
         {
            if (mReceivedDispatchNetworkMessages[i]->GetMessageType() == type)
               return mReceivedDispatchNetworkMessages[i];
         }
         return NULL;
      }
   private:
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedProcessMessages;
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedDispatchNetworkMessages;
};


class HLATests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(HLATests);

      //All the tests are run in one method
      //they all use the RTI, which takes a long time 
      //to start and stop it setup and teardown.
      CPPUNIT_TEST(RunAllTests);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void RunAllTests();

      void TestReflectAttributes();
      void TestReflectAttributesNoEntityType();
      void TestPrepareUpdate();
      void TestPrepareInteraction();
      void TestReceiveInteraction();
      void TestRuntimeMappingInfo();
      void TestSubscription();

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

      dtCore::RefPtr<dtGame::GameManager> mGameManager;
      dtCore::RefPtr<TestHLAComponent> mHLAComponent;
      dtCore::RefPtr<TestComponent> mTestComponent;

      RTI::ObjectClassHandle mClassHandle1, mClassHandle2;
      RTI::ObjectHandle mObjectHandle1, mObjectHandle2;

      static const std::string mTestGameActorLibrary;

      dtUtil::Log* logger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(HLATests);

const std::string HLATests::mTestGameActorLibrary = "testGameActorLibrary";

// Called implicitly by CPPUNIT when the app starts
void HLATests::setUp()
{
   try
   {
      dtDAL::Project::GetInstance().SetContext("data/ProjectContext");
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      std::string logName("HLATests");
      logger = &dtUtil::Log::GetInstance(logName);
      dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
      mGameManager = new dtGame::GameManager(*scene);
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);

      dtCore::RefPtr<dtGame::DefaultMessageProcessor> defMsgComp = new dtGame::DefaultMessageProcessor();
      mGameManager->AddComponent(*defMsgComp, dtGame::GameManager::ComponentPriority::HIGHEST);
      mTestComponent = new TestComponent();
      mGameManager->AddComponent(*mTestComponent, dtGame::GameManager::ComponentPriority::NORMAL);
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }

   mHLAComponent = new TestHLAComponent();

   try
   {
      mGameManager->AddComponent(*mHLAComponent, dtGame::GameManager::ComponentPriority::NORMAL);
      dtHLAGM::HLAComponentConfig config;
      config.LoadConfiguration(*mHLAComponent, "Federations/HLAMappingExample.xml");
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }

   try
   {
      const std::string fom = "RPR-FOM.fed";
      const std::string fedFile = dtCore::FindFileInPathList(fom);
      CPPUNIT_ASSERT_MESSAGE("Couldn't find \"" + fom +
                             "\", make sure you install the Delta3D data package and set the DELTA_DATA environment var.",
                             !fedFile.empty());
      CPPUNIT_ASSERT(mHLAComponent->GetRTIAmbassador() == NULL);
      mHLAComponent->JoinFederationExecution("hla", fedFile, "delta3d");
      CPPUNIT_ASSERT(mHLAComponent->GetRTIAmbassador() != NULL);
   }
   catch (const RTI::Exception& ex)
   {
      CPPUNIT_FAIL(std::string("Error joining federation : ") + ex._reason);
   }
   
}

// Called implicitly by CPPUNIT when the app terminates
void HLATests::tearDown()
{
   dtCore::System::GetInstance().Stop();
   if(mHLAComponent.valid())
   {
      mHLAComponent->LeaveFederationExecution();
      CPPUNIT_ASSERT(mHLAComponent->GetRTIAmbassador() == NULL);
   }
   if (mGameManager.valid())
   {
      mHLAComponent = NULL;
      mTestComponent = NULL;
      mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
      mGameManager = NULL;
   }
}

void HLATests::BetweenTestSetUp()
{
   RTI::RTIambassador* rtiamb = mHLAComponent->GetRTIAmbassador();
   CPPUNIT_ASSERT(rtiamb != NULL);

   try
   {
      mClassHandle1 = rtiamb->getObjectClassHandle(
         "BaseEntity.PhysicalEntity.Platform.GroundVehicle");
      mObjectHandle1 = rtiamb->registerObjectInstance(mClassHandle1,
                                                     "TestObject1");
      mClassHandle2 = rtiamb->getObjectClassHandle(
         "BaseEntity.PhysicalEntity.Platform.Aircraft");
      mObjectHandle2 = rtiamb->registerObjectInstance(mClassHandle2,
                                                     "TestObject2");
   }
   catch (RTI::NameNotFound &)
   {
      CPPUNIT_FAIL("Could not find Object Class Name");
   }
   catch (RTI::ObjectAlreadyRegistered &)
   {
      CPPUNIT_FAIL("Object was already registered!");
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

   mTestComponent->reset();
}

void HLATests::BetweenTestTearDown()
{
   mHLAComponent->GetRuntimeMappings().Clear();
   RTI::RTIambassador* rtiamb = mHLAComponent->GetRTIAmbassador();
   CPPUNIT_ASSERT(rtiamb != NULL);

   try
   {
      rtiamb->deleteObjectInstance(mObjectHandle1, "TestObject1");
      rtiamb->deleteObjectInstance(mObjectHandle2, "TestObject2");
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

void HLATests::RunAllTests()
{
   //This is a quick, read-only test.
   TestSubscription();

   BetweenTestSetUp();
   TestReflectAttributes();
   BetweenTestTearDown();
   
   BetweenTestSetUp();
   TestReflectAttributesNoEntityType();
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
}

void HLATests::TestRuntimeMappingInfo()
{
   dtHLAGM::ObjectRuntimeMappingInfo mappingInfo;
   try
   {

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

      //Fill the mapping again to make sure it can be cleared.
      CPPUNIT_ASSERT(mappingInfo.Put(eid1, id1));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle1, id1));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle1, *ota1));
      CPPUNIT_ASSERT(mappingInfo.Put(rtiid1, id1));
      CPPUNIT_ASSERT(mappingInfo.Put(eid2, id2));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle2, id2));
      CPPUNIT_ASSERT(mappingInfo.Put(mObjectHandle2, *ota2));
      CPPUNIT_ASSERT(mappingInfo.Put(rtiid2, id2));

      mappingInfo.Clear();

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

void HLATests::TestSubscription()
{
   std::string message("No Handle should be 0 after connecting to the RTI.");
   {
      std::vector<const dtHLAGM::ObjectToActor*> toFill;
      mHLAComponent->GetAllObjectToActorMappings(toFill);
      
      for (std::vector<const dtHLAGM::ObjectToActor*>::iterator i = toFill.begin();
         i != toFill.end(); ++i)
      {
         const dtHLAGM::ObjectToActor& ota = **i;
         
         CPPUNIT_ASSERT_MESSAGE(message, ota.GetObjectClassHandle() != 0);
         CPPUNIT_ASSERT_MESSAGE(message, ota.GetEntityIdAttributeHandle() != 0);
         CPPUNIT_ASSERT_MESSAGE(message, ota.GetDisIDAttributeHandle() != 0);
         
         for (std::vector<dtHLAGM::AttributeToPropertyList>::const_iterator j = ota.GetOneToManyMappingVector().begin();
            j != ota.GetOneToManyMappingVector().end(); ++j)
         {
            const dtHLAGM::AttributeToPropertyList& atpl = *j;
            CPPUNIT_ASSERT_MESSAGE(message, atpl.GetAttributeHandle() != 0);
         }
         
      }
   }

   {
      std::vector<const dtHLAGM::InteractionToMessage*> toFill;
      mHLAComponent->GetAllInteractionToMessageMappings(toFill);
      
      for (std::vector<const dtHLAGM::InteractionToMessage*>::iterator i = toFill.begin();
         i != toFill.end(); ++i)
      {
         const dtHLAGM::InteractionToMessage& itm = **i;
         
         CPPUNIT_ASSERT_MESSAGE(message, itm.GetInteractionClassHandle() != 0);
         
         for (std::vector<dtHLAGM::ParameterToParameterList>::const_iterator j = itm.GetOneToManyMappingVector().begin();
            j != itm.GetOneToManyMappingVector().end(); ++j)
         {
            const dtHLAGM::ParameterToParameterList& atpl = *j;
            CPPUNIT_ASSERT_MESSAGE(message, atpl.GetParameterHandle() != 0);
         }
         
      }
   }
}

void HLATests::TestReflectAttributesNoEntityType()
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

void HLATests::TestReflectAttributes()
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

      char encodedEntityType[8];

      dtHLAGM::EntityType entityType(1,1,222,2,4,6,0);

      entityType.Encode(encodedEntityType);

      AddAttribute("EntityType", mClassHandle1, *ahs, encodedEntityType,
                   entityType.EncodedLength());

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
      
      CPPUNIT_ASSERT(static_cast<const dtGame::ActorUpdateMessage&>(*msg).GetUpdateParameter("Mesh") != NULL);

      //check the entity id mapping.
      id = mHLAComponent->GetRuntimeMappings().GetId(entityId);
      CPPUNIT_ASSERT(id != NULL);

      dtCore::RefPtr<dtGame::GameActorProxy> proxy = mGameManager->FindGameActorById(*id);
      CPPUNIT_ASSERT(proxy.valid());

      osg::Vec3 expectedTranslation = mHLAComponent->GetCoordinateConverter().
         ConvertToLocalTranslation(osg::Vec3d(location.GetX(),
                                              location.GetY(),
                                              location.GetZ()));

      osg::Vec3 expectedRotation = mHLAComponent->GetCoordinateConverter().
         ConvertToLocalRotation(rotation.GetPsi(), rotation.GetTheta(), rotation.GetPhi());

      dtCore::Transform xform;
      proxy->GetGameActor().GetTransform(xform);
      const osg::Vec3& actualTranslation = xform.GetTranslation();
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
      dtDAL::ResourceDescriptor* rd = rap->GetValue();
      CPPUNIT_ASSERT_MESSAGE("the mesh resource should have been set.", rd != NULL);
      const std::string expectedMeshValue("StaticMeshes:articulation_test.ive");
      CPPUNIT_ASSERT_EQUAL(expectedMeshValue, rd->GetResourceIdentifier());

      //Clear the resource value.
      rap->SetValue(NULL);
      
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
         rap->GetValue() == NULL);

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

void HLATests::TestPrepareUpdate()
{
   try
   {
      RTI::AttributeHandleValuePairSet* ahs =
         RTI::AttributeSetFactory::create(4);
      
      dtCore::RefPtr<dtGame::ActorUpdateMessage> testMsg;
      mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED, testMsg);
      
      dtHLAGM::EntityIdentifier entityId(3,3,2);
      dtCore::UniqueId fakeActorId;

      //insert a bogus mapping to see if the interaction maps properly.
      mHLAComponent->GetRuntimeMappings().Put(entityId, fakeActorId);

      testMsg->SetSendingActorId(fakeActorId);
      testMsg->SetAboutActorId(fakeActorId);
      testMsg->SetName("Rubber Chicken");
      testMsg->SetActorTypeCategory("TestHLA");
      testMsg->SetActorTypeName("Tank");
      
      testMsg->AddUpdateParameter("Rotation", dtDAL::DataType::VEC3);
      
      dtCore::RefPtr<dtDAL::ActorType> testTankType = mGameManager->FindActorType(testMsg->GetActorTypeCategory(), testMsg->GetActorTypeName());
      
      CPPUNIT_ASSERT(testTankType.valid());
      
      const dtHLAGM::ObjectToActor* oToA = mHLAComponent->GetActorMapping(*testTankType);
      
      if (oToA != NULL)
      {
         mHLAComponent->TestPrepareUpdate(*testMsg, *ahs, *oToA, true);
         
         bool foundEntityTypeAttr = false;
         //There are two entity id's to be mapped because
         //one is coming from the sendingActorId and the other
         //is coming from the aboutActorId.  This is just
         //part of the test.  It makes absolutely no sense and wouldn't
         //work if you did this at runtime.
         bool foundEntityIdAttr1 = false;
         bool foundEntityIdAttr2 = false;
         bool foundOrientationAttr = false;
         bool foundDamageStateAttr = false;
         
         for (unsigned i = 0; i < ahs->size(); ++i)
         {
            RTI::AttributeHandle attrHandle = ahs->getHandle(i);
            
            if (attrHandle == oToA->GetDisIDAttributeHandle())
            {
               foundEntityTypeAttr = true;
               unsigned long length;
               char* buffer = ahs->getValuePointer(i, length);
               CPPUNIT_ASSERT_MESSAGE("The mapped parameter for the DISID should be of the proper length.", 
                  length == oToA->GetDisID()->EncodedLength());
                  
               dtHLAGM::EntityType actual;
               actual.Decode(buffer);
                        
               CPPUNIT_ASSERT_MESSAGE("The encoded entity type should match the value.",
                  *oToA->GetDisID() == actual);                  
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
                  if (aToPList.GetAttributeHandle() == attrHandle)
                  {
                     if (aToPList.GetParameterDefinitions()[0].GetGameName() == "Damage State")
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
                     else if (aToPList.GetParameterDefinitions()[0].GetGameName() == "Rotation")
                     {
                        foundOrientationAttr = true;                        
                        unsigned long length;
                        //I just want the length.
                        ahs->getValuePointer(i, length);
                        CPPUNIT_ASSERT_MESSAGE("The mapped parameter for the orientation should be the size of three floats.", 
                           length == aToPList.GetHLAType().GetEncodedLength() && length == 3 * sizeof(float));                        
                        //There are other tests that check the converter for rotation.
                     }                     
                     else if (aToPList.GetParameterDefinitions()[0].GetGameName() == "Translation")
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


void HLATests::TestPrepareInteraction()
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

void HLATests::TestReceiveInteraction()
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

   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}