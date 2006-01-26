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

#include <iostream>
#include <osg/Math>
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

#include <cppunit/extensions/HelperMacros.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define sleep(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define sleep(milliseconds) usleep(((milliseconds) * 1000))
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

        CPPUNIT_TEST(TestMachineInfo);

        CPPUNIT_TEST(TestActorSearching);
        CPPUNIT_TEST(TestAddActor);
        CPPUNIT_TEST(TestAddRemoveComponents);
    
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

   void TestMachineInfo();

   void TestActorSearching();
   void TestAddActor();
   void TestAddRemoveComponents();

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


private:
   static char* mTestGameActorLibrary;
   static char* mTestActorLibrary;
   dtCore::RefPtr<dtGame::GameManager> mManager;
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GameManagerTests);

#if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
char* GameManagerTests::mTestGameActorLibrary="testGameActorLibraryd";
char* GameManagerTests::mTestActorLibrary="testActorLibraryd";
#else
char* GameManagerTests::mTestGameActorLibrary="testGameActorLibrary";
char* GameManagerTests::mTestActorLibrary="testActorLibrary";
#endif

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
      try {
         dtCore::System::Instance()->Start();
         mManager->DeleteAllActors();
         mManager->UnloadActorRegistry(mTestGameActorLibrary);
         mManager->UnloadActorRegistry(mTestActorLibrary);  
         mManager = NULL;  
      } catch (const dtUtil::Exception& e) {
         CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
      }
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
         longTestString.append(1, 'a' + (char)(i % 10));

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
   }
   catch(const dtUtil::Exception &e) 
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
}

void GameManagerTests::TestResourceMessageParameter()
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
}

void GameManagerTests::TestStringMessageParameter()
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

}

void GameManagerTests::TestEnumMessageParameter()
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
}


void GameManagerTests::TestBooleanMessageParameter()
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
}

void GameManagerTests::TestUnsignedCharMessageParameter()
{
   //std::cout << "CURTISS TEST TEST TEST TEST TEST CURTISS TEST TEST TEST TEST TEST CURTISS TEST TEST TEST TEST TEST CURTISS TEST TEST TEST TEST TEST CURTISS TEST TEST TEST TEST TEST ";

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
}

void GameManagerTests::TestUnsignedIntMessageParameter()
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
}

void GameManagerTests::TestIntMessageParameter()
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
}

void GameManagerTests::TestUnsignedLongIntMessageParameter()
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
}

void GameManagerTests::TestLongIntMessageParameter()
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
}

void GameManagerTests::TestUnsignedShortIntMessageParameter()
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
}

void GameManagerTests::TestShortIntMessageParameter()
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
}

void GameManagerTests::TestFloatMessageParameter()
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
}

void GameManagerTests::TestDoubleMessageParameter()
{
   dtCore::RefPtr<dtGame::DoubleMessageParameter>  param = new dtGame::DoubleMessageParameter("a");
   CPPUNIT_ASSERT_MESSAGE("MessageParameter should default to 0.0", osg::equivalent(param->GetValue(), 0.0, 1e-2));

   double c = 201.32;
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
}

void GameManagerTests::TestVec2MessageParameters()
{
   TestVecMessageParameter<osg::Vec2,  dtGame::Vec2MessageParameter>(2);
   TestVecMessageParameter<osg::Vec2f, dtGame::Vec2fMessageParameter>(2);
   TestVecMessageParameter<osg::Vec2d, dtGame::Vec2dMessageParameter>(2);
}

void GameManagerTests::TestVec3MessageParameters()
{
   TestVecMessageParameter<osg::Vec3,  dtGame::Vec3MessageParameter>(3);
   TestVecMessageParameter<osg::Vec3f, dtGame::Vec3fMessageParameter>(3);
   TestVecMessageParameter<osg::Vec3d, dtGame::Vec3dMessageParameter>(3);
}

void GameManagerTests::TestVec4MessageParameters()
{
   TestVecMessageParameter<osg::Vec4,  dtGame::Vec4MessageParameter>(4);
   TestVecMessageParameter<osg::Vec4f, dtGame::Vec4fMessageParameter>(4);
   TestVecMessageParameter<osg::Vec4d, dtGame::Vec4dMessageParameter>(4);
}

