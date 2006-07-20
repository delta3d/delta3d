/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Eddie Johnson and David Guthrie
 */

#include <cstdlib>
#include <iostream>
#include <osg/Math>
#include <osg/io_utils>

#include <dtUtil/log.h>

#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/globals.h>

#include <dtDAL/datatype.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/actortype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/project.h>

#include <dtABC/application.h>

#include <dtGame/datastream.h>
#include <dtGame/messageparameter.h>
#include <dtGame/machineinfo.h>
#include <dtGame/gameactor.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/rulescomponent.h>
#include <dtGame/defaultmessageprocessor.h>

#include <osg/Endian>

#include <cppunit/extensions/HelperMacros.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

class GameManagerTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(GameManagerTests);

        CPPUNIT_TEST(TestDataStream);
        CPPUNIT_TEST(TestResourceMessageParameter);

        CPPUNIT_TEST(TestStringMessageParameter);
        CPPUNIT_TEST(TestEnumMessageParameter);
        CPPUNIT_TEST(TestBooleanMessageParameter);
        CPPUNIT_TEST(TestUnsignedCharMessageParameter);
        CPPUNIT_TEST(TestUnsignedIntMessageParameter);
        CPPUNIT_TEST(TestIntMessageParameter);
        CPPUNIT_TEST(TestUnsignedLongIntMessageParameter);
        CPPUNIT_TEST(TestLongIntMessageParameter);
        CPPUNIT_TEST(TestUnsignedShortIntMessageParameter);
        CPPUNIT_TEST(TestShortIntMessageParameter);
        CPPUNIT_TEST(TestFloatMessageParameter);
        CPPUNIT_TEST(TestDoubleMessageParameter);

        CPPUNIT_TEST(TestVec2MessageParameters);
        CPPUNIT_TEST(TestVec3MessageParameters);
        CPPUNIT_TEST(TestVec4MessageParameters);

        CPPUNIT_TEST(TestActorMessageParameter);

        CPPUNIT_TEST(TestApplicationMember);
        CPPUNIT_TEST(TestMachineInfo);

        CPPUNIT_TEST(TestActorSearching);
        CPPUNIT_TEST(TestAddActor);
        CPPUNIT_TEST(TestComplexScene);
        CPPUNIT_TEST(TestAddRemoveComponents);
        CPPUNIT_TEST(TestComponentPriority);

        CPPUNIT_TEST(TestTimers);

        CPPUNIT_TEST(TestOnAddedToGM);

   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();
   void TestDataStream();

   void TestResourceMessageParameter();
   void TestStringMessageParameter();
   void TestEnumMessageParameter();
   void TestBooleanMessageParameter();
   void TestUnsignedCharMessageParameter();
   void TestUnsignedIntMessageParameter();
   void TestIntMessageParameter();
   void TestUnsignedLongIntMessageParameter();
   void TestLongIntMessageParameter();
   void TestUnsignedShortIntMessageParameter();
   void TestShortIntMessageParameter();
   void TestFloatMessageParameter();
   void TestDoubleMessageParameter();

   void TestVec2MessageParameters();
   void TestVec3MessageParameters();
   void TestVec4MessageParameters();

   void TestActorMessageParameter();

   void TestApplicationMember();
   void TestMachineInfo();

   void TestActorSearching();
   void TestAddActor();
   void TestComplexScene();
   void TestAddRemoveComponents();
   void TestComponentPriority();

   void TestTimers();

   void TestOnAddedToGM();

   //this templated function can be used for an osg vector type and VecMessageParameter subclass.
   template <class VecType, class ParamType>
   void TestVecMessageParameter(int size)
   {
      dtCore::RefPtr<ParamType> param = new ParamType("a");

      VecType r = param->GetValue();
      for (int i = 0; i < size; ++i)
      {
         //everything has to be mode a double in the "equivalent" calls because
         //otherwise the float versions get some c++ ambiguity problems.
         CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0's",
            osg::equivalent(0.0, (double)r[i], 1e-2));
      }

      VecType c;
      for (int i = 0; i < size; ++i)
      {
         c[i] = i * 32.3;
      }
      param->SetValue(c);
      r = param->GetValue();
      for (int i = 0; i < size; ++i)
      {
         //everything has to be mode a double in the "equivalent" calls because
         //otherwise the float versions get some c++ ambiguity problems.
         CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set",
            osg::equivalent((double)c[i], (double)r[i], 1e-2));
      }

      std::string holder = param->ToString();

      VecType temp;
      for (int i = 0; i < size; ++i)
      {
         temp[i] = i * 100.33;
      }

      param->SetValue(temp);
      param->FromString(holder);

      r = param->GetValue();
      for (int i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set as a string",
            osg::equivalent((double)c[i], (double)r[i], 1e-2));
      }

      dtCore::RefPtr<ParamType> param2 = new ParamType("b");
      param2->CopyFrom(*param);
      for (int i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set",
            osg::equivalent((double)param->GetValue()[i], (double)param2->GetValue()[i], 1e-2));
      }

      dtGame::DataStream ds;
      dtCore::RefPtr<ParamType> param3 = new ParamType("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      for (int i = 0; i < size; ++i)
      {
         //std::cout << param->GetValue()[i] << " " << param3->GetValue()[i] << std::endl;
         CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set",
            osg::equivalent((double)param->GetValue()[i], (double)param3->GetValue()[i], 1e-2));
      }
   }

   /**
    * This templated method is very similar to the method for testing vector
    * classes above except this one tests the list form.
    */
   template <class ParameterType, class ValueType>
   void TestVecMessageParameterList(ValueType defaultValue, ValueType t1, ValueType t2,
      ValueType t3, ValueType t4, int size)
   {
      std::vector<ValueType> valueList;
      std::vector<ValueType> testList;
      int i;

      dtCore::RefPtr<ParameterType> param =
         new ParameterType("a",defaultValue,true);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should support a list of values.",
         param->IsList() == true);
      testList = param->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("List should have the default value as its only element,",
         testList.size() == 1);

      for (i=0; i<size; i++)
      {
         //everything has to be mode a double in the "equivalent" calls because
         //otherwise the float versions get some c++ ambiguity problems.
         CPPUNIT_ASSERT_MESSAGE("First element should be \"defaultValue\"",
            osg::equivalent((double)testList[0][i], (double)defaultValue[i], 1e-2));
      }

      try
      {
         param->GetValue();
         CPPUNIT_FAIL("Should not be allowed to get a single value from a parameter "
            "that contains a list.");
      }
      catch (dtUtil::Exception &e) { }

      //This quick test ensures that we cannot copy a parameter with a single value
      //into a parameter supporting a list of values.  Only parameters that
      //both contain a list of values my be copied.
      try
      {
         dtCore::RefPtr<ParameterType> paramSingleValue =
            new ParameterType("a");
         paramSingleValue->CopyFrom(*param);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception &e) { }

      try
      {
         dtCore::RefPtr<ParameterType> paramSingleValue =
            new ParameterType("a");
         param->CopyFrom(*paramSingleValue);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception &e) { }

      valueList.push_back(t1);
      valueList.push_back(t2);
      valueList.push_back(t3);
      valueList.push_back(t4);

      param->SetValueList(valueList);
      testList = param->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("Lists should be the same size.",
         valueList.size() == testList.size());
      CPPUNIT_ASSERT_MESSAGE("Number of values in parameter list should be 4.",
         testList.size() == 4);

      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct.",
            osg::equivalent((double)testList[0][i], (double)t1[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct.",
            osg::equivalent((double)testList[1][i], (double)t2[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct.",
            osg::equivalent((double)testList[2][i], (double)t3[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct.",
            osg::equivalent((double)testList[3][i], (double)t4[i], 1e-2));

      dtCore::RefPtr<ParameterType> param2 =
         new ParameterType("b",defaultValue,true);
      std::string holder = param->ToString();
      param2->FromString(holder);
      testList = param2->GetValueList();

      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after FromString().",
            osg::equivalent((double)testList[0][i], (double)t1[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after FromString().",
            osg::equivalent((double)testList[1][i], (double)t2[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after FromString().",
            osg::equivalent((double)testList[2][i], (double)t3[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after FromString().",
            osg::equivalent((double)testList[3][i], (double)t4[i], 1e-2));

      param2 = NULL;
      param2 = new ParameterType("b",defaultValue,true);

      dtGame::DataStream ds;
      param->ToDataStream(ds);
      param2->FromDataStream(ds);

      testList = param2->GetValueList();
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after FromDataString().",
            osg::equivalent((double)testList[0][i], (double)t1[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after FromDataString().",
            osg::equivalent((double)testList[1][i], (double)t2[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after FromDataString().",
            osg::equivalent((double)testList[2][i], (double)t3[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after FromDataString().",
            osg::equivalent((double)testList[3][i], (double)t4[i], 1e-2));

      param2 = NULL;
      param2 = new ParameterType("b",defaultValue,true);
      param2->CopyFrom(*param);
      testList = param2->GetValueList();
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after param copy.",
            osg::equivalent((double)testList[0][i], (double)t1[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after param copy.",
            osg::equivalent((double)testList[1][i], (double)t2[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after param copy.",
            osg::equivalent((double)testList[2][i], (double)t3[i], 1e-2));
      for (i=0; i<size; i++)
         CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after param copy.",
            osg::equivalent((double)testList[3][i], (double)t4[i], 1e-2));
   }

   /**
    * This templated method is used to test each of the different message
    * parameters when using their list form.
    */
   template <typename ParameterType, typename ValueType>
   void TestParameterList(ValueType defaultValue, ValueType t1, ValueType t2,
      ValueType t3, ValueType t4)
   {
      std::vector<ValueType> valueList;
      std::vector<ValueType> testList;

      dtCore::RefPtr<ParameterType> param =
         new ParameterType("a",defaultValue,true);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should support a list of values.",
         param->IsList() == true);
      testList = param->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("List should have the default value as its only element,",
         testList.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("First element should be \"defaultValue\"",
         testList[0] == defaultValue);

      try
      {
         param->GetValue();
         CPPUNIT_FAIL("Should not be allowed to get a single value from a parameter "
            "that contains a list.");
      }
      catch (dtUtil::Exception &e) { }

      //This quick test ensures that we cannot copy a parameter with a single value
      //into a parameter supporting a list of values.  Only parameters that
      //both contain a list of values my be copied.
      try
      {
         dtCore::RefPtr<ParameterType> paramSingleValue =
            new ParameterType("a");
         paramSingleValue->CopyFrom(*param);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception &e) { }

      try
      {
         dtCore::RefPtr<ParameterType> paramSingleValue =
            new ParameterType("a");
         param->CopyFrom(*paramSingleValue);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception &e) { }

      valueList.push_back(t1);
      valueList.push_back(t2);
      valueList.push_back(t3);
      valueList.push_back(t4);

      param->SetValueList(valueList);
      testList = param->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("Lists should be the same size.",
         valueList.size() == testList.size());
      CPPUNIT_ASSERT_MESSAGE("Number of values in parameter list should be 4.",
         testList.size() == 4);

      CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct.",
         testList[0] == t1);
      CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct.",
         testList[1] == t2);
      CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct.",
         testList[2] == t3);
      CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct.",
         testList[3] == t4);

      dtCore::RefPtr<ParameterType> param2 =
         new ParameterType("b",defaultValue,true);
      std::string holder = param->ToString();
      param2->FromString(holder);

      testList = param2->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after FromString().",
         testList[0] == t1);
      CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after FromString().",
         testList[1] == t2);
      CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after FromString().",
         testList[2] == t3);
      CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after FromString().",
         testList[3] == t4);

      param2 = NULL;
      param2 = new ParameterType("b",defaultValue,true);

      dtGame::DataStream ds;
      param->ToDataStream(ds);
      param2->FromDataStream(ds);

      testList = param2->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after FromDataStrean().",
         testList[0] == t1);
      CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after FromDataStrean().",
         testList[1] == t2);
      CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after FromDataStrean().",
         testList[2] == t3);
      CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after FromDataStrean().",
         testList[3] == t4);

      param2 = NULL;
      param2 = new ParameterType("b",defaultValue,true);
      param2->CopyFrom(*param);
      testList = param2->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after param copy.",
         testList[0] == t1);
      CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after param copy",
         testList[1] == t2);
      CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after param copy",
         testList[2] == t3);
      CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after param copy",
         testList[3] == t4);
   }

   /**
    * This templated method is used to test each of the different message
    * parameters when using their list form.
    */
   void TestResourceParameterList(const dtDAL::DataType &dataType,
      dtDAL::ResourceDescriptor &t1, dtDAL::ResourceDescriptor &t2,
      dtDAL::ResourceDescriptor &t3, dtDAL::ResourceDescriptor &t4)
   {
      std::vector<dtDAL::ResourceDescriptor> valueList;
      std::vector<dtDAL::ResourceDescriptor> testList;

      dtCore::RefPtr<dtGame::ResourceMessageParameter> param =
         new dtGame::ResourceMessageParameter(dataType,"a",true);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should support a list of values.",
         param->IsList() == true);
      testList = param->GetValueList();

      try
      {
         param->GetValue();
         CPPUNIT_FAIL("Should not be allowed to get a single value from a parameter "
            "that contains a list.");
      }
      catch (dtUtil::Exception &e) { }

      //This quick test ensures that we cannot copy a parameter with a single value
      //into a parameter supporting a list of values.  Only parameters that
      //both contain a list of values my be copied.
      try
      {
         dtCore::RefPtr<dtGame::ResourceMessageParameter> paramSingleValue =
            new dtGame::ResourceMessageParameter(dataType,"b");
         paramSingleValue->CopyFrom(*param);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception &e) { }

      try
      {
         dtCore::RefPtr<dtGame::ResourceMessageParameter> paramSingleValue =
            new dtGame::ResourceMessageParameter(dataType,"b");
         param->CopyFrom(*paramSingleValue);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception &e) { }

      valueList.push_back(t1);
      valueList.push_back(t2);
      valueList.push_back(t3);
      valueList.push_back(t4);

      param->SetValueList(valueList);
      testList = param->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("Lists should be the same size.",
         valueList.size() == testList.size());
      CPPUNIT_ASSERT_MESSAGE("Number of values in parameter list should be 4.",
         testList.size() == 4);

      CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct.",
         testList[0] == t1);
      CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct.",
         testList[1] == t2);
      CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct.",
         testList[2] == t3);
      CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct.",
         testList[3] == t4);

      dtCore::RefPtr<dtGame::ResourceMessageParameter> param2 =
            new dtGame::ResourceMessageParameter(dataType,"b",true);
      std::string holder = param->ToString();
      param2->FromString(holder);

      testList = param2->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after FromString().",
         testList[0] == t1);
      CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after FromString().",
         testList[1] == t2);
      CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after FromString().",
         testList[2] == t3);
      CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after FromString().",
         testList[3] == t4);

      param2 = NULL;
      param2 = new dtGame::ResourceMessageParameter(dataType,"b",true);

      dtGame::DataStream ds;
      param->ToDataStream(ds);
      param2->FromDataStream(ds);

      testList = param2->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after FromDataStrean().",
         testList[0] == t1);
      CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after FromDataStrean().",
         testList[1] == t2);
      CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after FromDataStrean().",
         testList[2] == t3);
      CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after FromDataStrean().",
         testList[3] == t4);

      param2 = NULL;
      param2 = new dtGame::ResourceMessageParameter(dataType,"b",true);
      param2->CopyFrom(*param);
      testList = param2->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after param copy.",
         testList[0] == t1);
      CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after param copy",
         testList[1] == t2);
      CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after param copy",
         testList[2] == t3);
      CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after param copy",
         testList[3] == t4);
   }

