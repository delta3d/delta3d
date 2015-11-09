/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
* @author Eddie Johnson and David Guthrie and Curtiss Murphy
*/
#include <prefix/unittestprefix.h>

#include <dtABC/application.h>

#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>

#include <dtCore/actortype.h>
#include <dtCore/actoractorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/namedgroupparameter.h>
#include <dtCore/namedarrayparameter.h>
#include <dtCore/namedpropertycontainerparameter.h>
#include <dtCore/resourcedescriptor.h>

#include <dtGame/gamemanager.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/messageparameter.h>

#include <dtUtil/datastream.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>

#include <cppunit/extensions/HelperMacros.h>

#include <osg/Endian>
#include <osg/io_utils>
#include <osg/Math>

#include <cstdlib>
#include <iostream>

extern dtABC::Application& GetGlobalApplication();

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

      CPPUNIT_TEST(TestNamedPropertyContainerParameterWithProperty);

      CPPUNIT_TEST(TestNamedGroupParameterCopy);
      CPPUNIT_TEST(TestNamedGroupParameterStream);
      CPPUNIT_TEST(TestNamedGroupParameterString);
      CPPUNIT_TEST(TestNamedGroupParameterWithProperty);

      CPPUNIT_TEST(TestNamedArrayParameterCopy);
      CPPUNIT_TEST(TestNamedArrayParameterStream);
      CPPUNIT_TEST(TestNamedArrayParameterString);
      CPPUNIT_TEST(TestNamedArrayParameterWithProperty);

      CPPUNIT_TEST(TestNamedVec2Parameters);
      CPPUNIT_TEST(TestNamedVec3Parameters);
      CPPUNIT_TEST(TestNamedVec4Parameters);

      CPPUNIT_TEST(TestNamedActorParameter);

      CPPUNIT_TEST(TestNamedFloatToFromDataStream);
      CPPUNIT_TEST(TestNamedVec3ToFromDataStream);
      CPPUNIT_TEST(TestFloatByteSwappedTypeCasts);
      CPPUNIT_TEST(TestDoubleByteSwappedTypeCasts);

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

   void TestNamedPropertyContainerParameterWithProperty();

   void TestNamedGroupParameterCopy();
   void TestNamedGroupParameterStream();
   void TestNamedGroupParameterString();
   void TestNamedGroupParameterStreamOrStream(bool stream);
   void TestNamedGroupParameterWithProperty();

   void TestNamedArrayParameterCopy();
   void TestNamedArrayParameterStream();
   void TestNamedArrayParameterString();
   void TestNamedArrayParameterWithProperty();

   void TestNamedVec2Parameters();
   void TestNamedVec3Parameters();
   void TestNamedVec4Parameters();

   void TestNamedActorParameter();

   void TestNamedFloatToFromDataStream();
   void TestNamedVec3ToFromDataStream();
   void TestFloatByteSwappedTypeCasts();
   void TestDoubleByteSwappedTypeCasts();


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

   //////////////////////////////////////////////////////////////////
   /// This is diff from above in that is allows you to pass in values to more 
   /// thoroughly test the ToDataStream() & FromDataStream(). There was an error in here
   template <class VecType, class ParamType>
   void TestNamedVecParameterToFromStream(int size, VecType testValue, unsigned int iterationNum)
   {
      dtCore::RefPtr<ParamType> param = new ParamType("a");

      // Test the basics. Can we set/get the values.
      param->SetValue(testValue);
      VecType readValue = param->GetValue();
      for (int i = 0; i < size; ++i)
      {
         std::ostringstream ss1;
         ss1 << "Testing param - GetValue [" << readValue[i] << "] should match SetValue [" << testValue[i] << "].";
         CPPUNIT_ASSERT_MESSAGE(ss1.str(),osg::equivalent(testValue[i], readValue[i], 1e-2f));
      }

      dtUtil::DataStream ds;
      ds.SetForceLittleEndian(true);
      dtCore::RefPtr<ParamType> param2 = new ParamType("b");

      // Test the to/from data stream
      param->ToDataStream(ds);
      param2->FromDataStream(ds);
      readValue = param2->GetValue();
      for (int i = 0; i < size; ++i)
      {
         std::ostringstream ss1;
         ss1 << "Test #[" << iterationNum << "] vector#[" << i << "] FromDataStream value [" << readValue[i] << "] should match ToDataStream original [" << testValue[i] << "].";
         CPPUNIT_ASSERT_MESSAGE(ss1.str(), osg::equivalent(testValue[i], readValue[i], 1e-2f));
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

      for (i=0; i<size; ++i)
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

      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct.",
            osg::equivalent((double)testList[0][i], (double)t1[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct.",
            osg::equivalent((double)testList[1][i], (double)t2[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct.",
            osg::equivalent((double)testList[2][i], (double)t3[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct.",
            osg::equivalent((double)testList[3][i], (double)t4[i], 1e-2));
      }

      dtCore::RefPtr<ParameterType> param2 =
         new ParameterType("b",defaultValue,true);
      std::string holder = param->ToString();
      param2->FromString(holder);
      testList = param2->GetValueList();

      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after FromString().",
            osg::equivalent((double)testList[0][i], (double)t1[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after FromString().",
            osg::equivalent((double)testList[1][i], (double)t2[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after FromString().",
            osg::equivalent((double)testList[2][i], (double)t3[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after FromString().",
            osg::equivalent((double)testList[3][i], (double)t4[i], 1e-2));
      }

      param2 = NULL;
      param2 = new ParameterType("b",defaultValue,true);

      dtUtil::DataStream ds;
      param->ToDataStream(ds);
      param2->FromDataStream(ds);

      testList = param2->GetValueList();
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after FromDataString().",
            osg::equivalent((double)testList[0][i], (double)t1[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after FromDataString().",
            osg::equivalent((double)testList[1][i], (double)t2[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after FromDataString().",
            osg::equivalent((double)testList[2][i], (double)t3[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after FromDataString().",
            osg::equivalent((double)testList[3][i], (double)t4[i], 1e-2));
      }

      param2 = NULL;
      param2 = new ParameterType("b",defaultValue,true);
      param2->CopyFrom(*param);
      testList = param2->GetValueList();
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after param copy.",
            osg::equivalent((double)testList[0][i], (double)t1[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after param copy.",
            osg::equivalent((double)testList[1][i], (double)t2[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after param copy.",
            osg::equivalent((double)testList[2][i], (double)t3[i], 1e-2));
      }
      for (i = 0; i < size; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after param copy.",
            osg::equivalent((double)testList[3][i], (double)t4[i], 1e-2));
      }
   }

   /**
    * This templated method is used to test each of the different message
    * parameters when using their list form.
    */
   template <typename ParameterType>
   void TestParameterList(typename ParameterType::value_type defaultValue, typename ParameterType::value_type t1, typename ParameterType::value_type t2,
      typename ParameterType::value_type t3, typename ParameterType::value_type t4)
   {
      std::vector<typename ParameterType::value_type> valueList;
      std::vector<typename ParameterType::value_type> testList;

      dtCore::RefPtr<ParameterType> param =
         new ParameterType("a",defaultValue,true);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should support a list of values.",
         param->IsList() == true);
      testList = param->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("List should have the default value as its only element,",
         testList.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("First element should be \"defaultValue\"",
         testList[0] == defaultValue);

      CPPUNIT_ASSERT_THROW_MESSAGE("Should not be allowed to get a single value from a parameter "
            "that contains a list.", param->GetValue(), dtUtil::Exception);

      {
         //This quick test ensures that we cannot copy a parameter with a single value
         //into a parameter supporting a list of values.  Only parameters that
         //both contain a list of values my be copied.
         dtCore::RefPtr<ParameterType> paramSingleValue =
            new ParameterType("a");
         CPPUNIT_ASSERT_THROW_MESSAGE("Should not be allowed to copy a list parameter into a single value parameter.",
               paramSingleValue->CopyFrom(*param), dtUtil::Exception);
      }

      {
         dtCore::RefPtr<ParameterType> paramSingleValue =
            new ParameterType("a");
         CPPUNIT_ASSERT_THROW_MESSAGE("Should not be allowed to copy a single value parameter into a list parameter.",
               param->CopyFrom(*paramSingleValue), dtUtil::Exception);
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
   void TestResourceParameterList(dtCore::DataType& dataType,
      dtCore::ResourceDescriptor& t1, dtCore::ResourceDescriptor& t2,
      dtCore::ResourceDescriptor& t3, dtCore::ResourceDescriptor& t4)
   {
      std::vector<dtCore::ResourceDescriptor> valueList;
      std::vector<dtCore::ResourceDescriptor> testList;

      dtCore::RefPtr<dtCore::NamedResourceParameter> param =
         new dtCore::NamedResourceParameter(dataType,"a",true);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should support a list of values.",
         param->IsList() == true);
      testList = param->GetValueList();

      try
      {
         param->GetValue();
         CPPUNIT_FAIL("Should not be allowed to get a single value from a parameter "
            "that contains a list.");
      }
      catch (dtUtil::Exception&)
      {
         // This is correct, do not cause an error
      }

      //This quick test ensures that we cannot copy a parameter with a single value
      //into a parameter supporting a list of values.  Only parameters that
      //both contain a list of values my be copied.
      try
      {
         dtCore::RefPtr<dtCore::NamedResourceParameter> paramSingleValue =
            new dtCore::NamedResourceParameter(dataType,"b");
         paramSingleValue->CopyFrom(*param);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception&)
      {
         // This is correct, do not cause an error
      }

      try
      {
         dtCore::RefPtr<dtCore::NamedResourceParameter> paramSingleValue =
            new dtCore::NamedResourceParameter(dataType,"b");
         param->CopyFrom(*paramSingleValue);
         CPPUNIT_FAIL("Should not be allowed to copy a list parameter into a single value parameter.");
      }
      catch (dtUtil::Exception&)
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

      dtCore::RefPtr<dtCore::NamedResourceParameter> param2 =
            new dtCore::NamedResourceParameter(dataType,"b",true);
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
      param2 = new dtCore::NamedResourceParameter(dataType,"b",true);

      dtUtil::DataStream ds;
      param->ToDataStream(ds);
      param2->FromDataStream(ds);

      testList = param2->GetValueList();
      CPPUNIT_ASSERT_MESSAGE("List element 0 was not correct after FromDataStream().",
         testList[0] == t1);
      CPPUNIT_ASSERT_MESSAGE("List element 1 was not correct after FromDataStream().",
         testList[1] == t2);
      CPPUNIT_ASSERT_MESSAGE("List element 2 was not correct after FromDataStream().",
         testList[2] == t3);
      CPPUNIT_ASSERT_MESSAGE("List element 3 was not correct after FromDataStream().",
         testList[3] == t4);

      param2 = NULL;
      param2 = new dtCore::NamedResourceParameter(dataType,"b",true);
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
   static const char* mTestActorLibrary;
   dtCore::RefPtr<dtGame::GameManager> mManager;

   dtCore::RefPtr<dtCore::NamedGroupParameter> CreateNamedGroupParameter(bool createPropCont = false);
   dtCore::RefPtr<dtCore::NamedArrayParameter> CreateNamedArrayParameter();
   dtCore::RefPtr<dtCore::BaseActorObject> mExampleActor;

   void TestNamedGroupParameter(dtCore::NamedGroupParameter& groupParam);
   void TestNamedArrayParameter(dtCore::NamedArrayParameter& arrarParam);
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(NamedParameterTests);

const char* NamedParameterTests::mTestActorLibrary="testActorLibrary";

void NamedParameterTests::setUp()
{
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
   try
   {
      //dtUtil::Log* logger;
      //logger = &dtUtil::Log::GetInstance("MessageParameter");
      //logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);

      mManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
      mManager->SetApplication(GetGlobalApplication());
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
         dtCore::ObserverPtr<dtCore::BaseActorObject> observerExampleActor = mExampleActor.get();
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
      dtCore::RefPtr<dtCore::NamedResourceParameter>  param = new dtCore::NamedResourceParameter(dtCore::DataType::STATIC_MESH, "a");
      dtCore::ResourceDescriptor c(dtCore::DataType::STATIC_MESH.GetName() + ":hello1",
         dtCore::DataType::STATIC_MESH.GetName() + ":hello1");
      param->SetValue(c);
      dtCore::ResourceDescriptor r = param->GetValue();

      CPPUNIT_ASSERT_MESSAGE("MessageParameter should not be NULL", !r.IsEmpty());
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      std::string holder = param->ToString();
      std::string testValue = dtCore::DataType::STATIC_MESH.GetName() + ":helloA";
      dtCore::ResourceDescriptor temp(testValue, testValue);
      param->SetValue(temp);
      param->FromString(holder);

      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should not be NULL", !r.IsEmpty());
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should return the value that was set", c == r);

      dtCore::RefPtr<dtCore::NamedResourceParameter> param2 = new dtCore::NamedResourceParameter(dtCore::DataType::STATIC_MESH, "b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedResourceParameter> param3 = new dtCore::NamedResourceParameter(dtCore::DataType::STATIC_MESH, "b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter should not be NULL", !param->GetValue().IsEmpty());
      CPPUNIT_ASSERT_MESSAGE("MessageParameter copy should not be NULL", !param3->GetValue().IsEmpty());
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      CPPUNIT_ASSERT(param->FromString(testValue));
      r = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("Setting the resource descriptor with a single string value should work.",
         r == temp);

      //Test the list version of the resource message parameter...
      dtCore::ResourceDescriptor t1(dtCore::DataType::STATIC_MESH.GetName()
         + ":hello1", dtCore::DataType::STATIC_MESH.GetName() + ":hello1");
      dtCore::ResourceDescriptor t2(dtCore::DataType::TEXTURE.GetName()
         + ":hello2", dtCore::DataType::TEXTURE.GetName() + ":hello2");
      dtCore::ResourceDescriptor t3(dtCore::DataType::TERRAIN.GetName()
         + ":hello3", dtCore::DataType::TERRAIN.GetName() + ":hello3");
      dtCore::ResourceDescriptor t4(dtCore::DataType::SOUND.GetName()
         + ":hello4", dtCore::DataType::SOUND.GetName() + ":hello4");

      TestResourceParameterList(dtCore::DataType::STATIC_MESH,t1,t2,t3,t4);
   }
   catch (dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedStringParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedStringParameter>  param = new dtCore::NamedStringParameter("a");
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

      dtCore::RefPtr<dtCore::NamedStringParameter> param2 = new dtCore::NamedStringParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedStringParameter> param3 = new dtCore::NamedStringParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedStringParameter>(
         "defaultValue","hello","from","the","stringlist");
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedEnumParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedEnumParameter>  param = new dtCore::NamedEnumParameter("a");
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

      dtCore::RefPtr<dtCore::NamedEnumParameter> param2 = new dtCore::NamedEnumParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedEnumParameter> param3 = new dtCore::NamedEnumParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedEnumParameter>(
         "defaultValue","enum1","enum2","enum3","enum4");
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedBooleanParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedBooleanParameter>  param = new dtCore::NamedBooleanParameter("a");
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

      dtCore::RefPtr<dtCore::NamedBooleanParameter> param2 = new dtCore::NamedBooleanParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      param->SetValue(false);
      dtCore::RefPtr<dtCore::NamedBooleanParameter> param3 = new dtCore::NamedBooleanParameter("b", true);

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedBooleanParameter>(
         true,false,true,false,true);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedUnsignedCharParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedUnsignedCharParameter>  param = new dtCore::NamedUnsignedCharParameter("a");
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

      dtCore::RefPtr<dtCore::NamedUnsignedCharParameter> param2 = new dtCore::NamedUnsignedCharParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedUnsignedCharParameter> param3 = new dtCore::NamedUnsignedCharParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedUnsignedCharParameter>(
         100,1,3,4,232);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedUnsignedIntParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedUnsignedIntParameter>  param = new dtCore::NamedUnsignedIntParameter("a");
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

      dtCore::RefPtr<dtCore::NamedUnsignedIntParameter> param2 = new dtCore::NamedUnsignedIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedUnsignedIntParameter> param3 = new dtCore::NamedUnsignedIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedUnsignedIntParameter>(
         100,1,3,4,232);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedIntParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedIntParameter>  param = new dtCore::NamedIntParameter("a");
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

      dtCore::RefPtr<dtCore::NamedIntParameter> param2 = new dtCore::NamedIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedIntParameter> param3 = new dtCore::NamedIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedIntParameter>(
         -100,-1,-3,-4,-232);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedUnsignedLongIntParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedUnsignedLongIntParameter>  param = new dtCore::NamedUnsignedLongIntParameter("a");
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

      dtCore::RefPtr<dtCore::NamedUnsignedLongIntParameter> param2 = new dtCore::NamedUnsignedLongIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedUnsignedLongIntParameter> param3 = new dtCore::NamedUnsignedLongIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedUnsignedLongIntParameter>(
         10089,167,3784,4456323,23264);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedLongIntParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedLongIntParameter>  param = new dtCore::NamedLongIntParameter("a");
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

      dtCore::RefPtr<dtCore::NamedLongIntParameter> param2 = new dtCore::NamedLongIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedLongIntParameter> param3 = new dtCore::NamedLongIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedLongIntParameter>(
         -10089,-167,-3784,-4456323,-23264);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedUnsignedShortIntParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedUnsignedShortIntParameter>  param = new dtCore::NamedUnsignedShortIntParameter("a");
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

      dtCore::RefPtr<dtCore::NamedUnsignedShortIntParameter> param2 = new dtCore::NamedUnsignedShortIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedUnsignedShortIntParameter> param3 = new dtCore::NamedUnsignedShortIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedUnsignedShortIntParameter>(
         100,1000,10000,2000,3000);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedShortIntParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedShortIntParameter>  param = new dtCore::NamedShortIntParameter("a");
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

      dtCore::RefPtr<dtCore::NamedShortIntParameter> param2 = new dtCore::NamedShortIntParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.", param->GetValue() == param2->GetValue());

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedShortIntParameter> param3 = new dtCore::NamedShortIntParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(param->GetValue() == param3->GetValue());

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedShortIntParameter>(
         -10089,-167,-3784,-423,-23264);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedFloatParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedFloatParameter>  param = new dtCore::NamedFloatParameter("a");
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

      dtCore::RefPtr<dtCore::NamedFloatParameter> param2 = new dtCore::NamedFloatParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.",
         osg::equivalent(param->GetValue(), param2->GetValue(), 1e-2f));

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedFloatParameter> param3 = new dtCore::NamedFloatParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(osg::equivalent(param->GetValue(), param3->GetValue(), 1e-2f));

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedFloatParameter>(
         -10089.0f,167.5f,-3784.24f,-4456323.3456f,-23264.0f);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedDoubleParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedDoubleParameter>  param = new dtCore::NamedDoubleParameter("a");
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

      dtCore::RefPtr<dtCore::NamedDoubleParameter> param2 = new dtCore::NamedDoubleParameter("b");
      param2->CopyFrom(*param);
      CPPUNIT_ASSERT_MESSAGE("Copied parameter should match the original.",
         osg::equivalent(param->GetValue(), param2->GetValue(), 1e-2));

      dtUtil::DataStream ds;
      dtCore::RefPtr<dtCore::NamedDoubleParameter> param3 = new dtCore::NamedDoubleParameter("b");

      param->ToDataStream(ds);
      param3->FromDataStream(ds);
      CPPUNIT_ASSERT(osg::equivalent(param->GetValue(), param3->GetValue(), 1e-2));

      //Test the list version of the message parameter.
      TestParameterList<dtCore::NamedDoubleParameter>(
         -10089.0, -167.5, 3784.24, -4456323.3456, 23264.0);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(e.what());
   }
}

void NamedParameterTests::TestNamedVec2Parameters()
{
   TestNamedVecParameter<osg::Vec2,  dtCore::NamedVec2Parameter>(2);
   TestNamedVecParameter<osg::Vec2f, dtCore::NamedVec2fParameter>(2);
   TestNamedVecParameter<osg::Vec2d, dtCore::NamedVec2dParameter>(2);

   try
   {
      TestNamedVecParameterList<dtCore::NamedVec2Parameter,osg::Vec2>(
         osg::Vec2(1,1),
         osg::Vec2(123,1423.234),osg::Vec2(11234.234,1.0),
         osg::Vec2(1234,-13241.294),osg::Vec2(1234.234,1.523),2
      );

      TestNamedVecParameterList<dtCore::NamedVec2fParameter,osg::Vec2f>(
         osg::Vec2f(1.0f,1.0f),
         osg::Vec2f(123.0f,1423.234f),osg::Vec2(11234.234f,1.0f),
         osg::Vec2f(1234.0f,-13241.294f),osg::Vec2(1234.234f,1.523f),2
      );

      TestNamedVecParameterList<dtCore::NamedVec2dParameter,osg::Vec2d>(
         osg::Vec2d(1,1),
         osg::Vec2d(123,1423.234),osg::Vec2d(11234.234,1.0),
         osg::Vec2d(1234,-13241.294),osg::Vec2d(1234.234,1.523),2
      );
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}

void NamedParameterTests::TestNamedVec3Parameters()
{
   TestNamedVecParameter<osg::Vec3,  dtCore::NamedVec3Parameter>(3);
   TestNamedVecParameter<osg::Vec3f, dtCore::NamedVec3fParameter>(3);
   TestNamedVecParameter<osg::Vec3d, dtCore::NamedVec3dParameter>(3);
   TestNamedVecParameter<osg::Vec3, dtGame::RGBColorMessageParameter>(3);

   try
   {
      TestNamedVecParameterList<dtCore::NamedVec3Parameter,osg::Vec3>(
         osg::Vec3(1,1,1),
         osg::Vec3(123,1423.234,23.2134),osg::Vec3(11234.234,1.0,23.21334),
         osg::Vec3(1234,-13241.294,2763.2134),osg::Vec3(1234.234,1.523,3423.2134),3
      );

      TestNamedVecParameterList<dtCore::NamedVec3fParameter,osg::Vec3f>(
         osg::Vec3f(1,1,1),
         osg::Vec3f(123,1423.234,23.2134),osg::Vec3f(11234.234,1.0,23.21334),
         osg::Vec3f(1234,-13241.294,2763.2134),osg::Vec3f(1234.234,1.523,3423.2134),3
      );

      TestNamedVecParameterList<dtCore::NamedVec3dParameter,osg::Vec3d>(
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
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}

void NamedParameterTests::TestNamedVec4Parameters()
{
   TestNamedVecParameter<osg::Vec4,  dtCore::NamedVec4Parameter>(4);
   TestNamedVecParameter<osg::Vec4f, dtCore::NamedVec4fParameter>(4);
   TestNamedVecParameter<osg::Vec4d, dtCore::NamedVec4dParameter>(4);
   TestNamedVecParameter<osg::Vec4,  dtGame::RGBAColorMessageParameter>(4);

   try
   {
      TestNamedVecParameterList<dtCore::NamedVec4Parameter,osg::Vec4>(
         osg::Vec4(1,1,1,1),
         osg::Vec4(123,1423.234,234.2832,564345.634623),osg::Vec4(11234.234,1.0,10,1),
         osg::Vec4(1234,-13241.294,200.325,2352.152343),osg::Vec4(1234.234,1.523,1,2),4
      );

      TestNamedVecParameterList<dtCore::NamedVec4fParameter,osg::Vec4f>(
        osg::Vec4f(1,1,1,1),
         osg::Vec4f(123,1423.234,234.2832,564345.634623),osg::Vec4f(11234.234,1.0,10,1),
         osg::Vec4f(1234,-13241.294,200.325,2352.152343),osg::Vec4f(1234.234,1.523,1,2),4
      );

      TestNamedVecParameterList<dtCore::NamedVec4dParameter,osg::Vec4d>(
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
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}

void NamedParameterTests::TestNamedActorParameter()
{
   try
   {
      dtCore::ActorRefPtrVector container;
      std::vector<dtCore::RefPtr<dtCore::ActorType> > types;

      dtCore::RefPtr<dtCore::BaseActorObject> ap = mManager->CreateActor("dtcore.examples", "Test All Properties");

      dtCore::ActorActorProperty* aap = NULL;

      std::vector<const dtCore::ActorProperty*> props;
      ap->GetPropertyList(props);
      for (unsigned int j = 0; j < props.size(); ++j)
      {
         dtCore::DataType& dt = props[j]->GetPropertyType();
         if (dt == dtCore::DataType::ACTOR)
            aap = static_cast<dtCore::ActorActorProperty*> (const_cast<dtCore::ActorProperty*>(props[j]));
      }

      CPPUNIT_ASSERT_MESSAGE("The actor actor property should not be NULL", aap != NULL);

      std::string value = aap->ToString();

      dtCore::RefPtr<dtGame::MessageParameter> amp = NULL;

      amp = dtCore::NamedParameter::CreateFromType(dtCore::DataType::ACTOR, "testNamedActorParameter");

      CPPUNIT_ASSERT_MESSAGE("The actor message parameter should not be NULL", amp != NULL);

      amp->FromString(value);

      CPPUNIT_ASSERT_MESSAGE("The actor message parameter value should have been set correctly", amp->ToString() == value);

      //Test the list version of the message parameter.
      dtCore::UniqueId id1 = dtCore::UniqueId();
      dtCore::UniqueId id2 = dtCore::UniqueId();
      dtCore::UniqueId id3 = dtCore::UniqueId();
      dtCore::UniqueId id4 = dtCore::UniqueId();
      dtCore::UniqueId id5 = dtCore::UniqueId();
      TestParameterList<dtCore::NamedActorParameter>(
         id1,id2,id3,id4,id5);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}

void NamedParameterTests::TestNamedPropertyContainerParameterWithProperty()
{
   try
   {
//      dtCore::RefPtr<dtCore::NamedGroupParameter> groupParam = CreateNamedGroupParameter();
//      TestNamedGroupParameter(*groupParam);
//
//      // Assign to a group property then read the value back out.
//      //dtCore::GroupActorProperty* groupProp = dynamic_cast<dtCore::GroupActorProperty*>(mExampleActor->GetProperty("TestGroup"));
//      dtCore::ActorProperty* prop = mExampleActor->GetProperty("TestGroup");
//      CPPUNIT_ASSERT(prop != NULL);
//
//      groupParam->ApplyValueToProperty(*prop);
//
//      dtCore::RefPtr<dtCore::NamedGroupParameter> groupCopy = new dtCore::NamedGroupParameter("testCopy");
//      groupCopy->SetFromProperty(*prop);
//
//      TestNamedGroupParameter(*groupCopy);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }

}

dtCore::RefPtr<dtCore::NamedGroupParameter> NamedParameterTests::CreateNamedGroupParameter(bool createPropCont)
{
   dtCore::RefPtr<dtCore::NamedGroupParameter> groupParam;
   if (createPropCont)
   {
      groupParam = new dtCore::NamedPropertyContainerParameter("test");
   }
   else
   {
      groupParam = new dtCore::NamedGroupParameter("test");
   }

   groupParam->AddParameter(*new dtCore::NamedStringParameter("test1"));
   groupParam->AddParameter("test2", dtCore::DataType::DOUBLE);
   groupParam->AddParameter(*new dtCore::NamedFloatParameter("test3"));
   groupParam->AddParameter("test4", dtCore::DataType::INT);

   CPPUNIT_ASSERT_MESSAGE("Should have received 4 in list for group param messages" , groupParam->GetParameterCount() == 4);

   dtCore::RefPtr<dtCore::NamedGroupParameter> internalGroup =
      static_cast<dtCore::NamedGroupParameter*>(groupParam->AddParameter("test5", dtCore::DataType::GROUP));

   internalGroup->AddParameter(*new dtCore::NamedStringParameter("test1"));
   internalGroup->AddParameter(*new dtCore::NamedDoubleParameter("test9"));
   return groupParam;
}

void NamedParameterTests::TestNamedGroupParameter(dtCore::NamedGroupParameter& groupParam)
{
   CPPUNIT_ASSERT(groupParam.GetParameter("test1") != NULL);
   CPPUNIT_ASSERT(groupParam.GetParameter("test1")->GetDataType() == dtCore::DataType::STRING);

   CPPUNIT_ASSERT(groupParam.GetParameter("test2") != NULL);
   CPPUNIT_ASSERT(groupParam.GetParameter("test2")->GetDataType() == dtCore::DataType::DOUBLE);

   CPPUNIT_ASSERT(groupParam.GetParameter("test3") != NULL);
   CPPUNIT_ASSERT(groupParam.GetParameter("test3")->GetDataType() == dtCore::DataType::FLOAT);

   CPPUNIT_ASSERT(groupParam.GetParameter("test4") != NULL);
   CPPUNIT_ASSERT(groupParam.GetParameter("test4")->GetDataType() == dtCore::DataType::INT);

   CPPUNIT_ASSERT(groupParam.GetParameter("test5") != NULL);
   CPPUNIT_ASSERT(groupParam.GetParameter("test5")->GetDataType() == dtCore::DataType::GROUP);

   dtCore::RefPtr<dtCore::NamedGroupParameter> internalGroup =
      static_cast<dtCore::NamedGroupParameter*>(groupParam.GetParameter("test5"));

   CPPUNIT_ASSERT(internalGroup->GetParameter("test1") != NULL);
   CPPUNIT_ASSERT(internalGroup->GetParameter("test1")->GetDataType() == dtCore::DataType::STRING);

   CPPUNIT_ASSERT(internalGroup->GetParameter("test9") != NULL);
   CPPUNIT_ASSERT(internalGroup->GetParameter("test9")->GetDataType() == dtCore::DataType::DOUBLE);


   CPPUNIT_ASSERT(groupParam.GetParameter("invalid-name") == NULL);

   //Now test getting parameters as a const group
   const dtCore::NamedGroupParameter& gmp = groupParam;

   CPPUNIT_ASSERT(gmp.GetParameter("test1") != NULL);
   CPPUNIT_ASSERT(gmp.GetParameter("test1")->GetDataType() == dtCore::DataType::STRING);

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
      dtCore::RefPtr<dtCore::NamedGroupParameter> groupParam = CreateNamedGroupParameter();
      TestNamedGroupParameter(*groupParam);
      dtCore::RefPtr<dtCore::NamedGroupParameter> groupCopy = new dtCore::NamedGroupParameter("testCopy");
      groupCopy->CopyFrom(*groupParam);
      TestNamedGroupParameter(*groupCopy);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}

void NamedParameterTests::TestNamedGroupParameterWithProperty()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedGroupParameter> groupParam = CreateNamedGroupParameter();
      TestNamedGroupParameter(*groupParam);

      // Assign to a group property then read the value back out.
      //dtCore::GroupActorProperty* groupProp = dynamic_cast<dtCore::GroupActorProperty*>(mExampleActor->GetProperty("TestGroup"));
      dtCore::ActorProperty* prop = mExampleActor->GetProperty("TestGroup");
      CPPUNIT_ASSERT(prop != NULL);

      groupParam->ApplyValueToProperty(*prop);

      dtCore::RefPtr<dtCore::NamedGroupParameter> groupCopy = new dtCore::NamedGroupParameter("testCopy");
      groupCopy->SetFromProperty(*prop);

      TestNamedGroupParameter(*groupCopy);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }

}

void NamedParameterTests::TestNamedGroupParameterStreamOrStream(bool stream)
{
   try
   {
      dtCore::RefPtr<dtCore::NamedGroupParameter> groupParam = CreateNamedGroupParameter();
      TestNamedGroupParameter(*groupParam);

      dtUtil::DataStream ds;
      std::string data;

      if (stream)
         groupParam->ToDataStream(ds);
      else
         data = groupParam->ToString();

      dtCore::RefPtr<dtCore::NamedGroupParameter> groupCopy = new dtCore::NamedGroupParameter("testCopy");

      dtCore::RefPtr<dtCore::NamedParameter> rightType = new dtCore::NamedStringParameter("test1");
      dtCore::RefPtr<dtCore::NamedParameter> wrongType = new dtCore::NamedFloatParameter("test2");
      dtCore::RefPtr<dtCore::NamedParameter> wrongListType = new dtCore::NamedFloatParameter("test3", 0.0f, true);
      groupCopy->AddParameter(*rightType);
      groupCopy->AddParameter(*wrongType);
      groupCopy->AddParameter(*wrongListType);

      if (stream)
         groupCopy->FromDataStream(ds);
      else
         groupCopy->FromString(data);


      CPPUNIT_ASSERT_MESSAGE("It should have reused the parameter if it matched the type.", rightType == groupCopy->GetParameter("test1"));
      CPPUNIT_ASSERT(wrongType != groupCopy->GetParameter("test2"));
      CPPUNIT_ASSERT(wrongListType != groupCopy->GetParameter("test3"));

      TestNamedGroupParameter(*groupCopy);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

void NamedParameterTests::TestNamedGroupParameterStream()
{
   TestNamedGroupParameterStreamOrStream(true);
}

void NamedParameterTests::TestNamedGroupParameterString()
{
   TestNamedGroupParameterStreamOrStream(false);

}
dtCore::RefPtr<dtCore::NamedArrayParameter> NamedParameterTests::CreateNamedArrayParameter()
{
   dtCore::RefPtr<dtCore::NamedArrayParameter> arrayParam;
   arrayParam = new dtCore::NamedArrayParameter("test");

   arrayParam->AddEmptyIndex();
   arrayParam->AddParameter(*new dtCore::NamedStringParameter("test1"));
   arrayParam->AddEmptyIndex();
   arrayParam->AddParameter("test2", dtCore::DataType::DOUBLE);
   arrayParam->AddParameter(*new dtCore::NamedFloatParameter("test3"));
   arrayParam->AddParameter("test4", dtCore::DataType::INT);
   arrayParam->AddParameter("test5", dtCore::DataType::VEC3);
   arrayParam->AddEmptyIndex();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Should have received 8 in list for group param messages" , size_t(8), arrayParam->GetSize());

   dtCore::RefPtr<dtCore::NamedArrayParameter> internalArray =
      static_cast<dtCore::NamedArrayParameter*>(arrayParam->AddParameter("test6", dtCore::DataType::ARRAY));

   internalArray->AddParameter(*new dtCore::NamedStringParameter("test1"));
   internalArray->AddEmptyIndex();
   internalArray->AddParameter(*new dtCore::NamedDoubleParameter("test9"));
   return arrayParam;
}

///////////////////////////////////////////////////////////////////////
void NamedParameterTests::TestNamedArrayParameter(dtCore::NamedArrayParameter& arrayParam)
{
   CPPUNIT_ASSERT_EQUAL(size_t(9), arrayParam.GetSize());

   CPPUNIT_ASSERT(arrayParam.GetParameter(0) == NULL);
   CPPUNIT_ASSERT(arrayParam.GetParameter(2) == NULL);
   CPPUNIT_ASSERT(arrayParam.GetParameter(7) == NULL);

   CPPUNIT_ASSERT(arrayParam.GetParameter(1) != NULL);
   CPPUNIT_ASSERT(arrayParam.GetParameter(1)->GetDataType() == dtCore::DataType::STRING);

   CPPUNIT_ASSERT(arrayParam.GetParameter(3) != NULL);
   CPPUNIT_ASSERT(arrayParam.GetParameter(3)->GetDataType() == dtCore::DataType::DOUBLE);

   CPPUNIT_ASSERT(arrayParam.GetParameter(4) != NULL);
   CPPUNIT_ASSERT(arrayParam.GetParameter(4)->GetDataType() == dtCore::DataType::FLOAT);

   CPPUNIT_ASSERT(arrayParam.GetParameter(5) != NULL);
   CPPUNIT_ASSERT(arrayParam.GetParameter(5)->GetDataType() == dtCore::DataType::INT);

   CPPUNIT_ASSERT(arrayParam.GetParameter(6) != NULL);
   CPPUNIT_ASSERT(arrayParam.GetParameter(6)->GetDataType() == dtCore::DataType::VEC3);

   CPPUNIT_ASSERT(arrayParam.GetParameter(8) != NULL);
   CPPUNIT_ASSERT(arrayParam.GetParameter(8)->GetDataType() == dtCore::DataType::ARRAY);

   dtCore::RefPtr<dtCore::NamedArrayParameter> internalArray =
      dynamic_cast<dtCore::NamedArrayParameter*>(arrayParam.GetParameter(8));

   CPPUNIT_ASSERT(internalArray->GetParameter(0) != NULL);
   CPPUNIT_ASSERT(internalArray->GetParameter(0)->GetDataType() == dtCore::DataType::STRING);

   CPPUNIT_ASSERT(internalArray->GetParameter(1) == NULL);

   CPPUNIT_ASSERT(internalArray->GetParameter(2) != NULL);
   CPPUNIT_ASSERT(internalArray->GetParameter(2)->GetDataType() == dtCore::DataType::DOUBLE);


   CPPUNIT_ASSERT(arrayParam.GetParameter(51) == NULL);

   //Now test getting parameters as a const group
   const dtCore::NamedArrayParameter& amp = arrayParam;

   CPPUNIT_ASSERT(amp.GetParameter(0) == NULL);

   CPPUNIT_ASSERT(amp.GetParameter(1) != NULL);
   CPPUNIT_ASSERT(amp.GetParameter(1)->GetDataType() == dtCore::DataType::STRING);

   CPPUNIT_ASSERT(amp.GetParameter(21) == NULL);

}
///////////////////////////////////////////////////////////////////////

void NamedParameterTests::TestNamedArrayParameterCopy()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedArrayParameter> arrayParam = CreateNamedArrayParameter();
      TestNamedArrayParameter(*arrayParam);
      dtCore::RefPtr<dtCore::NamedArrayParameter> arrayCopy = new dtCore::NamedArrayParameter("testCopy");
      arrayCopy->CopyFrom(*arrayParam);
      TestNamedArrayParameter(*arrayCopy);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(e.what());
//   }
}

///////////////////////////////////////////////////////////////////////
void NamedParameterTests::TestNamedArrayParameterWithProperty()
{
   try
   {
      dtUtil::RefString testName("joe");
      dtCore::RefPtr<dtCore::NamedArrayParameter> arrayParam = new dtCore::NamedArrayParameter(testName);
      arrayParam->AddParameter(*new dtCore::NamedStringParameter(testName, "hi"));
      arrayParam->AddParameter(*new dtCore::NamedStringParameter(testName, "Mom"));
      arrayParam->AddParameter(*new dtCore::NamedStringParameter(testName, "hi"));
      arrayParam->AddParameter(*new dtCore::NamedStringParameter(testName, "Dad"));

      // Assign to a array property then read the value back out.
      //dtCore::GroupActorProperty* arrayProp = dynamic_cast<dtCore::GroupActorProperty*>(mExampleActor->GetProperty("TestGroup"));
      dtCore::ActorProperty* prop = mExampleActor->GetProperty("TestStringArray");
      CPPUNIT_ASSERT(prop != NULL);

      arrayParam->ApplyValueToProperty(*prop);

      dtCore::RefPtr<dtCore::NamedArrayParameter> arrayCopy = new dtCore::NamedArrayParameter(testName);
      arrayCopy->SetFromProperty(*prop);

      CPPUNIT_ASSERT_EQUAL(arrayParam->GetSize(), arrayCopy->GetSize());

      for (unsigned i = 0; i < arrayCopy->GetSize(); ++i)
      {
         dtCore::NamedStringParameter* np = dynamic_cast<dtCore::NamedStringParameter*>(arrayParam->GetParameter(i));
         dtCore::NamedStringParameter* npCopy = dynamic_cast<dtCore::NamedStringParameter*>(arrayCopy->GetParameter(i));

         CPPUNIT_ASSERT(np != NULL);
         CPPUNIT_ASSERT(npCopy != NULL);

         CPPUNIT_ASSERT_EQUAL(np->GetDataType(), npCopy->GetDataType());
         CPPUNIT_ASSERT_EQUAL(np->GetValue(), npCopy->GetValue());
      }

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }

}

///////////////////////////////////////////////////////////////////////
void NamedParameterTests::TestNamedArrayParameterStream()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedArrayParameter> arrayParam = CreateNamedArrayParameter();
      CPPUNIT_ASSERT_MESSAGE("The packing default should be true.", arrayParam->GetPackData());
      TestNamedArrayParameter(*arrayParam);

      dtUtil::DataStream ds;

      arrayParam->ToDataStream(ds);

      dtCore::RefPtr<dtCore::NamedArrayParameter> arrayCopy = new dtCore::NamedArrayParameter("testCopy");


      dtUtil::DataStream ds2(const_cast<char*>(ds.GetBuffer()), ds.GetBufferSize(), false);
      arrayCopy->FromDataStream(ds2);

      TestNamedArrayParameter(*arrayCopy);

      ds.Rewind();
      ds2.Rewind();

      arrayParam->SetPackData(false);
      arrayParam->ToDataStream(ds);

      dtUtil::DataStream ds3(const_cast<char*>(ds.GetBuffer()), ds.GetBufferSize(), false);

      arrayCopy->SetPackData(false);
      arrayCopy->FromDataStream(ds3);
      TestNamedArrayParameter(*arrayCopy);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }

}

///////////////////////////////////////////////////////////////////////
void NamedParameterTests::TestNamedArrayParameterString()
{
   try
   {
      dtCore::RefPtr<dtCore::NamedArrayParameter> arrayParam = CreateNamedArrayParameter();
      CPPUNIT_ASSERT_MESSAGE("The packing default should be true.", arrayParam->GetPackData());
      TestNamedArrayParameter(*arrayParam);

      std::string s;

      s = arrayParam->ToString();

      dtCore::RefPtr<dtCore::NamedArrayParameter> arrayCopy = new dtCore::NamedArrayParameter("testCopy");
      arrayCopy->FromString(s);

      TestNamedArrayParameter(*arrayCopy);

      arrayParam->SetPackData(false);
      s = arrayParam->ToString();
      arrayCopy->SetPackData(false);
      arrayCopy->FromString(s);
      TestNamedArrayParameter(*arrayCopy);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }

}

///////////////////////////////////////////////////////////////////////
void NamedParameterTests::TestNamedFloatToFromDataStream()
{
   float baseValue(41.0213f);

   // do a loop - 1000 times is better, but 250 will eventually catch the error
   for (unsigned int counter = 0; counter < 250; counter ++)
   {
      // multiply base by some number
      float randMult = dtUtil::RandFloat(0.001f, 2.003f);
      float testValue = baseValue * randMult;

      dtCore::RefPtr<dtCore::NamedFloatParameter> param = new dtCore::NamedFloatParameter("a");
      param->SetValue(testValue);
      CPPUNIT_ASSERT_MESSAGE("Param GetValue() should be the same", 
         osg::equivalent(testValue, param->GetValue(), 1e-2f));


      // Test ToFrom Data Stream
      dtUtil::DataStream ds;
      //ds.SetForceLittleEndian(true);
      dtCore::RefPtr<dtCore::NamedFloatParameter> param2 = new dtCore::NamedFloatParameter("b");

      //if (counter == 29)
      //   std::cout << " used to fail here. " << std::endl;
      param->ToDataStream(ds);
      param2->FromDataStream(ds);
      std::ostringstream ss1;
      ss1.precision(32);
      ss1 << "Float DataStream Test #[" << counter << "] - GetValue [" << 
         param2->GetValue() << "] should match SetValue [" << testValue << "].";
      CPPUNIT_ASSERT_MESSAGE(ss1.str(), osg::equivalent(testValue, param2->GetValue(), 1e-2f));
   }
}

///////////////////////////////////////////////////////////////////////
void NamedParameterTests::TestFloatByteSwappedTypeCasts()
{
   // This tests the most basic part of assigning a number that was previously 
   // byte swapped. This is for the fix in datastream.cpp.

   char tempBuffer[4];

   float baseValue(41.0213f);
   // do a loop - 1000 times is better, but 250 will eventually catch the error
   for (unsigned int counter = 0; counter < 250; counter ++)
   {
      // multiply base by some number
      float randMult = dtUtil::RandFloat(0.001f, 2.003f);
      float testValue = baseValue * randMult;

      // Put our value into the buffer & byte swap it
      memcpy(tempBuffer, &testValue, sizeof(float));
      osg::swapBytes4(tempBuffer);
      
      // Pull our value out of the buffer, THEN byte swap (the order matters for the test)
      float resultValue;
      // The following exact syntax used to be in datastream.cpp. It doesn't work. See DataStream::Read(float) for more info.
      //resultValue = *((float *)((char *)tempBuffer));
      memcpy(&resultValue, tempBuffer, sizeof(float));
      osg::swapBytes((char*)&resultValue, sizeof(float));

      std::ostringstream ss1;
      ss1.precision(32);
      ss1 << "Float Byte Swapping Test #[" << counter << "] - Result [" << 
         resultValue << "] should match the test [" << testValue << "].";
      CPPUNIT_ASSERT_MESSAGE(ss1.str(), osg::equivalent(testValue, resultValue, 1e-2f));
   }
}

///////////////////////////////////////////////////////////////////////
void NamedParameterTests::TestDoubleByteSwappedTypeCasts()
{
   // This tests the most basic part of assigning a number that was previously 
   // byte swapped. This is for the fix in datastream.cpp.

   char tempBuffer[sizeof(double)];

   srand(time(NULL));

   double baseValue(41.02131118583832828583921558128f);
   // do a loop - 1000 times is better, but ... and, double's never fail anyway.
   for (unsigned int counter = 0; counter < 250; counter ++)
   {
      // multiply base by some number
      double randMult = (double) dtUtil::RandFloat(0.001f, 2.003f);
      double testValue = baseValue * randMult;

      // Put our value into the buffer & byte swap it
      memcpy(tempBuffer, &testValue, sizeof(double));
      osg::swapBytes(tempBuffer, sizeof(double));

      // Pull our value out of the buffer, THEN byte swap (the order matters for the test)
      double resultValue;
      // The following exact syntax used to be in datastream.cpp. It doesn't work. See DataStream::Read(float) for more info.
      resultValue = *((double *)((char *)tempBuffer));
      //memcpy(&resultValue, tempBuffer, sizeof(double));
      osg::swapBytes((char*)&resultValue, sizeof(double));

      std::ostringstream ss1;
      ss1.precision(32);
      ss1 << "DOUBLE Byte Swapping Test #[" << counter << "] - Result [" << 
         resultValue << "] should match the test [" << testValue << "].";
      CPPUNIT_ASSERT_MESSAGE(ss1.str(), osg::equivalent(testValue, resultValue, (double)1e-2f));
   }
}


///////////////////////////////////////////////////////////////////////
void NamedParameterTests::TestNamedVec3ToFromDataStream()
{
   // This only tests the float value - we are only worried about the to/from stream 
   // at this point. Other tests hit the float, double, etc...

   osg::Vec3f baseValue(196.0123f, 37.2345f, 1.02f);
   unsigned int counter;

   // do a loop - 1000 times is better, but 250 is faster and will eventually catch the error
   for (counter = 0; counter < 250; counter ++)
   {
      // multiply base by some number
      float randMult = dtUtil::RandFloat(0.001f, 2.003f);
      osg::Vec3f testValue = baseValue * randMult;

      TestNamedVecParameterToFromStream<osg::Vec3f,  dtCore::NamedVec3fParameter>(3, testValue, counter);
   }
}