void GameManagerTests::TestActorMessageParameter()
{
   try
   {
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > container;
      std::vector<dtCore::RefPtr<dtDAL::ActorType> > types;
      
      mManager->GetActorTypes(types);

      for(unsigned int i = 0; i < types.size(); i++)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> ap = mManager->CreateActor(const_cast<dtDAL::ActorType&>(*types[i]));
         
         if(ap->IsGameActorProxy())
         {
            dtGame::GameActorProxy *gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
            if(gap == NULL)
               CPPUNIT_FAIL("Somehow the game actor proxy is NULL");

            mManager->AddActor(*gap, false, false);
         }
         else 
            mManager->AddActor(*ap.get());
      }
      
      mManager->GetAllActors(container);

      dtDAL::ActorActorProperty *aap = NULL;

      for(unsigned int i = 0; i < container.size(); i++)
      {
         std::vector<const dtDAL::ActorProperty*> props;
         container[i]->GetPropertyList(props);
         for(unsigned int j = 0; j < props.size(); j++)
         {
            dtDAL::DataType &dt = props[j]->GetPropertyType();
            if(dt == dtDAL::DataType::ACTOR)
               aap = static_cast<dtDAL::ActorActorProperty*> (const_cast<dtDAL::ActorProperty*>(props[j]));
         }
      }

      CPPUNIT_ASSERT_MESSAGE("The actor actor property should not be NULL", aap != NULL);

      std::string value = aap->GetStringValue();

      dtCore::RefPtr<dtGame::MessageParameter> amp = NULL;

      amp = dtGame::MessageParameter::CreateFromType(dtDAL::DataType::ACTOR, "testActorMessageParameter");
      
      CPPUNIT_ASSERT_MESSAGE("The actor message parameter should not be NULL", amp != NULL);

      amp->FromString(value);

      CPPUNIT_ASSERT_MESSAGE("The actor message parameter value should have been set correctly", amp->ToString() == value);
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
      dtGame::GameManager& gm = *mManager;

      std::vector<dtCore::RefPtr<dtDAL::ActorType> > typeVec;

      gm.GetActorTypes(typeVec);

      for(unsigned int i = 0; i < typeVec.size(); i++)
         gm.AddActor(*gm.CreateActor(*typeVec[i]));

      std::vector<const dtDAL::ActorType*> supportedTypes;
      gm.GetActorTypes(supportedTypes);
      CPPUNIT_ASSERT_MESSAGE("The number of supported actor types should not be 0", !supportedTypes.empty());

      gm.DeleteAllActors();

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

      CPPUNIT_ASSERT_MESSAGE("The number of proxies found is not equal to the number that exist", supportedProxies.size() == size);


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

      CPPUNIT_ASSERT(rc->GetGameManager() == NULL);
      CPPUNIT_ASSERT(dmc->GetGameManager() == NULL);

      mManager->AddComponent(*rc);

      CPPUNIT_ASSERT(rc->GetGameManager() == mManager.get());

      //test regular get all
      std::vector<dtGame::GMComponent*> toFill;
      mManager->GetAllComponents(toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be exactly one component in the GameManager.",toFill.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("The one component should be the rules component.", toFill[0] == rc.get());

      mManager->AddComponent(*dmc);

      //test const get all
      std::vector<const dtGame::GMComponent*> toFill2;
      mManager->GetAllComponents(toFill2);
      CPPUNIT_ASSERT_MESSAGE("There should be exactly 2 components in the GameManager.",toFill2.size() == 2);
      CPPUNIT_ASSERT_MESSAGE("The first component should be the rules component.", toFill2[0] == rc.get());
      CPPUNIT_ASSERT_MESSAGE("The second component should be the defaulte message processor.", toFill2[1] == dmc.get());

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

void GameManagerTests::TestAddActor()
{

   dtCore::RefPtr<dtDAL::ActorType> type = mManager->FindActorType("ExampleActors","Test1Actor");
   for (int x = 0; x < 20; ++x)
   {
      if (x == 10)
         type = mManager->FindActorType("ExampleActors","Test2Actor");
                  
      CPPUNIT_ASSERT(type != NULL);
      dtCore::RefPtr<dtGame::GameActorProxy> proxy = dynamic_cast<dtGame::GameActorProxy*>(mManager->CreateActor(*type).get());
      CPPUNIT_ASSERT_MESSAGE("Proxy, the result of a dynamic_cast to dtGame::GameActorProxy, should not be NULL", proxy != NULL); 
      CPPUNIT_ASSERT_MESSAGE("IsGameActorProxy should return true", proxy->IsGameActorProxy());


      if (x % 2 == 0) 
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
         CPPUNIT_ASSERT_MESSAGE("The proxy should still be in the game manager", mManager->FindGameActorById(proxy->GetId()) != NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should not have the GameManager pointer set to NULL", proxy->GetGameManager() != NULL);
         //have to send a from event to make the actor get deleted
         sleep(10);
         dtCore::System::Instance()->Step();
         
         CPPUNIT_ASSERT_MESSAGE("The proxy should not still be in the game manager", mManager->FindGameActorById(proxy->GetId()) == NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should have the GameManager pointer set to NULL", proxy->GetGameManager() == NULL);
      }
      else
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
         //have to send a from event to make the actor get deleted
         sleep(10);
         dtCore::System::Instance()->Step();
         
         CPPUNIT_ASSERT_MESSAGE("The proxy should not still be in the game manager", mManager->FindGameActorById(proxy->GetId()) == NULL);
         CPPUNIT_ASSERT_MESSAGE("The proxy should have the GameManager pointer set to NULL", proxy->GetGameManager() == NULL);
      }
   }
}

