/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2010, Alion Science and Technology Corporation
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
 * @author Eddie Johnson, David Guthrie, Curtiss Murphy
 */

#include <prefix/unittestprefix.h>
#include <dtGame/testcomponent.h>

#include <testGameActorLibrary/testplayer.h>
#include <testGameActorLibrary/testgameactorlibrary.h>
#include <testGameActorLibrary/testgameactor.h>

#include <dtABC/application.h>

#include <dtActors/engineactorregistry.h>
#include <dtActors/gamemeshactor.h>
#include <dtActors/playerstartactorproxy.h>
#include <dtActors/taskactorgameevent.h>

#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>

#include <dtCore/actortype.h>
#include <dtCore/datatype.h>
#include <dtCore/map.h>
#include <dtCore/project.h>
#include <dtCore/resourcedescriptor.h>

#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>
#include <dtGame/environmentactor.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/gameactor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gamemanager.inl>
#include <dtGame/gmsettings.h>
#include <dtGame/machineinfo.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>
#include <dtGame/cascadingdeleteactorcomponent.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/datastream.h>
#include <dtUtil/log.h>

#include "basegmtests.h"

#include <osg/Endian>
#include <osg/io_utils>
#include <osg/Math>

#include <cstdlib>
#include <iostream>

class GameManagerTests : public dtGame::BaseGMTestFixture
{
   CPPUNIT_TEST_SUITE(GameManagerTests);

        CPPUNIT_TEST(TestFindActorByType);
        CPPUNIT_TEST(TestFindActorByWrongType);
        CPPUNIT_TEST(TestFindActorByName);

        CPPUNIT_TEST(TestDataStream);

        CPPUNIT_TEST(TestApplicationMember);
        CPPUNIT_TEST(TestMachineInfo);

        CPPUNIT_TEST(TestActorSearching);
        CPPUNIT_TEST(TestAddActor);
        CPPUNIT_TEST(TestAddActorNullID);
        CPPUNIT_TEST(TestAddActorCrash);
        CPPUNIT_TEST(TestCascadedDelete);
        CPPUNIT_TEST(TestCreateRemoteActor);
        CPPUNIT_TEST(TestGameActorTreeAddDelete);
        CPPUNIT_TEST(TestGameActorTreeRemote);
        CPPUNIT_TEST(TestComplexScene);
        CPPUNIT_TEST(TestAddRemoveComponents);
        CPPUNIT_TEST(TestComponentPriority);
        CPPUNIT_TEST(TestFindActorById);
        CPPUNIT_TEST(TestFindGameActorById);
        CPPUNIT_TEST(TestPrototypeActors);
        CPPUNIT_TEST(TestGMShutdown);
        CPPUNIT_TEST(TestGMSettingsServerClientRoles);
        CPPUNIT_TEST(TestOpenCloseAdditionalMaps);

        CPPUNIT_TEST(TestBatchAdd);
        CPPUNIT_TEST(TestBatchAddWithAddActor);

        CPPUNIT_TEST(TestTimers);
        CPPUNIT_TEST(TestTimersGetDeleted);

        CPPUNIT_TEST(TestIfOnAddedToGMIsCalled);
        CPPUNIT_TEST(TestIfGMSendsRestartedMessage);
        CPPUNIT_TEST(TestSetProjectContext);

        CPPUNIT_TEST(TestSwitchToLocal);
        CPPUNIT_TEST(TestSwitchToRemote);
   CPPUNIT_TEST_SUITE_END();

public:

   void TestFindActorByType();
   void TestFindActorByWrongType();
   void TestFindActorByName();

   void TestDataStream();

   void TestApplicationMember();
   void TestMachineInfo();

   void TestActorSearching();
   void TestAddActor();
   void TestAddActorNullID();
   void TestAddActorCrash();
   void TestCascadedDelete();

   void TestCreateRemoteActor();
   void TestGameActorTreeAddDelete();
   void TestGameActorTreeRemote();
   void TestComplexScene();
   void TestAddRemoveComponents();
   void TestComponentPriority();
   void TestFindActorById();
   void TestFindGameActorById();
   void TestPrototypeActors();
   void TestGMShutdown();
   void TestGMSettingsServerClientRoles();
   void TestOpenCloseAdditionalMaps();

   void TestBatchAdd();
   void TestBatchAddWithAddActor();

   void TestTimers();
   void TestTimersGetDeleted();

   void TestIfOnAddedToGMIsCalled();
   void TestIfGMSendsRestartedMessage();
   void TestSetProjectContext();

   void TestSwitchToLocal();
   void TestSwitchToRemote();

private:

};


class TestOrderComponent: public dtGame::GMComponent
{
   public:
      TestOrderComponent()
      : dtGame::GMComponent("order")
      , mCountWhenReceived()
      , locked()
      {}

      virtual void ProcessMessage(const dtGame::Message& msg)
      {
         //only count the first message.
         if (!locked)
         {
            mCountWhenReceived = MessageCounter;
            MessageCounter++;
            locked = true;
         }
      }

      virtual void DispatchNetworkMessage(const dtGame::Message& msg)
      {
         ProcessMessage(msg);
      }

      void reset()
      {
         mCountWhenReceived = 0;
         locked = false;
      }

      static void resetCounter()
      {
         MessageCounter = 0;
      }

      int GetCountWhenReceived() const { return mCountWhenReceived; }
   private:
      static int MessageCounter;
      int mCountWhenReceived;
      bool locked;
};

int TestOrderComponent::MessageCounter(0);

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GameManagerTests);



/////////////////////////////////////////////////
void GameManagerTests::TestFindActorByType()
{
   const unsigned short int numProxies = 10;
   dtCore::ActorPtrVector proxies;
   for (unsigned short int i = 0; i < numProxies; ++i)
   {
      dtCore::RefPtr<dtActors::GameMeshActor> p;
      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, p);
      CPPUNIT_ASSERT(p != NULL);
      mGM->AddActor(*p, false, false);
   }

   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> testEventProxy;
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE, testEventProxy);
   CPPUNIT_ASSERT(testEventProxy != NULL);
   mGM->AddActor(*testEventProxy, false, false);

   dtActors::GameMeshActor* meshActorProxy = NULL;
   mGM->FindActorsByType(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, proxies);
   CPPUNIT_ASSERT_MESSAGE("The GameManager didn't find the actor by type", !proxies.empty());

   mGM->FindActorByType(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, meshActorProxy);
   CPPUNIT_ASSERT_MESSAGE("The GameManager didn't find the single actor type.", meshActorProxy != NULL);

   CPPUNIT_ASSERT_MESSAGE("The result should equal the first in the list", meshActorProxy == proxies[0]);

   for (size_t i = proxies.size() - 1; i >= 1; --i)
   {
      CPPUNIT_ASSERT_MESSAGE("The result should not equal any other proxies except the first", meshActorProxy != proxies[i]);
   }
}

/////////////////////////////////////////////////
void GameManagerTests::TestFindActorByWrongType()
{
   dtCore::RefPtr<dtActors::GameMeshActor> meshActorProxy;
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, meshActorProxy);
   CPPUNIT_ASSERT(meshActorProxy != NULL);
   mGM->AddActor(*meshActorProxy, false, false);

   const std::string eventName = "testEvent";
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> gameEventProxy;
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE, gameEventProxy);
   CPPUNIT_ASSERT(gameEventProxy != NULL);
   gameEventProxy->SetName(eventName);
   mGM->AddActor(*gameEventProxy, false, false);

   //should not be able to mix up variable type and queried actor types
   dtActors::TaskActorGameEventProxy* testEventProxy = NULL;
   mGM->FindActorByType(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, testEventProxy);
   CPPUNIT_ASSERT_MESSAGE("The GameManager returned back a proxy of the wrong type", testEventProxy == NULL);

   //should not be able to mix up variable type and queried actor types
   dtActors::GameMeshActor* testMeshProxy = NULL;
   mGM->FindActorByName(eventName, testMeshProxy);
   CPPUNIT_ASSERT_MESSAGE("GameManager returned back a proxy of the wrong type.", testMeshProxy == NULL);
}

/////////////////////////////////////////////////
void GameManagerTests::TestFindActorByName()
{
   const unsigned short int numProxies = 10;
   for (unsigned short int i = 0; i < numProxies; ++i)
   {
      dtCore::RefPtr<dtActors::GameMeshActor> p;
      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, p);
      CPPUNIT_ASSERT(p != NULL);
      mGM->AddActor(*p, false, false);
   }

   const std::string eventName = "testEvent";
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> gameEventProxy;
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE, gameEventProxy);
   CPPUNIT_ASSERT(gameEventProxy != NULL);
   gameEventProxy->SetName(eventName);
   mGM->AddActor(*gameEventProxy, false, false);

   dtCore::ActorPtrVector proxies;
   mGM->FindActorsByName(eventName, proxies);
   CPPUNIT_ASSERT_MESSAGE("GameManager failed to find the actor by name.", !proxies.empty());

   dtActors::TaskActorGameEventProxy* testGameEventProxy = NULL;
   mGM->FindActorByName(eventName, testGameEventProxy);
   CPPUNIT_ASSERT_MESSAGE("GameManager failed to find the single actor by name.", testGameEventProxy != NULL);

   CPPUNIT_ASSERT_MESSAGE("The result should equal the first in the list", testGameEventProxy == proxies[0]);

   for (size_t i = proxies.size() - 1; i >= 1; --i)
   {
      CPPUNIT_ASSERT_MESSAGE("The result should not equal any other proxies except the first", testGameEventProxy != proxies[i]);
   }
}

