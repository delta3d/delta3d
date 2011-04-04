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
#include <algorithm>

#include <dtUtil/coordinates.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/scene.h>
#include <dtABC/application.h>
#include <dtDAL/actortype.h>
#include <dtDAL/actorpluginregistry.h>
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/namedgroupparameter.inl>
#include <dtGame/messagetype.h>
#include <dtGame/gamemanager.h>

#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/parametertoparameter.h>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/hlacomponentconfig.h>
#include <dtHLAGM/exceptionenum.h>
#include <dtHLAGM/attributetype.h>
#include <dtHLAGM/rprparametertranslator.h>
#include <dtHLAGM/ddmcameracalculatorgeographic.h>

extern dtABC::Application& GetGlobalApplication();

class HLAConfigTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(HLAConfigTests);

      CPPUNIT_TEST(TestConfigure);
      CPPUNIT_TEST(TestBrokenHLAMappingNoActorType);
      CPPUNIT_TEST(TestBrokenHLAMappingOverloadedLocalMapping);
      CPPUNIT_TEST(TestBrokenHLAMappingOverloadedRemoteMapping);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();
      void TestConfigure();
      void TestBrokenHLAMappingNoActorType();
      void TestBrokenHLAMappingOverloadedLocalMapping();
      void TestBrokenHLAMappingOverloadedRemoteMapping();

   private:
      dtUtil::Log* logger;
      dtCore::RefPtr<dtHLAGM::HLAComponent> mTranslator;
      dtCore::RefPtr<dtGame::GameManager> mGameManager;
      dtCore::RefPtr<dtHLAGM::DDMCameraCalculatorGeographic> mCalc;
      //to be filled with all mappings to make sure the GetAllObjectToActorMappings method works.
      std::vector<const dtHLAGM::ObjectToActor*> mToFillOta;
      //to be filled with all mappings to make sure the GetAllInteractionToMessageMappings method works.
      std::vector<const dtHLAGM::InteractionToMessage*> mToFillItm;

      static const char* const mHLAActorRegistry;

      void TestBroken(const std::string& brokenFile, const std::string& failMessage);

      void CheckObjectToActorMapping(
         const std::string& category,
         const std::string& name,
         const std::string& objectClassName,
         const std::string& entityIdAttrName,
         const std::string& entityTypeAttrName,
         const std::string& ddmSpace,
         const dtHLAGM::EntityType* entityType,
         bool remoteOnly,
         bool localOnly,
         const std::vector<dtHLAGM::AttributeToPropertyList>& props);

      void CheckInteractionToMessageMapping(
         const dtGame::MessageType& messageType,
         const std::string& interactionName,
         const std::string& ddmSpace,
         const std::vector<dtHLAGM::ParameterToParameterList>& params);
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(HLAConfigTests);

// The HLAMappingExample.xml file only references the library with the 'd',
// so it must match here, else the unit tets will fail.
const char* const HLAConfigTests::mHLAActorRegistry="testGameActorLibrary";

// Called implicitly by CPPUNIT when the app starts
void HLAConfigTests::setUp()
{

   dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList() + ":" + dtUtil::GetDeltaRootPath() + "/tests/data");
   std::string logName("HLAConfigTest");
   //Uncomment this to turn on config xml logging.
   //dtUtil::Log::GetInstance("hlafomconfigxml.cpp").SetLogLevel(dtUtil::Log::LOG_DEBUG);
   logger = &dtUtil::Log::GetInstance(logName);
   mTranslator = new dtHLAGM::HLAComponent();
   mCalc = new dtHLAGM::DDMCameraCalculatorGeographic;
   mCalc->SetName("Geographic");
   mTranslator->GetDDMSubscriptionCalculators().AddCalculator(*mCalc);


   mGameManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
   mGameManager->SetApplication(GetGlobalApplication());

}

// Called implicitly by CPPUNIT when the app terminates
void HLAConfigTests::tearDown()
{
   mTranslator = NULL;

   mToFillOta.clear();
   mToFillItm.clear();

   if(mGameManager.valid())
   {
      if(mGameManager->GetRegistry(mHLAActorRegistry) != NULL)
         mGameManager->UnloadActorRegistry(mHLAActorRegistry);

      mGameManager = NULL;
   }
}

