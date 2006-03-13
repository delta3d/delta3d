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
#include <dtCore/globals.h>
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtGame/messagetype.h>
#include <dtGame/clientgamemanager.h>
#include <dtUtil/coordinates.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/parametertoparameter.h>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/hlacomponentconfig.h>
#include <dtHLAGM/exceptionenum.h>
#include <dtGame/gamemanager.h>

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
         const dtHLAGM::EntityType* entityType,
         bool remoteOnly,
         const std::vector<dtHLAGM::AttributeToProperty>& props);
      void CheckInteractionToMessageMapping(
         const dtGame::MessageType& messageType,
         const std::string& interactionName, 
         const std::vector<dtHLAGM::ParameterToParameter>& params);
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(HLAConfigTests);

#if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
const char* const HLAConfigTests::mHLAActorRegistry="testGameActorLibraryd";
#else
const char* const HLAConfigTests::mHLAActorRegistry="testGameActorLibrary";
#endif


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
   const dtHLAGM::EntityType* entityType,
   bool remoteOnly,
   const std::vector<dtHLAGM::AttributeToProperty>& props)
{
   dtCore::RefPtr<dtDAL::ActorType> type = dtDAL::LibraryManager::GetInstance().FindActorType(category, name);
   CPPUNIT_ASSERT(type != NULL);  
   dtCore::RefPtr<const dtHLAGM::ObjectToActor> otoa = mTranslator->GetObjectMapping(objectClassName, entityType);
   
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

   CPPUNIT_ASSERT_MESSAGE("The remote only value of object class mapping for " + objectClassName + " with actor type "
      + category + "." + name + " has the wrong value of remote only.", otoa->IsRemoteOnly() == remoteOnly);

   CPPUNIT_ASSERT_MESSAGE("The GetActorType method should return the same type as it was mapped to.",
      otoa->GetActorType() == *type);
   CPPUNIT_ASSERT_MESSAGE("The GetObjectTypeName method should return " + objectClassName 
      + "  not " + otoa->GetObjectTypeName() + ".", 
      otoa->GetObjectTypeName() == objectClassName);
      
   if (entityType == NULL)
      CPPUNIT_ASSERT_MESSAGE("DIS ID should be NULL.", otoa->GetDisID() == NULL);
   else
   {
      CPPUNIT_ASSERT_MESSAGE("DIS ID should not be NULL.", otoa->GetDisID() != NULL);
      
      // Somehow this won't link because of an unresolved std::ostream& operator<<(std::ostream &o, const EntityType &et)
      // on VS2003...

      //std::ostringstream ss;
      //ss << "DIS ID should be equal to \"" << *entityType << "\" but it is \"" << *otoa->GetDisID() << ".\"";
      //CPPUNIT_ASSERT_MESSAGE(ss.str(), *otoa->GetDisID() == *entityType);      
      CPPUNIT_ASSERT(*otoa->GetDisID() == *entityType);
   }  
   
   const std::vector<dtHLAGM::AttributeToProperty>& propsActual = otoa->GetOneToOneMappingVector();
   CPPUNIT_ASSERT(propsActual.size() == props.size());
   for (unsigned i = 0; i < props.size(); ++i)
   {
      std::ostringstream ss;
      ss << "AttributeToProperty " << i << " with name " << props[i].GetGameName() << " should match the one in the mapping with name " << propsActual[i].GetGameName() << ".";
      if (propsActual[i].GetGameType() == dtDAL::DataType::ENUMERATION)
      {
         for (int j = 0; j < 100; ++j)
         {
            std::string value;
            if (propsActual[i].GetGameEnumerationValue(j, value))
            {
               ss << std::endl;      
               ss << "Actual HLA ID " << j << " maps to value " << value;
            }
            if (props[i].GetGameEnumerationValue(j, value))
            {
               ss << std::endl;      
               ss << "Expected HLA ID " << j << " maps to value " << value;
            }
         }
      }
      
      CPPUNIT_ASSERT_MESSAGE(ss.str(),  props[i] == propsActual[i]);
   }
}
   