private:
   static char* mTestGameActorLibrary;
   static char* mTestActorLibrary;
   dtCore::RefPtr<dtGame::GameManager> mManager;
};

class TestGMComponent: public dtGame::GMComponent
{
   public:
      TestGMComponent(const std::string& name = "TestComponent"): 
         dtGame::GMComponent(name),
         mWasOnAddedToGMCalled(false)
      {}

      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedProcessMessages()
      { return mReceivedProcessMessages; }
      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedDispatchNetworkMessages()
      { return mReceivedDispatchNetworkMessages; }

      virtual void OnAddedToGM()
      {
         mWasOnAddedToGMCalled = true;
      }

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

      bool mWasOnAddedToGMCalled;

   private:
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedProcessMessages;
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedDispatchNetworkMessages;
};

class TestOrderComponent: public dtGame::GMComponent
{
   public:
      TestOrderComponent(const std::string& name = "TestOrder"): dtGame::GMComponent(name)
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

char* GameManagerTests::mTestGameActorLibrary="testGameActorLibrary";
char* GameManagerTests::mTestActorLibrary="testActorLibrary";

void GameManagerTests::setUp()
{
   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
   try
   {
      dtUtil::Log* logger;
      logger = &dtUtil::Log::GetInstance("MessageParameter");
      //logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);

      dtCore::Scene* scene = new dtCore::Scene();
      mManager = new dtGame::GameManager(*scene);
      mManager->LoadActorRegistry(mTestGameActorLibrary);
      mManager->LoadActorRegistry(mTestActorLibrary);
      dtCore::System::Instance()->SetShutdownOnWindowClose(false);
      dtCore::System::Instance()->Start();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }

}

void GameManagerTests::tearDown()
{
   if (mManager.valid())
   {
      try
      {
         dtCore::System::Instance()->Stop();
         mManager->DeleteAllActors(true);
         mManager->UnloadActorRegistry(mTestGameActorLibrary);
         mManager->UnloadActorRegistry(mTestActorLibrary);
         mManager = NULL;
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
      }
   }
}

void GameManagerTests::TestApplicationMember()
{
   try
   {
      mManager->GetApplication();
      CPPUNIT_FAIL("Trying to get the application when it's NULL should fail.");
   } 
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_ASSERT(ex.TypeEnum() == dtGame::ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION);
   }
   