/////////////////////////////////////////////////
void GameManagerTests::TestPrototypeActors()
{
   //GameManager::CreateActor()
   dtCore::RefPtr<dtActors::GameMeshActor> toMakeAsAPrototype;
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, toMakeAsAPrototype);
   toMakeAsAPrototype->SetName("PrototypeActorProxy");

   dtCore::ActorPtrVector toFill;
   mGM->GetAllPrototypes(toFill);
   CPPUNIT_ASSERT_MESSAGE("GameManager shouldn't have had prototypes in it currently...", toFill.size() == 0);

   mGM->AddActorAsAPrototype(*toMakeAsAPrototype);

   mGM->GetAllPrototypes(toFill);
   CPPUNIT_ASSERT_MESSAGE("Tried adding a prototype into the game manager, but it surely didnt want to stay around.", toFill.size() != 0);

   toFill.clear();

   dtCore::BaseActorObject* prototypeToFill = mGM->FindPrototypeByID(toMakeAsAPrototype->GetId());
   CPPUNIT_ASSERT_MESSAGE("Tried finding a prototype that should be in the gm, but its not....",prototypeToFill != 0 );

   dtCore::RefPtr<dtCore::BaseActorObject> ourActualActor = mGM->CreateActorFromPrototype(toMakeAsAPrototype->GetId());

   CPPUNIT_ASSERT_MESSAGE("Tried cloning from a prototype, didn't work out too well...", ourActualActor != NULL);
   CPPUNIT_ASSERT_MESSAGE("Tried cloning from a prototype, didn't work out too well...", ourActualActor->GetName() == toMakeAsAPrototype->GetName());

   dtCore::RefPtr<dtActors::GameMeshActor> testFindPrototype;
   mGM->FindPrototypeByID(toMakeAsAPrototype->GetId(), testFindPrototype);
   CPPUNIT_ASSERT_MESSAGE("The prototype find by Id method should have been able to find the prototype",
      testFindPrototype.valid());
   CPPUNIT_ASSERT_MESSAGE("The prototype find by Id method should have found the correct prototype",
      testFindPrototype->GetId() == toMakeAsAPrototype->GetId());

   dtActors::GameMeshActor* testFindPrototypeByName = NULL;
   mGM->FindPrototypeByName(toMakeAsAPrototype->GetName(), testFindPrototypeByName);
   CPPUNIT_ASSERT_MESSAGE("The prototype find by name method should have been able to find the prototype",
      testFindPrototypeByName != NULL);
   CPPUNIT_ASSERT_MESSAGE("The prototype find by name method should have found the correct prototype",
      testFindPrototype->GetId() == testFindPrototypeByName->GetId());

   // set this so that when i creates an actor, I can see that it changes the value.
   toMakeAsAPrototype->SetInitialOwnership(dtGame::GameActorProxy::Ownership::PROTOTYPE);

   dtCore::RefPtr<dtActors::GameMeshActor> testCreatePrototype;
   mGM->CreateActorFromPrototype(toMakeAsAPrototype->GetId(), testCreatePrototype);
   CPPUNIT_ASSERT_MESSAGE("The prototyped method should have been able to create the prototype",
      testCreatePrototype.valid());
   mGM->AddActor(*testCreatePrototype, false, false);

   CPPUNIT_ASSERT_MESSAGE("I don't care what initial ownership it is changed to, that could change over time, but an actor created"
         "as a prototype should not continue to be marked as such once it is added to the world.",
         testCreatePrototype->GetInitialOwnership() != dtGame::GameActorProxy::Ownership::PROTOTYPE);
   mGM->DeleteActor(*testCreatePrototype);
   dtCore::System::GetInstance().Step(0.016f);
   CPPUNIT_ASSERT_EQUAL(1, testCreatePrototype->referenceCount());

   // set this so that when i creates an actor, I can see that the value is preserved.
   toMakeAsAPrototype->SetInitialOwnership(dtGame::GameActorProxy::Ownership::SERVER_PUBLISHED);
   testCreatePrototype = NULL;
   mGM->CreateActorFromPrototype(toMakeAsAPrototype->GetId(), testCreatePrototype);
   CPPUNIT_ASSERT_MESSAGE("The prototyped method should have been able to create the prototype",
      testCreatePrototype.valid());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The ownership should be the same since it was not PROTOTYPE",
      dtGame::GameActorProxy::Ownership::SERVER_PUBLISHED, testCreatePrototype->GetInitialOwnership());

}
/////////////////////////////////////////////////
void GameManagerTests::TestApplicationMember()
{
   dtCore::RefPtr<dtABC::Application> app = &mGM->GetApplication();

   app->SetConfigPropertyValue(dtGame::GameManager::CONFIG_STATISTICS_INTERVAL, "4");
   app->SetConfigPropertyValue(dtGame::GameManager::CONFIG_STATISTICS_TO_CONSOLE, "false");
   app->SetConfigPropertyValue(dtGame::GameManager::CONFIG_STATISTICS_OUTPUT_FILE, "testOut.txt");

   // Reset the app so we can test the config properties
   mGM->SetApplication(*app);

   CPPUNIT_ASSERT_EQUAL(mGM->GetStatisticsInterval(), 4);
   CPPUNIT_ASSERT(!mGM->GetStatisticsToConsole());
   CPPUNIT_ASSERT_EQUAL(mGM->GetStatisticsLogFilePath(), std::string("testOut.txt"));

}

/////////////////////////////////////////////////
void GameManagerTests::TestDataStream()
{
   dtUtil::DataStream ds;

   const bool           boolean1 =  false;
   const bool           boolean2 =  true;
   const unsigned char  uchar    =  'Q';
   const unsigned short ushor    =  25;
   const unsigned int   uint     =  1134;
   const short          shor     = -109;
   const float          flo      =  12.3456f;
   const double         dub      =  21.1234567890;
   const int            num      = -653927;
   const std::string    str      = "This is a test string";
   const osg::Vec2f      v2(1, 2);
   const osg::Vec3d      v3(1, 2, 3);
   const osg::Vec4f      v4(1, 2, 3, 4);
   const dtCore::UniqueId id;
   //unsigned int bufferSize    =  ds.GetBufferCapacity();

   try
   {
      bool           tempBool1  = true;
      bool           tempBool2  = false;
      unsigned char  tempUChar  = 'G';
      unsigned short tempUShort = 0;
      unsigned int   tempUInt   = 0;
      short          tempShort  = 0;
      float          tempFloat  = 0.0f;
      double         tempDouble = 0.0;
      int            tempInt    = 0;
      osg::Vec2f     tempVec2(0, 0);
      osg::Vec3d     tempVec3(0, 0, 0);
      osg::Vec4f     tempVec4(0, 0, 0, 0);
      std::string    tempStr;
      std::string    tempStr2;
      float          tempFloat2  = 0.0f;
      dtCore::UniqueId tempId("");

      std::string    longTestString;
      longTestString.reserve(2000);
      for (int i = 0; i < 2000; ++i)
      {
         longTestString.push_back( 'a' + std::string::value_type(i % 10) );
      }

      ds << boolean1;
      ds << boolean2;
      ds << dub;
      ds << num;
      ds << uint;
      ds << shor;
      ds << ushor;
      ds << uchar;
      ds << flo;
      ds << str;
      ds << id;
      ds << longTestString;
      //make sure something after a string can be read.
      ds << flo;

      ds >> tempBool1;
      ds >> tempBool2;
      ds >> tempDouble;
      ds >> tempInt;
      ds >> tempUInt;
      ds >> tempShort;
      ds >> tempUShort;
      ds >> tempUChar;
      ds >> tempFloat;
      ds >> tempStr;
      ds >> tempId;
      ds >> tempStr2;
      ds >> tempFloat2;

      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a bool out", tempBool1 == boolean1);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a bool out", tempBool2 == boolean2);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a double out", tempDouble == dub);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read an int out", tempInt == num);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read an unsigned int out", tempUInt == uint);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a short out", tempShort == shor);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read an unsigned short out", tempUShort == ushor);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read an unsigned char out", tempUChar == uchar);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a float out", tempFloat == flo);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a short string out", tempStr == str);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a unique id", tempId == id);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a long string out", tempStr2 == longTestString);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a float out after a string", tempFloat2 == flo);

      ds.Rewind();

      ds << v2;
      ds << v3;
      ds << v4;

      ds >> tempVec2;
      ds >> tempVec3;
      ds >> tempVec4;

      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a Vec2 out", tempVec2 == v2);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a Vec3 out", tempVec3 == v3);
      CPPUNIT_ASSERT_MESSAGE("DataStream should be able to read a Vec4 out", tempVec4 == v4);

      //This part of the data stream tests makes sure that little endian buffers can be read
      //no matter what platform the code is running on.  This is helpful so little endian
      //data files are supported in a cross platform manor.
      unsigned int numList[4];
      unsigned int numList2[4];
      numList[0] = 10;
      numList[1] = 20;
      numList[2] = 30;
      numList[3] = 40;

      //If we are on a big endian machine, we need swap the bytes in the test buffer
      //since we want the test buffer to always be in little endian.
      if (!ds.IsLittleEndian())
      {
         osg::swapBytes((char*)&numList[0], sizeof(unsigned int));
         osg::swapBytes((char*)&numList[1], sizeof(unsigned int));
         osg::swapBytes((char*)&numList[2], sizeof(unsigned int));
         osg::swapBytes((char*)&numList[3], sizeof(unsigned int));
      }

      dtUtil::DataStream ds2((char*)&numList[0], sizeof(unsigned int)*4, false);
      ds2.SetForceLittleEndian(true);
      ds2 >> numList2[0] >> numList2[1] >> numList2[2] >> numList2[3];

      //If we are on a big endian machine, make sure to swap the bytes back to big
      //endian format before we compare them.
      if (!ds.IsLittleEndian())
      {
         osg::swapBytes((char*)&numList[0], sizeof(unsigned int));
         osg::swapBytes((char*)&numList[1], sizeof(unsigned int));
         osg::swapBytes((char*)&numList[2], sizeof(unsigned int));
         osg::swapBytes((char*)&numList[3], sizeof(unsigned int));
      }

      CPPUNIT_ASSERT_MESSAGE("First element was not correct reading from datastream.",
         numList[0] == numList2[0]);
      CPPUNIT_ASSERT_MESSAGE("Second element was not correct reading from datastream.",
         numList[1] == numList2[1]);
      CPPUNIT_ASSERT_MESSAGE("Third element was not correct reading from datastream.",
         numList[2] == numList2[2]);
      CPPUNIT_ASSERT_MESSAGE("Fourth element was not correct reading from datastream.",
         numList[3] == numList2[3]);
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   //catch(const std::exception& e)
   //{
   //   CPPUNIT_FAIL(e.what());
   //}
}


/////////////////////////////////////////////////
void GameManagerTests::TestMachineInfo()
{
   dtCore::RefPtr<dtGame::MachineInfo> mp1 = new dtGame::MachineInfo("Bob");
   dtCore::RefPtr<dtGame::MachineInfo> mp2 = new dtGame::MachineInfo("Dan");
   dtGame::MachineInfo& m1 = *mp1;
   dtGame::MachineInfo& m2 = *mp2;
   dtCore::UniqueId id1, id2;

   m1.SetUniqueId(id1);
   m2.SetUniqueId(id1);
   CPPUNIT_ASSERT_MESSAGE("Machine infos should be equal", m1 == m2);
   m2.SetUniqueId(id2);
   CPPUNIT_ASSERT_MESSAGE("Machine infos should not be equal", m1 != m2);
   m2 = m1;
   CPPUNIT_ASSERT_MESSAGE("Machine infos should be equal", m1 == m2);
   m2.SetUniqueId(id2);
   CPPUNIT_ASSERT_MESSAGE("Machine infos should not be equal", m1 != m2);

   m2.SetName("Dan");
   CPPUNIT_ASSERT_MESSAGE("Machine info's names should not be equal", m1.GetName() != m2.GetName());
   m1.SetName("Dan");
   CPPUNIT_ASSERT_MESSAGE("Machine info's name should be set", m1.GetName() == "Dan");
   m1.SetHostName("Test Host");
   CPPUNIT_ASSERT_MESSAGE("Machine info's host name should be set", m1.GetHostName() == "Test Host");
   m1.SetIPAddress("127.0.0.1");
   CPPUNIT_ASSERT_MESSAGE("Machine info's ip address should be set", m1.GetIPAddress() == "127.0.0.1");
   m1.SetPing(1134);
   CPPUNIT_ASSERT_MESSAGE("Machine info's ping should be set", m1.GetPing() == 1134);
   m1.SetTimeStamp(53);
   CPPUNIT_ASSERT_MESSAGE("Machine info's time stamp should be set", m1.GetTimeStamp() == 53);

   m1 = m2;

   CPPUNIT_ASSERT_MESSAGE("After all the setting, m1 should now equal m2", m1 == m2);
}

