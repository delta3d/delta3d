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


class MappingClassTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(MappingClassTests);
  
      CPPUNIT_TEST(TestAttributeToProperty);
      CPPUNIT_TEST(TestParameterToParameter);
      CPPUNIT_TEST(TestObjectToActor);
      CPPUNIT_TEST(TestInteractionToMessage);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();
      void TestAttributeToProperty();
      void TestParameterToParameter();
      void TestObjectToActor();
      void TestInteractionToMessage();
   private:
      void TestOneToOneMapping(dtHLAGM::OneToOneMapping& thisOneToOneMapping);
      dtHLAGM::AttributeToProperty thisAttributeToPropertyMapping;
      std::string thisString;
      std::string thatString;
      dtUtil::Log* logger;   
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(MappingClassTests);

// Called implicitly by CPPUNIT when the app starts
void MappingClassTests::setUp()
{
   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());   
   std::string logName("HLAConfigTest");
   logger = &dtUtil::Log::GetInstance(logName);

   thisString = "ThisString";
   thatString = "0.1";

   thisAttributeToPropertyMapping.SetHLAName(thisString);
   thisAttributeToPropertyMapping.SetGameName(thisString);
   thisAttributeToPropertyMapping.SetHLAType(dtHLAGM::AttributeType::EULER_ANGLES_TYPE);
   thisAttributeToPropertyMapping.SetGameType(dtDAL::DataType::UINT);
   thisAttributeToPropertyMapping.SetDefaultValue("0.1");
   thisAttributeToPropertyMapping.SetRequiredForGame(true);
   thisAttributeToPropertyMapping.SetRequiredForHLA(false);
}

// Called implicitly by CPPUNIT when the app terminates
void MappingClassTests::tearDown()
{
}


void MappingClassTests::TestOneToOneMapping(dtHLAGM::OneToOneMapping& thisOneToOneMapping)
{
   thisOneToOneMapping.SetHLAName(thisString);
   CPPUNIT_ASSERT_MESSAGE("Set HLA Name should succeed.", thisOneToOneMapping.GetHLAName() == thisString);

   thisOneToOneMapping.SetGameName(thisString);
   CPPUNIT_ASSERT_MESSAGE("Set Game Name should succeed.", thisOneToOneMapping.GetGameName() == thisString);

   thisOneToOneMapping.SetHLAType(dtHLAGM::AttributeType::EULER_ANGLES_TYPE);
   CPPUNIT_ASSERT_MESSAGE("Set HLA Type should succeed.", thisOneToOneMapping.GetHLAType() == dtHLAGM::AttributeType::EULER_ANGLES_TYPE);

   thisOneToOneMapping.SetGameType(dtDAL::DataType::UINT);
   CPPUNIT_ASSERT_MESSAGE("Set Game Type should succeed.", thisOneToOneMapping.GetGameType() == dtDAL::DataType::UINT);

   thisOneToOneMapping.SetDefaultValue(thatString);
   CPPUNIT_ASSERT_MESSAGE("Set Default Value should succeed.", thisOneToOneMapping.GetDefaultValue() == thatString);

   CPPUNIT_ASSERT_MESSAGE("Set Attribute Required should succeed.", thisOneToOneMapping.IsRequiredForHLA() == false);
   CPPUNIT_ASSERT_MESSAGE("Set GameType Required should succeed.", thisOneToOneMapping.IsRequiredForGame() == false);

   thisOneToOneMapping.SetRequiredForGame(true);
   CPPUNIT_ASSERT_MESSAGE("Set GameType Required should succeed.", thisOneToOneMapping.IsRequiredForGame() == true);

   thisOneToOneMapping.SetRequiredForHLA(true);
   CPPUNIT_ASSERT_MESSAGE("Set Attribute Required should succeed.", thisOneToOneMapping.IsRequiredForHLA() == true);
   
   thisOneToOneMapping.AddEnumerationMapping(1, "hello");
   thisOneToOneMapping.AddEnumerationMapping(7, "chicken");
   thisOneToOneMapping.AddEnumerationMapping(8, "horse");
   
   std::string gameEnumValue;
   int hlaEnumValue;
   CPPUNIT_ASSERT(thisOneToOneMapping.GetHLAEnumerationValue("hello", hlaEnumValue));
   CPPUNIT_ASSERT(hlaEnumValue == 1);
   CPPUNIT_ASSERT(thisOneToOneMapping.GetHLAEnumerationValue("chicken", hlaEnumValue));
   CPPUNIT_ASSERT(hlaEnumValue == 7);
   CPPUNIT_ASSERT(thisOneToOneMapping.GetHLAEnumerationValue("horse", hlaEnumValue));
   CPPUNIT_ASSERT(hlaEnumValue == 8);

   CPPUNIT_ASSERT(thisOneToOneMapping.GetGameEnumerationValue(1, gameEnumValue));
   CPPUNIT_ASSERT(gameEnumValue == "hello");
   CPPUNIT_ASSERT(thisOneToOneMapping.GetGameEnumerationValue(7, gameEnumValue));
   CPPUNIT_ASSERT(gameEnumValue == "chicken");
   CPPUNIT_ASSERT(thisOneToOneMapping.GetGameEnumerationValue(8, gameEnumValue));
   CPPUNIT_ASSERT(gameEnumValue == "horse");
   
   CPPUNIT_ASSERT(!thisOneToOneMapping.GetGameEnumerationValue(99, gameEnumValue));
   CPPUNIT_ASSERT(!thisOneToOneMapping.GetGameEnumerationValue(2, gameEnumValue));
   CPPUNIT_ASSERT(!thisOneToOneMapping.GetGameEnumerationValue(-7, gameEnumValue));
   
   CPPUNIT_ASSERT(!thisOneToOneMapping.GetHLAEnumerationValue("gee", hlaEnumValue));
   CPPUNIT_ASSERT(!thisOneToOneMapping.GetHLAEnumerationValue("whiz", hlaEnumValue));
   CPPUNIT_ASSERT(!thisOneToOneMapping.GetHLAEnumerationValue("horsey", hlaEnumValue));
}

