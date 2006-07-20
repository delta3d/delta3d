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

#include <dtUtil/coordinates.h>
#include <dtCore/globals.h>
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtGame/messagetype.h>
#include <dtGame/clientgamemanager.h>
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

class HLAConfigTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(HLAConfigTests);

      CPPUNIT_TEST(TestConfigure);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();
      void TestConfigure();
   private:
      dtUtil::Log* logger;
      dtCore::RefPtr<dtHLAGM::HLAComponent> mTranslator;
      dtCore::RefPtr<dtGame::GameManager> mGameManager;
      static const char* const mHLAActorRegistry;
      void CheckObjectToActorMapping(
         const std::string& category,
         const std::string& name,
         const std::string& objectClassName,
         const std::string& entityIdAttrName,
         const dtHLAGM::EntityType* entityType,
         bool remoteOnly,
         const std::vector<dtHLAGM::AttributeToPropertyList>& props);
      void CheckInteractionToMessageMapping(
         const dtGame::MessageType& messageType,
         const std::string& interactionName,
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

   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
   std::string logName("HLAConfigTest");
   //Uncomment this to turn on config xml logging.
   //dtUtil::Log::GetInstance("hlafomconfigxml.cpp").SetLogLevel(dtUtil::Log::LOG_DEBUG);
   logger = &dtUtil::Log::GetInstance(logName);
   mTranslator = new dtHLAGM::HLAComponent();
   dtCore::Scene* scene = new dtCore::Scene();
   mGameManager = new dtGame::ClientGameManager(*scene);

}

// Called implicitly by CPPUNIT when the app terminates
void HLAConfigTests::tearDown()
{
   mTranslator = NULL;
   if (mGameManager->GetRegistry(mHLAActorRegistry) != NULL)
      mGameManager->UnloadActorRegistry(mHLAActorRegistry);

   mGameManager = NULL;

}

void HLAConfigTests::CheckObjectToActorMapping(
   const std::string& category,
   const std::string& name,
   const std::string& objectClassName,
   const std::string& entityIdAttrName,
   const dtHLAGM::EntityType* entityType,
   bool remoteOnly,
   const std::vector<dtHLAGM::AttributeToPropertyList>& props)
{
   dtCore::RefPtr<dtDAL::ActorType> type = dtDAL::LibraryManager::GetInstance().FindActorType(category, name);
   CPPUNIT_ASSERT(type != NULL);

   //make absolutely sure we call the const version of the method.
   dtCore::RefPtr<const dtHLAGM::ObjectToActor> otoa =
         static_cast<const dtHLAGM::HLAComponent*>(mTranslator.get())->GetObjectMapping(objectClassName, entityType);
   dtCore::RefPtr<dtHLAGM::ObjectToActor> otoaNonConst = mTranslator->GetObjectMapping(objectClassName, entityType);
   CPPUNIT_ASSERT(otoa.get() == otoaNonConst.get());

   CPPUNIT_ASSERT_MESSAGE("Object to actor mapping for type " + type->GetCategory() + "." + type->GetName() + " should exist.",
      otoa != NULL);

   if (remoteOnly)
   {
      CPPUNIT_ASSERT_MESSAGE("If the mapping is remote only, fetching by actor type should not return the same mapping object.",
         otoa != mTranslator->GetActorMapping(*type));
   }
   else
   {
      CPPUNIT_ASSERT_MESSAGE("If the mapping is not remote only, fetching by actor type should return the same mapping object.",
         otoa == mTranslator->GetActorMapping(*type));
   }

   CPPUNIT_ASSERT_MESSAGE("The entity id should be \"" + entityIdAttrName + "\" but it is \"" + otoa->GetEntityIdAttributeName() + "\".",
      otoa->GetEntityIdAttributeName() == entityIdAttrName);

   CPPUNIT_ASSERT_MESSAGE("The remote only value of object class mapping for " + objectClassName + " with actor type "
      + category + "." + name + " has the wrong value of remote only.", otoa->IsRemoteOnly() == remoteOnly);

   CPPUNIT_ASSERT_MESSAGE("The GetActorType method should return the same type as it was mapped to.",
      otoa->GetActorType() == *type);
   CPPUNIT_ASSERT_MESSAGE("The GetObjectClassName method should return " + objectClassName
      + "  not " + otoa->GetObjectClassName() + ".",
      otoa->GetObjectClassName() == objectClassName);

   if (entityType == NULL)
      CPPUNIT_ASSERT_MESSAGE("DIS ID should be NULL.", otoa->GetDisID() == NULL);
   else
   {
      CPPUNIT_ASSERT_MESSAGE("DIS ID should not be NULL.", otoa->GetDisID() != NULL);
 //     std::ostringstream ss;
//      ss << "DIS ID should be equal to \"" << *entityType << "\" but it is \"" << *otoa->GetDisID() << ".\"";
 //     CPPUNIT_ASSERT_MESSAGE(ss.str(), *otoa->GetDisID() == *entityType);
   }

   const std::vector<dtHLAGM::AttributeToPropertyList>& propsActual = otoa->GetOneToManyMappingVector();
   CPPUNIT_ASSERT(propsActual.size() == props.size());
   for (unsigned i = 0; i < props.size(); ++i)
   {
      std::ostringstream ss;
      ss << "AttributeToPropertyList" << i << " with name " << props[i].GetHLAName() << " should match the one in the mapping with name " << propsActual[i].GetHLAName() << ".";

      CPPUNIT_ASSERT_MESSAGE(ss.str(),  props[i] == propsActual[i]);
   }
}