/////////////////////////////////////////////////
void GameManagerTests::TestActorSearching()
{
   try
   {
      int numActorsSkipped = 0;
      dtGame::GameManager& gm = *mGM;

      dtCore::ActorTypeVec typeVec;

      gm.GetActorTypes(typeVec);

      for (unsigned int i = 0; i < typeVec.size() && i < 5; ++i)
      {
         // In order to keep the tests fasts, we skip the nasty slow ones.
         if (typeVec[i]->GetName() == dtActors::EngineActorRegistry::CLOUD_PLANE_ACTOR_TYPE->GetName() ||
            typeVec[i]->GetName() == dtActors::EngineActorRegistry::WEATHER_ENVIRONMENT_ACTOR_TYPE->GetName() ||
            typeVec[i]->GetName() == "Test Environment Actor")
         {
            ++numActorsSkipped;
            continue;
         }
         gm.AddActor(*gm.CreateActor(*typeVec[i]));
      }

      std::set<const dtCore::ActorType*> supportedTypes;
      gm.GetUsedActorTypes(supportedTypes);
      CPPUNIT_ASSERT_MESSAGE("The number of supported actor types should not be 0", !supportedTypes.empty());

      gm.DeleteAllActors(true);

      const unsigned int size = 5;

      dtCore::RefPtr<dtCore::BaseActorObject> proxies[size];

      for (unsigned i = 0; i < size; ++i)
      {
         proxies[i] = gm.CreateActor(const_cast<dtCore::ActorType&>(**supportedTypes.begin()));
         gm.AddActor(*proxies[i].get());
      }

      dtCore::ActorPtrVector supportedProxies;

      gm.FindActorsByType(**supportedTypes.begin(), supportedProxies);

      //unsigned t = supportedProxies.size();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The number of proxies found is not equal to the number that exist",
         supportedProxies.size(), size_t(size));


      dtCore::RefPtr<dtGame::GameActorProxy> gap = NULL;
      for (unsigned int i = 0; i < typeVec.size(); ++i)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> p = gm.CreateActor(*typeVec[i]);
         if (p->IsGameActor())
         {
            gap = dynamic_cast<dtGame::GameActorProxy*> (p.get());
            if (gap != NULL)
            {
               break;
            }
         }
      }

      CPPUNIT_ASSERT_MESSAGE("The test game actor library is loaded, the pointer should not be NULL", gap != NULL);

      gm.AddActor(*gap, false, false);

      CPPUNIT_ASSERT_MESSAGE("The GM should be able to find this game actor proxy by ID", gap == gm.FindGameActorById(gap->GetId()));
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
//   }
}

/////////////////////////////////////////////////
void GameManagerTests::TestAddRemoveComponents()
{
   dtGame::TestComponent* tc = NULL;
   mGM->GetComponentByName("TestComponent", tc);
   dtGame::DefaultMessageProcessor* dmc = NULL;
   mGM->GetComponentByName(dtGame::DefaultMessageProcessor::DEFAULT_NAME, dmc);

   CPPUNIT_ASSERT(tc != NULL);
   CPPUNIT_ASSERT(dmc != NULL);

   mGM->RemoveComponent(*tc);
   mGM->RemoveComponent(*dmc);

   //check default names.
   CPPUNIT_ASSERT_EQUAL(tc->GetName(), std::string("TestComponent"));
   CPPUNIT_ASSERT_EQUAL(dmc->GetName(), dtGame::DefaultMessageProcessor::DEFAULT_NAME);

   tc->SetName("rulesComp");
   dmc->SetName("defaultComp");

   //check default names.
   CPPUNIT_ASSERT(tc->GetName() == "rulesComp");
   CPPUNIT_ASSERT(dmc->GetName() == "defaultComp");

   CPPUNIT_ASSERT(!tc->GetId().ToString().empty());
   CPPUNIT_ASSERT(!dmc->GetId().ToString().empty());

   CPPUNIT_ASSERT(tc->GetGameManager() == NULL);
   CPPUNIT_ASSERT(dmc->GetGameManager() == NULL);

   CPPUNIT_ASSERT(tc->GetComponentPriority() == dtGame::GameManager::ComponentPriority::NORMAL);
   CPPUNIT_ASSERT(dmc->GetComponentPriority() == dtGame::GameManager::ComponentPriority::HIGHEST);

   dtGame::GMComponent* comp = mGM->GetComponentByName("defaultComp");
   CPPUNIT_ASSERT_MESSAGE("No components have been added, the return value should be NULL", comp == NULL);

   mGM->AddComponent(*tc, dtGame::GameManager::ComponentPriority::LOWER);

   try
   {
      dtCore::RefPtr<dtGame::GMComponent> whackComp = new dtGame::GMComponent("rulesComp");
      mGM->AddComponent(*whackComp, dtGame::GameManager::ComponentPriority::NORMAL);
      CPPUNIT_FAIL("A component with the name rulesComp was already added. An exception wasn't thrown when it should  have been.");
   }
   catch(const dtUtil::Exception&)
   {
      // Correct
   }


   CPPUNIT_ASSERT(tc->GetGameManager() == mGM.get());
   CPPUNIT_ASSERT(tc->GetComponentPriority() == dtGame::GameManager::ComponentPriority::LOWER);

   //test regular get all
   std::vector<dtGame::GMComponent*> toFill;
   mGM->GetAllComponents(toFill);
   CPPUNIT_ASSERT_MESSAGE("There should be exactly one component in the GameManager.",toFill.size() == 1);
   CPPUNIT_ASSERT_MESSAGE("The one component should be the rules component.", toFill[0] == tc);

   comp = mGM->GetComponentByName("rulesComp");
   CPPUNIT_ASSERT_MESSAGE("Finding the rules component by name should return the correct pointer", comp == tc);

   mGM->AddComponent(*dmc, dtGame::GameManager::ComponentPriority::HIGHER);
   CPPUNIT_ASSERT(dmc->GetComponentPriority() == dtGame::GameManager::ComponentPriority::HIGHER);

   comp = mGM->GetComponentByName("defaultComp");
   CPPUNIT_ASSERT_MESSAGE("Finding the default component by name should return the correct pointer", comp == dmc);

   comp = mGM->GetComponentByName("whack");
   CPPUNIT_ASSERT_MESSAGE("No component named whack was added. The pointer should be NULL", comp == NULL);

   //test const get all
   std::vector<const dtGame::GMComponent*> toFill2;
   mGM->GetAllComponents(toFill2);
   CPPUNIT_ASSERT_MESSAGE("There should be exactly 2 components in the GameManager.",toFill2.size() == 2);
   CPPUNIT_ASSERT_MESSAGE("The first component should be the default message processor, it has a higher priority.", toFill2[0] == dmc);
   CPPUNIT_ASSERT_MESSAGE("The second component should be the rules component.", toFill2[1] == tc);

   CPPUNIT_ASSERT(tc->GetGameManager() == mGM.get());
   CPPUNIT_ASSERT(dmc->GetGameManager() == mGM.get());

   mGM->RemoveComponent(*tc);

   mGM->GetAllComponents(toFill);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be exactly one component in the GameManager.", size_t(1), toFill.size());
   CPPUNIT_ASSERT_MESSAGE("The one component should be the default message processor.", toFill[0] == dmc);

   CPPUNIT_ASSERT(tc->GetGameManager() == NULL);
   CPPUNIT_ASSERT(dmc->GetGameManager() == mGM.get());

   //make sure the method doesn't crash when it's the last one holding onto the component.
   dtGame::DefaultMessageProcessor& dmcRef = *dmc;
   dmc = NULL;
   mGM->RemoveComponent(dmcRef);
   //the pointers are all deleted, so we can't really test anything else on that component.

   //test regular get-all clears the list.
   mGM->GetAllComponents(toFill);
   CPPUNIT_ASSERT_MESSAGE("There should be 0 components in the GameManager.",toFill.size() == 0);

   //test const get-all clears the list.
   mGM->GetAllComponents(toFill2);
   CPPUNIT_ASSERT_MESSAGE("There should be 0 components in the GameManager.",toFill2.size() == 0);
}

/////////////////////////////////////////////////
void GameManagerTests::TestComponentPriority()
{
   mGM->RemoveComponent(*mTestComp);
   mGM->RemoveComponent(*mDefMsgProc);

   std::vector<dtCore::RefPtr<TestOrderComponent> > tocList;

   for (unsigned i = 0; i < 10; ++i)
   {
      std::ostringstream ss;
      ss << "testOrder" << i;
      tocList.push_back(new TestOrderComponent());
      tocList[i]->SetName(ss.str());

      CPPUNIT_ASSERT(tocList[i]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::NORMAL);
   }

   mGM->AddComponent(*tocList[0], dtGame::GameManager::ComponentPriority::NORMAL);
   mGM->AddComponent(*tocList[1], dtGame::GameManager::ComponentPriority::LOWER);
   mGM->AddComponent(*tocList[2], dtGame::GameManager::ComponentPriority::HIGHER);
   mGM->AddComponent(*tocList[3], dtGame::GameManager::ComponentPriority::HIGHEST);
   mGM->AddComponent(*tocList[4], dtGame::GameManager::ComponentPriority::NORMAL);
   mGM->AddComponent(*tocList[5], dtGame::GameManager::ComponentPriority::LOWEST);
   mGM->AddComponent(*tocList[6], dtGame::GameManager::ComponentPriority::LOWER);
   mGM->AddComponent(*tocList[7], dtGame::GameManager::ComponentPriority::LOWEST);
   mGM->AddComponent(*tocList[8], dtGame::GameManager::ComponentPriority::HIGHEST);
   mGM->AddComponent(*tocList[9], dtGame::GameManager::ComponentPriority::HIGHER);

   CPPUNIT_ASSERT(tocList[0]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::NORMAL);
   CPPUNIT_ASSERT(tocList[1]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::LOWER);
   CPPUNIT_ASSERT(tocList[2]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::HIGHER);
   CPPUNIT_ASSERT(tocList[3]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::HIGHEST);
   CPPUNIT_ASSERT(tocList[4]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::NORMAL);
   CPPUNIT_ASSERT(tocList[5]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::LOWEST);
   CPPUNIT_ASSERT(tocList[6]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::LOWER);
   CPPUNIT_ASSERT(tocList[7]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::LOWEST);
   CPPUNIT_ASSERT(tocList[8]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::HIGHEST);
   CPPUNIT_ASSERT(tocList[9]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::HIGHER);

   unsigned order[10] = { 3, 8, 2, 9, 0, 4, 1, 6, 5, 7 };

   //test const get all
   std::vector<const dtGame::GMComponent*> toFill;
   mGM->GetAllComponents(toFill);
   CPPUNIT_ASSERT_MESSAGE("There should be exactly 10 components in the GameManager.",toFill.size() == 10);
   for (unsigned i = 0; i < 10; ++i)
   {
      std::ostringstream ssName;
      ssName << "testOrder" << order[i];

      std::ostringstream ss;
      ss << "component \"" << i << "\" should be \"" << ssName.str() << "\" but it is " << toFill[i]->GetName();

      CPPUNIT_ASSERT_MESSAGE(ss.str(), toFill[i] == tocList[order[i]].get());

      tocList[order[i]]->reset();
   }

   dtCore::System::GetInstance().Step(0.016f);

   //make sure the message are received in order
   //here we are using the tick message as the test message.
   for (unsigned i = 0; i < 10; ++i)
   {
      std::ostringstream ss;
      ss << "Test order component number \"" << order[i] << "\" should have been number \"" << i
         << "\" to receive the message, but it was \"" << tocList[order[i]]->GetCountWhenReceived() << ".\"" ;
      CPPUNIT_ASSERT_MESSAGE(ss.str(), tocList[order[i]]->GetCountWhenReceived() == int(i));
   }

}

/////////////////////////////////////////////////
void GameManagerTests::TestCreateRemoteActor()
{
   dtCore::RefPtr<const dtCore::ActorType> type = mGM->FindActorType("dtActors", "LinkedPoints");
   dtCore::RefPtr<const dtCore::ActorType> gameActorType = mGM->FindActorType("ExampleActors","Test1Actor");

   //sanity check.
   CPPUNIT_ASSERT(type.valid());
   CPPUNIT_ASSERT(gameActorType.valid());

   CPPUNIT_ASSERT_THROW(mGM->CreateRemoteActor(*type), dtUtil::Exception);

   dtCore::RefPtr<dtGame::GameActorProxy> actor;
   CPPUNIT_ASSERT_NO_THROW(actor = mGM->CreateRemoteActor(*gameActorType));
   CPPUNIT_ASSERT_MESSAGE("Actor should have a valid GM on it", actor->GetGameManager() != NULL);
   CPPUNIT_ASSERT_MESSAGE("The proxy created as remote should not be NULL.", actor.valid());
   CPPUNIT_ASSERT_MESSAGE("The proxy created as remote should have a valid actor.", actor->GetDrawable() != nullptr);
   CPPUNIT_ASSERT_MESSAGE("The proxy should already be remote.", actor->IsRemote());
}