void HLAConfigTests::CheckObjectToActorMapping(
   const std::string& category,
   const std::string& name,
   const std::string& objectClassName,
   const std::string& entityIdAttrName,
   const std::string& entityTypeAttrName,
   const std::string& ddmSpace,
   const dtHLAGM::EntityType* entityType,
   bool remoteOnly,
   bool localOnly,
   const std::vector<dtHLAGM::AttributeToPropertyList>& props)
{
   dtCore::RefPtr<const dtDAL::ActorType> type = mGameManager->FindActorType(category, name);
   CPPUNIT_ASSERT(type.valid());

   dtCore::RefPtr<const dtHLAGM::ObjectToActor> otoa;
   if (localOnly)
   {
      otoa = static_cast<const dtHLAGM::HLAComponent*>(mTranslator.get())->GetActorMapping(*type);

      //make absolutely sure we call the const version of the method.
      dtCore::RefPtr<dtHLAGM::ObjectToActor> otoaNonConst = mTranslator->GetActorMapping(*type);
      CPPUNIT_ASSERT(otoa.get() == otoaNonConst.get());

      CPPUNIT_ASSERT_MESSAGE("Object to actor mapping for type " + type->GetCategory() + "." + type->GetName() + " should exist.",
         otoa != NULL);
   }
   else
   {
      otoa = static_cast<const dtHLAGM::HLAComponent*>(mTranslator.get())->GetObjectMapping(objectClassName, entityType);

      //make absolutely sure we call the const version of the method.
      dtCore::RefPtr<dtHLAGM::ObjectToActor> otoaNonConst = mTranslator->GetObjectMapping(objectClassName, entityType);
      CPPUNIT_ASSERT(otoa.get() == otoaNonConst.get());

      CPPUNIT_ASSERT_MESSAGE("Object to actor mapping for type " + type->GetCategory() + "." + type->GetName() + " should exist.",
         otoa != NULL);
   }

   if (localOnly)
   {
      CPPUNIT_ASSERT_MESSAGE("If the mapping is local only, fetching by class name should not return the same mapping object.",
         otoa != mTranslator->GetObjectMapping(objectClassName, entityType));
   }
   else if (remoteOnly)
   {
      CPPUNIT_ASSERT_MESSAGE("If the mapping is remote only, fetching by actor type should not return the same mapping object.",
         otoa != mTranslator->GetActorMapping(*type));
   }
   else
   {
      CPPUNIT_ASSERT_MESSAGE("If the mapping is local and remote only, fetching by actor type should return the same mapping object.",
         otoa == mTranslator->GetActorMapping(*type));
   }

   CPPUNIT_ASSERT_MESSAGE("The entity id should be \"" + entityIdAttrName + "\" but it is \"" + otoa->GetEntityIdAttributeName() + "\".",
      otoa->GetEntityIdAttributeName() == entityIdAttrName);

   CPPUNIT_ASSERT_MESSAGE("The entity type should be \"" + entityTypeAttrName + "\" but it is \"" + otoa->GetEntityTypeAttributeName() + "\".",
      otoa->GetEntityTypeAttributeName() == entityTypeAttrName);

   CPPUNIT_ASSERT_MESSAGE("The remote only value of object class mapping for " + objectClassName + " with actor type "
      + category + "." + name + " has the wrong value of remote only.", otoa->IsRemoteOnly() == remoteOnly);

   CPPUNIT_ASSERT_MESSAGE("The GetActorType method should return the same type as it was mapped to.",
      otoa->GetActorType() == *type);
   CPPUNIT_ASSERT_MESSAGE("The GetObjectClassName method should return " + objectClassName
      + "  not " + otoa->GetObjectClassName() + ".",
      otoa->GetObjectClassName() == objectClassName);

   if (entityType == NULL)
   {
      CPPUNIT_ASSERT_MESSAGE("DIS ID should be NULL.", otoa->GetEntityType() == NULL);
   }
   else
   {
      CPPUNIT_ASSERT_MESSAGE("DIS ID should not be NULL.", otoa->GetEntityType() != NULL);
      std::ostringstream ss;
      ss << "DIS ID should be equal to \"" << *entityType << "\" but it is \"" << *otoa->GetEntityType() << ".\"";
      CPPUNIT_ASSERT_MESSAGE(ss.str(), *otoa->GetEntityType() == *entityType);
   }

   const std::vector<dtHLAGM::AttributeToPropertyList>& propsActual = otoa->GetOneToManyMappingVector();
   CPPUNIT_ASSERT_EQUAL(propsActual.size(), props.size());
   for (unsigned i = 0; i < props.size(); ++i)
   {
      std::ostringstream ss;
      ss << "AttributeToPropertyList" << i << " with name " << props[i].GetHLAName() << " should match the one in the mapping with name " << propsActual[i].GetHLAName() << ".";

      CPPUNIT_ASSERT_MESSAGE(ss.str(),  props[i] == propsActual[i]);
   }

   CPPUNIT_ASSERT_MESSAGE("Mapping of " + type->GetCategory() + "." + type->GetName() + " to object class " +
            objectClassName +
            " should have been returned by GetAllObjectToActorMappings",
            std::find(mToFillOta.begin(), mToFillOta.end(), otoa.get()) != mToFillOta.end());
}

