/* 
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

#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <vector>
#include <string>
#include <osg/Endian>
#include <dtUtil/coordinates.h>
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
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
#include <dtCore/system.h>
#include <dtCore/globals.h>

class TestHLAComponent;

class HLATests : public CPPUNIT_NS::TestFixture 
{
  CPPUNIT_TEST_SUITE(HLATests);
  
  CPPUNIT_TEST(TestFOMDataTranslation);
  CPPUNIT_TEST(TestHLAConnection);  
  CPPUNIT_TEST(TestDeleteActor);

  CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();
      void TestHLAConnection();
      void TestFOMDataTranslation();
      void TestDeleteActor();
      
   private:
      void TestSpecificIntTranslations(dtHLAGM::OneToOneMapping& mapping, 
         TestHLAComponent& testTrans, 
         long expectedResult);
      dtCore::RefPtr<dtGame::GameManager> mGameManager;
      
      //shared buffer.
      char* buffer;
      
      static const std::string mTestGameActorLibrary;
      
      dtUtil::Log* logger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(HLATests);

#if (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
  #if defined (_DEBUG)
      const std::string HLATests::mTestGameActorLibrary="testGameActorLibraryd";
  #else
      const std::string HLATests::mTestGameActorLibrary="testGameActorLibrary";
  #endif
#else
   const std::string HLATests::mTestGameActorLibrary="testGameActorLibrary";
#endif

class TestHLAComponent: public dtHLAGM::HLAComponent
{
   public:
      void TestMapHLAFieldToMessageParameter(const dtHLAGM::OneToOneMapping& mapping, const char* buffer, dtGame::MessageParameter& parameter)
      {
         MapHLAFieldToMessageParameter(mapping, buffer, parameter);      
      }
   
      void TestDeleteActor(const dtCore::UniqueId& actorId)
      {
         DeleteActor(actorId); 
      }
};

// Called implicitly by CPPUNIT when the app starts
void HLATests::setUp()
{
   try 
   {
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());   
      std::string logName("HLATests");
      logger = &dtUtil::Log::GetInstance(logName);
      dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
      mGameManager = new dtGame::GameManager(*scene);
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);
      
      dtCore::System::Instance()->Start();
      buffer = new char[512];
   }
   catch (const dtUtil::Exception& e) 
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

// Called implicitly by CPPUNIT when the app terminates
void HLATests::tearDown()
{
   dtCore::System::Instance()->Stop();
   if (mGameManager.valid())
   {
      mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
      mGameManager = NULL;
      delete[] buffer;
   }
}


void HLATests::TestHLAConnection()
{
   std::cout << "In the INCOMPLETE function TestHLAFomTranslator" << std::endl;

   //   dtDAL::ActorType *m1a1 = new dtDAL::ActorType("M1A1", "Vehicles",
   //						 "This actor represents a M1A1");
   //   std::string hlaM1A1 = "M1A1";
   // dtHLAGM::EntityType thisDisID(1,1,225,1,1,9,0);
  
   // std::vector<dtHLAGM::AttributeToProperty> thisAttributeToPropertyMappingVector;
   // thisAttributeToPropertyMappingVector.push_back(thisAttributeToPropertyMapping);
   // thisAttributeToPropertyMappingVector.push_back(thisAttributeToPropertyMapping);
   dtCore::RefPtr<dtHLAGM::HLAComponent> thisHLAComponent =  new dtHLAGM::HLAComponent();

   try
   {
      mGameManager->AddComponent(*thisHLAComponent, dtGame::GameManager::ComponentPriority::NORMAL);
      dtHLAGM::HLAComponentConfig config;
      config.LoadConfiguration(*thisHLAComponent, "Federations/HLAMappingExample.xml");
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }

   try 
   {
      std::string fedFile = osgDB::findDataFile("RPR-FOM.fed"); 
      CPPUNIT_ASSERT_MESSAGE("Couldn't find \"" + fedFile + "\", make sure you install the Delta3D data package and set the DELTA_DATA environment var.",
         !fedFile.empty());     
      thisHLAComponent->JoinFederationExecution("hla", fedFile, "delta3d");
   } 
   catch (const RTI::Exception& ex)
   {
      CPPUNIT_FAIL(std::string("Error joining federation : ") + ex._reason);
   }
   
   try
   {
      const unsigned int j = 20;
      for (unsigned int i = 0; i < j; i++)
      {
         std::cout << "Ticking RTI.  " << j-i << " ticks left.\n";

         dtCore::System::Instance()->Step();
      }
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}

void HLATests::TestDeleteActor()
{
   dtCore::RefPtr<dtGame::DefaultMessageProcessor> defMsgProcessor = new dtGame::DefaultMessageProcessor;
   mGameManager->AddComponent(*defMsgProcessor, dtGame::GameManager::ComponentPriority::HIGHEST);

   dtCore::RefPtr<TestHLAComponent> testTrans = new TestHLAComponent();
   mGameManager->AddComponent(*testTrans, dtGame::GameManager::ComponentPriority::NORMAL);   
   
   dtCore::RefPtr<dtDAL::ActorType> actorType = mGameManager->FindActorType("ExampleActors", "Test1Actor");
   CPPUNIT_ASSERT(actorType != NULL);

   dtCore::RefPtr<dtDAL::ActorProxy> proxy = mGameManager->CreateActor(*actorType);
   dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
   
   //add a remote actor.
   mGameManager->AddActor(*gap, true, false);
   CPPUNIT_ASSERT(mGameManager->FindGameActorById(gap->GetId()) != NULL);
   
   testTrans->TestDeleteActor(gap->GetId());
   dtCore::System::Instance()->Step();
   CPPUNIT_ASSERT_MESSAGE("The actor should have been deleted from the Game Manager", 
      mGameManager->FindGameActorById(gap->GetId()) == NULL);
   
}

void HLATests::TestFOMDataTranslation()
{
   osg::Endian endian = osg::getCpuByteOrder();
   dtCore::RefPtr<TestHLAComponent> testTrans = new TestHLAComponent();
   mGameManager->AddComponent(*testTrans, dtGame::GameManager::ComponentPriority::NORMAL);
   
   dtHLAGM::WorldCoordinate wc(1.1f, 2.2f, 3.3f);
   wc.Encode(buffer);
   dtHLAGM::AttributeToProperty mapping;
   mapping.SetGameName("jojo");
   mapping.SetHLAName("ChickenMan");
   
   
   dtCore::RefPtr<dtGame::Vec3MessageParameter> vec3Param = new dtGame::Vec3MessageParameter("test");
   mapping.SetGameType(vec3Param->GetDataType());
   mapping.SetHLAType(dtHLAGM::AttributeType::WORLD_COORDINATE_TYPE);
      
   testTrans->TestMapHLAFieldToMessageParameter(mapping, buffer, *vec3Param);
   
   osg::Vec3 expectedVec = testTrans->GetCoordinateConverter().ConvertToLocalTranslation(osg::Vec3d(wc.GetX(), wc.GetY(), wc.GetZ()));
   CPPUNIT_ASSERT(expectedVec == vec3Param->GetValue());

   buffer[0] = 1;
   strcpy(buffer+1, "FOA");
   dtCore::RefPtr<dtGame::StringMessageParameter> stringParam = new dtGame::StringMessageParameter("test");
   mapping.SetGameType(stringParam->GetDataType());
   mapping.SetHLAType(dtHLAGM::AttributeType::MARKING_TYPE);
      
   testTrans->TestMapHLAFieldToMessageParameter(mapping, buffer, *stringParam);
   CPPUNIT_ASSERT(stringParam->GetValue() == "FOA");

   buffer[0] = 1;
   //Test the max size of 11 by going over by one.
   strcpy(buffer+1, "123456789012");

   testTrans->TestMapHLAFieldToMessageParameter(mapping, buffer, *stringParam);
   CPPUNIT_ASSERT(stringParam->GetValue() == "12345678901");

   mapping.SetHLAType(dtHLAGM::AttributeType::EULER_ANGLES_TYPE);
  
   unsigned int testUInt = 44;
   unsigned short testUShort = 3;
   unsigned char testUChar = 9;

   *((unsigned int*)buffer) = testUInt;
   if (endian == osg::LittleEndian)
   {
      osg::swapBytes(buffer, sizeof(unsigned int));
   }
   mapping.SetHLAType(dtHLAGM::AttributeType::UNSIGNED_INT_TYPE);   
   TestSpecificIntTranslations(mapping, *testTrans, (long)testUInt);
   
   *((unsigned short*)buffer) = testUShort;
   if (endian == osg::LittleEndian)
   {
      osg::swapBytes(buffer, sizeof(unsigned short));
   }
   mapping.SetHLAType(dtHLAGM::AttributeType::UNSIGNED_SHORT_TYPE);
   TestSpecificIntTranslations(mapping, *testTrans, (long)testUShort);

   *((unsigned char*)buffer) = testUChar;
   if (endian == osg::LittleEndian)
   {
      osg::swapBytes(buffer, sizeof(unsigned char));
   }
   mapping.SetHLAType(dtHLAGM::AttributeType::UNSIGNED_CHAR_TYPE);
   TestSpecificIntTranslations(mapping, *testTrans, (long)testUChar);
   
}

void HLATests::TestSpecificIntTranslations(dtHLAGM::OneToOneMapping& mapping, 
   TestHLAComponent& testTrans, 
   long expectedResult)
{
   //Unsigned Int param
   dtCore::RefPtr<dtGame::UnsignedIntMessageParameter> uIntParam = new dtGame::UnsignedIntMessageParameter("test");      
   mapping.SetGameType(uIntParam->GetDataType());
   testTrans.TestMapHLAFieldToMessageParameter(mapping, buffer, *uIntParam);
   CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type unsigned int", 
      (unsigned)expectedResult == uIntParam->GetValue());   
 
   //Int param
   dtCore::RefPtr<dtGame::IntMessageParameter> intParam = new dtGame::IntMessageParameter("test");      
   mapping.SetGameType(intParam->GetDataType());
   testTrans.TestMapHLAFieldToMessageParameter(mapping, buffer, *intParam);
   CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type int", 
      (int)expectedResult == intParam->GetValue());   

   //short param
   dtCore::RefPtr<dtGame::ShortIntMessageParameter> shortParam = new dtGame::ShortIntMessageParameter("test");      
   mapping.SetGameType(shortParam->GetDataType());
   testTrans.TestMapHLAFieldToMessageParameter(mapping, buffer, *shortParam);
   CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type short", 
      (short int)expectedResult == shortParam->GetValue());   

   //unsigned short
   dtCore::RefPtr<dtGame::UnsignedShortIntMessageParameter> uShortParam = new dtGame::UnsignedShortIntMessageParameter("test");      
   mapping.SetGameType(shortParam->GetDataType());
   testTrans.TestMapHLAFieldToMessageParameter(mapping, buffer, *uShortParam);
   CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type unsigned short", 
      (short int)expectedResult == uShortParam->GetValue());   

   //boolean
   dtCore::RefPtr<dtGame::BooleanMessageParameter> boolParam = new dtGame::BooleanMessageParameter("test");      
   mapping.SetGameType(boolParam->GetDataType());
   testTrans.TestMapHLAFieldToMessageParameter(mapping, buffer, *boolParam);
   CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type bool", 
      (expectedResult != 0) == boolParam->GetValue());   


   dtCore::RefPtr<dtGame::EnumMessageParameter> enumParam = new dtGame::EnumMessageParameter("test");      
   mapping.SetGameType(shortParam->GetDataType());
   mapping.SetDefaultValue("hello");

   mapping.AddEnumerationMapping(1, "hello");
   mapping.AddEnumerationMapping(expectedResult, "correct");
   mapping.AddEnumerationMapping(800, "Another Value");

   testTrans.TestMapHLAFieldToMessageParameter(mapping, buffer, *enumParam);
   CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign param of type enumeration to \"correct\", but it is: " +  enumParam->GetValue(), 
      enumParam->GetValue() == "correct");   

   mapping.ClearEnumerationMapping();
   mapping.AddEnumerationMapping(1, "hello");
   mapping.AddEnumerationMapping(800, "Another Value");

   testTrans.TestMapHLAFieldToMessageParameter(mapping, buffer, *enumParam);
   CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type enumeration with the default value", 
      enumParam->GetValue() == mapping.GetDefaultValue());   
   
}