//////////////////////////////////////////////////////////////////////////
void GameManagerTests::TestAddActorCrash()
{
   dtCore::RefPtr<const dtCore::ActorType> type = mGM->FindActorType("ExampleActors", "TestCrash");
   CPPUNIT_ASSERT(type != NULL);
   dtCore::RefPtr<dtGame::GameActorProxy> proxy;
   mGM->CreateActor(*type, proxy);
   CPPUNIT_ASSERT_THROW_MESSAGE("Adding an actor as both remote and published should throw and exception.",
         mGM->AddActor(*proxy, true, true), dtUtil::Exception);

   CPPUNIT_ASSERT(!proxy->IsInGM());

   dtCore::System::GetInstance().Step(0.016f);
   CPPUNIT_ASSERT(mGM->FindGameActorById(proxy->GetId()) == NULL);
}

//////////////////////////////////////////////////////////////////////////
void GameManagerTests::TestAddActorNullID()
{
   dtCore::RefPtr<const dtCore::ActorType> type = mGM->FindActorType("ExampleActors", "Test1Actor");
   CPPUNIT_ASSERT(type != NULL);
   dtCore::RefPtr<dtGame::GameActorProxy> proxy;
   mGM->CreateActor(*type, proxy);
   proxy->SetId(dtCore::UniqueId(""));
   CPPUNIT_ASSERT_THROW_MESSAGE("Adding an actor with a null id should throw an exception.",
         mGM->AddActor(*proxy, false, false), dtUtil::Exception);

   CPPUNIT_ASSERT(!proxy->IsInGM());

   dtCore::System::GetInstance().Step(0.016f);
   CPPUNIT_ASSERT(mGM->FindGameActorById(proxy->GetId()) == NULL);

   CPPUNIT_ASSERT_THROW_MESSAGE("Adding an actor with a null id should throw an exception.",
         mGM->AddActor(*proxy), dtUtil::Exception);

   CPPUNIT_ASSERT(!proxy->IsInGM());

   dtCore::System::GetInstance().Step(0.016f);
   CPPUNIT_ASSERT(mGM->FindActorById(proxy->GetId()) == NULL);
}