void HLAConfigTests::CheckInteractionToMessageMapping(
   const dtGame::MessageType& messageType,
   const std::string& interactionName,
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
}


void HLAConfigTests::TestConfigure()
{
   try
   {
      CPPUNIT_ASSERT_MESSAGE("Library should not yet be loaded.",
         dtDAL::LibraryManager::GetInstance().GetRegistry(mHLAActorRegistry) == NULL);
      dtHLAGM::HLAComponentConfig config;

      try
      {
         config.LoadConfiguration(*mTranslator, "Federations/HLAMappingExample.xml");
         CPPUNIT_FAIL("It should fail since no game manager was assigned to the mTranslator.");
      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_ASSERT_MESSAGE("the exception should have been an XML_CONFIG_EXCEPTION",
            ex.TypeEnum() == dtHLAGM::ExceptionEnum::XML_CONFIG_EXCEPTION);
      }

      mGameManager->AddComponent(*mTranslator, dtGame::GameManager::ComponentPriority::NORMAL);

      config.LoadConfiguration(*mTranslator, "Federations/HLAMappingExample.xml");

      CPPUNIT_ASSERT_MESSAGE(std::string("Library should be loaded:") + mHLAActorRegistry,
         dtDAL::LibraryManager::GetInstance().GetRegistry(mHLAActorRegistry) != NULL);

      {
         dtHLAGM::EntityType type(1, 1, 222, 2, 4, 6, 0);

         std::vector<dtHLAGM::AttributeToPropertyList> props;

         {
            dtHLAGM::AttributeToPropertyList attrToProp("EntityType", dtHLAGM::RPRAttributeType::ENTITY_TYPE, true);
            attrToProp.GetParameterDefinitions().push_back(dtHLAGM::OneToManyMapping::ParameterDefinition("Entity Type", dtDAL::DataType::ENUMERATION, "", false)); 
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

            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Rotation", dtDAL::DataType::VEC3, "", true);
            attrToProp.GetParameterDefinitions().push_back(pd); 
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("WorldLocation", dtHLAGM::RPRAttributeType::WORLD_COORDINATE_TYPE, true);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Translation", dtDAL::DataType::VEC3, "", true);
            attrToProp.GetParameterDefinitions().push_back(pd); 
            props.push_back(attrToProp);
         }

         CheckObjectToActorMapping("TestHLA", "Tank",
            "BaseEntity.PhysicalEntity.Platform.GroundVehicle",
            "EntityIdentifier", &type, false, props);
      }

      {
         std::vector<dtHLAGM::AttributeToPropertyList> props;
         {
            dtHLAGM::AttributeToPropertyList attrToProp("EntityType", dtHLAGM::RPRAttributeType::ENTITY_TYPE, true);

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

            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Rotation", dtDAL::DataType::VEC3, "", true);
            attrToProp.GetParameterDefinitions().push_back(pd); 
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToPropertyList attrToProp("WorldLocation", dtHLAGM::RPRAttributeType::WORLD_COORDINATE_TYPE, true);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Translation", dtDAL::DataType::VEC3, "", true);
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
            "EntityIdentifier", &type1, false, props);

         dtHLAGM::EntityType type2(1, 2, 222, 20, 2, 6, 0);
         CheckObjectToActorMapping("TestHLA", "Helicopter", "BaseEntity.PhysicalEntity.Platform.Aircraft",
            "EntityIdentifier", &type2, true, props);
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

         CheckObjectToActorMapping("TestHLA", "Jet", "BaseEntity.PhysicalEntity.Platform.Aircraft", "", &type, true, props);
      }
      {
         dtHLAGM::EntityType type(1, 2, 225, 0, 0, 0, 0);
         std::vector<dtHLAGM::AttributeToPropertyList> props;
         //it should refuse to map it bidirectionally and spit out an error in the log, but it shouldn't blow up
         CheckObjectToActorMapping("TestHLA", "Jet", "TryingToMapTheJetBidirectionallyAgain", "",  &type, true, props);
      }

      {
         std::vector<dtHLAGM::ParameterToParameterList> params;
         {
            dtHLAGM::ParameterToParameterList paramToParam("DetonationLocation", dtHLAGM::RPRAttributeType::WORLD_COORDINATE_TYPE, true);
            dtHLAGM::OneToManyMapping::ParameterDefinition pd("Location", dtDAL::DataType::VEC3, "", true);
            paramToParam.GetParameterDefinitions().push_back(pd); 
            params.push_back(paramToParam);
         }

         CheckInteractionToMessageMapping(dtGame::MessageType::INFO_RESTARTED, "MunitionDetonation", params);
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

         CheckInteractionToMessageMapping(dtGame::MessageType::INFO_TIMER_ELAPSED, "WeaponFire", params);
      }
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}
