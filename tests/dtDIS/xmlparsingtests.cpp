/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
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
*/
#include <cppunit/extensions/HelperMacros.h>
#include <dtDIS/disxml.h>
#include <dtDIS/propertyname.h>
#include <dtCore/refptr.h>
#include <dtGame/gamemanager.h>
#include <dtCore/scene.h>
#include <dtUtil/xercesparser.h>
#include <dtCore/globals.h>
#include <dtActors/engineactorregistry.h>

namespace dtDIS
{
   class XMLParsing : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( XMLParsing );
      CPPUNIT_TEST( TestOverwritingPropertyNames );
      CPPUNIT_TEST( TestMappingEntityTypeToActorType );
      CPPUNIT_TEST_SUITE_END();

      void setup();
      void teardown();

      void TestOverwritingPropertyNames();
      void TestMappingEntityTypeToActorType();
   };

   using namespace dtDIS;

   CPPUNIT_TEST_SUITE_REGISTRATION( XMLParsing );

   ////////////////////////////////////////////////////////////////////////////////
   void XMLParsing::setup()
   {

   }

   ////////////////////////////////////////////////////////////////////////////////
   void XMLParsing::teardown()
   {

   }

   ////////////////////////////////////////////////////////////////////////////////
   void XMLParsing::TestOverwritingPropertyNames()
   {
      const std::string startPropertyName = dtDIS::EnginePropertyName::ENTITY_LOCATION;

      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList() + ":" + dtCore::GetDeltaRootPath() + "/tests/data");
      std::string path = dtCore::FindFileInPathList("disPropertyNameOverwrites.xml");
      CPPUNIT_ASSERT(dtUtil::FileUtils::GetInstance().FileExists(path));

      dtDIS::SharedState config("", path);


      const std::string endPropertyName = dtDIS::EnginePropertyName::ENTITY_LOCATION;

      CPPUNIT_ASSERT_MESSAGE("Referenced property name should have changed after loading the .xml file",
                            startPropertyName != endPropertyName);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Referenced property name should be set to what's in the .xml file",
         std::string("NewLocationName"), dtDIS::EnginePropertyName::ENTITY_LOCATION.Get());

   }

   ////////////////////////////////////////////////////////////////////////////////
   void XMLParsing::TestMappingEntityTypeToActorType()
   {
      //example file maps the DIS EntityType below to a StaticMesh actor

      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList() + ":" + dtCore::GetDeltaRootPath() + "/tests/data");
      std::string path = dtCore::FindFileInPathList("disActorTypeMapping.xml");
      CPPUNIT_ASSERT(dtUtil::FileUtils::GetInstance().FileExists(path));

      dtDIS::SharedState config("", path);
      DIS::EntityType eid;
      eid.setEntityKind(1);
      eid.setCategory(2);
      eid.setCountry(3);

      const dtDAL::ActorType* actorType = NULL;

      bool res = config.GetActorMap().GetMappedActor(eid, actorType);
      CPPUNIT_ASSERT_MESSAGE("Did not find a matching ActorType based on the supplied DIS::EntityType",
                            res == true);

      CPPUNIT_ASSERT_MESSAGE("ActorMap returned back the wrong ActorType for the supplied DIS::EntityType",
                              dtActors::EngineActorRegistry::STATIC_MESH_ACTOR_TYPE.get() == actorType);      
   }
}