//////////////////////////////////////////////////////////////////////////
void GameManagerTests::TestAddActor()
{

   dtCore::RefPtr<const dtCore::ActorType> type = mGM->FindActorType("ExampleActors", "Test1Actor");
   for (int x = 0; x < 21; ++x)
   {
      if (x == 10)
      {
         type = mGM->FindActorType("ExampleActors", "Test2Actor");
      }

      CPPUNIT_ASSERT(type != NULL);
      dtCore::RefPtr<dtGame::GameActorProxy> proxy;
      mGM->CreateActor(*type, proxy);
      CPPUNIT_ASSERT_MESSAGE("Proxy should not be in the gm yet", proxy->IsInGM() != true);
      CPPUNIT_ASSERT_MESSAGE("Proxy should have a valid GM on it", proxy->GetGameManager() != NULL);
      CPPUNIT_ASSERT_MESSAGE("Proxy, the result of a dynamic_cast to dtGame::GameActorProxy, should not be NULL",
            proxy != NULL);
      CPPUNIT_ASSERT_MESSAGE("IsGameActorProxy should return true", proxy->IsGameActor());

      mGM->CreateActor(type->GetCategory(), type->GetName(), proxy);
      CPPUNIT_ASSERT_MESSAGE("Proxy, the result of a dynamic_cast to dtGame::GameActorProxy, should not be NULL",
            proxy != NULL);
      CPPUNIT_ASSERT_MESSAGE("IsGameActorProxy should return true", proxy->IsGameActor());

      if (x % 3 == 0)
      {
         CPPUNIT_ASSERT_MESSAGE("The proxy should have the GameManager pointer set to valid",
               proxy->GetGameManager() != NULL);

         mGM->AddActor(*proxy, false, false);

         dtCore::RefPtr<dtCore::BaseActorObject> proxyFound = mGM->FindActorById(proxy->GetId());
         CPPUNIT_ASSERT(proxyFound != NULL);
         CPPUNIT_ASSERT(proxyFound.get() == proxy.get());
         CPPUNIT_ASSERT_MESSAGE("The proxy should have the GameManager pointer set",
               proxy->GetGameManager() == mGM.get());
         dtCore::RefPtr<dtGame::GameActorProxy> gameProxyFound =
            mGM->FindGameActorById(proxy->GetId());

         CPPUNIT_ASSERT(gameProxyFound != NULL);
         CPPUNIT_ASSERT(gameProxyFound.get() == proxy.get());
         CPPUNIT_ASSERT_MESSAGE("Actor should not be remote.", !proxy->IsRemote());
         CPPUNIT_ASSERT_MESSAGE("Actor should not be published.", !proxy->IsPublished());

         CPPUNIT_ASSERT_MESSAGE("The actor should have been added to the scene.",
            mGM->GetScene().GetChildIndex(proxy->GetDrawable())
            != mGM->GetScene().GetNumberOfAddedDrawable());

         try
         {
            mGM->PublishActor(*proxy);
         }
         catch (const dtGame::ActorIsRemoteException&)
         {
            CPPUNIT_FAIL("Exception thrown saying the actor is remote, but the actor should not be remote.");
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(std::string("Unknown Exception thrown publishing an actor: ") + ex.What());
         }

         CPPUNIT_ASSERT_MESSAGE("Actor should not be remote.", !proxy->IsRemote());
         CPPUNIT_ASSERT_MESSAGE("Actor should be published.", proxy->IsPublished());

         mGM->DeleteActor(*proxy);

         bool testIsInGM = proxy->IsInGM();

         CPPUNIT_ASSERT_MESSAGE("The proxy should not be in the gm", testIsInGM != true);
         CPPUNIT_ASSERT_MESSAGE("The proxy should not be marked deleted.", proxy->IsDeleted());

         CPPUNIT_ASSERT_MESSAGE("The actor should still be in the scene.",
            mGM->GetScene().GetChildIndex(proxy->GetDrawable()) != mGM->GetScene().GetNumberOfAddedDrawable());

         CPPUNIT_ASSERT_MESSAGE("The proxy should still be in the game manager", mGM->FindGameActorById(proxy->GetId()) != NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should not have the GameManager pointer set to NULL", proxy->GetGameManager() != NULL);
         dtCore::System::GetInstance().Step(0.01666f);

         CPPUNIT_ASSERT_MESSAGE("The actor should not be in the scene.",
            mGM->GetScene().GetChildIndex(proxy->GetDrawable()) == mGM->GetScene().GetNumberOfAddedDrawable());

         CPPUNIT_ASSERT_MESSAGE("The proxy should not still be in the game manager", mGM->FindGameActorById(proxy->GetId()) == NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should have the GameManager pointer set to NULL", proxy->GetGameManager() == NULL);
      }
      else if (x % 3 == 1)
      {
         CPPUNIT_ASSERT_THROW_MESSAGE(
               "Adding an actor as both remote and published should throw and exception.",
               mGM->AddActor(*proxy, true, true), dtUtil::Exception);


         CPPUNIT_ASSERT_MESSAGE("An actor should not be added to the GM if it's both remote and published.", mGM->FindGameActorById(proxy->GetId()) == NULL);

         // Setting the game manager to NULL to verify that add actor makes sure it's set.
         proxy->SetGameManager(NULL);
         CPPUNIT_ASSERT_NO_THROW_MESSAGE("Adding an actor as ", mGM->AddActor(*proxy, true, false));

         CPPUNIT_ASSERT(proxy->GetGameManager() == mGM.get());

         dtCore::RefPtr<dtCore::BaseActorObject> proxyFound = mGM->FindActorById(proxy->GetId());
         CPPUNIT_ASSERT(proxyFound != NULL);
         CPPUNIT_ASSERT(proxyFound.get() == proxy.get());
         dtCore::RefPtr<dtGame::GameActorProxy> gameProxyFound = mGM->FindGameActorById(proxy->GetId());
         CPPUNIT_ASSERT(gameProxyFound != NULL);
         CPPUNIT_ASSERT(gameProxyFound.get() == proxy.get());
         CPPUNIT_ASSERT_MESSAGE("Actor should be remote.", proxy->IsRemote());
         CPPUNIT_ASSERT_MESSAGE("Actor should not be published.", !proxy->IsPublished());
         CPPUNIT_ASSERT_MESSAGE("The actor should have been added to the scene.",
            mGM->GetScene().GetChildIndex(proxy->GetDrawable()) != mGM->GetScene().GetNumberOfAddedDrawable());


         CPPUNIT_ASSERT_THROW_MESSAGE("An actor may not be published if it's remote.",
                                      mGM->PublishActor(*proxy), dtGame::ActorIsRemoteException);

         mGM->DeleteActor(*proxy);

         bool testIsInGM = proxy->IsInGM();

         CPPUNIT_ASSERT_MESSAGE("The proxy should not be in the gm", testIsInGM != true);

         CPPUNIT_ASSERT_MESSAGE("The proxy should still be in the game manager", mGM->FindGameActorById(proxy->GetId()) != NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should not have the GameManager pointer set to NULL", proxy->GetGameManager() != NULL);
         CPPUNIT_ASSERT_MESSAGE("The actor should still be in the scene.",
            mGM->GetScene().GetChildIndex(proxy->GetDrawable()) != mGM->GetScene().GetNumberOfAddedDrawable());
         //have to send a from event to make the actor get deleted
         dtCore::System::GetInstance().Step(0.01666f);

         CPPUNIT_ASSERT_MESSAGE("The actor should not still be in the scene.",
            mGM->GetScene().GetChildIndex(proxy->GetDrawable()) == mGM->GetScene().GetNumberOfAddedDrawable());
         CPPUNIT_ASSERT_MESSAGE("The proxy should not still be in the game manager", mGM->FindGameActorById(proxy->GetId()) == NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should have the GameManager pointer set to NULL", proxy->GetGameManager() == NULL);
      }
      else
      {

         try
         {
            mGM->AddActor(*proxy);
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(std::string("Unknown Exception thrown adding an actor: ") + ex.What());
         }
         dtCore::RefPtr<dtCore::BaseActorObject> proxyFound = mGM->FindActorById(proxy->GetId());
         CPPUNIT_ASSERT(proxyFound != NULL);
         CPPUNIT_ASSERT(proxyFound.get() == proxy.get());
         dtCore::RefPtr<dtGame::GameActorProxy> gameProxyFound = mGM->FindGameActorById(proxy->GetId());
         CPPUNIT_ASSERT_MESSAGE("The actor should have been added as a game actor", gameProxyFound->GetId() == proxy->GetId());
         CPPUNIT_ASSERT_MESSAGE("The actor should have been added to the scene.",
            mGM->GetScene().GetChildIndex(proxy->GetDrawable()) != mGM->GetScene().GetNumberOfAddedDrawable());


         CPPUNIT_ASSERT_NO_THROW_MESSAGE("An actor may be published because it should be detected as a game actor.",
                                       mGM->PublishActor(*proxy));

         mGM->DeleteActor(static_cast<dtCore::BaseActorObject&>(*proxy));
         CPPUNIT_ASSERT_MESSAGE("The proxy should still be in the game manager", mGM->FindActorById(proxy->GetId()) != NULL);
         dtCore::System::GetInstance().Step(0.01666f);
         CPPUNIT_ASSERT_MESSAGE("The proxy should not still be in the game manager", mGM->FindActorById(proxy->GetId()) == NULL);
         CPPUNIT_ASSERT_MESSAGE("The actor not should still be in the scene.",
            mGM->GetScene().GetChildIndex(proxy->GetDrawable()) == mGM->GetScene().GetNumberOfAddedDrawable());
      }
   }
}

/////////////////////////////////////////////////
// This tests actors deleting other actors AND it tests the cascading delete actor component.
void GameManagerTests::TestCascadedDelete()
{
   dtCore::RefPtr<dtGame::DefaultNetworkPublishingComponent> dnpc = new dtGame::DefaultNetworkPublishingComponent();

   mGM->AddComponent(*dnpc, dtGame::GameManager::ComponentPriority::NORMAL);

   std::vector< dtCore::RefPtr<dtGame::GameActorProxy> > actors;

   for (unsigned i = 0; i < 10; ++i)
   {
      dtCore::RefPtr<dtGame::GameActorProxy> newActor;
      mGM->CreateActor("ExampleActors", "Test1Actor", newActor);
      if (i > 0)
      {
         dtGame::CascadingDeleteActorComponent::Connect(*newActor, *actors.back());
      }
      actors.push_back(newActor);
   }

   CPPUNIT_ASSERT(actors.size() == 10);
   for (unsigned i = 0; i < actors.size(); ++i)
   {
      //Add game actors as published
      mGM->AddActor(*actors[i], false, true);
      CPPUNIT_ASSERT(actors[i]->IsPublished());
   }

   dtCore::System::GetInstance().Step(0.016f);

   // Deleting the last should delete all of them when I step once.
   mGM->DeleteActor(*actors.back());

   dtCore::System::GetInstance().Step(0.016f);

   std::vector< dtCore::RefPtr<dtGame::GameActorProxy> > backup = actors;

   std::vector<dtCore::RefPtr<const dtGame::Message> > messages = mTestComp->GetReceivedProcessMessages();
   for (unsigned m = 0 ; m < messages.size(); ++m)
   {
      if (messages[m]->GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
      {
         bool foundDeleteForOneActorThisIteration = false;
         for (unsigned a = 0; a < actors.size(); ++a)
         {
            if (actors[a]->GetId() == messages[m]->GetAboutActorId())
            {
               actors.erase(actors.begin() + a);
               foundDeleteForOneActorThisIteration = true;
            }
         }
         CPPUNIT_ASSERT(foundDeleteForOneActorThisIteration);
      }
   }
   // All should have been
   CPPUNIT_ASSERT_EQUAL_MESSAGE("All of the actors should have been accounted for with delete message in the main queue in the first tick "
                          "This means either not all actor got deleted, or not all messages got sent",
                          actors.size(), size_t(0));

   // Network messages get dispatched the next tick
   dtCore::System::GetInstance().Step(0.016f);

   // Get the actor list back for a second pass to look at network dispatch.
   actors = backup;

   messages = mTestComp->GetReceivedDispatchNetworkMessages();
   for (unsigned m = 0 ; m < messages.size(); ++m)
   {
      if (messages[m]->GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
      {
         bool foundDeleteForOneActorThisIteration = false;
         for (unsigned a = 0; a < actors.size(); ++a)
         {
            if (actors[a]->GetId() == messages[m]->GetAboutActorId())
            {
               actors.erase(actors.begin() + a);
               foundDeleteForOneActorThisIteration = true;
            }
         }
         CPPUNIT_ASSERT(foundDeleteForOneActorThisIteration);
      }
   }
   // All should have been
   CPPUNIT_ASSERT_EQUAL_MESSAGE("All of the actors should have been accounted for with delete messages in the network dispatch queue since they were all published "
                          "This means either not all actor got deleted, not all messages got processed, or actors were removed before their messages were processed "
                          "the first tick.",
                          size_t(0), actors.size());
}
/////////////////////////////////////////////////
void GameManagerTests::TestGameActorTreeAddDelete()
{
   std::vector< dtGame::GameActorPtr > actors;

   for (unsigned i = 0; i < 10; ++i)
   {
      dtGame::GameActorPtr newActor;
      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, newActor);
      if (i > 0)
         newActor->SetParentActor(actors[i-1]);
      actors.push_back(newActor);
   }

   mGM->AddActor(*actors[0]);
   CPPUNIT_ASSERT_EQUAL(size_t(10), mGM->GetNumGameActors());
   CPPUNIT_ASSERT_EQUAL(2U, mGM->GetScene().GetNumChildren());
   mGM->DeleteActor(*actors[5]);
   dtCore::System::GetInstance().Step(0.0167f);

   CPPUNIT_ASSERT_EQUAL(size_t(5), mGM->GetNumGameActors());
   CPPUNIT_ASSERT_EQUAL(2U, mGM->GetScene().GetNumChildren());
   for (unsigned i = 0; i < actors.size(); ++i)
   {
      CPPUNIT_ASSERT_EQUAL(i < 5, actors[i]->IsInGM());
      CPPUNIT_ASSERT_EQUAL(i >= 5, actors[i]->IsDeleted());
   }
}


/////////////////////////////////////////////////
void GameManagerTests::TestGameActorTreeRemote()
{
   std::vector< dtGame::GameActorPtr > actors;

   for (unsigned i = 0; i < 10; ++i)
   {
      dtGame::GameActorPtr newActor;
      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, newActor);
      if (i > 0)
         newActor->SetParentActor(actors[i-1]);
      actors.push_back(newActor);
   }

   mGM->AddActor(*actors[0], true, false);
   int count = 0;

   mGM->ForEachActor([&count](dtCore::BaseActorObject& actor)
      {
         ++count;
         CPPUNIT_ASSERT(actor.IsGameActor());
         CPPUNIT_ASSERT(static_cast<dtGame::GameActorProxy&>(actor).IsRemote());
      } , true);
   CPPUNIT_ASSERT_EQUAL(10, count);
}

/////////////////////////////////////////////////
void GameManagerTests::TestComplexScene()
{
   //aka RemoveActorFromScene
   std::vector< dtCore::RefPtr<dtGame::GameActorProxy> > actors;

   for (unsigned i = 0; i < 10; ++i)
   {
      dtCore::RefPtr<dtGame::GameActorProxy> newActor;
      mGM->CreateActor("ExampleActors", "Test1Actor", newActor);
      actors.push_back(newActor);
   }

   CPPUNIT_ASSERT(actors.size() == 10);
   for (unsigned i = 0; i < actors.size(); ++i)
   {
      CPPUNIT_ASSERT_MESSAGE("Proxy, the result of a dynamic_cast to dtGame::GameActorProxy, should not be NULL", actors[i] != NULL);
      //Add regular actors
      if (i < 5)
      {
         mGM->AddActor(*actors[i]);
      }
      else
      {
         //Add game actors
         mGM->AddActor(*actors[i], false, false);
      }

      CPPUNIT_ASSERT_MESSAGE("Actor should be in the scene.",
      mGM->GetScene().GetChildIndex(actors[i]->GetDrawable()) != mGM->GetScene().GetNumberOfAddedDrawable());

   }

   dtCore::Scene& scene = mGM->GetScene();

   scene.RemoveChild(actors[0]->GetDrawable());
   scene.RemoveChild(actors[1]->GetDrawable());
   scene.RemoveChild(actors[2]->GetDrawable());
   scene.RemoveChild(actors[6]->GetDrawable());
   scene.RemoveChild(actors[7]->GetDrawable());
   scene.RemoveChild(actors[8]->GetDrawable());
   scene.RemoveChild(actors[9]->GetDrawable());

   //Add the regular actor drawables as children to the first game actor drawable.
   actors[5]->GetDrawable()->AddChild(actors[0]->GetDrawable());
   actors[5]->GetDrawable()->AddChild(actors[1]->GetDrawable());
   actors[5]->GetDrawable()->AddChild(actors[2]->GetDrawable());

   //create a plain actor so we can make sure it doesn't get moved up.
   dtCore::RefPtr<dtCore::Transformable> tx = new dtCore::Transformable;

   //Add the game actors removed as a child to a regular actor.
   actors[0]->GetDrawable()->AddChild(actors[6]->GetDrawable());
   actors[0]->GetDrawable()->AddChild(actors[7]->GetDrawable());
   actors[0]->GetDrawable()->AddChild(tx.get());

   actors[1]->GetDrawable()->AddChild(actors[8]->GetDrawable());
   actors[1]->GetDrawable()->AddChild(actors[9]->GetDrawable());

   //remove proxy 0 to make it's children move up one.
   mGM->DeleteActor(*actors[0]);

   dtCore::System::GetInstance().Step(0.016f);

   //check current children.
   CPPUNIT_ASSERT_MESSAGE("proxy 0 should not be a child of proxy 5.",
      !actors[5]->GetDrawable()->HasChild(actors[0]->GetDrawable()));
   CPPUNIT_ASSERT_MESSAGE("proxy 1 should still be a child of proxy 5.",
      actors[5]->GetDrawable()->HasChild(actors[1]->GetDrawable()));
   CPPUNIT_ASSERT_MESSAGE("proxy 0 should not be in the scene.",
      actors[0]->GetDrawable()->GetSceneParent() == NULL);

   //check that old children of 0 were moved up when they are supposed to.
   CPPUNIT_ASSERT_MESSAGE("proxy 6 should now be a child of the scene.",
         actors[6]->GetDrawable()->GetParent() == actors[7]->GetDrawable()->GetSceneParent());
   CPPUNIT_ASSERT_MESSAGE("proxy 7 should now be a child of the scene.",
         actors[7]->GetDrawable()->GetParent() == actors[7]->GetDrawable()->GetSceneParent());
   CPPUNIT_ASSERT_MESSAGE("The physical actor should not be a child of anything.",
         tx->GetSceneParent() == NULL);

   //remove proxy 5 to make it's children move up one.
   mGM->DeleteActor(*actors[5]);

   dtCore::System::GetInstance().Step(0.016f);

   unsigned currentInScene[] = { 1, 2, 3, 4, 6, 7 };

   //check current children.
   CPPUNIT_ASSERT_MESSAGE("proxy 5 should not be in the scene.",
      actors[5]->GetDrawable()->GetSceneParent() == NULL);
   CPPUNIT_ASSERT_MESSAGE("proxy 0 should not be in the scene.",
      actors[0]->GetDrawable()->GetSceneParent() == NULL);

   for (int i = 0; i < 6; ++i)
   {
      std::ostringstream ss;
      ss << "proxy[" << i << "] should be in the root of the scene.";
      CPPUNIT_ASSERT_MESSAGE(ss.str(),
         scene.GetChildIndex(actors[currentInScene[i]]->GetDrawable()) != scene.GetNumberOfAddedDrawable());
   }

   //check that children of 1 are still that way.
   CPPUNIT_ASSERT_MESSAGE("proxy 8 should still be a child of proxy 1.",
      actors[1]->GetDrawable()->HasChild(actors[8]->GetDrawable()));
   CPPUNIT_ASSERT_MESSAGE("proxy 9 should still be a child of proxy 1.",
      actors[1]->GetDrawable()->HasChild(actors[9]->GetDrawable()));

   actors[1]->GetDrawable()->Emancipate();

   mGM->DeleteActor(*actors[1]);
   dtCore::System::GetInstance().Step(0.016);

   //check that children of 1 are still that way, and that the GM didn't crash or try to add these drawables as children of something else.
    CPPUNIT_ASSERT_MESSAGE("proxy 8 should still be a child of proxy 1.",
       actors[1]->GetDrawable()->HasChild(actors[8]->GetDrawable()));
    CPPUNIT_ASSERT_MESSAGE("proxy 9 should still be a child of proxy 1.",
       actors[1]->GetDrawable()->HasChild(actors[9]->GetDrawable()));
}

/////////////////////////////////////////////////
void GameManagerTests::TestIfOnAddedToGMIsCalled()
{
   // Remove the default one in the tests so we can add a different one.
   mGM->RemoveComponent(*mTestComp);

   dtCore::RefPtr<dtGame::TestComponent> tc = new dtGame::TestComponent;
   CPPUNIT_ASSERT_MESSAGE("OnAddedToGM should not be called until added to the GM.",
      !(tc->mWasOnAddedToGMCalled));

   mGM->AddComponent(*tc.get(), dtGame::GameManager::ComponentPriority::NORMAL);
   CPPUNIT_ASSERT_MESSAGE("OnAddedToGM should be called when added to the GM.",
      tc->mWasOnAddedToGMCalled);

   mGM->RemoveComponent(*tc);
}

/////////////////////////////////////////////////
void GameManagerTests::TestIfGMSendsRestartedMessage()
{
   dtCore::System::GetInstance().Step(0.016f);
   CPPUNIT_ASSERT_MESSAGE("GM should have sent RESTARTED message on startup.",
                           mTestComp->FindProcessMessageOfType(dtGame::MessageType::INFO_RESTARTED).valid());

}

/////////////////////////////////////////////////
void GameManagerTests::TestTimersGetDeleted()
{
   dtCore::RefPtr<dtGame::GameActorProxy> proxy;
   mGM->CreateActor("ExampleActors","Test2Actor", proxy);

   CPPUNIT_ASSERT_MESSAGE("Proxy, the result of a dynamic_cast to dtGame::GameActorProxy, should not be NULL", proxy != NULL);
   CPPUNIT_ASSERT_MESSAGE("IsGameActorProxy should return true", proxy->IsGameActor());


   mGM->AddActor(*proxy, false, false);

   mGM->SetTimer("SimTimer1", proxy.get(), 0.07f);
   mGM->SetTimer("RepeatingTimer1", proxy.get(), 0.07f, true, true);

   // step now to make sure the time for the next one is fast.
   dtCore::System::GetInstance().Step(0.016f);

   mGM->DeleteActor(*proxy);

   const dtCore::Timer_t expectedSimTime  = mGM->GetSimulationClockTime() + 70000;
   const dtCore::Timer_t expectedRealTime = mGM->GetRealClockTime()       + 70000;
   dtCore::Timer_t currentSimTime  = mGM->GetSimulationClockTime();
   dtCore::Timer_t currentRealTime = mGM->GetRealClockTime();

   // A quick step with no sleep to make sure the delete happens.
   dtCore::System::GetInstance().Step(0.016f);

   //this shouldn't ever need to run more than once, but sometimes windows doesn't sleep as long as it's supposed to.
   while (currentSimTime < expectedSimTime)
   {
      dtCore::System::GetInstance().Step(0.016f);

      currentSimTime  = mGM->GetSimulationClockTime();
      currentRealTime = mGM->GetRealClockTime();
   }

   std::ostringstream msg1;
   msg1 << "(" << currentSimTime << ") should be > than (" <<expectedSimTime << ")";
   CPPUNIT_ASSERT_MESSAGE(msg1.str(), currentSimTime  > expectedSimTime);

   std::ostringstream msg2;
   msg2 << "(" << currentRealTime << ") should be > than (" <<expectedRealTime << ")";
   CPPUNIT_ASSERT_MESSAGE(msg2.str(), currentRealTime > expectedRealTime);

   std::vector<dtCore::RefPtr<const dtGame::Message> > msgs = mTestComp->GetReceivedProcessMessages();

   for (unsigned int i = 0; i < msgs.size(); ++i)
   {
      if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
      {
         const dtGame::TimerElapsedMessage* tem = static_cast<const dtGame::TimerElapsedMessage*> (msgs[i].get());

         if (tem->GetTimerName() == "SimTimer1")
         {
            CPPUNIT_FAIL("SimTimer1 was not deleted");
         }
         else if (tem->GetTimerName() == "RepeatingTimer1")
         {
            CPPUNIT_FAIL("RepeatingTimer1 was not deleted");
         }
         else
         {
            CPPUNIT_FAIL("An unknown timer fired.");
         }
      }
   }
}

/////////////////////////////////////////////////
void GameManagerTests::TestTimers()
{
   dtCore::RefPtr<dtGame::GameActorProxy> proxy;
   mGM->CreateActor("ExampleActors","Test2Actor", proxy);

   CPPUNIT_ASSERT_MESSAGE("Proxy, the result of a dynamic_cast to dtGame::GameActorProxy, should not be NULL", proxy != NULL);
   CPPUNIT_ASSERT_MESSAGE("IsGameActorProxy should return true", proxy->IsGameActor());

   mGM->AddActor(*proxy, false, false);

   mGM->SetTimer("SimTimer1", proxy.get(), 0.001f);
   mGM->SetTimer("RepeatingTimer1", proxy.get(), 0.001f, true, true);

   const dtCore::Timer_t expectedSimTime  = mGM->GetSimulationClockTime() + 1000;
   const dtCore::Timer_t expectedRealTime = mGM->GetRealClockTime()       + 1000;
   dtCore::Timer_t currentSimTime  = mGM->GetSimulationClockTime();
   dtCore::Timer_t currentRealTime = mGM->GetRealClockTime();

   //this shouldn't ever need to run more than once, but sometimes windows doesn't sleep as long as it's supposed to.
   while (currentSimTime < expectedSimTime)
   {
      dtCore::System::GetInstance().Step(0.016f);

      currentSimTime  = mGM->GetSimulationClockTime();
      currentRealTime = mGM->GetRealClockTime();
   }

   std::ostringstream msg1;
   msg1 << "(" << currentSimTime << ") should be > than (" <<expectedSimTime << ")";
   CPPUNIT_ASSERT_MESSAGE(msg1.str(), currentSimTime  > expectedSimTime);

   std::ostringstream msg2;
   msg2 << "(" << currentRealTime << ") should be > than (" <<expectedRealTime << ")";
   CPPUNIT_ASSERT_MESSAGE(msg2.str(), currentRealTime > expectedRealTime);

   const dtCore::Timer_t lateSimTime  = currentSimTime  - expectedSimTime;
   const dtCore::Timer_t lateRealTime = currentRealTime - expectedRealTime;

   std::vector<dtCore::RefPtr<const dtGame::Message> > msgs = mTestComp->GetReceivedProcessMessages();
   bool foundTimeMsg = false;

   for (unsigned int i = 0; i < msgs.size(); ++i)
   {
      if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
      {
         const dtGame::TimerElapsedMessage* tem = static_cast<const dtGame::TimerElapsedMessage*> (msgs[i].get());

         if (tem->GetTimerName() == "SimTimer1")
         {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The late time should be reasonably close to the expected late time",
                                                 double(lateSimTime) * 1e-6f, tem->GetLateTime(), 1e-5f);
         }
           //TODO This is commented out because it occasionally fails on certain hardware.
           // When failing, the above test passes and the following fails because
           // "lateRealTime" is much larger than what tem->GetLateTime() is returning.
           // Not sure where the problem actually is.
         else if (tem->GetTimerName() == "RepeatingTimer1")
         {
            // We think we fixed the sleep timer on windows, so I put this test back
            // if it starts failing, it will have to be commented out again.
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The late time should be reasonably close to the expected late time",
                                                  double(lateRealTime) * 1e-6f, tem->GetLateTime(), 1e-5f);
         }
         else
         {
            CPPUNIT_FAIL("A timer elapsed message was received with an unexpected name: " + tem->GetTimerName());
         }

         foundTimeMsg = true;
      }
   }
   CPPUNIT_ASSERT_MESSAGE("There should have been a timer elapsed message found", foundTimeMsg);

   msgs.clear();
   mTestComp->reset();
   dtCore::System::GetInstance().Step(0.016f);
   msgs = mTestComp->GetReceivedProcessMessages();
   foundTimeMsg = false;
   for (unsigned int i = 0; i < msgs.size(); ++i)
   {
      if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
      {
         const dtGame::TimerElapsedMessage* tem = static_cast<const dtGame::TimerElapsedMessage*>(msgs[i].get());
         if (tem->GetTimerName() == "RepeatingTimer1")
         {
            foundTimeMsg = true;
         }
      }
   }
   CPPUNIT_ASSERT_MESSAGE("The repeating timer should have been found again since it is repeating", foundTimeMsg);

   bool foundNonRepeatingTimeMsg = false;
   for (unsigned int i = 0; i < msgs.size(); ++i)
   {
      if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
      {
         const dtGame::TimerElapsedMessage* tem = static_cast<const dtGame::TimerElapsedMessage*>(msgs[i].get());
         if (tem->GetTimerName() == "SimTimer1")
         {
            foundNonRepeatingTimeMsg = true;
         }
      }
   }
   CPPUNIT_ASSERT_MESSAGE("The non repeating timer fired, and should no longer be in the list", !foundNonRepeatingTimeMsg);

   // The sim timer was non repeating, should have been cleared
   mGM->ClearTimer("RepeatingTimer1", proxy.get());

   msgs.clear();
   mTestComp->reset();
   dtCore::System::GetInstance().Step(0.016f);
   msgs = mTestComp->GetReceivedProcessMessages();
   unsigned int msgCount = 0;
   for (unsigned int i = 0; i < msgs.size(); ++i)
   {
      if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
      {
         ++msgCount;
      }
   }

   CPPUNIT_ASSERT_MESSAGE("The number of timer messages should be 0", msgCount == 0);

   const unsigned int numToTest = 20;
   for (unsigned int i = 0; i < numToTest; ++i)
   {
      std::ostringstream ss;
      ss << "Timer" << i;
      mGM->SetTimer(ss.str(), NULL, 0.001f);
   }

   msgs.clear();
   mTestComp->reset();

   dtCore::System::GetInstance().Step(0.016f);
   dtCore::System::GetInstance().Step(0.016f);

   msgs = mTestComp->GetReceivedProcessMessages();
   msgCount = 0;
   for (unsigned int i = 0; i < msgs.size(); ++i)
   {
      if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
      {
         ++msgCount;
      }
   }

   CPPUNIT_ASSERT_MESSAGE("The number of received messages should be equal to the number of timers set", msgCount == numToTest);
}

/////////////////////////////////////////////////
void GameManagerTests::TestFindActorById()
{
   dtCore::RefPtr<dtCore::TransformableActorProxy> transActor;
   mGM->CreateActor("dtcore", "Camera", transActor);
   CPPUNIT_ASSERT(transActor.valid());
   mGM->AddActor(*transActor);

   dtCore::TransformableActorProxy* value;
   mGM->FindActorById(transActor->GetId(), value);

   CPPUNIT_ASSERT_MESSAGE("The template version of FindGameActorById should not return NULL", transActor.valid());
   CPPUNIT_ASSERT_MESSAGE("The template version of FindGameActorById should return the correct pointer", value == transActor.get());

   dtActors::PlayerStartActorProxy* shouldBeNULL;
   mGM->FindActorById(transActor->GetId(), shouldBeNULL);
   CPPUNIT_ASSERT_MESSAGE("The template version of FindGameActorById should have returned NULL", shouldBeNULL == NULL);
}

/////////////////////////////////////////////////
void GameManagerTests::TestFindGameActorById()
{
   dtCore::RefPtr<dtGame::IEnvGameActorProxy> envActor;
   mGM->CreateActor("ExampleActors", "TestEnvironmentActor", envActor);
   CPPUNIT_ASSERT(envActor.valid());
   mGM->AddActor(*envActor, false, false);

   dtGame::IEnvGameActorProxy* value;
   mGM->FindGameActorById(envActor->GetId(), value);

   CPPUNIT_ASSERT_MESSAGE("The template version of FindGameActorById should not return NULL", envActor.valid());
   CPPUNIT_ASSERT_MESSAGE("The template version of FindGameActorById should return the correct pointer", value == envActor.get());

   TestPlayerProxy* shouldBeNULL;
   mGM->FindGameActorById(envActor->GetId(), shouldBeNULL);
   CPPUNIT_ASSERT_MESSAGE("The template version of FindGameActorById should have returned NULL", shouldBeNULL == NULL);
}

/////////////////////////////////////////////////
void GameManagerTests::TestSetProjectContext()
{
   const std::string context = "data/ProjectContext";
   try
   {
      mGM->SetProjectContext(context);
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }

   std::string absPath = dtUtil::FileUtils::GetInstance().GetAbsolutePath(context);
   std::string gmPC    = mGM->GetProjectContext();

   CPPUNIT_ASSERT_MESSAGE("The context should have been set", gmPC == absPath);
   CPPUNIT_ASSERT_MESSAGE("The dtCore::ProjectContext should be correct", dtCore::Project::GetInstance().GetContext() == absPath);
   CPPUNIT_ASSERT(mGM->GetProjectContext() == dtCore::Project::GetInstance().GetContext());
}

//////////////////////////////////////////////////
void GameManagerTests::TestGMShutdown()
{
   CPPUNIT_ASSERT(mGM.valid());

   dtCore::Project& project = dtCore::Project::GetInstance();
   try
   {
      const std::string context = "data/ProjectContext";
      project.SetContext(context);

      dtCore::Map& m = project.CreateMap("testMap", "aa");

      const unsigned int numActors = 20;
      std::vector<dtGame::GameActorPtr> backList;
      for (unsigned int i = 0; i < numActors; ++i)
      {
         dtGame::GameActorPtr newActor;
         mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, newActor);

         CPPUNIT_ASSERT(newActor.valid());

         backList.push_back(newActor);
         if (i > 0)
         {
            newActor->SetParentActor(backList[i-1]);
         }
         m.AddProxy(*newActor);

      }

      project.SaveMap(m);

      mGM->ChangeMap(m.GetName(), false);

      //Make sure the map change completes.
      dtCore::System::GetInstance().Step(0.016f);
      dtCore::System::GetInstance().Step(0.016f);
      dtCore::System::GetInstance().Step(0.016f);

      CPPUNIT_ASSERT_EQUAL(1U, unsigned(mGM->GetCurrentMapSet().size()));

      dtCore::ObserverPtr<dtCore::Map> mapPtr = &m;

      CPPUNIT_ASSERT(!mGM->IsShuttingDown());

      mGM->Shutdown();

      CPPUNIT_ASSERT_MESSAGE("The map should have been closed on shutdown.", !mapPtr.valid());

      CPPUNIT_ASSERT(mGM->IsShuttingDown());

      CPPUNIT_ASSERT_MESSAGE("Shutdown of the game manager should have flipped the removed from GM flag on the component",
         mTestComp->mWasOnRemovedFromGMCalled);

      CPPUNIT_ASSERT_MESSAGE("Shutdown of the game manager should have removed the test component",
         mGM->GetComponentByName(mTestComp->GetName()) == NULL);

      CPPUNIT_ASSERT(mGM->GetCurrentMapSet().empty());

      dtCore::ActorPtrVector proxies;
      mGM->GetAllActors(proxies);
      CPPUNIT_ASSERT_MESSAGE("Shut down of the game manager should have deleted the actors",
         proxies.empty());

      //calling it twice should be ok.
      //mGM->Shutdown();
      dtCore::Project::GetInstance().DeleteMap("testMap");
   }
   catch (...)
   {
      if (project.GetMapNames().find("testMap") != project.GetMapNames().end())
      {
         project.DeleteMap("testMap");
      }
      throw;
   }
}

//////////////////////////////////////////////////
void GameManagerTests::TestOpenCloseAdditionalMaps()
{
   CPPUNIT_ASSERT(mGM.valid());

   dtCore::Project& project = dtCore::Project::GetInstance();

   try
   {
      const std::string context = "data/ProjectContext";
      project.SetContext(context);

      dtCore::ActorRefPtrVector actorsInMaps;
      dtCore::ActorRefPtrVector prototypes;

      {
         dtCore::Map& m = project.CreateMap("testMap", "testMap");
         dtCore::Map& m2 = project.CreateMap("testMap2", "testMap2");
         dtCore::Map& mp = project.CreateMap("testPrototypeMap", "testPrototypeMap");

         dtCore::RefPtr<dtGame::GameActorProxy> envActor;
         dtCore::ActorTypePtr type = mGM->FindActorType("ExampleActors", "TestEnvironmentActor");
         CPPUNIT_ASSERT(type.valid());
         mGM->CreateActor(*type, envActor);
         CPPUNIT_ASSERT(envActor.valid());
         envActor->SetName("ENV");
         m.AddProxy(*envActor, false);
         m.SetEnvironmentActor(envActor);

         const unsigned int numActors = 10;
         std::vector<dtGame::GameActorPtr> backList;
         backList.reserve(numActors);
         actorsInMaps.reserve(numActors * 2);
         for (unsigned int i = 0; i < numActors; ++i)
         {
            dtGame::GameActorPtr newActor;
            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, newActor);

            CPPUNIT_ASSERT(newActor.valid());

            backList.push_back(newActor);
            if (i > 0)
            {
               newActor->SetParentActor(backList[i-1]);
            }
            m.AddProxy(*newActor);

            actorsInMaps.push_back(newActor);
         }

         for (unsigned i = 0; i < numActors; ++i)
         {
            dtCore::RefPtr<dtCore::BaseActorObject> actor =
               mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE);
            CPPUNIT_ASSERT(actor.valid());
            m2.AddProxy(*actor);

            actorsInMaps.push_back(actor);
         }

         backList.clear();
         prototypes.reserve(numActors);
         for (unsigned int i = 0; i < numActors; ++i)
         {
            dtGame::GameActorPtr newActor;
            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, newActor);

            CPPUNIT_ASSERT(newActor.valid());

            backList.push_back(newActor);
            if (i > 0)
            {
               newActor->SetParentActor(backList[i-1]);
            }
            m.AddProxy(*newActor);

            prototypes.push_back(newActor);
         }

         backList[0]->SetInitialOwnership(dtGame::GameActorProxy::Ownership::PROTOTYPE);

         project.SaveMap(m);
         project.SaveMap(m2);
         project.SaveMap(mp);

         project.CloseMap(m);
         project.CloseMap(m2);
         project.CloseMap(mp);
      }

      dtCore::RefPtr<dtGame::GameActorProxy> actorNoMap;
      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, actorNoMap);
      CPPUNIT_ASSERT(actorNoMap.valid());
      actorNoMap->SetName("I have no map");

      mGM->AddActor(*actorNoMap, false, false);

      std::vector<std::string> mapNames;
      mapNames.push_back("testMap");
      mapNames.push_back("testMap2");
      mapNames.push_back("testPrototypeMap");

      mGM->OpenAdditionalMapSet(mapNames);
      // process the messages.
      dtCore::System::GetInstance().Step(0.016f);

      dtCore::RefPtr<const dtGame::Message> msg = mTestComp->FindProcessMessageOfType(dtGame::MessageType::INFO_MAPS_OPENED);
      std::vector<std::string> retrievedMaps;
      CPPUNIT_ASSERT(msg.valid());
      dynamic_cast<const dtGame::MapMessage*>(msg.get())->GetMapNames(retrievedMaps);
      CPPUNIT_ASSERT_EQUAL(retrievedMaps.size(), mapNames.size());
      for (unsigned i = 0; i < retrievedMaps.size(); ++i)
      {
         CPPUNIT_ASSERT_EQUAL(mapNames[i], retrievedMaps[i]);
      }

      mTestComp->reset();

      dtGame::GameActorPtr envActor = mGM->GetEnvironmentActor();
      CPPUNIT_ASSERT(envActor != NULL);
      CPPUNIT_ASSERT_EQUAL(std::string("ENV"), envActor->GetName());

      CPPUNIT_ASSERT(mGM->FindGameActorById(actorNoMap->GetId()) == actorNoMap.get());
      CPPUNIT_ASSERT(actorNoMap->GetParentActor() != NULL);
      CPPUNIT_ASSERT(actorNoMap->GetParentActor() == envActor);

      dtCore::ActorRefPtrVector::iterator i, iend;
      i = actorsInMaps.begin();
      iend = actorsInMaps.end();
      for (; i != iend; ++i)
      {
         dtCore::BaseActorObject* bao = i->get();
         dtGame::GameActorPtr curActor = mGM->FindGameActorById(bao->GetId());
         CPPUNIT_ASSERT(curActor.valid());
         CPPUNIT_ASSERT(curActor->is_descendant_of(*envActor));
      }


      bool foundProto = false;
      i = prototypes.begin();
      iend = prototypes.end();
      for (; i != iend; ++i)
      {
         dtCore::BaseActorObject* bao = i->get();
         CPPUNIT_ASSERT_MESSAGE("None of the actors in the prototype tree should be found, even though only the root is marked as such.",
               mGM->FindActorById(bao->GetId()) == NULL);
         if (!foundProto)
         {
            CPPUNIT_ASSERT_MESSAGE("Only the first of the actors in the prototype tree should be found.",
                  mGM->FindPrototypeByID(bao->GetId()) != NULL);
            foundProto = true;
         }
         else
         {
            CPPUNIT_ASSERT_MESSAGE("Only one of the actors in the prototype tree should be found.",
                  mGM->FindPrototypeByID(bao->GetId()) == NULL);
         }
      }

      mGM->CloseAdditionalMapSet(mapNames);

      //Make sure the actor deletes complete.
      dtCore::System::GetInstance().Step(0.016f);

      msg = mTestComp->FindProcessMessageOfType(dtGame::MessageType::INFO_MAPS_CLOSED);
      CPPUNIT_ASSERT(msg.valid());
      dynamic_cast<const dtGame::MapMessage*>(msg.get())->GetMapNames(retrievedMaps);
      CPPUNIT_ASSERT_EQUAL(retrievedMaps.size(), mapNames.size());
      for (unsigned i = 0; i < retrievedMaps.size(); ++i)
      {
         CPPUNIT_ASSERT_EQUAL(mapNames[i], retrievedMaps[i]);
      }

      mTestComp->reset();


      CPPUNIT_ASSERT_MESSAGE("this on actor should still be in the gm.", mGM->FindGameActorById(actorNoMap->GetId()) == actorNoMap.get());

      // all of the map actors should be removed.
      i = actorsInMaps.begin();
      iend = actorsInMaps.end();
      for (; i != iend; ++i)
      {
         dtCore::BaseActorObject* bao = i->get();
         CPPUNIT_ASSERT(mGM->FindGameActorById(bao->GetId()) == NULL);
      }



      project.DeleteMap("testMap");
      project.DeleteMap("testMap2");
      project.DeleteMap("testPrototypeMap");
   }
   catch (const dtUtil::Exception& ex)
   {
      project.DeleteMap("testMap");
      project.DeleteMap("testMap2");
      project.DeleteMap("testPrototypeMap");
      CPPUNIT_FAIL(ex.ToString());
   }
   catch (...)
   {
      project.DeleteMap("testMap");
      project.DeleteMap("testMap2");
      project.DeleteMap("testPrototypeMap");
      throw;
   }
}