// Write your test code in this function
void MappingClassTests::TestAttributeToProperty()
{
   std::cout << "In the INCOMPLETE function TestAttributeToProperty\n";

   dtHLAGM::AttributeToProperty thisOneToOneMapping;

   TestOneToOneMapping(thisOneToOneMapping);  

   //NEED TO TEST ATTRIBUTE HANDLE
}

void MappingClassTests::TestParameterToParameter()
{
   std::cout << "In the INCOMPLETE function TestParameterToParameter\n";

   dtHLAGM::ParameterToParameter thisParameterToParameterMapping;

   TestOneToOneMapping(thisParameterToParameterMapping);  
  
   //NEED TO TEST PARAMETER HANDLE
}
void MappingClassTests::TestObjectToActor()
{
   std::cout << "In the INCOMPLETE function TestObjectToActor\n";

   dtCore::RefPtr<dtHLAGM::ObjectToActor> thisObjectToActor(new dtHLAGM::ObjectToActor);

   CPPUNIT_ASSERT_MESSAGE("The DIS ID should default to NULL", thisObjectToActor->GetDisID() == NULL);

   dtDAL::ActorType *thisActorType = new dtDAL::ActorType("Infinite Light",
                       "dtcore.Lights","Directional light actor.");
   thisObjectToActor->SetActorType(*thisActorType);
   CPPUNIT_ASSERT_MESSAGE("Set Actor Type should succeed.", &thisObjectToActor->GetActorType() == thisActorType);
  
   CPPUNIT_ASSERT_MESSAGE("Remote only should default to false.", !thisObjectToActor->IsRemoteOnly());
   thisObjectToActor->SetRemoteOnly(true);
   CPPUNIT_ASSERT_MESSAGE("Remote only should be true.", thisObjectToActor->IsRemoteOnly());
   thisObjectToActor->SetRemoteOnly(false);
   CPPUNIT_ASSERT_MESSAGE("Remote only should be false.", !thisObjectToActor->IsRemoteOnly());
  
   std::string thisObjectName = "ThisTank";

   thisObjectToActor->SetObjectTypeName(thisObjectName);
   CPPUNIT_ASSERT_MESSAGE("Set Object Type Name should succeed.", thisObjectToActor->GetObjectTypeName() == thisObjectName);

   dtHLAGM::EntityType thisDisID(1, 2, 34, 1, 1, 1, 1);

   thisObjectToActor->SetDisID(&thisDisID);
   CPPUNIT_ASSERT_MESSAGE("Set DIS ID should succeed.", *thisObjectToActor->GetDisID() == thisDisID);

   thisObjectToActor->SetDisID(NULL);
   CPPUNIT_ASSERT_MESSAGE("The DIS ID should be NULL again", thisObjectToActor->GetDisID() == NULL);

   std::vector<dtHLAGM::AttributeToProperty> thisAttributeToPropertyMappingVector;
   thisAttributeToPropertyMappingVector.push_back(thisAttributeToPropertyMapping);
   thisAttributeToPropertyMappingVector.push_back(thisAttributeToPropertyMapping);

   thisObjectToActor->SetOneToOneMappingVector(thisAttributeToPropertyMappingVector);
   CPPUNIT_ASSERT_MESSAGE("Set OneToOneMapping Vector should succeed.", thisObjectToActor->GetOneToOneMappingVector() == thisAttributeToPropertyMappingVector);

   thisObjectToActor->GetOneToOneMappingVector().push_back(thisAttributeToPropertyMapping);
   CPPUNIT_ASSERT_MESSAGE("The OneToOneMapping Vector should have one more attribute.", 
      thisObjectToActor->GetOneToOneMappingVector().size() == thisAttributeToPropertyMappingVector.size() + 1);

   dtCore::RefPtr<dtHLAGM::ObjectToActor> copyObjectToActor(new dtHLAGM::ObjectToActor);
   
   *copyObjectToActor = *thisObjectToActor;
   
   CPPUNIT_ASSERT_MESSAGE("copy should pass equivalency test",  *copyObjectToActor == *thisObjectToActor);
   CPPUNIT_ASSERT_MESSAGE("!= operator should work.",  !(*copyObjectToActor != *thisObjectToActor));
   
   CPPUNIT_ASSERT_MESSAGE("The mapping should have same number of items.", 
      thisObjectToActor->GetOneToOneMappingVector() == copyObjectToActor->GetOneToOneMappingVector());
   

   //NEED TO TEST OBJECT CLASS HANDLE
}