   dtCore::RefPtr<dtABC::Application> app = new dtABC::Application;
   
   mManager->SetApplication(*app);

   try
   {
      CPPUNIT_ASSERT(app == &mManager->GetApplication());
   } 
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL("The application should not be NULL.");
   }
   
}

void GameManagerTests::TestDataStream()
{
   dtGame::DataStream ds;

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
         osg::swapBytes((char *)&numList[0], sizeof(unsigned int));
         osg::swapBytes((char *)&numList[1], sizeof(unsigned int));
         osg::swapBytes((char *)&numList[2], sizeof(unsigned int));
         osg::swapBytes((char *)&numList[3], sizeof(unsigned int));
      }

      dtGame::DataStream ds2((char *)&numList[0],sizeof(unsigned int)*4,false);
      ds2.SetForceLittleEndian(true);
      ds2 >> numList2[0] >> numList2[1] >> numList2[2] >> numList2[3];

      //If we are on a big endian machine, make sure to swap the bytes back to big
      //endian format before we compare them.
      if (!ds.IsLittleEndian())
      {
         osg::swapBytes((char *)&numList[0], sizeof(unsigned int));
         osg::swapBytes((char *)&numList[1], sizeof(unsigned int));
         osg::swapBytes((char *)&numList[2], sizeof(unsigned int));
         osg::swapBytes((char *)&numList[3], sizeof(unsigned int));
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
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   //catch(const std::exception &e)
   //{
   //   CPPUNIT_FAIL(e.what());
   //}
}