//////////////////////////////////////////////////
void GameManagerTests::TestBatchAdd()
{
   CPPUNIT_ASSERT(mGM.valid());
   dtCore::RefPtr<const dtCore::ActorType> type = mGM->FindActorType("ExampleActors","Test1Actor");
   CPPUNIT_ASSERT(type != NULL);

   dtCore::RefPtr<TestGameActor1> deletedActor;
   mGM->CreateActor(*type.get(), deletedActor);
   deletedActor->SetName("Deleted");
   mGM->AddActor(*deletedActor, false, false);
   mGM->DeleteActor(deletedActor->GetId());
   CPPUNIT_ASSERT(deletedActor->IsDeleted());

   // Create a prototype actor to work with.
   dtCore::RefPtr<TestGameActor1> actors[4];
   for (unsigned i  = 0; i < 4; ++i)
   {
      mGM->CreateActor(*type.get(), actors[i]);
      actors[i]->SetName("Test" + dtUtil::ToString(i));
   }

   for (unsigned i = 0; i < 4; ++i)
   {
      CPPUNIT_ASSERT(actors[i] != NULL);
      // Each points to the next 2 in a circle.
      actors[i]->SetTestActorId(actors[(i+1)%4]->GetId());
      actors[i]->SetTestActorNameToLookup(actors[(i+2)%4]->GetName());
   }

   // Batch Begins
   {
      dtGame::ScopedGMBatchAdd batchTransation(*mGM);
      for (unsigned i = 0; i < 4; ++i)
      {
         // Adding the actors should not set IsInGM, and should not call on entered world.
         mGM->AddActor(*actors[i], false, false);
         CPPUNIT_ASSERT(!actors[i]->IsInGM());
         CPPUNIT_ASSERT(!actors[i]->GetTestActorIdFound());
         CPPUNIT_ASSERT(!actors[i]->GetTestActorNameFound());
       }
       // To make sure the batch code doesn't try to reset a deleted actor.
       CPPUNIT_ASSERT(mGM->FindActorById(deletedActor->GetId()) == deletedActor);
       dtGame::GameActorProxy* deletedActorPtr;
       mGM->FindActorByName(deletedActor->GetName(), deletedActorPtr);
       CPPUNIT_ASSERT(deletedActorPtr == deletedActor);
       CPPUNIT_ASSERT(!deletedActor->IsInGM());
       CPPUNIT_ASSERT(deletedActor->IsDeleted());
   }
   // Batch ends

   for (unsigned i = 0; i < 4; ++i)
   {
      // The actors should all be in the GM and should have been able to look up each other even though
      // they were added to the GM is the worst possible order for them to find each other.
      CPPUNIT_ASSERT(actors[i]->IsInGM());
      CPPUNIT_ASSERT(actors[i]->GetTestActorIdFound());
      CPPUNIT_ASSERT(actors[i]->GetTestActorIdInitialized());
      CPPUNIT_ASSERT(actors[i]->GetTestActorNameFound());
      CPPUNIT_ASSERT(actors[i]->GetTestActorNameInitialized());
    }


}

