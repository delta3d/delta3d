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
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <vector>
#include <string>
#include <osg/Endian>

#include <dtUtil/coordinates.h>
#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtDAL/datatype.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/messagetype.h>
#include <dtGame/defaultmessageprocessor.h>

#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/parametertoparameter.h>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/onetomanymapping.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/hlacomponentconfig.h>
#include <dtHLAGM/attributetype.h>
#include <dtHLAGM/rprparametertranslator.h>



class MappingClassTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(MappingClassTests);

      CPPUNIT_TEST(TestAttributeToProperty);
      CPPUNIT_TEST(TestParameterToParameter);
      CPPUNIT_TEST(TestAttributeToPropertyList);
      CPPUNIT_TEST(TestParameterToParameterList);
      CPPUNIT_TEST(TestObjectToActor);
      CPPUNIT_TEST(TestInteractionToMessage);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
         std::string logName("HLAConfigTest");
         logger = &dtUtil::Log::GetInstance(logName);

         thisString = "ThisString";
         thatString = "0.1";

         attrToPropListMapping.SetHLAName(thisString);
         attrToPropListMapping.SetHLAType(dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE);
         attrToPropListMapping.SetRequiredForHLA(false);

         mParamDef.SetGameName(thisString);
         mParamDef.SetGameType(dtDAL::DataType::UINT);
         mParamDef.SetDefaultValue("0.1");
         mParamDef.SetRequiredForGame(true);

         attrToPropListMapping.GetParameterDefinitions().push_back(mParamDef);


      }

      void tearDown()
      {
      }

      void TestAttributeToProperty()
      {
         dtHLAGM::AttributeToProperty thisOneToOneMapping;
         TestOneToOneMapping(thisOneToOneMapping);
      }


      void TestAttributeToPropertyList()
      {
         dtHLAGM::AttributeToPropertyList thisMapping;
         TestOneToManyMapping(thisMapping);
      }

      void TestParameterToParameter()
      {
         dtHLAGM::ParameterToParameter thisParameterToParameterMapping;
         TestOneToOneMapping(thisParameterToParameterMapping);
      }


      void TestParameterToParameterList()
      {
         dtHLAGM::ParameterToParameterList thisMapping;
         TestOneToManyMapping(thisMapping);
      }

      void TestObjectToActor()
      {
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

         thisObjectToActor->SetObjectClassName(thisObjectName);
         CPPUNIT_ASSERT_MESSAGE("Set Object Type Name should succeed.", thisObjectToActor->GetObjectClassName() == thisObjectName);

         dtHLAGM::EntityType thisDisID(1, 2, 34, 1, 1, 1, 1);

         thisObjectToActor->SetDisID(&thisDisID);
         CPPUNIT_ASSERT_MESSAGE("Set DIS ID should succeed.", *thisObjectToActor->GetDisID() == thisDisID);

         thisObjectToActor->SetDisID(NULL);
         CPPUNIT_ASSERT_MESSAGE("The DIS ID should be NULL again", thisObjectToActor->GetDisID() == NULL);

         CPPUNIT_ASSERT(thisObjectToActor->GetEntityIdAttributeName().empty());
         thisObjectToActor->SetEntityIdAttributeName("jojo");
         CPPUNIT_ASSERT(thisObjectToActor->GetEntityIdAttributeName() == "jojo");

         //This def is added in the setup.  This check is verifying it was added properly.
         CPPUNIT_ASSERT(attrToPropListMapping.GetParameterDefinitions()[0] == mParamDef);

         std::vector<dtHLAGM::AttributeToPropertyList> attrToPropListMappingVector;
         attrToPropListMappingVector.push_back(attrToPropListMapping);
         attrToPropListMappingVector.push_back(attrToPropListMapping);

         thisObjectToActor->SetOneToManyMappingVector(attrToPropListMappingVector);
         CPPUNIT_ASSERT_MESSAGE("Set OneToManyMapping Vector should succeed.", thisObjectToActor->GetOneToManyMappingVector() == attrToPropListMappingVector);

         thisObjectToActor->GetOneToManyMappingVector().push_back(attrToPropListMapping);
         CPPUNIT_ASSERT_MESSAGE("The OneToManyMapping Vector should have one more attribute.",
            thisObjectToActor->GetOneToManyMappingVector().size() == attrToPropListMappingVector.size() + 1);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The handle should default to 0", thisObjectToActor->GetObjectClassHandle(), RTI::ObjectClassHandle(0));
         thisObjectToActor->SetObjectClassHandle(32);
         CPPUNIT_ASSERT_EQUAL(thisObjectToActor->GetObjectClassHandle(), RTI::AttributeHandle(32));

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The handle should default to 0", thisObjectToActor->GetEntityIdAttributeHandle(), RTI::AttributeHandle(0));
         thisObjectToActor->SetEntityIdAttributeHandle(12);
         CPPUNIT_ASSERT_EQUAL(thisObjectToActor->GetEntityIdAttributeHandle(), RTI::AttributeHandle(12));

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The handle should default to 0", thisObjectToActor->GetDisIDAttributeHandle(), RTI::AttributeHandle(0));
         thisObjectToActor->SetDisIDAttributeHandle(24);
         CPPUNIT_ASSERT_EQUAL(thisObjectToActor->GetDisIDAttributeHandle(), RTI::AttributeHandle(24));

         dtCore::RefPtr<dtHLAGM::ObjectToActor> copyObjectToActor(new dtHLAGM::ObjectToActor);

         *copyObjectToActor = *thisObjectToActor;

         CPPUNIT_ASSERT(copyObjectToActor->GetEntityIdAttributeName() == thisObjectToActor->GetEntityIdAttributeName());

         CPPUNIT_ASSERT_MESSAGE("copy should pass equivalency test",  *copyObjectToActor == *thisObjectToActor);
         CPPUNIT_ASSERT_MESSAGE("!= operator should work.",  !(*copyObjectToActor != *thisObjectToActor));

         thisObjectToActor->SetEntityIdAttributeName("somethingElse");

         CPPUNIT_ASSERT_MESSAGE("copy should pass equivalency test",  *copyObjectToActor != *thisObjectToActor);
         CPPUNIT_ASSERT_MESSAGE("!= operator should work.",  !(*copyObjectToActor == *thisObjectToActor));

         CPPUNIT_ASSERT_MESSAGE("The mapping should have same number of items.",
            thisObjectToActor->GetOneToManyMappingVector() == copyObjectToActor->GetOneToManyMappingVector());


      }

      void TestInteractionToMessage()
      {
         dtCore::RefPtr<dtHLAGM::InteractionToMessage> thisInteractionToMessage(new dtHLAGM::InteractionToMessage);

         thisInteractionToMessage->SetMessageType(dtGame::MessageType::TICK_LOCAL);
         CPPUNIT_ASSERT_MESSAGE("Set Message Type should succeed.", thisInteractionToMessage->GetMessageType() == dtGame::MessageType::TICK_LOCAL);

         std::string thisInteractionName = "TestInteraction";

         thisInteractionToMessage->SetInteractionName(thisInteractionName);
         CPPUNIT_ASSERT_MESSAGE("Set Interaction Type should succeed.", thisInteractionToMessage->GetInteractionName() == thisInteractionName);

         dtHLAGM::ParameterToParameterList pToPListMapping;
         pToPListMapping.SetHLAName(thisString);
         pToPListMapping.SetHLAType(dtHLAGM::RPRAttributeType::VELOCITY_VECTOR_TYPE);
         pToPListMapping.SetRequiredForHLA(false);

         pToPListMapping.GetParameterDefinitions().push_back(mParamDef);
         CPPUNIT_ASSERT(pToPListMapping.GetParameterDefinitions()[0] == mParamDef);

         std::vector<dtHLAGM::ParameterToParameterList> pToPListMappingVector;
         pToPListMappingVector.push_back(pToPListMapping);
         pToPListMappingVector.push_back(pToPListMapping);

         thisInteractionToMessage->SetOneToManyMappingVector(pToPListMappingVector);
         CPPUNIT_ASSERT_MESSAGE("Set OneToManyMapping Vector should succeed.", thisInteractionToMessage->GetOneToManyMappingVector() == pToPListMappingVector);
         thisInteractionToMessage->GetOneToManyMappingVector().push_back(pToPListMapping);
         CPPUNIT_ASSERT_MESSAGE("The OneToManyMapping Vector should have one more item.",
            thisInteractionToMessage->GetOneToManyMappingVector().size() == pToPListMappingVector.size() + 1);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The handle should default to 0", thisInteractionToMessage->GetInteractionClassHandle(), RTI::InteractionClassHandle(0));
         thisInteractionToMessage->SetInteractionClassHandle(2);
         CPPUNIT_ASSERT_EQUAL(thisInteractionToMessage->GetInteractionClassHandle(), RTI::AttributeHandle(2));

         dtCore::RefPtr<dtHLAGM::InteractionToMessage> copyInterationToMessage(new dtHLAGM::InteractionToMessage);

         *copyInterationToMessage = *thisInteractionToMessage;

         CPPUNIT_ASSERT_MESSAGE("copy should pass equivalency test",  *copyInterationToMessage == *thisInteractionToMessage);
         CPPUNIT_ASSERT_MESSAGE("!= operator should work.",  !(*copyInterationToMessage != *thisInteractionToMessage));

         CPPUNIT_ASSERT_MESSAGE("The mapping should have same number of items.",
            thisInteractionToMessage->GetOneToManyMappingVector() == copyInterationToMessage->GetOneToManyMappingVector());
      }
   private:

      void TestOneToOneMapping(dtHLAGM::OneToOneMapping& thisOneToOneMapping)
      {
         TestOneToXMapping(thisOneToOneMapping);
         TestGameParameterMapping(thisOneToOneMapping);
      }

      void TestOneToManyMapping(dtHLAGM::OneToManyMapping& thisOneToManyMapping)
      {
         TestOneToXMapping(thisOneToManyMapping);

         const dtHLAGM::OneToManyMapping& constMapping = thisOneToManyMapping;
         std::vector<dtHLAGM::OneToManyMapping::ParameterDefinition>& mappingVector = thisOneToManyMapping.GetParameterDefinitions();
         CPPUNIT_ASSERT(&constMapping.GetParameterDefinitions() == &mappingVector);

         dtHLAGM::OneToManyMapping::ParameterDefinition pd;
         mappingVector.push_back(pd);

         CPPUNIT_ASSERT_MESSAGE("Copying the ParameterDefinition into the vector should yield an equivalent object.",
            mappingVector[0] == pd);

         TestGameParameterMapping(pd);
         TestGameParameterMapping(mappingVector[0]);

         CPPUNIT_ASSERT_MESSAGE("Setting all the parameters on two ParameterDefinition objects should yield two equivalent object.",
            mappingVector[0] == pd);

         mappingVector.push_back(pd);

         CPPUNIT_ASSERT_MESSAGE("Copying the ParameterDefinition into the vector should yield an equivalent object.",
            mappingVector[1] == pd);

      }

      template <typename OneToXMappingType>
      void TestOneToXMapping(OneToXMappingType& thisOneToXMapping)
      {
         thisOneToXMapping.SetHLAName(thisString);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Set HLA Name should succeed.",
                                thisOneToXMapping.GetHLAName(), thisString);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("HLA Type should default to unknown.",
                                thisOneToXMapping.GetHLAType(), dtHLAGM::RPRAttributeType::UNKNOWN);

         thisOneToXMapping.SetHLAType(dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE);
         CPPUNIT_ASSERT_MESSAGE("Set HLA Type should succeed.",
                                thisOneToXMapping.GetHLAType() == dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE);


         CPPUNIT_ASSERT_MESSAGE("HLA Attribute Required should default to false.", !thisOneToXMapping.IsRequiredForHLA());

         thisOneToXMapping.SetRequiredForHLA(true);
         CPPUNIT_ASSERT_MESSAGE("Set Attribute Required should succeed.", thisOneToXMapping.IsRequiredForHLA());
      }

      template <typename MappingType>
      void TestGameParameterMapping(MappingType& mapping)
      {
         mapping.SetGameName(thisString);
         CPPUNIT_ASSERT_MESSAGE("Set Game Name should succeed.",
                                mapping.GetGameName() == thisString);

         CPPUNIT_ASSERT_MESSAGE("Game Type should default to unknown.",
                                mapping.GetGameType() == dtDAL::DataType::UNKNOWN);

         mapping.SetGameType(dtDAL::DataType::UINT);
         CPPUNIT_ASSERT_MESSAGE("Set Game Type should succeed.",
                                mapping.GetGameType() == dtDAL::DataType::UINT);

         mapping.SetDefaultValue(thatString);
         CPPUNIT_ASSERT_MESSAGE("Set Default Value should succeed.",
                                mapping.GetDefaultValue() == thatString);
         CPPUNIT_ASSERT_MESSAGE("Set GameType Required should succeed.", mapping.IsRequiredForGame() == false);

         mapping.SetRequiredForGame(true);
         CPPUNIT_ASSERT_MESSAGE("Set GameType Required should succeed.", mapping.IsRequiredForGame() == true);

         mapping.AddEnumerationMapping("1", "hello");
         mapping.AddEnumerationMapping("7", "chicken");
         mapping.AddEnumerationMapping("8", "horse");

         std::string gameEnumValue;
         std::string hlaEnumValue;
         CPPUNIT_ASSERT(mapping.GetHLAEnumerationValue("hello", hlaEnumValue));
         CPPUNIT_ASSERT(hlaEnumValue == "1");
         CPPUNIT_ASSERT(mapping.GetHLAEnumerationValue("chicken", hlaEnumValue));
         CPPUNIT_ASSERT(hlaEnumValue == "7");
         CPPUNIT_ASSERT(mapping.GetHLAEnumerationValue("horse", hlaEnumValue));
         CPPUNIT_ASSERT(hlaEnumValue == "8");

         CPPUNIT_ASSERT(mapping.GetGameEnumerationValue("1", gameEnumValue));
         CPPUNIT_ASSERT(gameEnumValue == "hello");
         CPPUNIT_ASSERT(mapping.GetGameEnumerationValue("7", gameEnumValue));
         CPPUNIT_ASSERT(gameEnumValue == "chicken");
         CPPUNIT_ASSERT(mapping.GetGameEnumerationValue("8", gameEnumValue));
         CPPUNIT_ASSERT(gameEnumValue == "horse");

         CPPUNIT_ASSERT(!mapping.GetGameEnumerationValue("99", gameEnumValue));
         CPPUNIT_ASSERT(!mapping.GetGameEnumerationValue("2", gameEnumValue));
         CPPUNIT_ASSERT(!mapping.GetGameEnumerationValue("-7", gameEnumValue));

         CPPUNIT_ASSERT(!mapping.GetHLAEnumerationValue("gee", hlaEnumValue));
         CPPUNIT_ASSERT(!mapping.GetHLAEnumerationValue("whiz", hlaEnumValue));
         CPPUNIT_ASSERT(!mapping.GetHLAEnumerationValue("horsey", hlaEnumValue));
      }

      dtHLAGM::AttributeToPropertyList attrToPropListMapping;
      dtHLAGM::OneToManyMapping::ParameterDefinition mParamDef;

      //talk about badly named data members.
      std::string thisString;
      std::string thatString;
      dtUtil::Log* logger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(MappingClassTests);