void HLAConfigTests::CheckInteractionToMessageMapping(
   const dtGame::MessageType& messageType,
   const std::string& interactionName,
   const std::string& ddmSpace,
   const std::vector<dtHLAGM::ParameterToParameterList>& params)
{
   //make absolutely sure we call the const version of the method.
   dtCore::RefPtr<const dtHLAGM::InteractionToMessage> itom =
         static_cast<const dtHLAGM::HLAComponent*>(mTranslator.get())->GetMessageMapping(messageType);

   dtCore::RefPtr<dtHLAGM::InteractionToMessage> itomNonConst = mTranslator->GetMessageMapping(messageType);

   CPPUNIT_ASSERT(itomNonConst.get() == itom.get());

   CPPUNIT_ASSERT_MESSAGE("Interation To Message mapping for type " + messageType.GetName() + " should exist.",
      itom != NULL);

   CPPUNIT_ASSERT_MESSAGE("The GetActorType method should return the same type as it was mapped to.",
      itom->GetMessageType() == messageType);
   CPPUNIT_ASSERT_MESSAGE("The GetInteractionName method should return " +  interactionName
      + "  not " + itom->GetInteractionName() + ".",
      itom->GetInteractionName() == interactionName);

   const std::vector<dtHLAGM::ParameterToParameterList>& paramsActual = itom->GetOneToManyMappingVector();
   CPPUNIT_ASSERT(paramsActual.size() == params.size());
   for (unsigned i = 0; i < params.size(); ++i)
   {
      std::ostringstream ss;
      ss << "ParameterToParameter " << i << " with name " << params[i].GetHLAName() << " should match the one in the mapping with name " << paramsActual[i].GetHLAName() << ". " << paramsActual[i];
      CPPUNIT_ASSERT_MESSAGE(ss.str(),  params[i] == paramsActual[i]);
   }

   CPPUNIT_ASSERT_MESSAGE("Mapping of " + messageType.GetName() + " to interaction " + interactionName +
            " should have been returned by GetAllInteractionToMessageMappings",
            std::find(mToFillItm.begin(), mToFillItm.end(), itom.get()) != mToFillItm.end());
}

void HLAConfigTests::TestBroken(const std::string& brokenFile, const std::string& failMessage)
{
   dtHLAGM::HLAComponentConfig config;

   mGameManager->AddComponent(*mTranslator, dtGame::GameManager::ComponentPriority::NORMAL);

   std::string path = dtUtil::FindFileInPathList(brokenFile);
   CPPUNIT_ASSERT(dtUtil::FileUtils::GetInstance().FileExists(path));

   CPPUNIT_ASSERT_THROW_MESSAGE(failMessage, config.LoadConfiguration(*mTranslator, path), dtHLAGM::XmlConfigException);
}

void HLAConfigTests::TestBrokenHLAMappingNoActorType()
{
   TestBroken("BrokenHLAMappingNoActorType.xml",
            "It should fail to load the mapping file because an actor type in the file is invalid.");
}

void HLAConfigTests::TestBrokenHLAMappingOverloadedLocalMapping()
{
   TestBroken("BrokenHLAMappingOverloadingLocal.xml",
            "It should fail to load the mapping file because two local mappings share the same actor type.");
}

void HLAConfigTests::TestBrokenHLAMappingOverloadedRemoteMapping()
{
   TestBroken("BrokenHLAMappingOverloadingRemote.xml",
            "It should fail to load the mapping file because two remote mappings share the same dis id and object class.");
}