//////////////////////////////////////////////////
void GameManagerTests::TestBatchAddWithAddActor()
{
   CPPUNIT_ASSERT(mGM.valid());
   dtCore::RefPtr<const dtCore::ActorType> type = mGM->FindActorType("ExampleActors","Test1Actor");
   CPPUNIT_ASSERT(type != NULL);

   // Create a prototype actor to work with.
   dtCore::RefPtr<TestGameActor1> actors[4];
   for (unsigned i  = 0; i < 4; ++i)
   {
      mGM->CreateActor(*type.get(), actors[i]);
      actors[i]->SetName("Test" + dtUtil::ToString(i));
      dtCore::RefPtr<TestActorComponent1> component;
      mGM->CreateActor(*TestActorComponent1::TYPE, component);
      CPPUNIT_ASSERT(component.valid());
      actors[i]->AddComponent(*component);
      actors[i]->GetComponent<TestActorComponent1>()->SetAddAnotherActor(true);
   }

   // Batch Begins
   {
      dtGame::ScopedGMBatchAdd batchTransation(*mGM);
      for (unsigned i = 0; i < 4; ++i)
      {
         // Adding the actors should not set IsInGM, and should not call on entered world.
         mGM->AddActor(*actors[i], false, false);
         CPPUNIT_ASSERT(!actors[i]->IsInGM());
         CPPUNIT_ASSERT(!actors[i]->GetComponent<TestActorComponent1>()->GetIsInGM());
         CPPUNIT_ASSERT(!actors[i]->GetComponent<TestActorComponent1>()->GetAddActorInitialized());
       }
   }
   // Batch ends

   for (unsigned i = 0; i < 4; ++i)
   {
      // The actor component should enter the world, add an actor, and it should be immediately intialized at the end of the batch.
      mGM->AddActor(*actors[i], false, false);
      CPPUNIT_ASSERT(actors[i]->IsInGM());
      CPPUNIT_ASSERT(actors[i]->GetComponent<TestActorComponent1>()->GetIsInGM());
      CPPUNIT_ASSERT(actors[i]->GetComponent<TestActorComponent1>()->GetAddActorInitialized());
   }

}


