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
* @author Olen A. Bruce
* @author David Guthrie
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <vector>
#include <string>
#include <osg/Endian>

#include <dtUtil/coordinates.h>
#include <dtCore/system.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/datatype.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/messagetype.h>
#include <dtGame/defaultmessageprocessor.h>

#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/parametertoparameter.h>
#include <dtHLAGM/onetomanymapping.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/hlacomponentconfig.h>
#include <dtHLAGM/attributetype.h>
#include <dtHLAGM/rprparametertranslator.h>



class MappingClassTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(MappingClassTests);

      CPPUNIT_TEST(TestAttributeToPropertyList);
      CPPUNIT_TEST(TestParameterToParameterList);
      CPPUNIT_TEST(TestObjectToActor);
      CPPUNIT_TEST(TestInteractionToMessage);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
         std::string logName("HLAConfigTest");
         logger = &dtUtil::Log::GetInstance(logName);

         thisString = "ThisString";
         thatString = "0.1";

         attrToPropListMapping.SetHLAName(thisString);
         attrToPropListMapping.SetHLAType(dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE);
         attrToPropListMapping.SetRequiredForHLA(false);

         mParamDef.SetGameName(thisString);
         mParamDef.SetGameType(dtCore::DataType::UINT);
         mParamDef.SetDefaultValue("0.1");
         mParamDef.SetRequiredForGame(true);

         attrToPropListMapping.GetParameterDefinitions().push_back(mParamDef);


      }

      void tearDown()
      {
      }

      void TestAttributeToPropertyList()
      {
         dtHLAGM::AttributeToPropertyList thisMapping;
         TestOneToManyMapping(thisMapping);
      }

      void TestParameterToParameterList()
      {
         dtHLAGM::ParameterToParameterList thisMapping;
         TestOneToManyMapping(thisMapping);
      }

      void TestObjectToActor()
      {      
         dtCore::RefPtr<dtHLAGM::ObjectToActor> thisObjectToActor(new dtHLAGM::ObjectToActor);

         CPPUNIT_ASSERT(thisObjectToActor->GetDDMCalculatorName().empty());
         std::string testCalcName("jojoTest"); 
         thisObjectToActor->SetDDMCalculatorName(testCalcName);
         CPPUNIT_ASSERT_EQUAL(testCalcName, thisObjectToActor->GetDDMCalculatorName());
      
         CPPUNIT_ASSERT_MESSAGE("The DIS ID should default to NULL", thisObjectToActor->GetEntityType() == NULL);

         dtCore::ActorType *thisActorType = new dtCore::ActorType("Infinite Light",
                             "dtcore.Lights","Directional light actor.");
         thisObjectToActor->SetActorType(*thisActorType);
         CPPUNIT_ASSERT_MESSAGE("Set Actor Type should succeed.", &thisObjectToActor->GetActorType() == thisActorType);

         CPPUNIT_ASSERT_MESSAGE("Remote only should default to false.", !thisObjectToActor->IsRemoteOnly());
         CPPUNIT_ASSERT_MESSAGE("Local only should default to false.", !thisObjectToActor->IsLocalOnly());
         thisObjectToActor->SetLocalOrRemoteType(dtHLAGM::ObjectToActor::LocalOrRemoteType::REMOTE_ONLY);
         CPPUNIT_ASSERT_MESSAGE("Remote only should be true.", thisObjectToActor->IsRemoteOnly());
         CPPUNIT_ASSERT_MESSAGE("Local only should be false.", !thisObjectToActor->IsLocalOnly());
         thisObjectToActor->SetLocalOrRemoteType(dtHLAGM::ObjectToActor::LocalOrRemoteType::LOCAL_ONLY);
         CPPUNIT_ASSERT_MESSAGE("Remote only should be false.", !thisObjectToActor->IsRemoteOnly());
         CPPUNIT_ASSERT_MESSAGE("Local only should be true.", thisObjectToActor->IsLocalOnly());
         thisObjectToActor->SetLocalOrRemoteType(dtHLAGM::ObjectToActor::LocalOrRemoteType::LOCAL_AND_REMOTE);
         CPPUNIT_ASSERT_MESSAGE("Remote only should be false.", !thisObjectToActor->IsRemoteOnly());
         CPPUNIT_ASSERT_MESSAGE("Local only should be false.", !thisObjectToActor->IsLocalOnly());

         std::string thisObjectName = "ThisTank";

         thisObjectToActor->SetObjectClassName(thisObjectName);
         CPPUNIT_ASSERT_MESSAGE("Set Object Type Name should succeed.", thisObjectToActor->GetObjectClassName() == thisObjectName);

         dtHLAGM::EntityType thisEntityType(1, 2, 34, 1, 1, 1, 1);

         thisObjectToActor->SetEntityType(&thisEntityType);
         CPPUNIT_ASSERT_MESSAGE("Set DIS ID should succeed.", *thisObjectToActor->GetEntityType() == thisEntityType);

         thisObjectToActor->SetEntityType(NULL);
         CPPUNIT_ASSERT_MESSAGE("The DIS ID should be NULL again", thisObjectToActor->GetEntityType() == NULL);

         std::string overrideAttrName("jojo");

         CPPUNIT_ASSERT(thisObjectToActor->GetEntityIdAttributeName().empty());
         thisObjectToActor->SetEntityIdAttributeName(overrideAttrName);
         CPPUNIT_ASSERT(thisObjectToActor->GetEntityIdAttributeName() == overrideAttrName);

         CPPUNIT_ASSERT(thisObjectToActor->GetEntityTypeAttributeName().empty());
         thisObjectToActor->SetEntityTypeAttributeName(overrideAttrName);
         CPPUNIT_ASSERT(thisObjectToActor->GetEntityTypeAttributeName() == overrideAttrName);

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

//         CPPUNIT_ASSERT_EQUAL_MESSAGE("The handle should default to 0", thisObjectToActor->GetObjectClassHandle().get(), NULL);
//         thisObjectToActor->SetObjectClassHandle(32);
//         CPPUNIT_ASSERT_EQUAL(thisObjectToActor->GetObjectClassHandle(), RTI::AttributeHandle(32));
//
//         CPPUNIT_ASSERT_EQUAL_MESSAGE("The handle should default to 0", thisObjectToActor->GetEntityIdAttributeHandle(), RTI::AttributeHandle(0));
//         thisObjectToActor->SetEntityIdAttributeHandle(12);
//         CPPUNIT_ASSERT_EQUAL(thisObjectToActor->GetEntityIdAttributeHandle(), RTI::AttributeHandle(12));
//
//         CPPUNIT_ASSERT_EQUAL_MESSAGE("The handle should default to 0", thisObjectToActor->GetEntityTypeAttributeHandle(), RTI::AttributeHandle(0));
//         thisObjectToActor->SetEntityTypeAttributeHandle(24);
//         CPPUNIT_ASSERT_EQUAL(thisObjectToActor->GetEntityTypeAttributeHandle(), RTI::AttributeHandle(24));

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

         CPPUNIT_ASSERT(thisInteractionToMessage->GetDDMCalculatorName().empty());
         std::string testCalcName("jojoTest"); 
         thisInteractionToMessage->SetDDMCalculatorName(testCalcName);
         CPPUNIT_ASSERT_EQUAL(testCalcName, thisInteractionToMessage->GetDDMCalculatorName());

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

         //CPPUNIT_ASSERT_EQUAL_MESSAGE("The handle should default to 0", thisInteractionToMessage->GetInteractionClassHandle(), RTI::InteractionClassHandle(0));
         //thisInteractionToMessage->SetInteractionClassHandle(2);
         //CPPUNIT_ASSERT_EQUAL(thisInteractionToMessage->GetInteractionClassHandle(), RTI::AttributeHandle(2));

         dtCore::RefPtr<dtHLAGM::InteractionToMessage> copyInterationToMessage(new dtHLAGM::InteractionToMessage);

         *copyInterationToMessage = *thisInteractionToMessage;

         CPPUNIT_ASSERT_MESSAGE("copy should pass equivalency test",  *copyInterationToMessage == *thisInteractionToMessage);
         CPPUNIT_ASSERT_MESSAGE("!= operator should work.",  !(*copyInterationToMessage != *thisInteractionToMessage));

         CPPUNIT_ASSERT_MESSAGE("The mapping should have same number of items.",
            thisInteractionToMessage->GetOneToManyMappingVector() == copyInterationToMessage->GetOneToManyMappingVector());
      }
   private:

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

         CPPUNIT_ASSERT_MESSAGE("Should not default to special", !thisOneToXMapping.GetSpecial());
         thisOneToXMapping.SetSpecial(true);
         CPPUNIT_ASSERT_MESSAGE("Should be special.", thisOneToXMapping.GetSpecial());

         CPPUNIT_ASSERT_MESSAGE("Should not default to array", !thisOneToXMapping.GetIsArray());
         thisOneToXMapping.SetIsArray(true);
         CPPUNIT_ASSERT_MESSAGE("Should be an array.", thisOneToXMapping.GetIsArray());
      }

      template <typename MappingType>
      void TestGameParameterMapping(MappingType& mapping)
      {
         mapping.SetGameName(thisString);
         CPPUNIT_ASSERT_MESSAGE("Set Game Name should succeed.",
                                mapping.GetGameName() == thisString);

         CPPUNIT_ASSERT_MESSAGE("Game Type should default to unknown.",
                                mapping.GetGameType() == dtCore::DataType::UNKNOWN);

         mapping.SetGameType(dtCore::DataType::UINT);
         CPPUNIT_ASSERT_MESSAGE("Set Game Type should succeed.",
                                mapping.GetGameType() == dtCore::DataType::UINT);

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