void GameManagerTests::TestResourceMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::ResourceMessageParameter>  param = new dtGame::ResourceMessageParameter(dtDAL::DataType::STATIC_MESH, "a");
      dtDAL::ResourceDescriptor c(dtDAL::DataType::STATIC_MESH.GetName() + ":hello1",
         dtDAL::DataType::STATIC_MESH.GetName() + ":hello1");
      param->SetValue(&c);
      const dtDAL::ResourceDescriptor* r = param->GetValue();

      CPPUNIT_ASSERT_MESSAGE("MessageParameter should not be NULL", r != NULL);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == *r);

      std::string holder = param->ToString();
      std::string testValue = dtDAL::DataType::STATIC_MESH.GetName() + ":helloA";
      dtDAL::ResourceDescriptor temp(testValue, testValue);
      param->SetValue(&temp);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should not be NULL", r != NULL);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == *r);

      dtCore::RefPtr<dtGame::ResourceMessageParameter> param2 = new dtGame::ResourceMessageParameter(dtDAL::DataType::STATIC_MESH, "b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", *param->GetValue() == *param2->GetValue());

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::ResourceMessageParameter> param3 = new dtGame::ResourceMessageParameter(dtDAL::DataType::STATIC_MESH, "b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should not be NULL", param->GetValue() != NULL);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter copy should not be NULL", param3->GetValue() != NULL);
      CPPUNIT_ASSERT(*param->GetValue() == *param3->GetValue());

      CPPUNIT_ASSERT(param->FromString(testValue));
      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("Setting the resource descriptor with a single string value should work.",
         *r == temp);

      //Test the list version of the resource message parameter...
      dtDAL::ResourceDescriptor t1(dtDAL::DataType::STATIC_MESH.GetName()
         + ":hello1", dtDAL::DataType::STATIC_MESH.GetName() + ":hello1");
      dtDAL::ResourceDescriptor t2(dtDAL::DataType::TEXTURE.GetName()
         + ":hello2", dtDAL::DataType::TEXTURE.GetName() + ":hello2");
      dtDAL::ResourceDescriptor t3(dtDAL::DataType::TERRAIN.GetName()
         + ":hello3", dtDAL::DataType::TERRAIN.GetName() + ":hello3");
      dtDAL::ResourceDescriptor t4(dtDAL::DataType::SOUND.GetName()
         + ":hello4", dtDAL::DataType::SOUND.GetName() + ":hello4");

      TestResourceParameterList(dtDAL::DataType::STATIC_MESH,t1,t2,t3,t4);
   }
   catch (dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestStringMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::StringMessageParameter>  param = new dtGame::StringMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to empty", param->GetValue() == "");

      std::string c("doofus");
      param->SetValue(c);
      std::string r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      param->SetValue("chicken");
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtGame::StringMessageParameter> param2 = new dtGame::StringMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::StringMessageParameter> param3 = new dtGame::StringMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtGame::StringMessageParameter,std::string>(
         "defaultValue","hello","from","the","stringlist");
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestEnumMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::EnumMessageParameter>  param = new dtGame::EnumMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to empty", param->GetValue() == "");

      std::string c("doofus");
      param->SetValue(c);
      std::string r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      param->SetValue("chicken");
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtGame::EnumMessageParameter> param2 = new dtGame::EnumMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::EnumMessageParameter> param3 = new dtGame::EnumMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtGame::EnumMessageParameter,std::string>(
         "defaultValue","enum1","enum2","enum3","enum4");
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestBooleanMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::BooleanMessageParameter>  param = new dtGame::BooleanMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to false", param->GetValue() == false);

      bool c = true;
      param->SetValue(c);
      bool r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      c = false;
      param->SetValue(c);
      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      param->SetValue(true);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtGame::BooleanMessageParameter> param2 = new dtGame::BooleanMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtGame::DataStream ds;
      param->SetValue(false);
      dtCore::RefPtr<dtGame::BooleanMessageParameter> param3 = new dtGame::BooleanMessageParameter("b", true);

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtGame::BooleanMessageParameter,bool>(
         true,false,true,false,true);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestUnsignedCharMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::UnsignedCharMessageParameter>  param = new dtGame::UnsignedCharMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0", param->GetValue() == 0);

      unsigned char c = 201;
      param->SetValue(c);
      unsigned char r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      param->SetValue(3);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtGame::UnsignedCharMessageParameter> param2 = new dtGame::UnsignedCharMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::UnsignedCharMessageParameter> param3 = new dtGame::UnsignedCharMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtGame::UnsignedCharMessageParameter,unsigned char>(
         100,1,3,4,232);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestUnsignedIntMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::UnsignedIntMessageParameter>  param = new dtGame::UnsignedIntMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0", param->GetValue() == 0);

      unsigned int c = 201;
      param->SetValue(c);
      unsigned int r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      param->SetValue(3);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtGame::UnsignedIntMessageParameter> param2 = new dtGame::UnsignedIntMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::UnsignedIntMessageParameter> param3 = new dtGame::UnsignedIntMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtGame::UnsignedIntMessageParameter,unsigned int>(
         100,1,3,4,232);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestIntMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::IntMessageParameter>  param = new dtGame::IntMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0", param->GetValue() == 0);

      int c = 201;
      param->SetValue(c);
      int r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      param->SetValue(3);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtGame::IntMessageParameter> param2 = new dtGame::IntMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::IntMessageParameter> param3 = new dtGame::IntMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtGame::IntMessageParameter,int>(
         -100,-1,-3,-4,-232);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestUnsignedLongIntMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::UnsignedLongIntMessageParameter>  param = new dtGame::UnsignedLongIntMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0", param->GetValue() == 0);

      unsigned long c = 201;
      param->SetValue(c);
      unsigned long r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      param->SetValue(3);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtGame::UnsignedLongIntMessageParameter> param2 = new dtGame::UnsignedLongIntMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::UnsignedLongIntMessageParameter> param3 = new dtGame::UnsignedLongIntMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtGame::UnsignedLongIntMessageParameter,unsigned long>(
         10089,167,3784,4456323,23264);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestLongIntMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::LongIntMessageParameter>  param = new dtGame::LongIntMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0", param->GetValue() == 0);

      long c = 201;
      param->SetValue(c);
      long r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      param->SetValue(3);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtGame::LongIntMessageParameter> param2 = new dtGame::LongIntMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::LongIntMessageParameter> param3 = new dtGame::LongIntMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtGame::LongIntMessageParameter,long>(
         -10089,-167,-3784,-4456323,-23264);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestUnsignedShortIntMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::UnsignedShortIntMessageParameter>  param = new dtGame::UnsignedShortIntMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0", param->GetValue() == 0);

      unsigned short c = 201;
      param->SetValue(c);
      unsigned short r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      param->SetValue(3);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtGame::UnsignedShortIntMessageParameter> param2 = new dtGame::UnsignedShortIntMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::UnsignedShortIntMessageParameter> param3 = new dtGame::UnsignedShortIntMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtGame::UnsignedShortIntMessageParameter,unsigned short>(
         100,1000,10000,2000,3000);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestShortIntMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::ShortIntMessageParameter>  param = new dtGame::ShortIntMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0", param->GetValue() == 0);

      short c = 201;
      param->SetValue(c);
      short r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      param->SetValue(3);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtGame::ShortIntMessageParameter> param2 = new dtGame::ShortIntMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::ShortIntMessageParameter> param3 = new dtGame::ShortIntMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtGame::ShortIntMessageParameter,short>(
         -10089,-167,-3784,-423,-23264);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestFloatMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::FloatMessageParameter>  param = new dtGame::FloatMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0.0", osg::equivalent(param->GetValue(), 0.0f, 1e-2f));

      float c = 201.32;
      param->SetValue(c);
      float r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set",
         osg::equivalent(c, r, 1e-2f));

      std::string holder = param->ToString();
      param->SetValue(3.45);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set",
         osg::equivalent(c, r, 1e-2f));

      dtCore::RefPtr<dtGame::FloatMessageParameter> param2 = new dtGame::FloatMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.",
         osg::equivalent(param->GetValue(), param2->GetValue(), 1e-2f));

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::FloatMessageParameter> param3 = new dtGame::FloatMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(osg::equivalent(param->GetValue(), param3->GetValue(), 1e-2f));

      //Test the list version of the message parameter.
      TestParameterList<dtGame::FloatMessageParameter,float>(
         -10089.0f,167.5f,-3784.24f,-4456323.3456f,-23264.0f);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestDoubleMessageParameter()
{
   try
   {
      dtCore::RefPtr<dtGame::DoubleMessageParameter>  param = new dtGame::DoubleMessageParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0.0", osg::equivalent(param->GetValue(), 0.0, 1e-2));

      double c = -23425201.32234;
      param->SetValue(c);
      double r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set",
         osg::equivalent(c, r, 1e-2));

      std::string holder = param->ToString();
      param->SetValue(3.45);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set",
         osg::equivalent(c, r, 1e-2));

      dtCore::RefPtr<dtGame::DoubleMessageParameter> param2 = new dtGame::DoubleMessageParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.",
         osg::equivalent(param->GetValue(), param2->GetValue(), 1e-2));

      dtGame::DataStream ds;
      dtCore::RefPtr<dtGame::DoubleMessageParameter> param3 = new dtGame::DoubleMessageParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(osg::equivalent(param->GetValue(), param3->GetValue(), 1e-2));

      //Test the list version of the message parameter.
      TestParameterList<dtGame::DoubleMessageParameter,double>(
         -10089.0, -167.5, 3784.24, -4456323.3456, 23264.0);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestVec2MessageParameters()
{
   TestVecMessageParameter<osg::Vec2,  dtGame::Vec2MessageParameter>(2);
   TestVecMessageParameter<osg::Vec2f, dtGame::Vec2fMessageParameter>(2);
   TestVecMessageParameter<osg::Vec2d, dtGame::Vec2dMessageParameter>(2);

   try
   {
      TestVecMessageParameterList<dtGame::Vec2MessageParameter,osg::Vec2>(
         osg::Vec2(1,1),
         osg::Vec2(123,1423.234),osg::Vec2(11234.234,1.0),
         osg::Vec2(1234,-13241.294),osg::Vec2(1234.234,1.523),2
      );

      TestVecMessageParameterList<dtGame::Vec2fMessageParameter,osg::Vec2f>(
         osg::Vec2f(1.0f,1.0f),
         osg::Vec2f(123.0f,1423.234f),osg::Vec2(11234.234f,1.0f),
         osg::Vec2f(1234.0f,-13241.294f),osg::Vec2(1234.234f,1.523f),2
      );

      TestVecMessageParameterList<dtGame::Vec2dMessageParameter,osg::Vec2d>(
         osg::Vec2d(1,1),
         osg::Vec2d(123,1423.234),osg::Vec2d(11234.234,1.0),
         osg::Vec2d(1234,-13241.294),osg::Vec2d(1234.234,1.523),2
      );
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestVec3MessageParameters()
{
   TestVecMessageParameter<osg::Vec3,  dtGame::Vec3MessageParameter>(3);
   TestVecMessageParameter<osg::Vec3f, dtGame::Vec3fMessageParameter>(3);
   TestVecMessageParameter<osg::Vec3d, dtGame::Vec3dMessageParameter>(3);

   try
   {
      TestVecMessageParameterList<dtGame::Vec3MessageParameter,osg::Vec3>(
         osg::Vec3(1,1,1),
         osg::Vec3(123,1423.234,23.2134),osg::Vec3(11234.234,1.0,23.21334),
         osg::Vec3(1234,-13241.294,2763.2134),osg::Vec3(1234.234,1.523,3423.2134),3
      );

      TestVecMessageParameterList<dtGame::Vec3fMessageParameter,osg::Vec3f>(
         osg::Vec3f(1,1,1),
         osg::Vec3f(123,1423.234,23.2134),osg::Vec3f(11234.234,1.0,23.21334),
         osg::Vec3f(1234,-13241.294,2763.2134),osg::Vec3f(1234.234,1.523,3423.2134),3
      );

      TestVecMessageParameterList<dtGame::Vec3dMessageParameter,osg::Vec3d>(
         osg::Vec3d(1,1,1),
         osg::Vec3d(123,1423.234,23.2134),osg::Vec3d(11234.234,1.0,23.21334),
         osg::Vec3d(1234,-13241.294,2763.2134),osg::Vec3d(1234.234,1.523,3423.2134),3
      );
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestVec4MessageParameters()
{
   TestVecMessageParameter<osg::Vec4,  dtGame::Vec4MessageParameter>(4);
   TestVecMessageParameter<osg::Vec4f, dtGame::Vec4fMessageParameter>(4);
   TestVecMessageParameter<osg::Vec4d, dtGame::Vec4dMessageParameter>(4);

   try
   {
      TestVecMessageParameterList<dtGame::Vec4MessageParameter,osg::Vec4>(
         osg::Vec4(1,1,1,1),
         osg::Vec4(123,1423.234,234.2832,564345.634623),osg::Vec4(11234.234,1.0,10,1),
         osg::Vec4(1234,-13241.294,200.325,2352.152343),osg::Vec4(1234.234,1.523,1,2),4
      );

      TestVecMessageParameterList<dtGame::Vec4fMessageParameter,osg::Vec4f>(
        osg::Vec4f(1,1,1,1),
         osg::Vec4f(123,1423.234,234.2832,564345.634623),osg::Vec4f(11234.234,1.0,10,1),
         osg::Vec4f(1234,-13241.294,200.325,2352.152343),osg::Vec4f(1234.234,1.523,1,2),4
      );

      TestVecMessageParameterList<dtGame::Vec4dMessageParameter,osg::Vec4d>(
         osg::Vec4d(1,1,1,1),
         osg::Vec4d(123,1423.234,234.2832,564345.634623),osg::Vec4d(11234.234,1.0,10,1),
         osg::Vec4d(1234,-13241.294,200.325,2352.152343),osg::Vec4d(1234.234,1.523,1,2),4
      );
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void GameManagerTests::TestActorMessageParameter()
{
   try
   {
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > container;
      std::vector<dtCore::RefPtr<dtDAL::ActorType> > types;

      dtCore::RefPtr<dtDAL::ActorProxy> ap = mManager->CreateActor("dtcore.examples", "Test All Properties");

      dtDAL::ActorActorProperty *aap = NULL;

      std::vector<const dtDAL::ActorProperty*> props;
      ap->GetPropertyList(props);
      for(unsigned int j = 0; j < props.size(); j++)
      {
         dtDAL::DataType &dt = props[j]->GetPropertyType();
         if(dt == dtDAL::DataType::ACTOR)
            aap = static_cast<dtDAL::ActorActorProperty*> (const_cast<dtDAL::ActorProperty*>(props[j]));
      }

      CPPUNIT_ASSERT_MESSAGE("The actor actor property should not be NULL", aap != NULL);

      std::string value = aap->GetStringValue();

      dtCore::RefPtr<dtGame::MessageParameter> amp = NULL;

      amp = dtGame::MessageParameter::CreateFromType(dtDAL::DataType::ACTOR, "testActorMessageParameter");

      CPPUNIT_ASSERT_MESSAGE("The actor message parameter should not be NULL", amp != NULL);

      amp->FromString(value);

      CPPUNIT_ASSERT_MESSAGE("The actor message parameter value should have been set correctly", amp->ToString() == value);

      //Test the list version of the message parameter.
      dtCore::UniqueId id1 = dtCore::UniqueId();
      dtCore::UniqueId id2 = dtCore::UniqueId();
      dtCore::UniqueId id3 = dtCore::UniqueId();
      dtCore::UniqueId id4 = dtCore::UniqueId();
      dtCore::UniqueId id5 = dtCore::UniqueId();
      TestParameterList<dtGame::ActorMessageParameter,dtCore::UniqueId>(
         id1,id2,id3,id4,id5);
   }
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch(const std::exception &e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

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

void GameManagerTests::TestActorSearching()
{
   try
   {
      int numActorsSkipped = 0;
      dtGame::GameManager& gm = *mManager;

      std::vector<dtCore::RefPtr<dtDAL::ActorType> > typeVec;

      gm.GetActorTypes(typeVec);

      for(unsigned int i = 0; i < typeVec.size() && i < 5; i++)
      {
         // In order to keep the tests fasts, we skip the nasty slow ones.
         if (typeVec[i]->GetName() == "Cloud Plane" || typeVec[i]->GetName() == "Environment" || 
            typeVec[i]->GetName() == "Test Environment Actor") 
         {
            numActorsSkipped ++;
            continue;
         }
         gm.AddActor(*gm.CreateActor(*typeVec[i]));
      }

      std::vector<const dtDAL::ActorType*> supportedTypes;
      gm.GetActorTypes(supportedTypes);
      CPPUNIT_ASSERT_MESSAGE("The number of supported actor types should not be 0", !supportedTypes.empty());

      gm.DeleteAllActors(true);

      const unsigned int size = 5;

      dtCore::RefPtr<dtDAL::ActorProxy> proxies[size];

      for(unsigned i = 0; i < size; i++)
      {
         proxies[i] = gm.CreateActor(const_cast<dtDAL::ActorType&>(*supportedTypes[0]));
         gm.AddActor(*proxies[i].get());
      }

      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > supportedProxies;

      gm.FindActorsByType(*supportedTypes[0], supportedProxies);

      //unsigned t = supportedProxies.size();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The number of proxies found is not equal to the number that exist", 
         supportedProxies.size(), size_t(size));


      dtCore::RefPtr<dtGame::GameActorProxy> gap = NULL;
      for(unsigned int i = 0; i < typeVec.size(); i++)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> p = gm.CreateActor(*typeVec[i].get());
         if (p->IsGameActorProxy())
         {
            gap = dynamic_cast<dtGame::GameActorProxy*> (p.get());
            if (gap != NULL)
               break;
         }
      }

      CPPUNIT_ASSERT_MESSAGE("The test game actor library is loaded, the pointer should not be NULL", gap != NULL);

      gm.AddActor(*gap, false, false);

      CPPUNIT_ASSERT_MESSAGE("The GM should be able to find this game actor proxy by ID", gap == gm.FindGameActorById(gap->GetId()));
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

void GameManagerTests::TestAddRemoveComponents()
{
   dtCore::RefPtr<dtGame::RulesComponent> rc = new dtGame::RulesComponent();
   dtCore::RefPtr<dtGame::DefaultMessageProcessor> dmc = new dtGame::DefaultMessageProcessor();

   //check default names.
   CPPUNIT_ASSERT(rc->GetName() == "rules");
   CPPUNIT_ASSERT(dmc->GetName() == "default");

   rc->SetName("rulesComp");
   dmc->SetName("defaultComp");

   //check default names.
   CPPUNIT_ASSERT(rc->GetName() == "rulesComp");
   CPPUNIT_ASSERT(dmc->GetName() == "defaultComp");

   CPPUNIT_ASSERT(!rc->GetUniqueId().ToString().empty());
   CPPUNIT_ASSERT(!dmc->GetUniqueId().ToString().empty());

   CPPUNIT_ASSERT(rc->GetGameManager() == NULL);
   CPPUNIT_ASSERT(dmc->GetGameManager() == NULL);

   CPPUNIT_ASSERT(rc->GetComponentPriority() == dtGame::GameManager::ComponentPriority::NORMAL);
   CPPUNIT_ASSERT(dmc->GetComponentPriority() == dtGame::GameManager::ComponentPriority::NORMAL);

   dtGame::GMComponent *comp = mManager->GetComponentByName("defaultComp");
   CPPUNIT_ASSERT_MESSAGE("No components have been added, the return value should be NULL", comp == NULL);

   mManager->AddComponent(*rc, dtGame::GameManager::ComponentPriority::LOWER);

   try
   {
      dtCore::RefPtr<dtGame::GMComponent> whackComp = new dtGame::GMComponent("rulesComp");
      mManager->AddComponent(*whackComp, dtGame::GameManager::ComponentPriority::NORMAL);
      CPPUNIT_FAIL("A component with the name rulesComp was already added. An exception wasn't thrown when it should  have been.");
   }
   catch(const dtUtil::Exception &e) 
   {
      // Correct
   }
   

   CPPUNIT_ASSERT(rc->GetGameManager() == mManager.get());
   CPPUNIT_ASSERT(rc->GetComponentPriority() == dtGame::GameManager::ComponentPriority::LOWER);

   //test regular get all
   std::vector<dtGame::GMComponent*> toFill;
   mManager->GetAllComponents(toFill);
   CPPUNIT_ASSERT_MESSAGE("There should be exactly one component in the GameManager.",toFill.size() == 1);
   CPPUNIT_ASSERT_MESSAGE("The one component should be the rules component.", toFill[0] == rc.get());

   comp = mManager->GetComponentByName("rulesComp");
   CPPUNIT_ASSERT_MESSAGE("Finding the rules component by name should return the correct pointer", comp == rc.get());
   
   mManager->AddComponent(*dmc, dtGame::GameManager::ComponentPriority::HIGHEST);
   CPPUNIT_ASSERT(dmc->GetComponentPriority() == dtGame::GameManager::ComponentPriority::HIGHEST);

   comp = mManager->GetComponentByName("defaultComp");
   CPPUNIT_ASSERT_MESSAGE("Finding the default component by name should return the correct pointer", comp == dmc.get());

   comp = mManager->GetComponentByName("whack");
   CPPUNIT_ASSERT_MESSAGE("No component named whack was added. The pointer should be NULL", comp == NULL);
   
   //test const get all
   std::vector<const dtGame::GMComponent*> toFill2;
   mManager->GetAllComponents(toFill2);
   CPPUNIT_ASSERT_MESSAGE("There should be exactly 2 components in the GameManager.",toFill2.size() == 2);
   CPPUNIT_ASSERT_MESSAGE("The first component should be the default message processor, it has a higher priority.", toFill2[0] == dmc.get());
   CPPUNIT_ASSERT_MESSAGE("The second component should be the rules component.", toFill2[1] == rc.get());

   CPPUNIT_ASSERT(rc->GetGameManager() == mManager.get());
   CPPUNIT_ASSERT(dmc->GetGameManager() == mManager.get());

   mManager->RemoveComponent(*rc);

   mManager->GetAllComponents(toFill);
   CPPUNIT_ASSERT_MESSAGE("There should be exactly one component in the GameManager.",toFill.size() == 1);
   CPPUNIT_ASSERT_MESSAGE("The one component should be the default message processor.", toFill[0] == dmc.get());

   CPPUNIT_ASSERT(rc->GetGameManager() == NULL);
   CPPUNIT_ASSERT(dmc->GetGameManager() == mManager.get());

   //make sure the method doesn't crash when it's the last one holding onto the component.
   dtGame::DefaultMessageProcessor& dmcRef = *dmc;
   dmc = NULL;
   mManager->RemoveComponent(dmcRef);
   //the pointers are all deleted, so we can't really test anything else on that component.

   //test regular get-all clears the list.
   mManager->GetAllComponents(toFill);
   CPPUNIT_ASSERT_MESSAGE("There should be 0 components in the GameManager.",toFill.size() == 0);

   //test const get-all clears the list.
   mManager->GetAllComponents(toFill2);
   CPPUNIT_ASSERT_MESSAGE("There should be 0 components in the GameManager.",toFill2.size() == 0);
}

void GameManagerTests::TestComponentPriority()
{
   std::vector<dtCore::RefPtr<TestOrderComponent> > tocList;

   for (unsigned i = 0; i < 10; ++i)
   {
      std::ostringstream ss;
      ss << "testOrder" << i;
      tocList.push_back(new TestOrderComponent(ss.str()));

      CPPUNIT_ASSERT(tocList[i]->GetComponentPriority() == dtGame::GameManager::ComponentPriority::NORMAL);
   }

   mManager->AddComponent(*tocList[0], dtGame::GameManager::ComponentPriority::NORMAL);
   mManager->AddComponent(*tocList[1], dtGame::GameManager::ComponentPriority::LOWER);
   mManager->AddComponent(*tocList[2], dtGame::GameManager::ComponentPriority::HIGHER);
   mManager->AddComponent(*tocList[3], dtGame::GameManager::ComponentPriority::HIGHEST);
   mManager->AddComponent(*tocList[4], dtGame::GameManager::ComponentPriority::NORMAL);
   mManager->AddComponent(*tocList[5], dtGame::GameManager::ComponentPriority::LOWEST);
   mManager->AddComponent(*tocList[6], dtGame::GameManager::ComponentPriority::LOWER);
   mManager->AddComponent(*tocList[7], dtGame::GameManager::ComponentPriority::LOWEST);
   mManager->AddComponent(*tocList[8], dtGame::GameManager::ComponentPriority::HIGHEST);
   mManager->AddComponent(*tocList[9], dtGame::GameManager::ComponentPriority::HIGHER);

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
   mManager->GetAllComponents(toFill);
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

   dtCore::System::Instance()->Step();

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


void GameManagerTests::TestAddActor()
{

   dtCore::RefPtr<dtDAL::ActorType> type = mManager->FindActorType("ExampleActors","Test1Actor");
   for (int x = 0; x < 21; ++x)
   {
      if (x == 10)
         type = mManager->FindActorType("ExampleActors","Test2Actor");

      CPPUNIT_ASSERT(type != NULL);
      dtCore::RefPtr<dtGame::GameActorProxy> proxy;
      mManager->CreateActor(*type, proxy);
      CPPUNIT_ASSERT_MESSAGE("Proxy, the result of a dynamic_cast to dtGame::GameActorProxy, should not be NULL", proxy != NULL);
      CPPUNIT_ASSERT_MESSAGE("IsGameActorProxy should return true", proxy->IsGameActorProxy());

      mManager->CreateActor(type->GetCategory(), type->GetName(), proxy);
      CPPUNIT_ASSERT_MESSAGE("Proxy, the result of a dynamic_cast to dtGame::GameActorProxy, should not be NULL", proxy != NULL);
      CPPUNIT_ASSERT_MESSAGE("IsGameActorProxy should return true", proxy->IsGameActorProxy());


      if (x % 3 == 0)
      {
         CPPUNIT_ASSERT_MESSAGE("The proxy should have the GameManager pointer set to NULL", proxy->GetGameManager() == NULL);
         mManager->AddActor(*proxy, false, false);
         dtCore::RefPtr<dtDAL::ActorProxy> proxyFound = mManager->FindActorById(proxy->GetId());
         CPPUNIT_ASSERT(proxyFound != NULL);
         CPPUNIT_ASSERT(proxyFound.get() == proxy.get());
         CPPUNIT_ASSERT_MESSAGE("The proxy should have the GameManager pointer set", proxy->GetGameManager() == mManager.get());
         dtCore::RefPtr<dtGame::GameActorProxy> gameProxyFound = mManager->FindGameActorById(proxy->GetId());
         CPPUNIT_ASSERT(gameProxyFound != NULL);
         CPPUNIT_ASSERT(gameProxyFound.get() == proxy.get());
         CPPUNIT_ASSERT_MESSAGE("Actor should not be remote.", !proxy->IsRemote());
         CPPUNIT_ASSERT_MESSAGE("Actor should not be published.", !proxy->IsPublished());

         CPPUNIT_ASSERT_MESSAGE("The actor should have been added to the scene.",
            mManager->GetScene().GetDrawableIndex(proxy->GetActor()) != mManager->GetScene().GetNumberOfAddedDrawable());

         try
         {
            mManager->PublishActor(*proxy);
         }
         catch (const dtUtil::Exception& ex)
         {
            if (ex.TypeEnum() == dtGame::ExceptionEnum::ACTOR_IS_REMOTE)
               CPPUNIT_FAIL("Exception thrown saying the actor is remote, but the actor should not be remote.");
            else
               CPPUNIT_FAIL(std::string("Unknown Exception thrown publishing an actor: ") + ex.TypeEnum().GetName() + " " + ex.What());
         }
         CPPUNIT_ASSERT_MESSAGE("Actor should not be remote.", !proxy->IsRemote());
         CPPUNIT_ASSERT_MESSAGE("Actor should be published.", proxy->IsPublished());

         mManager->DeleteActor(*proxy);

         CPPUNIT_ASSERT_MESSAGE("The actor should still be in the scene.",
            mManager->GetScene().GetDrawableIndex(proxy->GetActor()) != mManager->GetScene().GetNumberOfAddedDrawable());

         CPPUNIT_ASSERT_MESSAGE("The proxy should still be in the game manager", mManager->FindGameActorById(proxy->GetId()) != NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should not have the GameManager pointer set to NULL", proxy->GetGameManager() != NULL);
         //have to send a from event to make the actor get deleted
         SLEEP(10);
         dtCore::System::Instance()->Step();

         CPPUNIT_ASSERT_MESSAGE("The actor should not be in the scene.",
            mManager->GetScene().GetDrawableIndex(proxy->GetActor()) == mManager->GetScene().GetNumberOfAddedDrawable());

         CPPUNIT_ASSERT_MESSAGE("The proxy should not still be in the game manager", mManager->FindGameActorById(proxy->GetId()) == NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should have the GameManager pointer set to NULL", proxy->GetGameManager() == NULL);
      }
      else if (x % 3 == 1)
      {
         try
         {
            mManager->AddActor(*proxy, true, true);
            CPPUNIT_FAIL("An actor may not be both remote and published.");
         }
         catch (const dtUtil::Exception& ex)
         {
            if (ex.TypeEnum() != dtGame::ExceptionEnum::ACTOR_IS_REMOTE)
               CPPUNIT_FAIL(std::string("Unknown Exception thrown publishing an actor: ") + ex.TypeEnum().GetName() + " " + ex.What());
            //Ok
         }

         try
         {
            mManager->AddActor(*proxy, true, false);
         }
         catch (const dtUtil::Exception& ex)
         {
            if (ex.TypeEnum() != dtGame::ExceptionEnum::ACTOR_IS_REMOTE)
               CPPUNIT_FAIL(std::string("Unknown Exception thrown publishing an actor: ") + ex.TypeEnum().GetName() + " " + ex.What());
            else
               CPPUNIT_FAIL("Exception thrown saying the actor is remote, but it should allowed for the actor to be remote.");
         }
         dtCore::RefPtr<dtDAL::ActorProxy> proxyFound = mManager->FindActorById(proxy->GetId());
         CPPUNIT_ASSERT(proxyFound != NULL);
         CPPUNIT_ASSERT(proxyFound.get() == proxy.get());
         dtCore::RefPtr<dtGame::GameActorProxy> gameProxyFound = mManager->FindGameActorById(proxy->GetId());
         CPPUNIT_ASSERT(gameProxyFound != NULL);
         CPPUNIT_ASSERT(gameProxyFound.get() == proxy.get());
         CPPUNIT_ASSERT_MESSAGE("Actor should be remote.", proxy->IsRemote());
         CPPUNIT_ASSERT_MESSAGE("Actor should not be published.", !proxy->IsPublished());
         CPPUNIT_ASSERT_MESSAGE("The actor should have been added to the scene.",
            mManager->GetScene().GetDrawableIndex(proxy->GetActor()) != mManager->GetScene().GetNumberOfAddedDrawable());

         try
         {
            mManager->PublishActor(*proxy);
            CPPUNIT_FAIL("An actor may not be published if it's remote.");
         }
         catch (const dtUtil::Exception& ex)
         {
            if (ex.TypeEnum() != dtGame::ExceptionEnum::ACTOR_IS_REMOTE)
               CPPUNIT_FAIL(std::string("Unknown Exception thrown publishing an actor: ") + ex.TypeEnum().GetName() + " " + ex.What());
            //OK
         }
         mManager->DeleteActor(*proxy);
         CPPUNIT_ASSERT_MESSAGE("The proxy should still be in the game manager", mManager->FindGameActorById(proxy->GetId()) != NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should not have the GameManager pointer set to NULL", proxy->GetGameManager() != NULL);
         CPPUNIT_ASSERT_MESSAGE("The actor should still be in the scene.",
            mManager->GetScene().GetDrawableIndex(proxy->GetActor()) != mManager->GetScene().GetNumberOfAddedDrawable());
         //have to send a from event to make the actor get deleted
         SLEEP(10);
         dtCore::System::Instance()->Step();

         CPPUNIT_ASSERT_MESSAGE("The actor should not still be in the scene.",
            mManager->GetScene().GetDrawableIndex(proxy->GetActor()) == mManager->GetScene().GetNumberOfAddedDrawable());
         CPPUNIT_ASSERT_MESSAGE("The proxy should not still be in the game manager", mManager->FindGameActorById(proxy->GetId()) == NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should have the GameManager pointer set to NULL", proxy->GetGameManager() == NULL);
      }
      else
      {

         try
         {
            mManager->AddActor(*proxy);
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(std::string("Unknown Exception thrown adding an actor: ") + ex.TypeEnum().GetName() + " " + ex.What());
         }
         dtCore::RefPtr<dtDAL::ActorProxy> proxyFound = mManager->FindActorById(proxy->GetId());
         CPPUNIT_ASSERT(proxyFound != NULL);
         CPPUNIT_ASSERT(proxyFound.get() == proxy.get());
         dtCore::RefPtr<dtGame::GameActorProxy> gameProxyFound = mManager->FindGameActorById(proxy->GetId());
         CPPUNIT_ASSERT_MESSAGE("The actor should not have been added as a game actor", gameProxyFound == NULL);
         CPPUNIT_ASSERT_MESSAGE("The actor should have been added to the scene.",
            mManager->GetScene().GetDrawableIndex(proxy->GetActor()) != mManager->GetScene().GetNumberOfAddedDrawable());

         try
         {
            mManager->PublishActor(*proxy);
            CPPUNIT_FAIL("An actor may not be published if it's not added as a game actor.");
         }
         catch (const dtUtil::Exception& ex)
         {
            if (ex.TypeEnum() != dtGame::ExceptionEnum::INVALID_ACTOR_STATE)
               CPPUNIT_FAIL(std::string("Unknown Exception thrown publishing an actor: ") + ex.TypeEnum().GetName() + " " + ex.What());
            //OK
         }
         mManager->DeleteActor(static_cast<dtDAL::ActorProxy&>(*proxy));
         CPPUNIT_ASSERT_MESSAGE("The proxy should not still be in the game manager", mManager->FindActorById(proxy->GetId()) == NULL);
         CPPUNIT_ASSERT_MESSAGE("The actor should not still be in the scene.",
            mManager->GetScene().GetDrawableIndex(proxy->GetActor()) == mManager->GetScene().GetNumberOfAddedDrawable());
      }
   }
}

void GameManagerTests::TestComplexScene()
{
   dtCore::RefPtr<dtDAL::ActorType> type = mManager->FindActorType("ExampleActors","Test1Actor");

   CPPUNIT_ASSERT(type != NULL);

   std::vector<dtCore::RefPtr<dtGame::GameActorProxy> > proxies;

   for (unsigned i = 0; i < 10; ++i)
   {
      proxies.push_back(dynamic_cast<dtGame::GameActorProxy*>(mManager->CreateActor(*type).get()));
   }

   CPPUNIT_ASSERT(proxies.size() == 10);
   for (unsigned i = 0; i < proxies.size(); ++i)
   {
      CPPUNIT_ASSERT_MESSAGE("Proxy, the result of a dynamic_cast to dtGame::GameActorProxy, should not be NULL", proxies[i] != NULL);
      //Add regular actors
      if (i < 5)
         mManager->AddActor(*proxies[i]);
      else
         //Add game actors
         mManager->AddActor(*proxies[i], false, false);

      CPPUNIT_ASSERT_MESSAGE("Actor should be in the scene.",
         mManager->GetScene().GetDrawableIndex(proxies[i]->GetActor()) != mManager->GetScene().GetNumberOfAddedDrawable());

   }


   dtCore::Scene& scene = mManager->GetScene();

   scene.RemoveDrawable(proxies[0]->GetActor());
   scene.RemoveDrawable(proxies[1]->GetActor());
   scene.RemoveDrawable(proxies[2]->GetActor());
   scene.RemoveDrawable(proxies[6]->GetActor());
   scene.RemoveDrawable(proxies[7]->GetActor());
   scene.RemoveDrawable(proxies[8]->GetActor());
   scene.RemoveDrawable(proxies[9]->GetActor());

   //Add the regular actors as children to the first game actor.
   proxies[5]->GetActor()->AddChild(proxies[0]->GetActor());
   proxies[5]->GetActor()->AddChild(proxies[1]->GetActor());
   proxies[5]->GetActor()->AddChild(proxies[2]->GetActor());

   //create a plain actor so we can make sure it doesn't get moved up.
   dtCore::RefPtr<dtCore::Physical> ph = new dtCore::Physical;

   //Add the game actors removed as a child to a regular actor.
   proxies[0]->GetActor()->AddChild(proxies[6]->GetActor());
   proxies[0]->GetActor()->AddChild(proxies[7]->GetActor());
   proxies[0]->GetActor()->AddChild(ph.get());

   proxies[1]->GetActor()->AddChild(proxies[8]->GetActor());
   proxies[1]->GetActor()->AddChild(proxies[9]->GetActor());

   //remove proxy 0 to make it's children move up one.
   mManager->DeleteActor(*proxies[0]);

   SLEEP(2);
   dtCore::System::Instance()->Step();

   //check current children.
   CPPUNIT_ASSERT_MESSAGE("proxy 0 should not be a child of proxy 5.",
      proxies[5]->GetActor()->GetChildIndex(proxies[0]->GetActor()) == proxies[5]->GetActor()->GetNumChildren());
   CPPUNIT_ASSERT_MESSAGE("proxy 1 should still be a child of proxy 5.",
      proxies[5]->GetActor()->GetChildIndex(proxies[1]->GetActor()) != proxies[5]->GetActor()->GetNumChildren());
   CPPUNIT_ASSERT_MESSAGE("proxy 0 should not be in the scene.",
      proxies[0]->GetActor()->GetSceneParent() == NULL);

   //check that old children of 0 were moved up when they are supposed to.
   CPPUNIT_ASSERT_MESSAGE("proxy 6 should now be a child of proxy 5.",
      proxies[5]->GetActor()->GetChildIndex(proxies[6]->GetActor()) != proxies[5]->GetActor()->GetNumChildren());
   CPPUNIT_ASSERT_MESSAGE("proxy 7 should now be a child of proxy 5.",
      proxies[5]->GetActor()->GetChildIndex(proxies[7]->GetActor()) != proxies[5]->GetActor()->GetNumChildren());
   CPPUNIT_ASSERT_MESSAGE("The physical actor should not be a child of proxy 5.",
      proxies[5]->GetActor()->GetChildIndex(ph.get()) == proxies[5]->GetActor()->GetNumChildren());

   //remove proxy 5 to make it's children move up one.
   mManager->DeleteActor(*proxies[5]);

   SLEEP(2);
   dtCore::System::Instance()->Step();

   unsigned currentInScene[] = { 1, 2, 3, 4, 6, 7 };

   //check current children.
   CPPUNIT_ASSERT_MESSAGE("proxy 5 should not be in the scene.",
      proxies[5]->GetActor()->GetSceneParent() == NULL);
   CPPUNIT_ASSERT_MESSAGE("proxy 0 should not be in the scene.",
      proxies[0]->GetActor()->GetSceneParent() == NULL);

   for (int i = 0; i < 6; ++i)
   {
      std::ostringstream ss;
      ss << "proxy[" << i << "] should be in the root of the scene.";
      CPPUNIT_ASSERT_MESSAGE(ss.str(),
         scene.GetDrawableIndex(proxies[currentInScene[i]]->GetActor()) != scene.GetNumberOfAddedDrawable());
   }

   //check that children of 1 are still that way.
   CPPUNIT_ASSERT_MESSAGE("proxy 8 should still be a child of proxy 1.",
      proxies[1]->GetActor()->GetChildIndex(proxies[8]->GetActor()) != proxies[1]->GetActor()->GetNumChildren());
   CPPUNIT_ASSERT_MESSAGE("proxy 9 should still be a child of proxy 1.",
      proxies[1]->GetActor()->GetChildIndex(proxies[9]->GetActor()) != proxies[1]->GetActor()->GetNumChildren());

}

void GameManagerTests::TestOnAddedToGM()
{
   dtCore::RefPtr<TestGMComponent> tc = new TestGMComponent;
   CPPUNIT_ASSERT_MESSAGE("OnAddedToGM should not be called until added to the GM.", 
      !(tc->mWasOnAddedToGMCalled));

   mManager->AddComponent(*tc.get(), dtGame::GameManager::ComponentPriority::NORMAL);
   CPPUNIT_ASSERT_MESSAGE("OnAddedToGM should be called when added to the GM.", 
      tc->mWasOnAddedToGMCalled);

   dtCore::System::Instance()->Step();
   CPPUNIT_ASSERT_MESSAGE("GM should have sent RESTARTED message on startup.", 
      tc->FindProcessMessageOfType(dtGame::MessageType::INFO_RESTARTED).valid());

   std::vector<dtCore::RefPtr<const dtGame::Message> > msgs = tc->GetReceivedProcessMessages();
   CPPUNIT_ASSERT_MESSAGE("INFO_RESTARTED should be very first message from GM. Always.", 
      msgs[0]->GetMessageType() == dtGame::MessageType::INFO_RESTARTED);
}

void GameManagerTests::TestTimers()
{
    dtCore::RefPtr<dtDAL::ActorType> type = mManager->FindActorType("ExampleActors","Test2Actor");
   CPPUNIT_ASSERT(type != NULL);

   dtCore::RefPtr<dtGame::GameActorProxy> proxy;
   mManager->CreateActor(*type, proxy);
   CPPUNIT_ASSERT_MESSAGE("Proxy, the result of a dynamic_cast to dtGame::GameActorProxy, should not be NULL", proxy != NULL);
   CPPUNIT_ASSERT_MESSAGE("IsGameActorProxy should return true", proxy->IsGameActorProxy());

   dtCore::RefPtr<TestGMComponent> tc = new TestGMComponent;
   mManager->AddComponent(*tc.get(), dtGame::GameManager::ComponentPriority::NORMAL);

   mManager->SetTimer("SimTimer1", proxy.get(), 0.001f);
   mManager->SetTimer("RepeatingTimer1", proxy.get(), 0.001f, true, true);

   dtCore::Timer_t expectedSimTime  = mManager->GetSimulationClockTime() + 1000;
   dtCore::Timer_t expectedRealTime = mManager->GetRealClockTime()       + 1000;
   SLEEP(2);
   dtCore::System::Instance()->Step();
   dtCore::Timer_t currentSimTime  = mManager->GetSimulationClockTime();
   dtCore::Timer_t currentRealTime = mManager->GetRealClockTime();

   CPPUNIT_ASSERT(currentSimTime  > expectedSimTime);
   CPPUNIT_ASSERT(currentRealTime > expectedRealTime);

   dtCore::Timer_t lateSimTime  = currentSimTime  - expectedSimTime;
   dtCore::Timer_t lateRealTime = currentRealTime - expectedRealTime;

   std::vector<dtCore::RefPtr<const dtGame::Message> > msgs = tc->GetReceivedProcessMessages();
   bool foundTimeMsg = false;

   for(unsigned int i = 0; i < msgs.size(); i++)
   {
      if(msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
      {
         const dtGame::TimerElapsedMessage *tem = static_cast<const dtGame::TimerElapsedMessage*> (msgs[i].get());

         if(tem->GetTimerName() == "SimTimer1")
            CPPUNIT_ASSERT_MESSAGE("The late time should be reasonably close to the expected late time",
               osg::equivalent(tem->GetLateTime(), float(lateSimTime) * 1e-6f, 1e-5f));
         else if(tem->GetTimerName() == "RepeatingTimer1")
            CPPUNIT_ASSERT_MESSAGE("The late time should be reasonably close to the expected late time",
               osg::equivalent(tem->GetLateTime(), float(lateRealTime) * 1e-6f, 1e-5f));

         foundTimeMsg = true;
      }
   }
   CPPUNIT_ASSERT_MESSAGE("There should have been a timer elapsed message found", foundTimeMsg);

   msgs.clear();
   tc->reset();
   SLEEP(2);
   dtCore::System::Instance()->Step();
   msgs = tc->GetReceivedProcessMessages();
   foundTimeMsg = false;
   for(unsigned int i = 0; i < msgs.size(); i++)
   {
      if(msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
      {
         const dtGame::TimerElapsedMessage *tem = static_cast<const dtGame::TimerElapsedMessage*>(msgs[i].get());
         if(tem->GetTimerName() == "RepeatingTimer1")
            foundTimeMsg = true;
      }
   }
   CPPUNIT_ASSERT_MESSAGE("The repeating timer should have been found again since it is repeating", foundTimeMsg);

   bool foundNonRepeatingTimeMsg = false;
   for(unsigned int i = 0; i < msgs.size(); i++)
   {
      if(msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
      {
         const dtGame::TimerElapsedMessage *tem = static_cast<const dtGame::TimerElapsedMessage*>(msgs[i].get());
         if(tem->GetTimerName() == "SimTimer1")
            foundNonRepeatingTimeMsg = true;
      }
   }
   CPPUNIT_ASSERT_MESSAGE("The non repeating timer fired, and should no longer be in the list", !foundNonRepeatingTimeMsg);

   // The sim timer was non repeating, should have been cleared
   mManager->ClearTimer("RepeatingTimer1", proxy.get());

   msgs.clear();
   tc->reset();
   SLEEP(2);
   dtCore::System::Instance()->Step();
   msgs = tc->GetReceivedProcessMessages();
   unsigned int msgCount = 0;
   for(unsigned int i = 0; i < msgs.size(); i++)
      if(msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
         ++msgCount;

   CPPUNIT_ASSERT_MESSAGE("The number of timer messages should be 0", msgCount == 0);

   const unsigned int numToTest = 20;
   for(unsigned int i = 0; i < numToTest; i++)
   {
      std::ostringstream ss;
      ss << "Timer" << i;
      mManager->SetTimer(ss.str(), NULL, 0.001f);
   }

   msgs.clear();
   tc->reset();

   SLEEP(5);
   dtCore::System::Instance()->Step();
   SLEEP(1);
   dtCore::System::Instance()->Step();

   msgs = tc->GetReceivedProcessMessages();
   msgCount = 0;
   for(unsigned int i = 0; i < msgs.size(); i++)
      if(msgs[i]->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
         ++msgCount;

   CPPUNIT_ASSERT_MESSAGE("The number of received messages should be equal to the number of timers set", msgCount == numToTest);
}