//////////////////////////////////////////////////
void GameManagerTests::TestGMSettingsServerClientRoles()
{
   CPPUNIT_ASSERT(mGM.valid());

   dtCore::Project& project = dtCore::Project::GetInstance();

   dtCore::Map& m = project.CreateMap("testMap", "bbbb");

   CPPUNIT_ASSERT_MESSAGE("Client role should default to true", 
      mGM->GetGMSettings().GetClientRole());
   CPPUNIT_ASSERT_MESSAGE("Server role should default to true", 
      mGM->GetGMSettings().GetServerRole());

   // Test with client off, server on
   mGM->GetGMSettings().SetClientRole(false);
   mGM->GetGMSettings().SetServerRole(true);

   // PROTOTYPE  
   dtCore::RefPtr<dtCore::BaseActorObject> proxy5 =
      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE);
   CPPUNIT_ASSERT(proxy5.valid());
   dtCore::RefPtr<dtGame::GameActorProxy> gap5 = dynamic_cast<dtGame::GameActorProxy*> (proxy5.get());
   CPPUNIT_ASSERT(gap5.valid());
   gap5->SetName("PROTOTYPE");
   gap5->SetInitialOwnership(dtGame::GameActorProxy::Ownership::PROTOTYPE);
   m.AddProxy(*proxy5);


   // CLIENT_AND_SERVER_LOCAL  
   dtCore::RefPtr<dtCore::BaseActorObject> proxy4 =
      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE);
   CPPUNIT_ASSERT(proxy4.valid());
   dtCore::RefPtr<dtGame::GameActorProxy> gap4 = dynamic_cast<dtGame::GameActorProxy*> (proxy4.get());
   CPPUNIT_ASSERT(gap4.valid());
   gap4->SetName("CLIENT_AND_SERVER_LOCAL");
   gap4->SetInitialOwnership(dtGame::GameActorProxy::Ownership::CLIENT_AND_SERVER_LOCAL);
   m.AddProxy(*proxy4);

   // SERVER_LOCAL  
   dtCore::RefPtr<dtCore::BaseActorObject> proxy3 =
      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE);
   CPPUNIT_ASSERT(proxy3.valid());
   dtCore::RefPtr<dtGame::GameActorProxy> gap3 = dynamic_cast<dtGame::GameActorProxy*> (proxy3.get());
   CPPUNIT_ASSERT(gap3.valid());
   gap3->SetName("SERVER_LOCAL");
   gap3->SetInitialOwnership(dtGame::GameActorProxy::Ownership::SERVER_LOCAL);
   m.AddProxy(*proxy3);


   // SERVER_PUBLISHED  
   dtCore::RefPtr<dtCore::BaseActorObject> proxy2 =
      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE);
   CPPUNIT_ASSERT(proxy2.valid());
   dtCore::RefPtr<dtGame::GameActorProxy> gap2 = dynamic_cast<dtGame::GameActorProxy*> (proxy2.get());
   CPPUNIT_ASSERT(gap2.valid());
   gap2->SetName("SERVER_PUBLISHED");
   gap2->SetInitialOwnership(dtGame::GameActorProxy::Ownership::SERVER_PUBLISHED);
   m.AddProxy(*proxy2);

   // CLIENT_LOCAL  
   dtCore::RefPtr<dtCore::BaseActorObject> proxy1 =
      mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE);
   CPPUNIT_ASSERT(proxy1.valid());
   dtCore::RefPtr<dtGame::GameActorProxy> gap1 = dynamic_cast<dtGame::GameActorProxy*> (proxy1.get());
   CPPUNIT_ASSERT(gap1.valid());
   gap1->SetName("CLIENT_LOCAL");
   gap1->SetInitialOwnership(dtGame::GameActorProxy::Ownership::CLIENT_LOCAL);
   m.AddProxy(*proxy1);


   project.SaveMap(m);

   mGM->ChangeMap(m.GetName(), false);

   //Make sure the map change completes.
   dtCore::System::GetInstance().Step(0.016f);
   dtCore::System::GetInstance().Step(0.016f);
   dtCore::System::GetInstance().Step(0.016f);

   // Test that there are 4 game actors and 1 prototype in the GM. 
   dtCore::ActorPtrVector proxies;
   mGM->GetAllPrototypes(proxies);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Should be 1 prototype.",(int) proxies.size(), 1);

   mGM->GetAllActors(proxies);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Should be 3 total actors.", 3, (int) proxies.size());

   dtActors::GameMeshActor* testMeshProxy = NULL;
   mGM->FindActorByName("CLIENT_LOCAL", testMeshProxy);
   CPPUNIT_ASSERT_MESSAGE("CLIENT_LOCAL should NOT be in GM.", testMeshProxy == NULL);

   mGM->FindActorByName("SERVER_PUBLISHED", testMeshProxy);
   CPPUNIT_ASSERT_MESSAGE("SERVER_PUBLISHED should exist in GM.", testMeshProxy != NULL);
   CPPUNIT_ASSERT_MESSAGE("SERVER_PUBLISHED should be published.", testMeshProxy->IsPublished());

   mGM->FindActorByName("SERVER_LOCAL", testMeshProxy);
   CPPUNIT_ASSERT_MESSAGE("SERVER_LOCAL should exist in GM.", testMeshProxy != NULL);
   CPPUNIT_ASSERT_MESSAGE("SERVER_LOCAL should NOT be published.", !testMeshProxy->IsPublished());

   mGM->FindActorByName("CLIENT_AND_SERVER_LOCAL", testMeshProxy);
   CPPUNIT_ASSERT_MESSAGE("CLIENT_AND_SERVER_LOCAL should exist in GM.", testMeshProxy != NULL);
   CPPUNIT_ASSERT_MESSAGE("CLIENT_AND_SERVER_LOCAL should NOT be published.", !testMeshProxy->IsPublished());


   dtCore::Project::GetInstance().DeleteMap("testMap");
}

void GameManagerTests::TestSwitchToLocal()
{
   dtCore::RefPtr<dtActors::GameMeshActor> gameMeshActor;
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, gameMeshActor);

   mGM->AddActor(*gameMeshActor, true, false);

   CPPUNIT_ASSERT(gameMeshActor->IsRemote());

   mGM->SwitchActorToRemote(*gameMeshActor);
   CPPUNIT_ASSERT(gameMeshActor->IsRemote());

   mGM->SwitchActorToLocal(*gameMeshActor, true);

   CPPUNIT_ASSERT(!gameMeshActor->IsRemote());
   CPPUNIT_ASSERT(gameMeshActor->GetDrawable()->GetSceneParent() != NULL);
   CPPUNIT_ASSERT(gameMeshActor->IsPublished());
}

void GameManagerTests::TestSwitchToRemote()
{
   dtCore::RefPtr<dtActors::GameMeshActor> gameMeshActor;
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, gameMeshActor);

   mGM->AddActor(*gameMeshActor, false, true);

   CPPUNIT_ASSERT(!gameMeshActor->IsRemote());
   CPPUNIT_ASSERT(gameMeshActor->IsPublished());
   mGM->SwitchActorToLocal(*gameMeshActor, true);
   CPPUNIT_ASSERT(!gameMeshActor->IsRemote());

   mGM->SwitchActorToRemote(*gameMeshActor);

   CPPUNIT_ASSERT(gameMeshActor->IsRemote());
   CPPUNIT_ASSERT(gameMeshActor->GetDrawable()->GetSceneParent() != NULL);
   CPPUNIT_ASSERT(!gameMeshActor->IsPublished());
}