void HLAConfigTests::CheckInteractionToMessageMapping(
   const dtGame::MessageType& messageType,
   const std::string& interactionName, 
   const std::vector<dtHLAGM::ParameterToParameter>& params)
{
   dtCore::RefPtr<const dtHLAGM::InteractionToMessage> itom = mTranslator->GetMessageMapping(messageType);
   CPPUNIT_ASSERT_MESSAGE("Interation To Message mapping for type " + messageType.GetName() + " should exist.", 
      itom != NULL);

   CPPUNIT_ASSERT_MESSAGE("The GetActorType method should return the same type as it was mapped to.",
      itom->GetMessageType() == messageType);
   CPPUNIT_ASSERT_MESSAGE("The GetInteractionName method should return " +  interactionName
      + "  not " + itom->GetInteractionName() + ".", 
      itom->GetInteractionName() == interactionName);
         
   const std::vector<dtHLAGM::ParameterToParameter>& paramsActual = itom->GetOneToOneMappingVector();
   CPPUNIT_ASSERT(paramsActual.size() == params.size());
   for (unsigned i = 0; i < params.size(); ++i)
   {
      std::ostringstream ss;
      ss << "ParameterToParameter " << i << " with name " << params[i].GetGameName() << " should match the one in the mapping with name " << paramsActual[i].GetGameName() << ".";
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
         
         std::vector<dtHLAGM::AttributeToProperty> props;
         
         {
            dtHLAGM::AttributeToProperty attrToProp("EntityType", "Entity Type", dtHLAGM::AttributeType::ENTITY_TYPE,
               dtDAL::DataType::ENUMERATION, "", true, false);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToProperty attrToProp("DamageState", "Damage State", dtHLAGM::AttributeType::UNSIGNED_INT_TYPE,
               dtDAL::DataType::ENUMERATION, "Destroyed", false, false);
            attrToProp.AddEnumerationMapping(0, "No Damage");
            attrToProp.AddEnumerationMapping(1, "Slight Damage");
            attrToProp.AddEnumerationMapping(2, "Moderate Damage");
            attrToProp.AddEnumerationMapping(3, "Destroyed");
            props.push_back(attrToProp);
         }
         
         {
            dtHLAGM::AttributeToProperty attrToProp("Orientation", "Rotation", dtHLAGM::AttributeType::EULER_ANGLES_TYPE,
               dtDAL::DataType::VEC3, "", true, true);
            props.push_back(attrToProp);
         }
         
         {
            dtHLAGM::AttributeToProperty attrToProp("WorldLocation", "Location", dtHLAGM::AttributeType::WORLD_COORDINATE_TYPE,
               dtDAL::DataType::VEC3F, "", true, true);
            props.push_back(attrToProp);
         }

         CheckObjectToActorMapping("TestHLA", "Tank", "BaseEntity.PhysicalEntity.Platform.GroundVehicle", &type, false, props);
      }

      {
         std::vector<dtHLAGM::AttributeToProperty> props;
         {
            dtHLAGM::AttributeToProperty attrToProp("EntityType", "Entity Type", dtHLAGM::AttributeType::ENTITY_TYPE,
               dtDAL::DataType::ENUMERATION, "", true, false);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToProperty attrToProp("DamageState", "Damage State", dtHLAGM::AttributeType::UNSIGNED_INT_TYPE,
               dtDAL::DataType::ENUMERATION, "Destroyed", false, false);
            attrToProp.AddEnumerationMapping(0, "No Damage");
            attrToProp.AddEnumerationMapping(1, "Slight Damage");
            attrToProp.AddEnumerationMapping(2, "Moderate Damage");
            attrToProp.AddEnumerationMapping(3, "Destroyed");
            props.push_back(attrToProp);
         }
         
         {
            dtHLAGM::AttributeToProperty attrToProp("Orientation", "Rotation", dtHLAGM::AttributeType::EULER_ANGLES_TYPE,
               dtDAL::DataType::VEC3, "", true, true);
            props.push_back(attrToProp);
         }
         
         {
            dtHLAGM::AttributeToProperty attrToProp("WorldLocation", "Location", dtHLAGM::AttributeType::WORLD_COORDINATE_TYPE,
               dtDAL::DataType::VEC3F, "", true, true);
            props.push_back(attrToProp);
         }

         {
            dtHLAGM::AttributeToProperty attrToProp("VelocityVector", "Velocity Vector", dtHLAGM::AttributeType::VELOCITY_VECTOR_TYPE,
               dtDAL::DataType::VEC3D, "", false, false);
            props.push_back(attrToProp);
         }

         dtHLAGM::EntityType type1(1, 2, 225, 1, 9, 4, 0);
         CheckObjectToActorMapping("TestHLA", "Jet", "BaseEntity.PhysicalEntity.Platform.Aircraft", &type1, false, props);
         
         dtHLAGM::EntityType type2(1, 2, 222, 20, 2, 6, 0);
         CheckObjectToActorMapping("TestHLA", "Helicopter", "BaseEntity.PhysicalEntity.Platform.Aircraft", &type2, true, props);
      }
            
      {
         dtHLAGM::EntityType type(1, 2, 225, 0, 0, 0, 0);
         std::vector<dtHLAGM::AttributeToProperty> props;
         {
            dtHLAGM::AttributeToProperty attrToProp("VelocityVector", "Velocity Vector", dtHLAGM::AttributeType::VELOCITY_VECTOR_TYPE,
               dtDAL::DataType::VEC3D, "", false, false);
            props.push_back(attrToProp);
         }

         CheckObjectToActorMapping("TestHLA", "Jet", "BaseEntity.PhysicalEntity.Platform.Aircraft", &type, true, props);
      }
      {
         dtHLAGM::EntityType type(1, 2, 225, 0, 0, 0, 0);
         std::vector<dtHLAGM::AttributeToProperty> props;
         //it should refuse to map it bidirectionally and spit out an error in the log, but it shouldn't blow up
         CheckObjectToActorMapping("TestHLA", "Jet", "TryingToMapTheJetBidirectionallyAgain", &type, true, props);
      }

      {
         std::vector<dtHLAGM::ParameterToParameter> params;
         {
            dtHLAGM::ParameterToParameter paramToParam("DetonationLocation", "Location", dtHLAGM::AttributeType::WORLD_COORDINATE_TYPE,
               dtDAL::DataType::VEC3, "", true, true);
            params.push_back(paramToParam);
         }

         CheckInteractionToMessageMapping(dtGame::MessageType::INFO_RESTARTED, "InteractionRoot.MunitionDetonation", params);
         
      }
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}