void HLAConfigTests::TestConfigure()
{
   try
   {

      CPPUNIT_ASSERT_MESSAGE("Library should not yet be loaded.",
         mGameManager->GetRegistry(mHLAActorRegistry) == NULL);
      dtHLAGM::HLAComponentConfig config;

      CPPUNIT_ASSERT_THROW_MESSAGE("It should fail since no game manager was assigned to the mTranslator.",
                config.LoadConfiguration(*mTranslator, "Federations/HLAMappingExample.xml"), dtHLAGM::XmlConfigException);

      mGameManager->AddComponent(*mTranslator, dtGame::GameManager::ComponentPriority::NORMAL);

      config.LoadConfiguration(*mTranslator, "Federations/HLAMappingExample.xml");

      mTranslator->GetAllObjectToActorMappings(mToFillOta);
      mTranslator->GetAllInteractionToMessageMappings(mToFillItm);

      CPPUNIT_ASSERT_MESSAGE(std::string("Library should be loaded:") + mHLAActorRegistry,
         mGameManager->GetRegistry(mHLAActorRegistry) != NULL);

      CPPUNIT_ASSERT(mTranslator->IsDDMEnabled());

      CPPUNIT_ASSERT(mCalc->GetFriendlyRegionType() == dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE);
      CPPUNIT_ASSERT(mCalc->GetEnemyRegionType() == dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
      CPPUNIT_ASSERT_EQUAL(7L, mCalc->GetFriendlyAppSpace());
      CPPUNIT_ASSERT_EQUAL(8L, mCalc->GetEnemyAppSpace());
      CPPUNIT_ASSERT_EQUAL(9L, mCalc->GetNeutralAppSpace());
      CPPUNIT_ASSERT_EQUAL(0L, mCalc->GetAppSpaceMinimum());
      CPPUNIT_ASSERT_EQUAL(199L, mCalc->GetAppSpaceMaximum());


      const std::string& entityTypeHLAAttrName = dtHLAGM::HLAComponent::ATTR_NAME_ENTITY_TYPE;
      const std::string& mappingHLAAttrName = dtHLAGM::HLAComponent::ATTR_NAME_MAPPING_NAME;

      {
         dtHLAGM::EntityType type(1, 1, 222, 2, 4, 6, 0);

         std::vector<dtHLAGM::AttributeToPropertyList> props;

         // Set Entity Type mapped to the default ENUMERATION Game Type
         {
            dtHLAGM::AttributeToPropertyList attrToProp( entityTypeHLAAttrName, dtHLAGM::RPRAttributeType::ENTITY_TYPE, true);
            attrToProp.GetParameterDefinitions().push_back(
               dtHLAGM::OneToManyMapping::ParameterDefinition(
                  "Entity Type", dtDAL::DataType::ENUMERATION, "", false));
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("EntityIdentifier", dtHLAGM::RPRAttributeType::ENTITY_IDENTIFIER_TYPE, true);
            attrToProp.GetParameterDefinitions().push_back(dtHLAGM::OneToManyMapping::ParameterDefinition("sendingActorId", dtDAL::DataType::ACTOR, "", true));
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("DamageState", dtHLAGM::RPRAttributeType::UNSIGNED_INT_TYPE, false);

            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Damage State", dtDAL::DataType::ENUMERATION, "Destroyed", false);

            pd.AddEnumerationMapping("0", "No Damage");
            pd.AddEnumerationMapping("1", "Damaged");
            pd.AddEnumerationMapping("2", "Damaged");
            pd.AddEnumerationMapping("3", "Destroyed");

            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("Orientation", dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE, true);

            dtHLAGM::OneToManyMapping::ParameterDefinition pd(dtDAL::TransformableActorProxy::PROPERTY_ROTATION, dtDAL::DataType::VEC3, "", true);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("WorldLocation", dtHLAGM::RPRAttributeType::WORLD_COORDINATE_TYPE, true);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION, dtDAL::DataType::VEC3, "", true);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("", dtHLAGM::AttributeType::UNKNOWN, false);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Mesh", dtDAL::DataType::STATIC_MESH, "StaticMeshes:articulation_test.ive", false);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         // Set Entity Type mapped to STRING Game Type
         {
            dtHLAGM::AttributeToPropertyList attrToProp( entityTypeHLAAttrName, dtHLAGM::RPRAttributeType::ENTITY_TYPE, true);
            attrToProp.GetParameterDefinitions().push_back(
               dtHLAGM::OneToManyMapping::ParameterDefinition(
                  "Entity Type As String", dtDAL::DataType::STRING, "", false));
            props.push_back(attrToProp);
         }

         // Set a property for capturing the Object Mapping Name
         {
            dtHLAGM::AttributeToPropertyList attrToProp(mappingHLAAttrName, dtHLAGM::RPRAttributeType::STRING_TYPE, false);
            attrToProp.GetParameterDefinitions().push_back(
               dtHLAGM::OneToManyMapping::ParameterDefinition(
                  "Object Mapping Name", dtDAL::DataType::STRING, "", false));
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("ArticulatedParametersArray", dtHLAGM::RPRAttributeType::ARTICULATED_PART_SINGLE_TYPE, false);
            attrToProp.SetIsArray(true);

            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Articulated Parameters Array", dtDAL::DataType::GROUP, "unknown", false);

            pd.AddEnumerationMapping("0", "unknown");
            pd.AddEnumerationMapping("2048", "dof_periscope");
            pd.AddEnumerationMapping("4416", "dof_gun_01");
            pd.AddEnumerationMapping("4096", "dof_turret_01");

            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         CheckObjectToActorMapping("TestHLA", "Tank",
            "BaseEntity.PhysicalEntity.Platform.GroundVehicle",
            "EntityIdentifier", "AlternateEntityType", "Geographic", &type, false, false, props);
      }

      {
         std::vector<dtHLAGM::AttributeToPropertyList> props;
         {
            dtHLAGM::AttributeToPropertyList attrToProp(entityTypeHLAAttrName, dtHLAGM::RPRAttributeType::ENTITY_TYPE, true);

            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Entity Type", dtDAL::DataType::ENUMERATION, "", false);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("EntityIdentifier", dtHLAGM::RPRAttributeType::ENTITY_IDENTIFIER_TYPE, true);

            dtHLAGM::OneToManyMapping::ParameterDefinition pd("sendingActorId", dtDAL::DataType::ACTOR, "", true);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("DamageState", dtHLAGM::RPRAttributeType::UNSIGNED_INT_TYPE, false);

            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Damage State", dtDAL::DataType::ENUMERATION, "Destroyed", false);

            pd.AddEnumerationMapping("0", "No Damage");
            pd.AddEnumerationMapping("1", "Damaged");
            pd.AddEnumerationMapping("2", "Damaged");
            pd.AddEnumerationMapping("3", "Destroyed");

            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("Orientation", dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE, true);

            dtHLAGM::OneToManyMapping::ParameterDefinition pd(dtDAL::TransformableActorProxy::PROPERTY_ROTATION, dtDAL::DataType::VEC3, "", true);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("WorldLocation", dtHLAGM::RPRAttributeType::WORLD_COORDINATE_TYPE, true);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION, dtDAL::DataType::VEC3, "", true);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("VelocityVector", dtHLAGM::RPRAttributeType::VELOCITY_VECTOR_TYPE, false);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Velocity Vector", dtDAL::DataType::VEC3D, "", false);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         dtHLAGM::EntityType type1(1, 2, 225, 1, 9, 4, 0);
         CheckObjectToActorMapping("TestHLA", "Jet", "BaseEntity.PhysicalEntity.Platform.Aircraft",
            "EntityIdentifier", "", "Geographic", &type1, false, false, props);

         dtHLAGM::EntityType type2(1, 2, 222, 20, 2, 6, 0);

         //There is a remote only mapping only for this.
         CheckObjectToActorMapping("TestHLA", "Helicopter", "BaseEntity.PhysicalEntity.Platform.Aircraft",
            "EntityIdentifier", "", "Another Space", &type2, true, false, props);

         //There is a local only mapping for this that matches the remote one..
         CheckObjectToActorMapping("TestHLA", "Helicopter", "BaseEntity.PhysicalEntity.Platform.Aircraft",
            "EntityIdentifier", "", "Another Space", &type2, false, true, props);
      }

      {
         dtHLAGM::EntityType type(1, 2, 225, 0, 0, 0, 0);
         std::vector<dtHLAGM::AttributeToPropertyList> props;
         {
            dtHLAGM::AttributeToPropertyList attrToProp("VelocityVector", dtHLAGM::RPRAttributeType::VELOCITY_VECTOR_TYPE, false);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Velocity Vector", dtDAL::DataType::VEC3D, "", false);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("AccelerationVector", dtHLAGM::RPRAttributeType::VELOCITY_VECTOR_TYPE, false);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd0("accelX", dtDAL::DataType::FLOAT, "", false);
            attrToProp.GetParameterDefinitions().push_back(pd0);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd1("accelY", dtDAL::DataType::FLOAT, "", false);
            attrToProp.GetParameterDefinitions().push_back(pd1);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd2("accelZ", dtDAL::DataType::FLOAT, "", false);
            attrToProp.GetParameterDefinitions().push_back(pd2);
            props.push_back(attrToProp);
         }

         CheckObjectToActorMapping("TestHLA", "Jet", "BaseEntity.PhysicalEntity.Platform.Aircraft", "", "", "", &type, true, false, props);
      }
      {
         std::vector<dtHLAGM::AttributeToPropertyList> props;
         {
            dtHLAGM::AttributeToPropertyList attrToProp("Orientation", dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE, true);

            dtHLAGM::OneToManyMapping::ParameterDefinition pd(dtDAL::TransformableActorProxy::PROPERTY_ROTATION, dtDAL::DataType::VEC3, "", true);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }
         // Test a NULL dis id.
         CheckObjectToActorMapping("TestHLA", "CulturalFeature", "BaseEntity.PhysicalEntity.CulturalFeature", "", "", "", NULL, false, false, props);
      }

      // Test Non-Entity Types
      {
         std::vector<dtHLAGM::AttributeToPropertyList> props;
         {
            dtHLAGM::AttributeToPropertyList attrToProp("Orientation", dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE, true);

            dtHLAGM::OneToManyMapping::ParameterDefinition pd(dtDAL::TransformableActorProxy::PROPERTY_ROTATION, dtDAL::DataType::VEC3, "", true);
            attrToProp.GetParameterDefinitions().push_back(pd);
            props.push_back(attrToProp);
         }
         // Test a NULL dis id.
         CheckObjectToActorMapping("TestHLA", "Sensor", "BaseEntity.PhysicalEntity.Sensor", "", "", "", NULL, false, false, props);
      }

      {
         std::vector<dtHLAGM::ParameterToParameterList> params;
         {
            dtHLAGM::ParameterToParameterList paramToParam("DetonationLocation", dtHLAGM::RPRAttributeType::WORLD_COORDINATE_TYPE, true);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Location", dtDAL::DataType::VEC3, "", true);
            paramToParam.GetParameterDefinitions().push_back(pd);
            params.push_back(paramToParam);
         }

         CheckInteractionToMessageMapping(dtGame::MessageType::INFO_RESTARTED, "MunitionDetonation", "Geographic", params);
      }

      {
         std::vector<dtHLAGM::ParameterToParameterList> params;
         {
            dtHLAGM::ParameterToParameterList paramToParam("FiringObjectIdentifier", dtHLAGM::RPRAttributeType::ENTITY_IDENTIFIER_TYPE, true);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("aboutActorId", dtDAL::DataType::ACTOR, "", true);
            paramToParam.GetParameterDefinitions().push_back(pd);
            params.push_back(paramToParam);
         }
         {
            dtHLAGM::ParameterToParameterList paramToParam("FireControlSolutionRange", dtHLAGM::RPRAttributeType::FLOAT_TYPE, true);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("LateTime", dtDAL::DataType::FLOAT, "", true);
            paramToParam.GetParameterDefinitions().push_back(pd);
            params.push_back(paramToParam);
         }
         {
            dtHLAGM::ParameterToParameterList paramToParam("MunitionType", dtHLAGM::RPRAttributeType::ENTITY_TYPE, true);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("TestProp", dtDAL::DataType::ENUMERATION, "LARGE EXPLOSION", true);

            pd.AddEnumerationMapping("2 2 225 2 2 0 0", "LARGE BULLET");
            pd.AddEnumerationMapping("2 9 225 2 14 0 0", "SHORT SMOKE");
            pd.AddEnumerationMapping("2 9 225 2 14 1 0", "LONG SMOKE");

            paramToParam.GetParameterDefinitions().push_back(pd);
            params.push_back(paramToParam);

         }

         // Add a parameter to capture the name of the interaction mapping.
         {
            dtHLAGM::ParameterToParameterList paramToParam(
               dtHLAGM::HLAComponent::PARAM_NAME_MAPPING_NAME, dtHLAGM::RPRAttributeType::STRING_TYPE, false);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Mapping Name", dtDAL::DataType::STRING, "", false);
            paramToParam.GetParameterDefinitions().push_back(pd);
            params.push_back(paramToParam);
         }

         CheckInteractionToMessageMapping(dtGame::MessageType::INFO_TIMER_ELAPSED, "WeaponFire", "Geographic", params);
      }
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}
