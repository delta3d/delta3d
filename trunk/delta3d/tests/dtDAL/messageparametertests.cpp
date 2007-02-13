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
#include <prefix/dtgameprefix-src.h>
#include <cstdlib>
#include <iostream>
#include <osg/Math>
#include <osg/io_utils>
#include <dtUtil/log.h>
#include <dtUtil/datastream.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/actortype.h>
#include <dtGame/gamemanager.h>
#include <dtDAL/namedparameter.h>
#include <osg/Endian>
#include <cppunit/extensions/HelperMacros.h>

class NamedParameterTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(NamedParameterTests);

      CPPUNIT_TEST(TestNamedResourceParameter);

      CPPUNIT_TEST(TestNamedStringParameter);
      CPPUNIT_TEST(TestNamedEnumParameter);
      CPPUNIT_TEST(TestNamedBooleanParameter);
      CPPUNIT_TEST(TestNamedUnsignedCharParameter);
      CPPUNIT_TEST(TestNamedUnsignedIntParameter);
      CPPUNIT_TEST(TestNamedIntParameter);
      CPPUNIT_TEST(TestNamedUnsignedLongIntParameter);
      CPPUNIT_TEST(TestNamedLongIntParameter);
      CPPUNIT_TEST(TestNamedUnsignedShortIntParameter);
      CPPUNIT_TEST(TestNamedShortIntParameter);
      CPPUNIT_TEST(TestNamedFloatParameter);
      CPPUNIT_TEST(TestNamedDoubleParameter);

      CPPUNIT_TEST(TestNamedGroupParameterCopy);
      CPPUNIT_TEST(TestNamedGroupParameterStream);
      ///This test currently fails and is not being run.
      //CPPUNIT_TEST(TestNamedGroupParameterString);
      CPPUNIT_TEST(TestNamedGroupParameterWithProperty);

      CPPUNIT_TEST(TestNamedVec2Parameters);
      CPPUNIT_TEST(TestNamedVec3Parameters);
      CPPUNIT_TEST(TestNamedVec4Parameters);

      CPPUNIT_TEST(TestNamedActorParameter);


   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestNamedResourceParameter();
   void TestNamedStringParameter();
   void TestNamedEnumParameter();
   void TestNamedBooleanParameter();
   void TestNamedUnsignedCharParameter();
   void TestNamedUnsignedIntParameter();
   void TestNamedIntParameter();
   void TestNamedUnsignedLongIntParameter();
   void TestNamedLongIntParameter();
   void TestNamedUnsignedShortIntParameter();
   void TestNamedShortIntParameter();
   void TestNamedFloatParameter();
   void TestNamedDoubleParameter();

   void TestNamedGroupParameterCopy();
   void TestNamedGroupParameterStream();
   void TestNamedGroupParameterString();
   void TestNamedGroupParameterWithProperty();

   void TestNamedVec2Parameters();
   void TestNamedVec3Parameters();
   void TestNamedVec4Parameters();

   void TestNamedActorParameter();

   //this templated function can be used for an osg vector type and NamedVecParameter subclass.
   template <class VecType, class ParamType>
   void TestNamedVecParameter(int size)
   {
      dtCore::RefPtr<ParamType> param = new ParamType("a");
      
      unsigned int expectedPrecision = 2 * sizeof(param->GetValue()[0]) + 1;
      
      std::ostringstream ss;
      ss << expectedPrecision;

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Precision should be " + ss.str(), expectedPrecision, param->GetNumberPrecision());

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

      dtUtil::DataStream ds;
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
   void TestNamedVecParameterList(ValueType defaultValue, ValueType t1, ValueType t2,
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
      catch (dtUtil::Exception&) 
      {
      
      }

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
      catch (dtUtil::Exception&) 
      { 
      
      }

      try
      {
         dtCore::RefPtr<ParameterType> paramSingleValue =
            new ParameterType("a");
         param->CopyFrom(*paramSingleValue);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception&)
      { 
      
      }

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

      dtUtil::DataStream ds;
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
      catch (dtUtil::Exception&)
      { 
      
      }

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
      catch (dtUtil::Exception&) 
      {

      }

      try
      {
         dtCore::RefPtr<ParameterType> paramSingleValue =
            new ParameterType("a");
         param->CopyFrom(*paramSingleValue);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception&) 
      {

      }

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

      dtUtil::DataStream ds;
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

      dtCore::RefPtr<dtDAL::NamedResourceParameter> param =
         new dtDAL::NamedResourceParameter(dataType,"a",true);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should support a list of values.",
         param->IsList() == true);
      testList = param->GetValueList();

      try
      {
         param->GetValue();
         CPPUNIT_FAIL("Should not be allowed to get a single value from a parameter "
            "that contains a list.");
      }
      catch (dtUtil::Exception &) 
      { 
         // This is correct, do not cause an error
      }

      //This quick test ensures that we cannot copy a parameter with a single value
      //into a parameter supporting a list of values.  Only parameters that
      //both contain a list of values my be copied.
      try
      {
         dtCore::RefPtr<dtDAL::NamedResourceParameter> paramSingleValue =
            new dtDAL::NamedResourceParameter(dataType,"b");
         paramSingleValue->CopyFrom(*param);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception &) 
      { 
         // This is correct, do not cause an error
      }

      try
      {
         dtCore::RefPtr<dtDAL::NamedResourceParameter> paramSingleValue =
            new dtDAL::NamedResourceParameter(dataType,"b");
         param->CopyFrom(*paramSingleValue);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception &) 
      { 
         // This is correct, do not cause an error
      }

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

      dtCore::RefPtr<dtDAL::NamedResourceParameter> param2 =
            new dtDAL::NamedResourceParameter(dataType,"b",true);
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
      param2 = new dtDAL::NamedResourceParameter(dataType,"b",true);

      dtUtil::DataStream ds;
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
      param2 = new dtDAL::NamedResourceParameter(dataType,"b",true);
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
   static char* mTestActorLibrary;
   dtCore::RefPtr<dtGame::GameManager> mManager;

   dtCore::RefPtr<dtDAL::NamedGroupParameter> CreateNamedGroupParameter();
   dtCore::RefPtr<dtDAL::ActorProxy> mExampleActor;

   void TestNamedGroupParameter(dtDAL::NamedGroupParameter& groupParam);
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(NamedParameterTests);

char* NamedParameterTests::mTestActorLibrary="testActorLibrary";

void NamedParameterTests::setUp()
{
   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
   try
   {
      dtUtil::Log* logger;
      logger = &dtUtil::Log::GetInstance("MessageParameter");
      //logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);

      dtCore::Scene* scene = new dtCore::Scene();
      mManager = new dtGame::GameManager(*scene);
      mManager->LoadActorRegistry(mTestActorLibrary);
      
      mExampleActor = mManager->CreateActor("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT(mExampleActor.valid());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }

}

void NamedParameterTests::tearDown()
{
   if (mManager.valid())
   {
      try
      {
         dtCore::ObserverPtr<dtDAL::ActorProxy> observerExampleActor = mExampleActor.get();
         mExampleActor = NULL;
         mManager->UnloadActorRegistry(mTestActorLibrary);
         mManager = NULL;
         CPPUNIT_ASSERT(!observerExampleActor.valid());
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
      }
   }
}

void NamedParameterTests::TestNamedResourceParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedResourceParameter>  param = new dtDAL::NamedResourceParameter(dtDAL::DataType::STATIC_MESH, "a");
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

      dtCore::RefPtr<dtDAL::NamedResourceParameter> param2 = new dtDAL::NamedResourceParameter(dtDAL::DataType::STATIC_MESH, "b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", *param->GetValue() == *param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedResourceParameter> param3 = new dtDAL::NamedResourceParameter(dtDAL::DataType::STATIC_MESH, "b");

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

void NamedParameterTests::TestNamedStringParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedStringParameter>  param = new dtDAL::NamedStringParameter("a");
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

      dtCore::RefPtr<dtDAL::NamedStringParameter> param2 = new dtDAL::NamedStringParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedStringParameter> param3 = new dtDAL::NamedStringParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedStringParameter,std::string>(
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

void NamedParameterTests::TestNamedEnumParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedEnumParameter>  param = new dtDAL::NamedEnumParameter("a");
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

      dtCore::RefPtr<dtDAL::NamedEnumParameter> param2 = new dtDAL::NamedEnumParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedEnumParameter> param3 = new dtDAL::NamedEnumParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedEnumParameter,std::string>(
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

void NamedParameterTests::TestNamedBooleanParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedBooleanParameter>  param = new dtDAL::NamedBooleanParameter("a");
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

      dtCore::RefPtr<dtDAL::NamedBooleanParameter> param2 = new dtDAL::NamedBooleanParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      param->SetValue(false);
      dtCore::RefPtr<dtDAL::NamedBooleanParameter> param3 = new dtDAL::NamedBooleanParameter("b", true);

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedBooleanParameter,bool>(
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

void NamedParameterTests::TestNamedUnsignedCharParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedUnsignedCharParameter>  param = new dtDAL::NamedUnsignedCharParameter("a");
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

      dtCore::RefPtr<dtDAL::NamedUnsignedCharParameter> param2 = new dtDAL::NamedUnsignedCharParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedUnsignedCharParameter> param3 = new dtDAL::NamedUnsignedCharParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedUnsignedCharParameter,unsigned char>(
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

void NamedParameterTests::TestNamedUnsignedIntParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedUnsignedIntParameter>  param = new dtDAL::NamedUnsignedIntParameter("a");
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

      dtCore::RefPtr<dtDAL::NamedUnsignedIntParameter> param2 = new dtDAL::NamedUnsignedIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedUnsignedIntParameter> param3 = new dtDAL::NamedUnsignedIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedUnsignedIntParameter,unsigned int>(
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

void NamedParameterTests::TestNamedIntParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedIntParameter>  param = new dtDAL::NamedIntParameter("a");
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

      dtCore::RefPtr<dtDAL::NamedIntParameter> param2 = new dtDAL::NamedIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedIntParameter> param3 = new dtDAL::NamedIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedIntParameter,int>(
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

void NamedParameterTests::TestNamedUnsignedLongIntParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedUnsignedLongIntParameter>  param = new dtDAL::NamedUnsignedLongIntParameter("a");
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

      dtCore::RefPtr<dtDAL::NamedUnsignedLongIntParameter> param2 = new dtDAL::NamedUnsignedLongIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedUnsignedLongIntParameter> param3 = new dtDAL::NamedUnsignedLongIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedUnsignedLongIntParameter,unsigned long>(
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

void NamedParameterTests::TestNamedLongIntParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedLongIntParameter>  param = new dtDAL::NamedLongIntParameter("a");
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

      dtCore::RefPtr<dtDAL::NamedLongIntParameter> param2 = new dtDAL::NamedLongIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedLongIntParameter> param3 = new dtDAL::NamedLongIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedLongIntParameter,long>(
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

void NamedParameterTests::TestNamedUnsignedShortIntParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedUnsignedShortIntParameter>  param = new dtDAL::NamedUnsignedShortIntParameter("a");
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

      dtCore::RefPtr<dtDAL::NamedUnsignedShortIntParameter> param2 = new dtDAL::NamedUnsignedShortIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedUnsignedShortIntParameter> param3 = new dtDAL::NamedUnsignedShortIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedUnsignedShortIntParameter,unsigned short>(
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

void NamedParameterTests::TestNamedShortIntParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedShortIntParameter>  param = new dtDAL::NamedShortIntParameter("a");
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

      dtCore::RefPtr<dtDAL::NamedShortIntParameter> param2 = new dtDAL::NamedShortIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedShortIntParameter> param3 = new dtDAL::NamedShortIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedShortIntParameter,short>(
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

void NamedParameterTests::TestNamedFloatParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedFloatParameter>  param = new dtDAL::NamedFloatParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0.0", osg::equivalent(param->GetValue(), 0.0f, 1e-2f));

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Precision should be 9", (unsigned int)9, param->GetNumberPrecision());

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

      dtCore::RefPtr<dtDAL::NamedFloatParameter> param2 = new dtDAL::NamedFloatParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.",
         osg::equivalent(param->GetValue(), param2->GetValue(), 1e-2f));

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedFloatParameter> param3 = new dtDAL::NamedFloatParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(osg::equivalent(param->GetValue(), param3->GetValue(), 1e-2f));

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedFloatParameter,float>(
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

void NamedParameterTests::TestNamedDoubleParameter()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedDoubleParameter>  param = new dtDAL::NamedDoubleParameter("a");
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0.0", osg::equivalent(param->GetValue(), 0.0, 1e-2));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Precision should be 17", (unsigned int)17, param->GetNumberPrecision());

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

      dtCore::RefPtr<dtDAL::NamedDoubleParameter> param2 = new dtDAL::NamedDoubleParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.",
         osg::equivalent(param->GetValue(), param2->GetValue(), 1e-2));

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtDAL::NamedDoubleParameter> param3 = new dtDAL::NamedDoubleParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(osg::equivalent(param->GetValue(), param3->GetValue(), 1e-2));

      //Test the list version of the message parameter.
      TestParameterList<dtDAL::NamedDoubleParameter,double>(
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

void NamedParameterTests::TestNamedVec2Parameters()
{
   TestNamedVecParameter<osg::Vec2,  dtDAL::NamedVec2Parameter>(2);
   TestNamedVecParameter<osg::Vec2f, dtDAL::NamedVec2fParameter>(2);
   TestNamedVecParameter<osg::Vec2d, dtDAL::NamedVec2dParameter>(2);

   try
   {
      TestNamedVecParameterList<dtDAL::NamedVec2Parameter,osg::Vec2>(
         osg::Vec2(1,1),
         osg::Vec2(123,1423.234),osg::Vec2(11234.234,1.0),
         osg::Vec2(1234,-13241.294),osg::Vec2(1234.234,1.523),2
      );

      TestNamedVecParameterList<dtDAL::NamedVec2fParameter,osg::Vec2f>(
         osg::Vec2f(1.0f,1.0f),
         osg::Vec2f(123.0f,1423.234f),osg::Vec2(11234.234f,1.0f),
         osg::Vec2f(1234.0f,-13241.294f),osg::Vec2(1234.234f,1.523f),2
      );

      TestNamedVecParameterList<dtDAL::NamedVec2dParameter,osg::Vec2d>(
         osg::Vec2d(1,1),
         osg::Vec2d(123,1423.234),osg::Vec2d(11234.234,1.0),
         osg::Vec2d(1234,-13241.294),osg::Vec2d(1234.234,1.523),2
      );
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
//   catch(const std::exception &e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}

void NamedParameterTests::TestNamedVec3Parameters()
{
   TestNamedVecParameter<osg::Vec3,  dtDAL::NamedVec3Parameter>(3);
   TestNamedVecParameter<osg::Vec3f, dtDAL::NamedVec3fParameter>(3);
   TestNamedVecParameter<osg::Vec3d, dtDAL::NamedVec3dParameter>(3);
   TestNamedVecParameter<osg::Vec3, dtGame::RGBColorMessageParameter>(3);

   try
   {
      TestNamedVecParameterList<dtDAL::NamedVec3Parameter,osg::Vec3>(
         osg::Vec3(1,1,1),
         osg::Vec3(123,1423.234,23.2134),osg::Vec3(11234.234,1.0,23.21334),
         osg::Vec3(1234,-13241.294,2763.2134),osg::Vec3(1234.234,1.523,3423.2134),3
      );

      TestNamedVecParameterList<dtDAL::NamedVec3fParameter,osg::Vec3f>(
         osg::Vec3f(1,1,1),
         osg::Vec3f(123,1423.234,23.2134),osg::Vec3f(11234.234,1.0,23.21334),
         osg::Vec3f(1234,-13241.294,2763.2134),osg::Vec3f(1234.234,1.523,3423.2134),3
      );

      TestNamedVecParameterList<dtDAL::NamedVec3dParameter,osg::Vec3d>(
         osg::Vec3d(1,1,1),
         osg::Vec3d(123,1423.234,23.2134),osg::Vec3d(11234.234,1.0,23.21334),
         osg::Vec3d(1234,-13241.294,2763.2134),osg::Vec3d(1234.234,1.523,3423.2134),3
      );

      TestNamedVecParameterList<dtGame::RGBColorMessageParameter,osg::Vec3>(
         osg::Vec3(1,1,1),
         osg::Vec3(123,1423.234,23.2134),osg::Vec3(11234.234,1.0,23.21334),
         osg::Vec3(1234,-13241.294,2763.2134),osg::Vec3(1234.234,1.523,3423.2134),3
      );
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
//   catch(const std::exception &e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}

void NamedParameterTests::TestNamedVec4Parameters()
{
   TestNamedVecParameter<osg::Vec4,  dtDAL::NamedVec4Parameter>(4);
   TestNamedVecParameter<osg::Vec4f, dtDAL::NamedVec4fParameter>(4);
   TestNamedVecParameter<osg::Vec4d, dtDAL::NamedVec4dParameter>(4);
   TestNamedVecParameter<osg::Vec4,  dtGame::RGBAColorMessageParameter>(4);

   try
   {
      TestNamedVecParameterList<dtDAL::NamedVec4Parameter,osg::Vec4>(
         osg::Vec4(1,1,1,1),
         osg::Vec4(123,1423.234,234.2832,564345.634623),osg::Vec4(11234.234,1.0,10,1),
         osg::Vec4(1234,-13241.294,200.325,2352.152343),osg::Vec4(1234.234,1.523,1,2),4
      );

      TestNamedVecParameterList<dtDAL::NamedVec4fParameter,osg::Vec4f>(
        osg::Vec4f(1,1,1,1),
         osg::Vec4f(123,1423.234,234.2832,564345.634623),osg::Vec4f(11234.234,1.0,10,1),
         osg::Vec4f(1234,-13241.294,200.325,2352.152343),osg::Vec4f(1234.234,1.523,1,2),4
      );

      TestNamedVecParameterList<dtDAL::NamedVec4dParameter,osg::Vec4d>(
         osg::Vec4d(1,1,1,1),
         osg::Vec4d(123,1423.234,234.2832,564345.634623),osg::Vec4d(11234.234,1.0,10,1),
         osg::Vec4d(1234,-13241.294,200.325,2352.152343),osg::Vec4d(1234.234,1.523,1,2),4
      );

      TestNamedVecParameterList<dtGame::RGBAColorMessageParameter,osg::Vec4>(
         osg::Vec4(1,1,1,1),
         osg::Vec4(123,1423.234,234.2832,564345.634623),osg::Vec4(11234.234,1.0,10,1),
         osg::Vec4(1234,-13241.294,200.325,2352.152343),osg::Vec4(1234.234,1.523,1,2),4
      );
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
//   catch(const std::exception &e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}

void NamedParameterTests::TestNamedActorParameter()
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

      amp = dtGame::MessageParameter::CreateFromType(dtDAL::DataType::ACTOR, "testNamedActorParameter");

      CPPUNIT_ASSERT_MESSAGE("The actor message parameter should not be NULL", amp != NULL);

      amp->FromString(value);

      CPPUNIT_ASSERT_MESSAGE("The actor message parameter value should have been set correctly", amp->ToString() == value);

      //Test the list version of the message parameter.
      dtCore::UniqueId id1 = dtCore::UniqueId();
      dtCore::UniqueId id2 = dtCore::UniqueId();
      dtCore::UniqueId id3 = dtCore::UniqueId();
      dtCore::UniqueId id4 = dtCore::UniqueId();
      dtCore::UniqueId id5 = dtCore::UniqueId();
      TestParameterList<dtDAL::NamedActorParameter,dtCore::UniqueId>(
         id1,id2,id3,id4,id5);
   }
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
//   catch(const std::exception &e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}


dtCore::RefPtr<dtDAL::NamedGroupParameter> NamedParameterTests::CreateNamedGroupParameter()
{
   dtCore::RefPtr<dtDAL::NamedGroupParameter> groupParam = new dtDAL::NamedGroupParameter("test");
   
   groupParam->AddParameter(*new dtDAL::NamedStringParameter("test1"));
   groupParam->AddParameter("test2", dtDAL::DataType::DOUBLE);
   groupParam->AddParameter(*new dtDAL::NamedFloatParameter("test3"));
   groupParam->AddParameter("test4", dtDAL::DataType::INT);

   CPPUNIT_ASSERT_MESSAGE("Should have received 4 in list for group param messages" , groupParam->GetParameterCount() == 4);

   dtCore::RefPtr<dtDAL::NamedGroupParameter> internalGroup = 
      static_cast<dtDAL::NamedGroupParameter*>(groupParam->AddParameter("test5", dtDAL::DataType::GROUP));
   
   internalGroup->AddParameter(*new dtDAL::NamedStringParameter("test1"));
   internalGroup->AddParameter(*new dtDAL::NamedDoubleParameter("test9"));
   return groupParam;
}

void NamedParameterTests::TestNamedGroupParameter(dtDAL::NamedGroupParameter& groupParam)
{
   
   CPPUNIT_ASSERT(groupParam.GetParameter("test1") != NULL);
   CPPUNIT_ASSERT(groupParam.GetParameter("test1")->GetDataType() == dtDAL::DataType::STRING);

   CPPUNIT_ASSERT(groupParam.GetParameter("test2") != NULL);
   CPPUNIT_ASSERT(groupParam.GetParameter("test2")->GetDataType() == dtDAL::DataType::DOUBLE);

   CPPUNIT_ASSERT(groupParam.GetParameter("test3") != NULL);
   CPPUNIT_ASSERT(groupParam.GetParameter("test3")->GetDataType() == dtDAL::DataType::FLOAT);

   CPPUNIT_ASSERT(groupParam.GetParameter("test4") != NULL);
   CPPUNIT_ASSERT(groupParam.GetParameter("test4")->GetDataType() == dtDAL::DataType::INT);

   CPPUNIT_ASSERT(groupParam.GetParameter("test5") != NULL);
   CPPUNIT_ASSERT(groupParam.GetParameter("test5")->GetDataType() == dtDAL::DataType::GROUP);

   dtCore::RefPtr<dtDAL::NamedGroupParameter> internalGroup = 
      static_cast<dtDAL::NamedGroupParameter*>(groupParam.GetParameter("test5"));

   CPPUNIT_ASSERT(internalGroup->GetParameter("test1") != NULL);
   CPPUNIT_ASSERT(internalGroup->GetParameter("test1")->GetDataType() == dtDAL::DataType::STRING);

   CPPUNIT_ASSERT(internalGroup->GetParameter("test9") != NULL);
   CPPUNIT_ASSERT(internalGroup->GetParameter("test9")->GetDataType() == dtDAL::DataType::DOUBLE);


   CPPUNIT_ASSERT(groupParam.GetParameter("invalid-name") == NULL);
   
   //Now test getting parameters as a const group
   const dtDAL::NamedGroupParameter& gmp = groupParam;

   CPPUNIT_ASSERT(gmp.GetParameter("test1") != NULL);
   CPPUNIT_ASSERT(gmp.GetParameter("test1")->GetDataType() == dtDAL::DataType::STRING);

   CPPUNIT_ASSERT(gmp.GetParameter("invalid-name") == NULL);

   //get all the params in a vector
   std::vector<dtGame::MessageParameter*> toFill;
   groupParam.GetParameters(toFill);
   CPPUNIT_ASSERT_EQUAL(size_t(5), toFill.size());

   //get all the params in a const vector
   std::vector<const dtGame::MessageParameter*> toFillConst;
   groupParam.GetParameters(toFillConst);
   CPPUNIT_ASSERT_EQUAL(size_t(5), toFillConst.size());
}

void NamedParameterTests::TestNamedGroupParameterCopy()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedGroupParameter> groupParam = CreateNamedGroupParameter();
      TestNamedGroupParameter(*groupParam);
      dtCore::RefPtr<dtDAL::NamedGroupParameter> groupCopy = new dtDAL::NamedGroupParameter("testCopy");
      groupCopy->CopyFrom(*groupParam);
      TestNamedGroupParameter(*groupCopy);      
   }
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
//   catch(const std::exception &e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}

void NamedParameterTests::TestNamedGroupParameterWithProperty()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedGroupParameter> groupParam = CreateNamedGroupParameter();
      TestNamedGroupParameter(*groupParam);

      // Assign to a group property then read the value back out.
      dtDAL::GroupActorProperty* groupProp = static_cast<dtDAL::GroupActorProperty*>(mExampleActor->GetProperty("TestGroup"));
      CPPUNIT_ASSERT(groupProp != NULL);
      
      groupParam->ApplyValueToProperty(*groupProp);

      dtCore::RefPtr<dtDAL::NamedGroupParameter> groupCopy = new dtDAL::NamedGroupParameter("testCopy");
      groupCopy->SetFromProperty(*groupProp);

      TestNamedGroupParameter(*groupCopy);      
   }
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }

}

void NamedParameterTests::TestNamedGroupParameterStream()
{
   try
   {
      dtCore::RefPtr<dtDAL::NamedGroupParameter> groupParam = CreateNamedGroupParameter();
      TestNamedGroupParameter(*groupParam);

      dtUtil::DataStream ds;
      
      groupParam->ToDataStream(ds);

      dtCore::RefPtr<dtDAL::NamedGroupParameter> groupCopy = new dtDAL::NamedGroupParameter("testCopy");
      groupCopy->FromDataStream(ds);

      TestNamedGroupParameter(*groupCopy);      
   }
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }

}

void NamedParameterTests::TestNamedGroupParameterString()
{
   ///This test currently fails and is not being run.
   try
   {
      dtCore::RefPtr<dtDAL::NamedGroupParameter> groupParam = CreateNamedGroupParameter();
      TestNamedGroupParameter(*groupParam);

      std::string s;
      
      s = groupParam->ToString();

      dtCore::RefPtr<dtDAL::NamedGroupParameter> groupCopy = new dtDAL::NamedGroupParameter("testCopy");
      groupCopy->FromString(s);

      TestNamedGroupParameter(*groupCopy);      
   }
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }

}