void MappingClassTests::TestInteractionToMessage()
{
   std::cout << "In the INCOMPLETE function TestInteractionToMessage\n";

   dtCore::RefPtr<dtHLAGM::InteractionToMessage> thisInteractionToMessage(new dtHLAGM::InteractionToMessage);

   thisInteractionToMessage->SetMessageType(dtGame::MessageType::TICK_LOCAL);
   CPPUNIT_ASSERT_MESSAGE("Set Message Type should succeed.", thisInteractionToMessage->GetMessageType() == dtGame::MessageType::TICK_LOCAL);

   std::string thisInteractionName = "TestInteraction";

   thisInteractionToMessage->SetInteractionName(thisInteractionName);
   CPPUNIT_ASSERT_MESSAGE("Set Interaction Type should succeed.", thisInteractionToMessage->GetInteractionName() == thisInteractionName);

   dtHLAGM::ParameterToParameter thisParameterToParameterMapping;
   thisParameterToParameterMapping.SetHLAName(thisString);
   thisParameterToParameterMapping.SetGameName(thisString);
   thisParameterToParameterMapping.SetHLAType(dtHLAGM::AttributeType::VELOCITY_VECTOR_TYPE);
   thisParameterToParameterMapping.SetGameType(dtDAL::DataType::UINT);
   thisParameterToParameterMapping.SetDefaultValue("0.1");
   thisParameterToParameterMapping.SetRequiredForGame(true);
   thisParameterToParameterMapping.SetRequiredForHLA(false);
   
   std::vector<dtHLAGM::ParameterToParameter> thisParameterToParameterMappingVector;
   thisParameterToParameterMappingVector.push_back(thisParameterToParameterMapping);
   thisParameterToParameterMappingVector.push_back(thisParameterToParameterMapping);

   thisInteractionToMessage->SetOneToOneMappingVector(thisParameterToParameterMappingVector);
   CPPUNIT_ASSERT_MESSAGE("Set OneToOneMapping Vector should succeed.", thisInteractionToMessage->GetOneToOneMappingVector() == thisParameterToParameterMappingVector);  
   thisInteractionToMessage->GetOneToOneMappingVector().push_back(thisParameterToParameterMapping);
   CPPUNIT_ASSERT_MESSAGE("The OneToOneMapping Vector should have one more item.", 
      thisInteractionToMessage->GetOneToOneMappingVector().size() == thisParameterToParameterMappingVector.size() + 1);  

   dtCore::RefPtr<dtHLAGM::InteractionToMessage> copyInterationToMessage(new dtHLAGM::InteractionToMessage);
   
   *copyInterationToMessage = *thisInteractionToMessage;
   
   CPPUNIT_ASSERT_MESSAGE("copy should pass equivalency test",  *copyInterationToMessage == *thisInteractionToMessage);
   CPPUNIT_ASSERT_MESSAGE("!= operator should work.",  !(*copyInterationToMessage != *thisInteractionToMessage));
   
   CPPUNIT_ASSERT_MESSAGE("The mapping should have same number of items.", 
      thisInteractionToMessage->GetOneToOneMappingVector() == copyInterationToMessage->GetOneToOneMappingVector());
   
   //NEED TO TEST INTERACTION CLASS HANDLE
}
