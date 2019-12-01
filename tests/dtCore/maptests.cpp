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
 * @author David Guthrie
 */

#include <prefix/unittestprefix.h>
#include <testActorLibrary/testactorlib.h>
#include <testActorLibrary/testdalenvironmentactor.h>

#include <dtABC/application.h>

#include <dtActors/engineactorregistry.h>

#include <dtCore/scene.h>
#include <dtCore/timer.h>

#include <dtCore/abstractenumactorproperty.h>
#include <dtCore/actoractorproperty.h>
#include <dtCore/actoridactorproperty.h>
#include <dtCore/actorproxy.h>
#include <dtCore/arrayactorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/doubleactorproperty.h>
#include <dtCore/environmentactor.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/propertycontaineractorproperty.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/groupactorproperty.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/actorfactory.h>
#include <dtCore/map.h>
#include <dtCore/mapxml.h>
#include <dtCore/namedactorparameter.h>
#include <dtCore/namedbooleanparameter.h>
#include <dtCore/nameddoubleparameter.h>
#include <dtCore/namedenumparameter.h>
#include <dtCore/namedfloatparameter.h>
#include <dtCore/namedgameeventparameter.h>
#include <dtCore/namedintparameter.h>
#include <dtCore/namedlongintparameter.h>
#include <dtCore/namedresourceparameter.h>
#include <dtCore/namedrgbacolorparameter.h>
#include <dtCore/namedstringparameter.h>
#include <dtCore/namedvectorparameters.h>
#include <dtCore/namedgroupparameter.h>
#include <dtCore/namedgroupparameter.inl>
#include <dtCore/transformableactorproxy.h>
#include <dtCore/project.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/bitmaskactorproperty.h>
#include <dtCore/actorcomponentcontainer.h>
#include <dtCore/mapxml.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/xercesutils.h>

#include <cppunit/extensions/HelperMacros.h>

#include <xercesc/sax/SAXParseException.hpp>

#include <osg/io_utils>
#include <osg/Math>

#include <cstdio>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>

extern dtABC::Application& GetGlobalApplication();

///////////////////////////////////////////////////////////////////////////////////////
class MapTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(MapTests);
   CPPUNIT_TEST(TestAddRegistryWithoutLibrary);
   CPPUNIT_TEST(TestMapAddRemoveProxies);
   CPPUNIT_TEST(TestMapProxySearch);
   CPPUNIT_TEST(TestMapLibraryHandling);
   CPPUNIT_TEST(TestMapEventsModified);
   CPPUNIT_TEST(TestIsMapFileValid);
   CPPUNIT_TEST(TestLoadMapIntoScene);
   CPPUNIT_TEST(TestMapSaveAndLoad);
   CPPUNIT_TEST(TestMapSaveAndLoadEvents);
   CPPUNIT_TEST(TestMapSaveAndLoadGroup);
   CPPUNIT_TEST(TestMapSaveAndLoadPropertyContainerProperty);
   CPPUNIT_TEST(TestMapSaveAndLoadNestedPropertyContainerArray);
   CPPUNIT_TEST(TestMapSaveAndLoadActorGroups);
   CPPUNIT_TEST(TestMapAddLibrariesOnSave);
   CPPUNIT_TEST(TestMapCorrectLibraryListSetsModified);
   CPPUNIT_TEST(TestPrefabLoadHeader);
   CPPUNIT_TEST(TestPrefabCorrectLibraryList);
   CPPUNIT_TEST(TestShouldSaveProperty);
   CPPUNIT_TEST(TestLibraryMethods);
   CPPUNIT_TEST(TestWildCard);
   CPPUNIT_TEST(TestEnvironmentMapLoading);
   CPPUNIT_TEST(TestLoadEnvironmentMapIntoScene);
   CPPUNIT_TEST(TestActorProxyRemoveProperties);
   CPPUNIT_TEST(TestRemovePropertiesByPointer);
   CPPUNIT_TEST(TestCreateMapsMultiContext);
   CPPUNIT_TEST(TestSaveAsMultiContext);
   CPPUNIT_TEST(TestParsingMapHeaderData);
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestAddRegistryWithoutLibrary();
   void TestMapAddRemoveProxies();
   void TestMapProxySearch();
   void TestMapLibraryHandling();
   void TestMapEventsModified();
   void TestMapSaveAndLoad();
   void TestMapSaveAndLoadEvents();
   void TestMapSaveAndLoadGroup();
   void TestMapSaveAndLoadPropertyContainerProperty();
   void TestMapSaveAndLoadNestedPropertyContainerArray();
   void TestMapSaveAndLoadActorGroups();
   void TestMapAddLibrariesOnSave();
   void TestMapCorrectLibraryListSetsModified();
   void TestPrefabLoadHeader();
   void TestPrefabCorrectLibraryList();
   void TestShouldSaveProperty();
   void TestIsMapFileValid();
   void TestLoadMapIntoScene();
   void TestLibraryMethods();
   void TestEnvironmentMapLoading();
   void TestLoadEnvironmentMapIntoScene();
   void TestWildCard();
   void TestActorProxyRemoveProperties();
   void TestRemovePropertiesByPointer();
   void TestCreateMapsMultiContext();
   void TestSaveAsMultiContext();
   void TestParsingMapHeaderData();

   static const std::string TEST_PROJECT_DIR;
   static const std::string TEST_PROJECT_DIR_2;

private:
   static const std::string mDTActorsLibraryName;
   static const std::string mExampleLibraryName;
   static const std::string mExampleGameLibraryName;

   void createActors(dtCore::Map& map);
   dtCore::ActorProperty* getActorProperty(dtCore::Map& map,
         const std::string& propName, dtCore::DataType& type, unsigned which = 0);

   dtUtil::Log* logger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(MapTests);

const std::string MapTests::mDTActorsLibraryName="dtActors";
const std::string MapTests::mExampleLibraryName="testActorLibrary";
const std::string MapTests::mExampleGameLibraryName="testGameActorLibrary";

const std::string MapTests::TEST_PROJECT_DIR="WorkingMapProject";
const std::string MapTests::TEST_PROJECT_DIR_2="WorkingMapProject2";

const std::string DATA_DIR = dtUtil::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR + "examples/data";
const std::string TESTS_DIR = dtUtil::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR + "tests";
const std::string MAPPROJECTCONTEXT = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtCore" + dtUtil::FileUtils::PATH_SEPARATOR + MapTests::TEST_PROJECT_DIR;

class TestObjectRegistry : public dtCore::ActorPluginRegistry
{
public:
   TestObjectRegistry(const std::string& name, const std::string& desc)
: dtCore::ActorPluginRegistry(name, desc)
   {}
   virtual void RegisterActorTypes() {}
};

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::setUp()
{
   try
   {
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(dtCore::ActorFactory::DEFAULT_ACTOR_LIBRARY);
      dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
      std::string logName("mapTest");

      logger = &dtUtil::Log::GetInstance(logName);

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      std::string currentDir = fileUtils.CurrentDirectory();
      std::string projectDir("dtCore");
      if (currentDir.substr(currentDir.size() - projectDir.size()) != projectDir)
      {
         fileUtils.PushDirectory(projectDir);
      }

      if (fileUtils.DirExists(TEST_PROJECT_DIR))
      {
         fileUtils.DirDelete(TEST_PROJECT_DIR, true);
      }

      if (fileUtils.DirExists(TEST_PROJECT_DIR_2))
      {
         fileUtils.DirDelete(TEST_PROJECT_DIR_2, true);
      }

      // Create without a maps directory so the code will test that will be created on demand.
      dtCore::Project::GetInstance().CreateContext(TEST_PROJECT_DIR, false);
      dtCore::Project::GetInstance().SetContext(TEST_PROJECT_DIR);
      dtCore::Project::GetInstance().CreateContext(TEST_PROJECT_DIR_2, true);
      dtCore::Project::GetInstance().AddContext(TEST_PROJECT_DIR_2);

      CPPUNIT_ASSERT_MESSAGE("Context 0 should have no maps dir",
            !fileUtils.DirExists(dtCore::Project::GetInstance().GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + "maps"));
      CPPUNIT_ASSERT_MESSAGE("Context 1 should have a maps dir",
            fileUtils.DirExists(dtCore::Project::GetInstance().GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + "maps"));

      //copy the vector because the act of deleting a map will reload the map names list.
      const std::set<std::string> v = dtCore::Project::GetInstance().GetMapNames();

      for (std::set<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
      {
         dtCore::Project::GetInstance().DeleteMap(*i, true);
      }
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}


///////////////////////////////////////////////////////////////////////////////////////
void MapTests::tearDown()
{
   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   bool shouldPopDir;

   std::string currentDir = fileUtils.CurrentDirectory();
   std::string projectDir("dtCore");
   shouldPopDir = currentDir.substr(currentDir.size() - projectDir.size()) == projectDir;

   try
   {
      dtCore::Project::GetInstance().SetContext(TEST_PROJECT_DIR);
      dtCore::Project::GetInstance().AddContext(TEST_PROJECT_DIR_2);
      //copy the vector because the act of deleting a map will reload the map names list.
      const std::set<std::string> v = dtCore::Project::GetInstance().GetMapNames();

      for (std::set<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
      {
         dtCore::Project::GetInstance().DeleteMap(*i);
      }


      dtCore::Project::GetInstance().ClearAllContexts();

      if (fileUtils.DirExists(TEST_PROJECT_DIR))
      {
         fileUtils.DirDelete(TEST_PROJECT_DIR, true);
      }

      if (fileUtils.DirExists(TEST_PROJECT_DIR_2))
      {
         fileUtils.DirDelete(TEST_PROJECT_DIR_2, true);
      }

      if (dtCore::ActorFactory::GetInstance().GetRegistry(mExampleLibraryName) != NULL)
      {
         dtCore::ActorFactory::GetInstance().UnloadActorRegistry(mExampleLibraryName);
      }
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(dtCore::ActorFactory::DEFAULT_ACTOR_LIBRARY);

      if (shouldPopDir)
      {
         fileUtils.PopDirectory();
      }
   }
   catch (const dtUtil::Exception& e)
   {
      if (shouldPopDir)
      {
         fileUtils.PopDirectory();
      }
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::createActors(dtCore::Map& map)
{
   dtCore::ActorFactory& libMgr = dtCore::ActorFactory::GetInstance();
   dtCore::ActorTypeVec actorTypes;
   std::vector<dtCore::ActorProperty*> props;

   libMgr.GetActorTypes(actorTypes);

   int skippedActors = 0;
   int nameCounter = 0;
   char nameAsString[21];

   logger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, "Adding one of each proxy type to the map:");

   //only create half the actors :-)
   for (unsigned int i=0; i < actorTypes.size(); ++i)
   {
      // In order to keep the tests fasts, we skip the nasty slow ones.
      if (actorTypes[i]->GetName() == dtActors::EngineActorRegistry::CLOUD_PLANE_ACTOR_TYPE->GetName() ||
            actorTypes[i]->GetName() == dtActors::EngineActorRegistry::CLOUD_DOME_ACTOR_TYPE->GetName()  ||
            actorTypes[i]->GetName() == dtActors::EngineActorRegistry::WEATHER_ENVIRONMENT_ACTOR_TYPE->GetName() ||
            actorTypes[i]->GetName() == "Test Environment Actor" ||
            actorTypes[i]->GetName() == "Waypoint" ||
            actorTypes[i]->GetName() == "Sound Actor")
      {
         ++skippedActors;
         continue; // go to next actor
      }

      dtCore::RefPtr<dtCore::BaseActorObject> proxy;
      // Test timing Stuff
      dtCore::Timer testClock;
      dtCore::Timer_t testClockStart = testClock.Tick();

      logger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
            "Creating actor proxy with type \"" + actorTypes[i]->GetFullName() + "\"." );

      proxy = libMgr.CreateActor(*actorTypes[i]);
      snprintf(nameAsString, 21, "%d", nameCounter);
      proxy->SetName(std::string(nameAsString));
      ++nameCounter;

      logger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
            "Set proxy name to: %s", proxy->GetName().c_str());

      proxy->GetPropertyList(props);
      for (unsigned int j=0; j<props.size(); ++j)
      {
         logger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
               "Property: Name: %s, Type: %s",
               props[j]->GetName().c_str(), props[j]->GetDataType().GetName().c_str());
      }

      // Temporary timing for map tests...  when we get a slow one, we should exclude it from
      // the tests so that tests don't slow down universally.
      dtCore::Timer_t testClockDone = testClock.Tick();
      double timeToCreate = testClock.DeltaSec(testClockStart, testClockDone);
      if (timeToCreate > 0.5) // more than .5 seconds is too long for 1 object in a test
         logger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
               "SLOW ACTOR CREATED IN TESTS - Type: %s, time[%f].  To ignore this slow actor in tests, modify these files (maptests.cpp, messagetests.cpp, proxytests.cpp, and gamemanagertests.cpp)",
               actorTypes[i]->GetName().c_str(), timeToCreate);

      map.AddProxy(*proxy);

      CPPUNIT_ASSERT_MESSAGE("Proxy list has the wrong size.",
            map.GetAllProxies().size() == (i + 1 - skippedActors));
      CPPUNIT_ASSERT_MESSAGE("Last proxy in the list should equal the new proxy.",
            map.GetAllProxies().find(proxy->GetId())->second == proxy.get());
   }
   CPPUNIT_ASSERT_MESSAGE("The actors that should have been skipped when creating the actors for the test were not found.  This code is out of date.",
         skippedActors >= 2);
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestAddRegistryWithoutLibrary()
{
   try
   {
      const std::string& TESTREG("MrTestReg");
      dtCore::ActorFactory& regManager = dtCore::ActorFactory::GetInstance();
      CPPUNIT_ASSERT(regManager.GetRegistry(TESTREG) == NULL);
      dtCore::ActorFactory::RegistryEntry entry;
      entry.registry = new TestObjectRegistry(TESTREG, TESTREG);
      regManager.AddRegistryEntry(TESTREG, entry);
      CPPUNIT_ASSERT(regManager.GetRegistry(TESTREG) != NULL);
      CPPUNIT_ASSERT(regManager.GetRegistry(TESTREG) == entry.registry);
      regManager.UnloadActorRegistry(TESTREG);
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapAddRemoveProxies()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      dtCore::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

      createActors(map);

      std::set<std::string> copy;
      CPPUNIT_ASSERT_MESSAGE("The set of actor classes should not be empty.",
            !map.GetProxyActorClasses().empty());
      copy.insert(map.GetProxyActorClasses().begin(), map.GetProxyActorClasses().end());
      map.RebuildProxyActorClassSet();
      CPPUNIT_ASSERT_MESSAGE("The set of actor classes should not be empty.",
            !map.GetProxyActorClasses().empty());
      CPPUNIT_ASSERT_MESSAGE("The set of actor classes should be the same size as the copy.",
            map.GetProxyActorClasses().size() == copy.size() );

      unsigned int maxId = map.GetAllProxies().size();
      for (unsigned int x = 0;  x < maxId; ++x)
      {
         CPPUNIT_ASSERT_MESSAGE("Unable to remove item 0",
               map.RemoveProxy(*map.GetAllProxies().begin()->second));

         CPPUNIT_ASSERT_MESSAGE("Proxy list has the wrong size.",
               map.GetAllProxies().size() == (unsigned int)(maxId - (x + 1)));
      }
      map.RebuildProxyActorClassSet();
      CPPUNIT_ASSERT_MESSAGE("The set of actor classes should be empty.",
            map.GetProxyActorClasses().empty());

      dtCore::ActorRefPtrVector proxies;
      map.GetAllProxies(proxies);

      map.AddLibrary(mExampleLibraryName, "1.0");
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);

      dtCore::RefPtr<const dtCore::ActorType> exampleType = dtCore::ActorFactory::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT_MESSAGE("The example type is NULL", exampleType.valid());

      dtCore::RefPtr<dtCore::BaseActorObject> proxy1 = dtCore::ActorFactory::GetInstance().CreateActor(*exampleType);
      CPPUNIT_ASSERT_MESSAGE("proxy1 is NULL", proxy1.valid());

      dtCore::RefPtr<dtCore::BaseActorObject> proxy2 = dtCore::ActorFactory::GetInstance().CreateActor(*exampleType);
      CPPUNIT_ASSERT_MESSAGE("proxy2 is NULL", proxy2.valid());

      dtCore::RefPtr<dtCore::BaseActorObject> proxy3 = dtCore::ActorFactory::GetInstance().CreateActor(*exampleType);
      CPPUNIT_ASSERT_MESSAGE("proxy3 is NULL", proxy3.valid());

      dtCore::RefPtr<dtCore::BaseActorObject> proxy4 = dtCore::ActorFactory::GetInstance().CreateActor(*exampleType);
      CPPUNIT_ASSERT_MESSAGE("proxy4 is NULL", proxy4.valid());

      map.AddProxy(*proxy1.get());
      map.AddProxy(*proxy2.get());
      map.AddProxy(*proxy3.get());
      map.AddProxy(*proxy4.get());

      dynamic_cast<dtCore::ActorIDActorProperty*>(proxy1->GetProperty("Test_Actor"))->SetValue(proxy2->GetId());
      dynamic_cast<dtCore::ActorIDActorProperty*>(proxy2->GetProperty("Test_Actor"))->SetValue(proxy4->GetId());
      dynamic_cast<dtCore::ActorIDActorProperty*>(proxy3->GetProperty("Test_Actor"))->SetValue(proxy4->GetId());

      map.RemoveProxy(*proxy4.get());

      CPPUNIT_ASSERT_MESSAGE("Proxy 1 should still be linked to proxy2", dynamic_cast<dtCore::ActorIDActorProperty*>(proxy1->GetProperty("Test_Actor"))->GetValue() == proxy2->GetId());
      CPPUNIT_ASSERT_MESSAGE("Proxy 2 is linked still", dynamic_cast<dtCore::ActorIDActorProperty*>(proxy2->GetProperty("Test_Actor"))->GetValue().ToString().empty());
      CPPUNIT_ASSERT_MESSAGE("Proxy 3 is linked still", dynamic_cast<dtCore::ActorIDActorProperty*>(proxy3->GetProperty("Test_Actor"))->GetValue().ToString().empty());
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapAddLibrariesOnSave()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      dtCore::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);

      dtCore::RefPtr<const dtCore::ActorType> exampleType = dtCore::ActorFactory::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT_MESSAGE("The example actor type is NULL", exampleType.valid());

      dtCore::RefPtr<dtCore::BaseActorObject> actor1 = dtCore::ActorFactory::GetInstance().CreateActor(*exampleType);
      CPPUNIT_ASSERT_MESSAGE("actor1 is NULL", actor1.valid());

      map.AddProxy(*actor1);

      project.SaveMap(map);

      CPPUNIT_ASSERT(map.HasLibrary(mExampleLibraryName));
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapCorrectLibraryListSetsModified()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      dtCore::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);

      dtCore::RefPtr<const dtCore::ActorType> exampleType = dtCore::ActorFactory::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT_MESSAGE("The example actor type is NULL", exampleType.valid());

      dtCore::RefPtr<dtCore::BaseActorObject> actor1 = dtCore::ActorFactory::GetInstance().CreateActor(*exampleType);
      CPPUNIT_ASSERT_MESSAGE("actor1 is NULL", actor1.valid());

      map.AddProxy(*actor1);
      CPPUNIT_ASSERT(map.IsModified());

      map.SetModified(false);

      CPPUNIT_ASSERT(!map.IsModified());

      map.CorrectLibraryList(false);

      CPPUNIT_ASSERT(map.HasLibrary(mExampleLibraryName));

      CPPUNIT_ASSERT(map.IsModified());
      map.SetModified(false);
      map.CorrectLibraryList(false);

      CPPUNIT_ASSERT_MESSAGE("Correcting the list where nothing changes should not result in the map being modified.", !map.IsModified());

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestPrefabLoadHeader()
{
   try
   {
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);
      dtCore::RefPtr<const dtCore::ActorType> example1Type = dtCore::ActorFactory::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT_MESSAGE("The example 1 actor type is null", example1Type.valid());
      int numberedName = 0;

      dtCore::ActorRefPtrVector actors, actorsResult;
      dtCore::RefPtr<dtCore::BaseActorObject> actor1 = dtCore::ActorFactory::GetInstance().CreateActor(*example1Type);
      CPPUNIT_ASSERT_MESSAGE("actor1 is null", actor1.valid());
      actor1->SetName(dtUtil::ToString(numberedName++));
      actors.push_back(actor1);

      std::string description("jibbly"), iconfile("T-Money");
      dtCore::ResourceDescriptor rd = dtCore::Project::GetInstance().SavePrefab("ExampleTest.dtprefab", "General:Test", actors, description, iconfile);

      dtCore::MapPtr header = dtCore::Project::GetInstance().GetPrefabHeader(rd);
      CPPUNIT_ASSERT(header.valid());
      CPPUNIT_ASSERT_EQUAL(description, header->GetDescription());
      CPPUNIT_ASSERT_EQUAL(iconfile, header->GetIconFile());

      CPPUNIT_ASSERT(header->GetAllLibraries().empty());
      CPPUNIT_ASSERT(header->GetAllProxies().empty());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestPrefabCorrectLibraryList()
{
   try
   {
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mDTActorsLibraryName);
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleGameLibraryName);
      dtCore::RefPtr<const dtCore::ActorType> example1Type = dtCore::ActorFactory::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT_MESSAGE("The example 1 actor type is null", example1Type.valid());

      dtCore::RefPtr<const dtCore::ActorType> example2Type = dtCore::ActorFactory::GetInstance().FindActorType("ExampleActors", "Test1Actor");
      CPPUNIT_ASSERT_MESSAGE("The example 2 actor type is null", example2Type.valid());

      dtCore::RefPtr<const dtCore::ActorType> exampleACType = dtCore::ActorFactory::GetInstance().FindActorType("ActorComponents","DeadReckoningActComp");
      CPPUNIT_ASSERT_MESSAGE("The example actor component type is null", exampleACType.valid());

      int numberedName = 0;
      dtCore::ActorRefPtrVector actors, actorsResult;
      dtCore::RefPtr<dtCore::BaseActorObject> actor1 = dtCore::ActorFactory::GetInstance().CreateActor(*example1Type);
      CPPUNIT_ASSERT_MESSAGE("actor1 is null", actor1.valid());
      actor1->SetName(dtUtil::ToString(numberedName++));
      actors.push_back(actor1);
      dtCore::RefPtr<dtCore::BaseActorObject> actor2 = dtCore::ActorFactory::GetInstance().CreateActor(*example2Type);
      CPPUNIT_ASSERT_MESSAGE("actor2 is null", actor2.valid());
      actor2->SetName(dtUtil::ToString(numberedName++));
      actors.push_back(actor2);
      dtCore::RefPtr<dtCore::BaseActorObject> actorComponent = dtCore::ActorFactory::GetInstance().CreateActor(*exampleACType);
      CPPUNIT_ASSERT_MESSAGE("actorComponent is null", actorComponent.valid());

      auto acc = dynamic_cast<dtCore::ActorComponentContainer*>(actor2.get());
      CPPUNIT_ASSERT(acc != nullptr);
      actorComponent->SetName(dtUtil::ToString(numberedName++));
      acc->AddComponent(*actorComponent);

      std::string description("jibbly"), iconfile("T-Money");
      dtCore::ResourceDescriptor rd = dtCore::Project::GetInstance().SavePrefab("ExampleTest.dtprefab", "General:Test", actors, description, iconfile);

      dtCore::MapPtr prefabMap = dtCore::Project::GetInstance().LoadPrefab(rd, actorsResult);
      CPPUNIT_ASSERT(prefabMap.valid());
      CPPUNIT_ASSERT(prefabMap->HasLibrary(mExampleLibraryName));
      CPPUNIT_ASSERT(prefabMap->HasLibrary(mDTActorsLibraryName));
      CPPUNIT_ASSERT(prefabMap->HasLibrary(mExampleGameLibraryName));
      CPPUNIT_ASSERT_EQUAL(description, prefabMap->GetDescription());
      CPPUNIT_ASSERT_EQUAL(iconfile, prefabMap->GetIconFile());

      CPPUNIT_ASSERT_EQUAL(actors.size(), prefabMap->GetAllProxies().size());
      CPPUNIT_ASSERT_EQUAL(actors.size(), actorsResult.size());

      dtCore::ActorRefPtrVector searchResult;
      for (auto i = actors.begin(), iend = actors.end(); i != iend; ++i)
      {
         prefabMap->FindProxies(searchResult, (*i)->GetName());
         CPPUNIT_ASSERT(!searchResult.empty());
         CPPUNIT_ASSERT_EQUAL(searchResult[0]->GetActorType(), (*i)->GetActorType());
         CPPUNIT_ASSERT(searchResult[0]->GetId() != (*i)->GetId());
         auto acc = dynamic_cast<dtCore::ActorComponentContainer*>(searchResult[0].get());
         auto accOrig = dynamic_cast<dtCore::ActorComponentContainer*>(i->get());
         if (acc != nullptr)
         {
            CPPUNIT_ASSERT(accOrig != nullptr);
            dtCore::ActorPtrVector comps, compsOrig;
            acc->GetAllComponents(comps);
            accOrig->GetAllComponents(compsOrig);
            CPPUNIT_ASSERT_EQUAL(comps.size(), compsOrig.size());
            // TODO go through all the components and check parent actors.
         }
         else
         {
            CPPUNIT_ASSERT(accOrig == nullptr);
         }
      }

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapProxySearch()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      project.SetContext(TEST_PROJECT_DIR);

      dtCore::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

      createActors(map);

      unsigned maxId = map.GetAllProxies().size();

      dtCore::ActorRefPtrVector results;

      map.FindProxies(results, "", "dtcore", "");

      map.FindProxies(results, "", "dtc", "");

      CPPUNIT_ASSERT_MESSAGE("Only full category names should return results.", results.size() == 0);

      map.FindProxies(results, "", "dtcore.", "");
      CPPUNIT_ASSERT_MESSAGE("trailing dots should end with no results.", results.size() == 0);

      map.FindProxies(results, "", "", "","", dtCore::Map::Placeable);
      for (dtCore::ActorRefPtrVector::iterator i = results.begin();
            i != results.end(); ++i)
      {
         CPPUNIT_ASSERT_MESSAGE(std::string("Proxy ") + (*i)->GetName()
               + " should not be in the results, it is not placeable",
               (*i)->IsPlaceable());
      }

      map.FindProxies(results, "", "", "","", dtCore::Map::NotPlaceable);
      for (dtCore::ActorRefPtrVector::iterator i = results.begin();
            i != results.end(); ++i)
      {
         CPPUNIT_ASSERT_MESSAGE(std::string("Proxy ") + (*i)->GetName()
               + " should not be in the results, it is placeable",
               !(*i)->IsPlaceable());
      }

      map.FindProxies(results, "", "", "","dtCore::Light");
      CPPUNIT_ASSERT_MESSAGE("There should be some lights in the results.", results.size() >= 3);


      for (dtCore::ActorRefPtrVector::iterator i = results.begin();
            i != results.end(); ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("All results should be instances of dtCore::Light",
               (*i)->IsInstanceOf("dtCore::Light"));
      }

      dtCore::ActorRefPtrVector proxies;

      map.GetAllProxies(proxies);

      for (unsigned x = 0;  x < proxies.size(); ++x)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> proxyPTR = map.GetProxyById(proxies[x]->GetId());

         CPPUNIT_ASSERT_MESSAGE("Proxy should be found in the map by the project.", &map == dtCore::Project::GetInstance().GetMapForActor(*proxyPTR));

         CPPUNIT_ASSERT_MESSAGE((std::string("Proxy not found with id: ") + proxies[x]->GetId().ToString()).c_str()
               , proxyPTR != NULL);

         dtCore::BaseActorObject& proxy = *proxyPTR.get();

         CPPUNIT_ASSERT_MESSAGE((std::string("Proxy has the wrong id. ") + proxy.GetId().ToString()).c_str() ,
               proxy.GetId() == proxies[x]->GetId());

         const std::string& cat = proxy.GetActorType().GetCategory();
         const std::string& typeName = proxy.GetActorType().GetName();

         map.FindProxies(results, proxy.GetName(), cat, typeName, "",
               proxy.IsPlaceable() ? dtCore::Map::Placeable : dtCore::Map::NotPlaceable);

         CPPUNIT_ASSERT_MESSAGE((std::string("Results should have exactly one proxy. Id:") +
               proxies[x]->GetId().ToString()).c_str() , results.size() == 1);
         CPPUNIT_ASSERT_MESSAGE((std::string("Correct proxy was not found with full search. Id:") +
               proxies[x]->GetId().ToString()).c_str() ,
               results.front() == &proxy);

         map.FindProxies(results, proxyPTR->GetName());

         CPPUNIT_ASSERT_EQUAL_MESSAGE((std::string("Results should have exactly one proxy. Id:") +
               proxies[x]->GetId().ToString()).c_str(), size_t(1), results.size());
         CPPUNIT_ASSERT_MESSAGE((std::string("Correct proxy was not found with name search. Id:") +
               proxies[x]->GetId().ToString()).c_str() ,
               results.front() == &proxy);

         map.FindProxies(results, std::string(""), cat, typeName);

         for (unsigned j = 0; j < results.size(); ++j)
         {
            const dtCore::ActorType& at = results[j]->GetActorType();
            std::ostringstream ss;
            ss << "Each proxy in the results should have the type or be a subtype of \"" << cat << "." << typeName <<
                  "\".  The result has type \"" << at << "\"";
            CPPUNIT_ASSERT_MESSAGE(ss.str(), at.InstanceOf(proxy.GetActorType()));
         }

         map.RemoveProxy(proxy);

         CPPUNIT_ASSERT_MESSAGE("Proxy list has the wrong size.",
               map.GetAllProxies().size() == (unsigned)(maxId - (x + 1)));
      }
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
dtCore::ActorProperty* MapTests::getActorProperty(dtCore::Map& map,
      const std::string& propName, dtCore::DataType& type, unsigned which)
{
   for (std::map<dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> >::const_iterator i = map.GetAllProxies().begin();
         i != map.GetAllProxies().end(); ++i)
   {

      dtCore::BaseActorObject* proxy = map.GetProxyById(i->first);

      CPPUNIT_ASSERT_MESSAGE("ERROR: Proxy is NULL", proxy!= NULL );

      if (propName != "")
      {
         dtCore::ActorProperty* prop = proxy->GetProperty(propName);
         //if a prop of a certain name is requested, readonly is allowed since the called should know which property
         //They will be getting.
         if (prop != NULL)
         {
            if (prop->GetDataType() == type && which-- == 0)
            {
               LOGN_DEBUG("maptests.cpp", proxy->GetActorType().GetName());
               return prop;
            }
         }
      }
      else
      {
         std::vector<dtCore::ActorProperty*> props;
         proxy->GetPropertyList(props);
         for (std::vector<dtCore::ActorProperty*>::iterator j = props.begin(); j<props.end(); ++j)
         {
            dtCore::ActorProperty* prop = *j;
            if (!prop->IsReadOnly() && prop->GetDataType() == type && which-- == 0)
            {
               //std::cout << "Using prop " << prop->GetName() << " on actor with id " << proxy->GetId() << std::endl;
               //std::cout << "  " << proxy->GetActorType().GetName() << std::endl;
               return prop;
            }
         }
      }
   }

   CPPUNIT_FAIL(std::string("No property found with name \"")
   + propName + "\", type \"" + type.GetName() + "\".");
   //This line will never be reached because fail will throw and exception.
   return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestLibraryMethods()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      dtCore::Map* map = &project.CreateMap("Neato Map", "neatomap");

      std::string lib1("hello1");
      std::string lib2("hello2");
      std::string lib3("hello3");

      map->AddLibrary(lib1, "");

      CPPUNIT_ASSERT_MESSAGE(lib1 +" should be added to the map.", map->HasLibrary(lib1));
      CPPUNIT_ASSERT_MESSAGE(lib2 +" should not be added to the map.", !map->HasLibrary(lib2));
      CPPUNIT_ASSERT_MESSAGE(lib3 +" should not be added to the map.", !map->HasLibrary(lib3));

      CPPUNIT_ASSERT_MESSAGE(lib1 + "version should be \"\"", map->GetLibraryVersion(lib1) == "");

      CPPUNIT_ASSERT_MESSAGE("Map should have 1 library", map->GetAllLibraries().size() == 1);

      map->InsertLibrary(0, lib1, "1.0");

      CPPUNIT_ASSERT_MESSAGE(lib1 +" should be added to the map.", map->HasLibrary(lib1));
      CPPUNIT_ASSERT_MESSAGE(lib1 + "version should be \"\"", map->GetLibraryVersion(lib1) == "1.0");
      CPPUNIT_ASSERT_MESSAGE("Map should have 1 library", map->GetAllLibraries().size() == 1);

      map->AddLibrary(lib1, "2.0");

      CPPUNIT_ASSERT_MESSAGE(lib1 +" should be added to the map.", map->HasLibrary(lib1));
      CPPUNIT_ASSERT_MESSAGE(lib1 + "version should be \"\"", map->GetLibraryVersion(lib1) == "2.0");
      CPPUNIT_ASSERT_MESSAGE("Map should have 1 library", map->GetAllLibraries().size() == 1);

      map->AddLibrary(lib2, "");

      CPPUNIT_ASSERT_MESSAGE(lib1 +" should be added to the map.", map->HasLibrary(lib1));
      CPPUNIT_ASSERT_MESSAGE(lib2 +" should be added to the map.", map->HasLibrary(lib2));
      CPPUNIT_ASSERT_MESSAGE(lib1 + "version should be \"\"", map->GetLibraryVersion(lib1) == "2.0");
      CPPUNIT_ASSERT_MESSAGE(lib2 + "version should be \"\"", map->GetLibraryVersion(lib2) == "");
      CPPUNIT_ASSERT_MESSAGE("Map should have 2 libraries", map->GetAllLibraries().size() == 2);
      CPPUNIT_ASSERT_MESSAGE(lib1 + " should be the first lib.", map->GetAllLibraries()[0] == lib1);
      CPPUNIT_ASSERT_MESSAGE(lib2 + " should be the second lib.", map->GetAllLibraries()[1] == lib2);

      map->InsertLibrary(1, lib3, "1.0");

      CPPUNIT_ASSERT_MESSAGE(lib1 +" should be added to the map.", map->HasLibrary(lib1));
      CPPUNIT_ASSERT_MESSAGE(lib2 +" should be added to the map.", map->HasLibrary(lib2));
      CPPUNIT_ASSERT_MESSAGE(lib3 +" should be added to the map.", map->HasLibrary(lib3));
      CPPUNIT_ASSERT_MESSAGE(lib1 + "version should be \"\"", map->GetLibraryVersion(lib1) == "2.0");
      CPPUNIT_ASSERT_MESSAGE(lib2 + "version should be \"\"", map->GetLibraryVersion(lib2) == "");
      CPPUNIT_ASSERT_MESSAGE(lib3 + "version should be \"\"", map->GetLibraryVersion(lib3) == "1.0");
      CPPUNIT_ASSERT_MESSAGE("Map should have 3 libraries", map->GetAllLibraries().size() == 3);
      CPPUNIT_ASSERT_MESSAGE(lib1 + " should be the first lib.", map->GetAllLibraries()[0] == lib1);
      CPPUNIT_ASSERT_MESSAGE(lib3 + " should be the second lib.", map->GetAllLibraries()[1] == lib3);
      CPPUNIT_ASSERT_MESSAGE(lib2 + " should be the third lib.", map->GetAllLibraries()[2] == lib2);

      map->InsertLibrary(0, lib3, "3.0");

      CPPUNIT_ASSERT_MESSAGE(lib1 +" should be added to the map.", map->HasLibrary(lib1));
      CPPUNIT_ASSERT_MESSAGE(lib2 +" should be added to the map.", map->HasLibrary(lib2));
      CPPUNIT_ASSERT_MESSAGE(lib3 +" should be added to the map.", map->HasLibrary(lib3));
      CPPUNIT_ASSERT_MESSAGE(lib1 + "version should be \"\"", map->GetLibraryVersion(lib1) == "2.0");
      CPPUNIT_ASSERT_MESSAGE(lib2 + "version should be \"\"", map->GetLibraryVersion(lib2) == "");
      CPPUNIT_ASSERT_MESSAGE(lib3 + "version should be \"\"", map->GetLibraryVersion(lib3) == "3.0");
      CPPUNIT_ASSERT_MESSAGE("Map should have 3 libraries", map->GetAllLibraries().size() == 3);
      CPPUNIT_ASSERT_MESSAGE(lib3 + " should be the first lib.", map->GetAllLibraries()[0] == lib3);
      CPPUNIT_ASSERT_MESSAGE(lib1 + " should be the second lib.", map->GetAllLibraries()[1] == lib1);
      CPPUNIT_ASSERT_MESSAGE(lib2 + " should be the third lib.", map->GetAllLibraries()[2] == lib2);

      map->RemoveLibrary(lib3);

      CPPUNIT_ASSERT_MESSAGE(lib1 +" should be added to the map.", map->HasLibrary(lib1));
      CPPUNIT_ASSERT_MESSAGE(lib2 +" should be added to the map.", map->HasLibrary(lib2));
      CPPUNIT_ASSERT_MESSAGE(lib3 +" should be NOT added to the map.", !map->HasLibrary(lib3));
      CPPUNIT_ASSERT_MESSAGE(lib1 + "version should be \"\"", map->GetLibraryVersion(lib1) == "2.0");
      CPPUNIT_ASSERT_MESSAGE(lib2 + "version should be \"\"", map->GetLibraryVersion(lib2) == "");
      CPPUNIT_ASSERT_MESSAGE("Map should have 2 libraries", map->GetAllLibraries().size() == 2);
      CPPUNIT_ASSERT_MESSAGE(lib1 + " should be the first lib.", map->GetAllLibraries()[0] == lib1);
      CPPUNIT_ASSERT_MESSAGE(lib2 + " should be the second lib.", map->GetAllLibraries()[1] == lib2);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapLibraryHandling()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      const std::string mapName("Neato Map");
      const std::string mapFileName("neatomap");

      dtCore::Map* map = &project.CreateMap(mapName, mapFileName);
      const std::string fileNameToTest = mapFileName + "." + dtCore::Map::MAP_FILE_EXTENSION;

      CPPUNIT_ASSERT_EQUAL_MESSAGE("A newly created Map doesn't have the expected generated filename.",
            fileNameToTest, map->GetFileName());

      map->AddLibrary(mExampleLibraryName, "1.0");
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);

      createActors(*map);

      dtCore::ActorPluginRegistry* reg = dtCore::ActorFactory::GetInstance().GetRegistry(mExampleLibraryName);
      CPPUNIT_ASSERT_MESSAGE("Registry for testActorLibrary should not be NULL.", reg != NULL);

      project.SaveMap(*map);

      project.CloseMap(*map, true);

      reg = dtCore::ActorFactory::GetInstance().GetRegistry(mExampleLibraryName);
      CPPUNIT_ASSERT_MESSAGE("testActorLibrary should have been closed.", reg == NULL);

      map = &project.GetMap(mapName);

      reg = dtCore::ActorFactory::GetInstance().GetRegistry(mExampleLibraryName);
      CPPUNIT_ASSERT_MESSAGE("Registry for testActorLibrary should not be NULL.", reg != NULL);

      dtCore::ActorRefPtrVector proxies;
      //hold onto all the proxies so that the actor libraries can't be closed.
      map->GetAllProxies(proxies);

      project.CloseMap(*map, true);

      reg = dtCore::ActorFactory::GetInstance().GetRegistry(mExampleLibraryName);
      CPPUNIT_ASSERT_MESSAGE("Registry for testActorLibrary should not be NULL.", reg != NULL);

      //cleanup the proxies
      proxies.clear();

      map = &project.GetMap(mapName);
      //create a new map that will ALSO use the same libraries
      project.CreateMap(mapName + "1", mapFileName + "1").AddLibrary(mExampleLibraryName, "1.0");

      createActors(project.GetMap(mapName + "1"));

      project.CloseMap(*map, true);

      reg = dtCore::ActorFactory::GetInstance().GetRegistry(mExampleLibraryName);
      CPPUNIT_ASSERT_MESSAGE("Registry for testActorLibrary should not be NULL.", reg != NULL);

      //when the second map is closed, the libraries should not close if false is passed.
      project.CloseMap(project.GetMap(mapName + "1"), false);

      reg = dtCore::ActorFactory::GetInstance().GetRegistry(mExampleLibraryName);
      CPPUNIT_ASSERT_MESSAGE("Registry for testActorLibrary should not be NULL.", reg != NULL);

      //reopen the map and close it with true to make sure the libraries close.
      project.CloseMap(project.GetMap(mapName), true);

      reg = dtCore::ActorFactory::GetInstance().GetRegistry(mExampleLibraryName);
      CPPUNIT_ASSERT_MESSAGE("testActorLibrary should have been closed.", reg == NULL);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapEventsModified()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();
      dtCore::Map& map = project.CreateMap("Neato Map", "neatomap");

      CPPUNIT_ASSERT(!map.IsModified());

      dtCore::RefPtr<dtCore::GameEvent> event = new dtCore::GameEvent("jojo", "helo");
      map.GetEventManager().AddEvent(*event);
      CPPUNIT_ASSERT_MESSAGE("Adding an event should mark the map modified.", map.IsModified());

      map.SetModified(false);

      map.GetEventManager().RemoveEvent(*event);
      CPPUNIT_ASSERT_MESSAGE("Removing an event should mark the map modified.", map.IsModified());

      //re-add the event to remove it again
      map.GetEventManager().AddEvent(*event);
      map.SetModified(false);

      map.GetEventManager().RemoveEvent(event->GetUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Removing an event by id should mark the map modified.", map.IsModified());

      //re-add the event to clear all
      map.GetEventManager().AddEvent(*event);
      map.SetModified(false);

      map.GetEventManager().ClearAllEvents();
      CPPUNIT_ASSERT_MESSAGE("Clearing all events should mark the map modified.", map.IsModified());

      project.DeleteMap(map);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.What());
   }
}

////////////////////////////////////////////////////////////////////////////////
void MapTests::TestIsMapFileValid()
{
   dtCore::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data");

   std::string validFile("maps/MyCoolMap.xml");
   std::string invalidFile("SkeletalMeshes/Marine/marine_test.xml");

   dtCore::Project& project = dtCore::Project::GetInstance();
   CPPUNIT_ASSERT(project.IsValidMapFile(validFile));

   // Turn off the default level logging to hide the error message.
   dtUtil::LoggingOff lo;
   CPPUNIT_ASSERT(!project.IsValidMapFile(invalidFile));
}

//////////////////////////////////////////////////////////////////////////
template<class PropertyType>
PropertyType* GetActorProperty(dtCore::ActorProxy& proxy, dtCore::DataType& type)
{
   std::vector<dtCore::ActorProperty*> props;
   proxy.GetPropertyList(props);

   std::vector<dtCore::ActorProperty*>::iterator propItr = props.begin();
   while (propItr != props.end())
   {
      if ((*propItr)->GetDataType() == type)
      {
         return dynamic_cast<PropertyType*>((*propItr));
      }
      ++propItr;
   }

   CPPUNIT_FAIL(std::string("No property found with type:") + type.GetName());
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapSaveAndLoad()
{
   //Create a Map, add an Actor, and set it's properties with known values.
   //Then close and reopen the Map, verify the Actor's Properties have the same
   //values as when set.

   const std::string mapName("Neato Map");
   const std::string mapFileName("neatomap");

   dtCore::Map* map = NULL;

   try
   {
      //create new map
      map = &dtCore::Project::GetInstance().CreateMap(mapName, mapFileName);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.ToString());
   }

   map->AddLibrary(mExampleLibraryName, "1.0");

   dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);

   //create test actor
   dtCore::RefPtr<dtCore::BaseActorObject> proxy;
   try
   {
      proxy = dtCore::ActorFactory::GetInstance().CreateActor(*ExampleActorLib::TEST_ACTOR_PROPERTY_TYPE.get());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.ToString());
   }

   map->AddProxy(*proxy);

   //dtCore::ActorProperty* prop(NULL);

   const std::string TEST_STRING("test123  !@# < hello > ");
   const bool TEST_BOOL(true);
   const float TEST_FLOAT(12345.12345f);
   const double TEST_DOUBLE(12345.54321);
   const int TEST_INT(123345);
   const osg::Vec3 TEST_VEC3(1.0f, 2.0f, 3.0f);
   const osg::Vec3 TEST_VEC3F(123.123f, 456.456f, 789.789f);
   const osg::Vec3d TEST_VEC3D(123.123, 456.456, 789.789);
   const osg::Vec4f TEST_VEC4F(1.0f, 2.0f, 3.0f, 4.0f);
   const osg::Vec4d TEST_VEC4D(2.0, 3.0, 4.0, 5.0);
   const osg::Vec4 TEST_RGBA(255.0f, 245.0f, 235.0f, 1235.0f);
   const dtCore::UniqueId TEST_ACTORID;
   const dtCore::ResourceDescriptor TEST_RESOURCE("test", "somethingelse");
   const unsigned int TEST_BIT(0xFF00FF00);

   //string
   {
      dtCore::StringActorProperty* prop = GetActorProperty<dtCore::StringActorProperty>(*proxy, dtCore::DataType::STRING);
      prop->SetValue(TEST_STRING);
   }

   //boolean
   {
      dtCore::BooleanActorProperty* prop = GetActorProperty<dtCore::BooleanActorProperty>(*proxy, dtCore::DataType::BOOLEAN);
      prop->SetValue(TEST_BOOL);
   }

   //float
   {
      dtCore::FloatActorProperty* prop = GetActorProperty<dtCore::FloatActorProperty>(*proxy, dtCore::DataType::FLOAT);
      prop->SetValue(TEST_FLOAT);
   }

   //double
   {
      dtCore::DoubleActorProperty* prop = GetActorProperty<dtCore::DoubleActorProperty>(*proxy, dtCore::DataType::DOUBLE);
      prop->SetValue(TEST_DOUBLE);
   }

   //int
   {
      dtCore::IntActorProperty* prop = GetActorProperty<dtCore::IntActorProperty>(*proxy, dtCore::DataType::INT);
      prop->SetValue(TEST_INT);
   }


   //vec3 / vec3f
   {
      dtCore::Vec3ActorProperty* prop = GetActorProperty<dtCore::Vec3ActorProperty>(*proxy, dtCore::DataType::VEC3);
      prop->SetValue(TEST_VEC3);
   }


   //vec3d
   {
      dtCore::Vec3dActorProperty* prop = GetActorProperty<dtCore::Vec3dActorProperty>(*proxy, dtCore::DataType::VEC3D);
      prop->SetValue(TEST_VEC3D);
   }

   //vec4f
   {
      dtCore::Vec4fActorProperty* prop = GetActorProperty<dtCore::Vec4fActorProperty>(*proxy, dtCore::DataType::VEC4F);
      prop->SetValue(TEST_VEC4F);
   }

   //vec4d
   {
      dtCore::Vec4dActorProperty* prop = GetActorProperty<dtCore::Vec4dActorProperty>(*proxy, dtCore::DataType::VEC4D);
      prop->SetValue(TEST_VEC4D);
   }

   //rgbacolor
   {
      dtCore::ColorRgbaActorProperty* prop = GetActorProperty<dtCore::ColorRgbaActorProperty>(*proxy, dtCore::DataType::RGBACOLOR);
      prop->SetValue(TEST_RGBA);
   }

   // ActorIDProperty
   {
      dtCore::ActorIDActorProperty* prop = GetActorProperty<dtCore::ActorIDActorProperty>(*proxy, dtCore::DataType::ACTOR);
      prop->SetValue(TEST_ACTORID);
   }

   //enumeration
   {
      dtCore::AbstractEnumActorProperty* prop = GetActorProperty<dtCore::AbstractEnumActorProperty>(*proxy, dtCore::DataType::ENUMERATION);
      prop->SetEnumValue( *(*prop->GetList().begin()) );
   }

   //Resource
   {
      dtCore::ResourceActorProperty* prop = GetActorProperty<dtCore::ResourceActorProperty>(*proxy, dtCore::DataType::SOUND);
      prop->SetValue(TEST_RESOURCE);
   }

   //bit mask
   {
      dtCore::BitMaskActorProperty* prop = GetActorProperty<dtCore::BitMaskActorProperty>(*proxy, dtCore::DataType::BIT_MASK);
      prop->SetValue(TEST_BIT);
   }

   //save map
   dtCore::Project::GetInstance().SaveMap(*map);

   //close map
   dtCore::Project::GetInstance().CloseMap(*map, true);
   map = NULL;

   //reopen map
   map = &dtCore::Project::GetInstance().GetMap(mapName);

   const std::string kAssertMsg("Property value returned from Map doesn't match what was set");

   //string
   {
      dtCore::StringActorProperty* prop = GetActorProperty<dtCore::StringActorProperty>(*proxy, dtCore::DataType::STRING);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_STRING, prop->GetValue());
   }

   //boolean
   {
      dtCore::BooleanActorProperty* prop = GetActorProperty<dtCore::BooleanActorProperty>(*proxy, dtCore::DataType::BOOLEAN);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_BOOL, prop->GetValue());
   }

   //float
   {
      dtCore::FloatActorProperty* prop = GetActorProperty<dtCore::FloatActorProperty>(*proxy, dtCore::DataType::FLOAT);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_FLOAT, prop->GetValue());
   }

   //double
   {
      dtCore::DoubleActorProperty* prop = GetActorProperty<dtCore::DoubleActorProperty>(*proxy, dtCore::DataType::DOUBLE);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_DOUBLE, prop->GetValue());
   }

   //int
   {
      dtCore::IntActorProperty* prop = GetActorProperty<dtCore::IntActorProperty>(*proxy, dtCore::DataType::INT);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_INT, prop->GetValue());
   }


   //vec3 /vec3f
   {
      dtCore::Vec3ActorProperty* prop = GetActorProperty<dtCore::Vec3ActorProperty>(*proxy, dtCore::DataType::VEC3);
      CPPUNIT_ASSERT_MESSAGE(kAssertMsg, dtUtil::Equivalent(TEST_VEC3, prop->GetValue(), 1e-2f));
   }

   //vec3d
   {
      dtCore::Vec3dActorProperty* prop = GetActorProperty<dtCore::Vec3dActorProperty>(*proxy, dtCore::DataType::VEC3D);
      CPPUNIT_ASSERT_MESSAGE(kAssertMsg, dtUtil::Equivalent(TEST_VEC3D, prop->GetValue(), 1e-2));
   }

   //vec4f
   {
      dtCore::Vec4fActorProperty* prop = GetActorProperty<dtCore::Vec4fActorProperty>(*proxy, dtCore::DataType::VEC4F);
      CPPUNIT_ASSERT_MESSAGE(kAssertMsg, dtUtil::Equivalent(TEST_VEC4F, prop->GetValue(), 1e-2f));
   }

   //vec4d
   {
      dtCore::Vec4dActorProperty* prop = GetActorProperty<dtCore::Vec4dActorProperty>(*proxy, dtCore::DataType::VEC4D);
      CPPUNIT_ASSERT_MESSAGE(kAssertMsg, dtUtil::Equivalent(TEST_VEC4D, prop->GetValue(), 1e-2));
   }

   //rgbacolor
   {
      dtCore::ColorRgbaActorProperty* prop = GetActorProperty<dtCore::ColorRgbaActorProperty>(*proxy, dtCore::DataType::RGBACOLOR);
      CPPUNIT_ASSERT_MESSAGE(kAssertMsg, dtUtil::Equivalent(TEST_RGBA, prop->GetValue(), 1e-2f));
   }

   // ActoridProperty
   {
      dtCore::ActorIDActorProperty* prop = GetActorProperty<dtCore::ActorIDActorProperty>(*proxy, dtCore::DataType::ACTOR);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_ACTORID, prop->GetValue());
   }

   //enumeration
   {
      dtCore::AbstractEnumActorProperty* prop = GetActorProperty<dtCore::AbstractEnumActorProperty>(*proxy, dtCore::DataType::ENUMERATION);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, (*prop->GetList().begin())->GetName(), prop->GetEnumValue().GetName());
   }

   //Resource
   {
      dtCore::ResourceActorProperty* prop = GetActorProperty<dtCore::ResourceActorProperty>(*proxy, dtCore::DataType::SOUND);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_RESOURCE.GetResourceIdentifier(), prop->GetValue().GetResourceIdentifier());
   }

   //Bit Mask
   {
      dtCore::BitMaskActorProperty* prop = GetActorProperty<dtCore::BitMaskActorProperty>(*proxy, dtCore::DataType::BIT_MASK);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_BIT, prop->GetValue());
   }

   dtCore::Project::GetInstance().DeleteMap(*map, true);
}


///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapSaveAndLoadEvents()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      const std::string mapName("Neato Map");
      const std::string mapFileName("neatomap");

      dtCore::Map* map = &project.CreateMap(mapName, mapFileName);

      map->SetDescription("Teague is league with a \"t\".");

      const unsigned eventCount = 6;

      std::vector<dtCore::RefPtr<dtCore::GameEvent> > events;
      for (unsigned i = 0; i < eventCount; ++i)
      {
         std::ostringstream ss;
         ss << "name" << i;
         dtCore::RefPtr<dtCore::GameEvent> ge = new dtCore::GameEvent(ss.str(), "Test Description");
         map->GetEventManager().AddEvent(*ge);
         events.push_back(ge);
      }

      project.SaveMap(*map);

      project.CloseMap(*map);
      map = NULL;

      map = &project.GetMap(mapName);

      CPPUNIT_ASSERT_EQUAL(eventCount, map->GetEventManager().GetNumEvents());

      for (unsigned i = 0; i < eventCount; ++i)
      {
         dtCore::GameEvent& expectedEvent = *events[i];
         dtCore::GameEvent* ge = map->GetEventManager().FindEvent(expectedEvent.GetUniqueId());

         CPPUNIT_ASSERT_MESSAGE("All of the game events saved should have been loaded with the same id's",
               ge != NULL);

         CPPUNIT_ASSERT_EQUAL(expectedEvent.GetName(), ge->GetName());
         CPPUNIT_ASSERT_EQUAL(expectedEvent.GetDescription(), ge->GetDescription());
      }
      project.DeleteMap(*map, true);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapSaveAndLoadGroup()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      std::string mapName("Neato Map");
      std::string mapFileName("neatomap");

      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);

      const dtCore::ActorType* at = dtCore::ActorFactory::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT(at != NULL);

      dtCore::RefPtr<dtCore::NamedGroupParameter> expectedResult = new dtCore::NamedGroupParameter("TestGroup");
      dtCore::RefPtr<dtCore::NamedGroupParameter> secondInternalGroup = static_cast<dtCore::NamedGroupParameter*>(expectedResult->AddParameter("FloatGroup", dtCore::DataType::GROUP));

      {
         dtCore::Map* map = &project.CreateMap(mapName, mapFileName);
         map->AddLibrary(mExampleLibraryName, "1.0");

         dtCore::RefPtr<dtCore::BaseActorObject> proxy = dtCore::ActorFactory::GetInstance().CreateActor(*at);

         dtCore::GroupActorProperty* groupProp;
         proxy->GetProperty("TestGroup", groupProp);

         const dtCore::ResourceDescriptor rd("StaticMeshes:Chicken:Horse.ive", "StaticMeshes:Chicken:Horse.ive");

         dtCore::GameEvent* ge = new dtCore::GameEvent("cow", "chicken");
         map->GetEventManager().AddEvent(*ge);

         expectedResult->AddValue("SillyInt", 24);
         expectedResult->AddValue("SillyLong", 37L);
         expectedResult->AddValue("SillyString", std::string("Jojo"));
         static_cast<dtCore::NamedResourceParameter&>(*expectedResult->AddParameter("SillyResource1", dtCore::DataType::STATIC_MESH)).SetValue(rd);
         static_cast<dtCore::NamedResourceParameter&>(*expectedResult->AddParameter("SillyResource2", dtCore::DataType::TEXTURE)).SetValue(dtCore::ResourceDescriptor::NULL_RESOURCE);

         dtCore::NamedGroupParameter& internalGroup = static_cast<dtCore::NamedGroupParameter&>(*expectedResult->AddParameter("SillyGroup", dtCore::DataType::GROUP));
         internalGroup.AddValue("CuteString", std::string("Just a string"));
         static_cast<dtCore::NamedGameEventParameter&>(*internalGroup.AddParameter("CuteEvent", dtCore::DataType::GAME_EVENT)).SetValue(ge->GetUniqueId());
         static_cast<dtCore::NamedActorParameter&>(*internalGroup.AddParameter("CuteActor", dtCore::DataType::ACTOR)).SetValue(proxy->GetId());
         internalGroup.AddValue("CuteBool", true);

         static_cast<dtCore::NamedVec2Parameter&>(*secondInternalGroup->AddParameter("CuteVec2", dtCore::DataType::VEC2)).SetValue(osg::Vec2(1.0f, 1.3f));
         static_cast<dtCore::NamedVec3Parameter&>(*secondInternalGroup->AddParameter("CuteVec3", dtCore::DataType::VEC3)).SetValue(osg::Vec3(1.0f, 1.3f, 34.7f));
         static_cast<dtCore::NamedVec4Parameter&>(*secondInternalGroup->AddParameter("CuteVec4", dtCore::DataType::VEC4)).SetValue(osg::Vec4(1.0f, 1.3f, 34.7f, 77.6f));
         static_cast<dtCore::NamedRGBAColorParameter&>(*secondInternalGroup->AddParameter("CuteColor", dtCore::DataType::RGBACOLOR)).SetValue(osg::Vec4(1.0f, 0.6f, 0.3f, 0.11f));
         static_cast<dtCore::NamedFloatParameter&>(*secondInternalGroup->AddParameter("CuteFloat", dtCore::DataType::FLOAT)).SetValue(3.8f);
         static_cast<dtCore::NamedDoubleParameter&>(*secondInternalGroup->AddParameter("CuteDouble", dtCore::DataType::DOUBLE)).SetValue(3.8f);

         groupProp->SetValue(*expectedResult);

         //remove the floats so that they can compared separately using epsilons.
         expectedResult->RemoveParameter(secondInternalGroup->GetName());

         map->AddProxy(*proxy);

         project.SaveMap(*map);
         project.CloseMap(*map);
      }

      dtCore::Map* map = &project.GetMap(mapName);

      dtCore::ActorRefPtrVector toFill;
      map->GetAllProxies(toFill);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The map was saved with one proxy.  It should have one when loaded.", toFill.size(), size_t(1));

      dtCore::RefPtr<dtCore::BaseActorObject> proxy = toFill[0];

      dtCore::GroupActorProperty* groupProp;
      proxy->GetProperty("TestGroup", groupProp);

      dtCore::RefPtr<dtCore::NamedGroupParameter> actualResult = groupProp->GetValue();
      CPPUNIT_ASSERT(actualResult.valid());

      dtCore::RefPtr<dtCore::NamedParameter> floatGroup = actualResult->RemoveParameter(secondInternalGroup->GetName());
      dtCore::RefPtr<dtCore::NamedGroupParameter> actualFloatGroup(dynamic_cast<dtCore::NamedGroupParameter*>(floatGroup.get()));

      CPPUNIT_ASSERT_MESSAGE("The loaded result parameter should have group filled with floats.", actualFloatGroup.valid());

      CPPUNIT_ASSERT_MESSAGE("The returned group parameter doesn't match the actual\n" + actualResult->ToString() + " \n\n " + expectedResult->ToString(),
            *expectedResult == *actualResult);

      std::vector<dtCore::NamedParameter*> savedFloatParams;
      secondInternalGroup->GetParameters(savedFloatParams);
      for (unsigned i = 0; i < savedFloatParams.size(); ++i)
      {
         dtCore::NamedParameter* np = actualFloatGroup->GetParameter(savedFloatParams[i]->GetName());
         CPPUNIT_ASSERT_MESSAGE(np->GetName() + " should be a parameter in the FloatGroup parameter group loaded from the map." , np != NULL);
         CPPUNIT_ASSERT_MESSAGE(np->GetName() + " parameter should have the same data type as it did before it was saved in a map." ,
               np->GetDataType() == savedFloatParams[i]->GetDataType());
      }

      std::string valueString = actualFloatGroup->ToString() + "\n\n" + secondInternalGroup->ToString();

      CPPUNIT_ASSERT_MESSAGE("The loaded vec2 parameter should match the one saved: \n" + valueString,
            dtUtil::Equivalent(
                  static_cast<dtCore::NamedVec2Parameter*>(secondInternalGroup->GetParameter("CuteVec2"))->GetValue(),
                  static_cast<dtCore::NamedVec2Parameter*>(actualFloatGroup->GetParameter("CuteVec2"))->GetValue(), 1e-3f));

      CPPUNIT_ASSERT_MESSAGE("The loaded vec3 parameter should match the one saved: \n" + valueString,
            dtUtil::Equivalent(
                  static_cast<dtCore::NamedVec3Parameter*>(secondInternalGroup->GetParameter("CuteVec3"))->GetValue(),
                  static_cast<dtCore::NamedVec3Parameter*>(actualFloatGroup->GetParameter("CuteVec3"))->GetValue(), 1e-3f));

      CPPUNIT_ASSERT_MESSAGE("The loaded vec4 parameter should match the one saved: \n" + valueString,
            dtUtil::Equivalent(
                  static_cast<dtCore::NamedVec4Parameter*>(secondInternalGroup->GetParameter("CuteVec4"))->GetValue(),
                  static_cast<dtCore::NamedVec4Parameter*>(actualFloatGroup->GetParameter("CuteVec4"))->GetValue(), 1e-3f));

      CPPUNIT_ASSERT_MESSAGE("The loaded color parameter should match the one saved: \n" + valueString,
            dtUtil::Equivalent(
                  static_cast<dtCore::NamedRGBAColorParameter*>(secondInternalGroup->GetParameter("CuteColor"))->GetValue(),
                  static_cast<dtCore::NamedRGBAColorParameter*>(actualFloatGroup->GetParameter("CuteColor"))->GetValue(), 1e-3f));

      CPPUNIT_ASSERT_MESSAGE("The loaded float parameter should match the one saved: \n" + valueString,
            osg::equivalent(
                  static_cast<dtCore::NamedFloatParameter*>(secondInternalGroup->GetParameter("CuteFloat"))->GetValue(),
                  static_cast<dtCore::NamedFloatParameter*>(actualFloatGroup->GetParameter("CuteFloat"))->GetValue(), 1e-3f));
      CPPUNIT_ASSERT_MESSAGE("The loaded double parameter should match the one saved: \n" + valueString,
            osg::equivalent(
                  static_cast<dtCore::NamedDoubleParameter*>(secondInternalGroup->GetParameter("CuteDouble"))->GetValue(),
                  static_cast<dtCore::NamedDoubleParameter*>(actualFloatGroup->GetParameter("CuteDouble"))->GetValue(), 1e-3));

      project.DeleteMap(*map, false);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapSaveAndLoadPropertyContainerProperty()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      std::string mapName("Neato Map");
      std::string mapFileName("neatomap");

      dtCore::Map* map = &project.CreateMap(mapName, mapFileName);
      map->AddLibrary(mExampleLibraryName, "1.0");
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);

      const dtCore::ActorType* at = dtCore::ActorFactory::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT(at != NULL);

      dtCore::RefPtr<dtCore::BaseActorObject> actor = dtCore::ActorFactory::GetInstance().CreateActor(*at);
      map->AddProxy(*actor);
      dtCore::UniqueId idToSave = actor->GetId();

      dtCore::BasePropertyContainerActorProperty* bpc = NULL;
      actor->GetProperty("TestPropertyContainer", bpc);
      CPPUNIT_ASSERT(bpc != NULL);

      dtCore::PropertyContainer* pc = bpc->GetValue();
      CPPUNIT_ASSERT(pc != NULL);

      const float testFloat(37.36f);
      const double testDouble(-393.334);
      const bool testBool(true);
      const int testInt(-347);

      dtCore::FloatActorProperty* fap;
      pc->GetProperty("Test_Float", fap);
      CPPUNIT_ASSERT(fap != NULL);
      fap->SetValue(testFloat);

      dtCore::DoubleActorProperty* dap;
      pc->GetProperty("Test_Double", dap);
      CPPUNIT_ASSERT(dap != NULL);
      dap->SetValue(testDouble);

      dtCore::BooleanActorProperty* bap;
      pc->GetProperty("Test_Boolean", bap);
      CPPUNIT_ASSERT(bap != NULL);
      bap->SetValue(testBool);

      dtCore::IntActorProperty* iap;
      pc->GetProperty("Test_Int", iap);
      CPPUNIT_ASSERT(iap != NULL);
      iap->SetValue(testInt);


      project.SaveMap(*map);
      project.CloseMap(*map);
      map = NULL;
      map = &project.GetMap(mapName);

      actor = map->GetProxyById(idToSave);
      CPPUNIT_ASSERT(actor != NULL);

      actor->GetProperty("TestPropertyContainer", bpc);
      CPPUNIT_ASSERT(bpc != NULL);

      pc = bpc->GetValue();
      CPPUNIT_ASSERT(pc != NULL);

      pc->GetProperty("Test_Float", fap);
      CPPUNIT_ASSERT(fap != NULL);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testFloat, fap->GetValue(), 0.01f);

      pc->GetProperty("Test_Double", dap);
      CPPUNIT_ASSERT(dap != NULL);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(testDouble, dap->GetValue(), 0.01);

      pc->GetProperty("Test_Boolean", bap);
      CPPUNIT_ASSERT(bap != NULL);
      CPPUNIT_ASSERT(bap->GetValue());

      pc->GetProperty("Test_Int", iap);
      CPPUNIT_ASSERT(iap != NULL);
      CPPUNIT_ASSERT_EQUAL(testInt, iap->GetValue());

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapSaveAndLoadNestedPropertyContainerArray()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      std::string mapName("Neato Map");
      std::string mapFileName("neatomap");

      dtCore::Map* map = &project.CreateMap(mapName, mapFileName);
      map->AddLibrary(mExampleGameLibraryName, "1.0");
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleGameLibraryName);

      const dtCore::ActorType* at = dtCore::ActorFactory::GetInstance().FindActorType("ExampleActors", "TestGamePropertyActor");
      CPPUNIT_ASSERT(at != NULL);

      dtCore::RefPtr<dtCore::BaseActorObject> actor = dtCore::ActorFactory::GetInstance().CreateActor(*at);
      map->AddProxy(*actor);
      dtCore::UniqueId idToSave = actor->GetId();

      dtCore::BasePropertyContainerActorProperty* bpc = NULL;
      actor->GetProperty("TestPropertyContainer", bpc);
      CPPUNIT_ASSERT(bpc != NULL);

      dtCore::PropertyContainer* pc = bpc->GetValue();
      CPPUNIT_ASSERT(pc != NULL);

      dtCore::ArrayActorPropertyBase* arrayProp = NULL;
      pc->GetProperty("TestNestedContainerArray", arrayProp);
      CPPUNIT_ASSERT(arrayProp != NULL);

      CPPUNIT_ASSERT_EQUAL(0, arrayProp->GetArraySize());
      arrayProp->PushBack();
      arrayProp->PushBack();
      arrayProp->PushBack();
      CPPUNIT_ASSERT_EQUAL(3, arrayProp->GetArraySize());
      arrayProp->PopBack();
      CPPUNIT_ASSERT_EQUAL(2, arrayProp->GetArraySize());

      // Select the second item.
      arrayProp->SetIndex(1);

      dtCore::BasePropertyContainerActorProperty* bpcD1 = dynamic_cast<dtCore::BasePropertyContainerActorProperty*>(arrayProp->GetArrayProperty());
      CPPUNIT_ASSERT(bpcD1 != NULL);

      // Get the new property container at index 1 on the depth 1 array.
      dtCore::PropertyContainer* pcD1i1 = bpcD1->GetValue();
      dtCore::ArrayActorPropertyBase* arrayPropD1 = NULL;
      pcD1i1->GetProperty("TestNestedContainerArray", arrayPropD1);

      CPPUNIT_ASSERT(arrayPropD1 != NULL);

      arrayPropD1->PushBack();
      arrayPropD1->PushBack();
      arrayPropD1->PushBack();
      arrayPropD1->PushBack();
      CPPUNIT_ASSERT_EQUAL(4, arrayPropD1->GetArraySize());

      project.SaveMap(*map);
      project.CloseMap(*map);
      map = NULL;
      map = &project.GetMap(mapName);

      actor = map->GetProxyById(idToSave);
      CPPUNIT_ASSERT(actor != NULL);

      actor->GetProperty("TestPropertyContainer", bpc);
      CPPUNIT_ASSERT(bpc != NULL);

      pc = bpc->GetValue();
      CPPUNIT_ASSERT(pc != NULL);

      pc->GetProperty("TestNestedContainerArray", arrayProp);

      CPPUNIT_ASSERT_EQUAL(2, arrayProp->GetArraySize());

      // Select the second item.
      arrayProp->SetIndex(1);

      bpcD1 = dynamic_cast<dtCore::BasePropertyContainerActorProperty*>(arrayProp->GetArrayProperty());
      CPPUNIT_ASSERT(bpcD1 != NULL);

      // Get the new property container at index 1 on the depth 1 array.
      pcD1i1 = bpcD1->GetValue();
      pcD1i1->GetProperty("TestNestedContainerArray", arrayPropD1);

      CPPUNIT_ASSERT(arrayPropD1 != NULL);

      CPPUNIT_ASSERT_EQUAL(4, arrayPropD1->GetArraySize());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
//This short test actually tests a lot of fairly complex things.
//It tests that Group actor properties can be set and cause an actor to link actors when
//loading from a map.  It tests the feature of looking into the current map being parsed
//when calling Project::GetMapForActorProxy.  It also tests the mParsing flag on the dtCore::MapParser
//and it tests the SubTasks property on the task actors.
void MapTests::TestMapSaveAndLoadActorGroups()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      std::string mapName("Neato Map");
      std::string mapFileName("neatomap");

      dtCore::Map* map = &project.CreateMap(mapName, mapFileName);

      dtCore::ActorFactory& libraryManager = dtCore::ActorFactory::GetInstance();

      const dtCore::ActorType* at = libraryManager.FindActorType("dtcore.Tasks", "Task Actor");
      CPPUNIT_ASSERT(at != NULL);

      dtCore::RefPtr<dtCore::BaseActorObject> proxy = libraryManager.CreateActor(*at);

      map->AddProxy(*proxy);

      std::vector<dtCore::UniqueId> subTasks;
      //all of these actors are added to the map AFTER the main proxy
      //so they won't be loaded yet when the map is loaded.
      //This tests that the group property will load correctly regardless of actor ordering.
      std::ostringstream ss;
      for (unsigned i = 0; i < 10; ++i)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> proxy = libraryManager.CreateActor(*at);
         subTasks.push_back(proxy->GetId());
         map->AddProxy(*proxy);
         ss.str("");
         ss << i;
      }

      dtCore::ArrayActorProperty<dtCore::UniqueId>* arrayProp = NULL;
      proxy->GetProperty("SubTaskList", arrayProp);
      CPPUNIT_ASSERT_MESSAGE("The 'SubTaskList' ArrayActorProperty was not found in the Task Actor", arrayProp != NULL);
      arrayProp->SetValue(subTasks);

      std::vector<dtCore::UniqueId> actualResult = arrayProp->GetValue();

      CPPUNIT_ASSERT_EQUAL(subTasks.size(), actualResult.size());
      CPPUNIT_ASSERT(subTasks == actualResult);
      project.SaveMap(*map);
      project.CloseMap(*map);

      map = &project.GetMap(mapName);

      //Here the old proxy will be deleted, but we get the id for it to load the new instance in the map.
      proxy = map->GetProxyById(proxy->GetId());
      CPPUNIT_ASSERT(proxy.valid());
      proxy->GetProperty("SubTaskList", arrayProp);
      actualResult = arrayProp->GetValue();

      CPPUNIT_ASSERT_EQUAL(subTasks.size() , actualResult.size());
      CPPUNIT_ASSERT(subTasks == actualResult);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.What());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestShouldSaveProperty()
{
   const dtCore::ActorType* at = dtCore::ActorFactory::GetInstance().FindActorType("ExampleActors", "TestGamePropertyActor");
   CPPUNIT_ASSERT(at != NULL);

   dtCore::RefPtr<dtCore::BaseActorObject> actor = dtCore::ActorFactory::GetInstance().CreateActor(*at);
   dtCore::ActorProperty* prop = actor->GetProperty(dtCore::TransformableActorProxy::PROPERTY_ACTIVE);
   CPPUNIT_ASSERT_MESSAGE("Property should NOT save because it has the default value", !actor->ShouldPropertySave(*prop));
   prop->SetAlwaysSave(true);
   CPPUNIT_ASSERT_MESSAGE("Property SHOULD save because it is set to always save, even though it is the default.", actor->ShouldPropertySave(*prop));
   prop->SetAlwaysSave(false);
   prop->FromString("false");
   CPPUNIT_ASSERT_MESSAGE("Property SHOULD save because it is not the default value.", actor->ShouldPropertySave(*prop));
   prop->SetIgnoreWhenSaving(true);
   CPPUNIT_ASSERT_MESSAGE("Property should NOT save because it is set to ignore.", !actor->ShouldPropertySave(*prop));
   prop->SetIgnoreWhenSaving(false);
   prop->SetReadOnly(true);
   CPPUNIT_ASSERT_MESSAGE("Property should NOT save because it is set to read only.", !actor->ShouldPropertySave(*prop));
   prop->SetReadOnly(false);
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestWildCard()
{
   CPPUNIT_ASSERT(dtCore::Map::WildMatch("*", "sthsthsth"));
   CPPUNIT_ASSERT(dtCore::Map::WildMatch("sth*", "sthsthsth"));
   CPPUNIT_ASSERT(dtCore::Map::WildMatch("a*eda", "aeeeda"));
   CPPUNIT_ASSERT(dtCore::Map::WildMatch("a*eda", "aedededa"));
   CPPUNIT_ASSERT(dtCore::Map::WildMatch("a*eda", "aedededa"));
   CPPUNIT_ASSERT(dtCore::Map::WildMatch("*Cur?is? *Murphy*", "Curtiss Murphy"));
   CPPUNIT_ASSERT(dtCore::Map::WildMatch("?????", "12345"));

   CPPUNIT_ASSERT(!dtCore::Map::WildMatch("*Cur?i? *Murphy*", "Curtiss Murphy"));
   CPPUNIT_ASSERT(!dtCore::Map::WildMatch("??????", "12345"));
   CPPUNIT_ASSERT(!dtCore::Map::WildMatch("a?eda", "aedededa"));
}


///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestLoadMapIntoScene()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();

      dtCore::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

      createActors(map);

      std::set<dtCore::UniqueId> ids;

      //add all the names of the actors that should be in the scene to set so we can track them.
      for (std::map<dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> >::const_iterator i = map.GetAllProxies().begin();
            i != map.GetAllProxies().end(); ++i)
      {
         const dtCore::BaseActorObject::RenderMode &renderMode = const_cast<dtCore::BaseActorObject&>(*i->second).GetRenderMode();

         if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR ||
               renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
            ids.insert(i->first);
      }

      dtCore::RefPtr<dtABC::Application> app(&GetGlobalApplication());
      dtCore::Scene& scene = *app->GetScene();
      //actually load the map into the scene.
      //TODO, test with the last param as false to make sure ALL proxies end up in the scene.
      project.LoadMapIntoScene(map, scene);

      //spin through the scene removing each actor found from the set.
      for (unsigned x = 0; x < (unsigned)scene.GetNumberOfAddedDrawable(); ++x)
      {
         dtCore::DeltaDrawable* dd = scene.GetChild(x);
         std::set<dtCore::UniqueId>::iterator found = ids.find(dd->GetUniqueId());
         //Need to check to see if the actor exists in the set before removing it
         //because this is a test and because the scene could add drawables itself.
         if (found != ids.end())
         {
            ids.erase(found);
         }
      }

      std::set<dtCore::UniqueId>::iterator idItr,idItrEnd;
      idItr = ids.begin();
      idItrEnd = ids.end();

      for (; idItr != idItrEnd; ++idItr)
      {
         dtCore::BaseActorObject* actTmp = map.GetProxyById(*idItr);
         CPPUNIT_ASSERT_MESSAGE("All actors in the map not found in the scene should not have drawables.", actTmp->GetDrawable() == NULL);
      }

   }
   catch (dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL((std::string("Error: ") + ex.What()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestEnvironmentMapLoading()
{
   try
   {
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);
      dtCore::Project &project = dtCore::Project::GetInstance();
      dtCore::Map &map = project.CreateMap("TestEnvironmentMap", "TestEnvironmentMap");

      dtCore::ActorRefPtrVector container;
      CPPUNIT_ASSERT(container.empty());

      const unsigned int numProxies = 4;
      for (unsigned int i = 0; i < numProxies; ++i)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> p =
               dtCore::ActorFactory::GetInstance().CreateActor("dtcore.examples", "Test All Properties");
         CPPUNIT_ASSERT(p.valid());
         container.push_back(p);
      }

      CPPUNIT_ASSERT(container.size() == numProxies);

      for (unsigned int i = 0; i < container.size(); ++i)
      {
         map.AddProxy(*container[i]);
      }

      dtCore::ActorRefPtrVector mapProxies;
      map.GetAllProxies(mapProxies);
      CPPUNIT_ASSERT_MESSAGE("The map should have the correct number of proxies", mapProxies.size() == numProxies);

      dtCore::RefPtr<dtCore::BaseActorObject> envProxy = dtCore::ActorFactory::GetInstance().CreateActor("Test Environment Actor", "Test Environment Actor");
      CPPUNIT_ASSERT(envProxy.valid());

      map.SetEnvironmentActor(envProxy.get());
      mapProxies.clear();

      map.GetAllProxies(mapProxies);
      CPPUNIT_ASSERT_MESSAGE("The map should have the correct number of proxies + the environment actor", mapProxies.size() == numProxies + 1);
      CPPUNIT_ASSERT_MESSAGE("GetEnvironmentActor should return what was set", map.GetEnvironmentActor() == envProxy.get());

      std::string mapName = map.GetName();
      project.SaveMap(mapName);
      project.CloseMap(map);

      dtCore::Map &savedMap = project.GetMap(mapName);
      mapProxies.clear();
      savedMap.GetAllProxies(mapProxies);
      CPPUNIT_ASSERT_MESSAGE("Saved map should have the correct number of proxies in it", mapProxies.size() == numProxies + 1);

      envProxy = savedMap.GetEnvironmentActor();
      CPPUNIT_ASSERT_MESSAGE("The environment actor should not be NULL", envProxy.valid());
      project.DeleteMap(mapName, true);
   }
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestLoadEnvironmentMapIntoScene()
{
   dtCore::ActorFactory::GetInstance().LoadActorRegistry(mExampleLibraryName);
   dtCore::Project &project = dtCore::Project::GetInstance();
   dtCore::Map &map = project.CreateMap("TestEnvironmentMap", "TestEnvironmentMap");
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene;

   scene->RemoveAllDrawables();
   CPPUNIT_ASSERT(scene->GetNumberOfAddedDrawable() == 0);

   dtCore::ActorRefPtrVector container;
   CPPUNIT_ASSERT(container.empty());

   const unsigned int numProxies = 4;
   for (unsigned int i = 0; i < numProxies; ++i)
   {
      dtCore::RefPtr<dtCore::BaseActorObject> p =
            dtCore::ActorFactory::GetInstance().CreateActor("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT(p.valid());
      container.push_back(p);
   }

   CPPUNIT_ASSERT(container.size() == numProxies);

   for (unsigned int i = 0; i < container.size(); ++i)
   {
      map.AddProxy(*container[i]);
   }

   dtCore::ActorRefPtrVector mapProxies;
   map.GetAllProxies(mapProxies);
   CPPUNIT_ASSERT_MESSAGE("The map should have the correct number of proxies", mapProxies.size() == numProxies);

   dtCore::RefPtr<dtCore::BaseActorObject> envProxy = dtCore::ActorFactory::GetInstance().CreateActor("Test Environment Actor", "Test Environment Actor");
   CPPUNIT_ASSERT(envProxy.valid());

   project.LoadMapIntoScene(map, *scene);
   unsigned int numDrawables = scene->GetNumberOfAddedDrawable();
   CPPUNIT_ASSERT_MESSAGE("The number of drawables should equal the number of proxies", numDrawables == numProxies);
   scene->RemoveAllDrawables();
   CPPUNIT_ASSERT(scene->GetNumberOfAddedDrawable() == 0);

   dtCore::IEnvironmentActor *tea = dynamic_cast<dtCore::IEnvironmentActor*>(envProxy->GetDrawable());
   CPPUNIT_ASSERT(tea != NULL);
   unsigned int numChildren = tea->GetNumEnvironmentChildren();
   CPPUNIT_ASSERT(numChildren == 0);

   map.SetEnvironmentActor(envProxy.get());
   project.LoadMapIntoScene(map, *scene);
   numDrawables = scene->GetNumberOfAddedDrawable();
   CPPUNIT_ASSERT_MESSAGE("The number of drawables should only be 1, the environment actor", numDrawables == 1);

   numChildren = tea->GetNumEnvironmentChildren();
   CPPUNIT_ASSERT_MESSAGE("The environment actor should have all the proxies as its children", numChildren == numProxies);
}

class OverriddenActorProxy : public dtCore::TransformableActorProxy
{
   //dtCore::RefPtr<dtCore::Transformable> mActor;
public:
   OverriddenActorProxy() {SetClassName("OverriddenActorProxy");}
   void CreateDrawable(){SetDrawable(*new dtCore::Transformable());}
   bool RemoveTheProperty(std::string& stringToRemove)
   {
      // not in the list
      if (GetProperty(stringToRemove) == NULL) return false;
      // is in the list
      RemoveProperty(stringToRemove);
      return (GetProperty(stringToRemove) == NULL);
   }

   bool RemoveTheProperty(dtCore::ActorProperty* prop)
   {
      if (prop == NULL) return false;
      // Save it in a ref ptr because it will be deleted otherwise.
      dtCore::RefPtr<dtCore::ActorProperty> saveProp = prop;
      // not in the list
      if (GetProperty(prop->GetName()) == NULL) return false;
      // is in the list
      RemoveProperty(prop);
      return (GetProperty(saveProp->GetName()) == NULL);
   }

protected:
   virtual ~OverriddenActorProxy() {}
};

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestActorProxyRemoveProperties()
{
   dtCore::RefPtr<OverriddenActorProxy> actorProxy = new OverriddenActorProxy;
   std::string NameToRemove = dtCore::TransformableActorProxy::PROPERTY_ROTATION;
   std::string DoesntExist = "TeagueGrowsRutabaga";
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property before initialized should have returned false", actorProxy->RemoveTheProperty(NameToRemove) == false );
   actorProxy->CreateDrawable();
   actorProxy->BuildPropertyMap();
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property after initialized should have returned true", actorProxy->RemoveTheProperty(NameToRemove) == true );
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property after initialized for a second time should have returned false", actorProxy->RemoveTheProperty(NameToRemove) == false );
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property that we know doesnt exist should have returned false", actorProxy->RemoveTheProperty(DoesntExist) == false );
}


///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestRemovePropertiesByPointer()
{
   dtCore::RefPtr<OverriddenActorProxy> actorProxy = new OverriddenActorProxy;
   std::string NameToRemove = dtCore::TransformableActorProxy::PROPERTY_ROTATION;
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property before initialized should have returned false", actorProxy->RemoveTheProperty(actorProxy->GetProperty(NameToRemove)) == false );
   actorProxy->CreateDrawable();
   actorProxy->BuildPropertyMap();
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property after initialized should have returned true", actorProxy->RemoveTheProperty(actorProxy->GetProperty(NameToRemove)) == true );
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property after initialized for a second time should have returned false", actorProxy->RemoveTheProperty(actorProxy->GetProperty(NameToRemove)) == false );
   // This shouldn't crash.
   actorProxy->RemoveProperty(NULL);
}

void MapTests::TestCreateMapsMultiContext()
{
   try
   {
      dtCore::Project& p = dtCore::Project::GetInstance();
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      const std::string map1("Orz");
      const std::string map2("Androsynth");
      const std::string map3("Frumple");

      CPPUNIT_ASSERT_NO_THROW(p.CreateMap(map1, map1, 1));

      CPPUNIT_ASSERT(fileUtils.FileExists(p.GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map1 + ".dtmap"));
      CPPUNIT_ASSERT_MESSAGE( "The new map file should not be in context 0",
            ! fileUtils.FileExists(p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map1 + ".dtmap"));

      CPPUNIT_ASSERT_NO_THROW(p.CreateMap(map2, map2, 0));

      CPPUNIT_ASSERT(fileUtils.FileExists(p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map2 + ".dtmap"));
      CPPUNIT_ASSERT_MESSAGE( "The new map file should not be in context 1",
            ! fileUtils.FileExists(p.GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map2 + ".dtmap"));

      // Same file name, different name, different context.
      CPPUNIT_ASSERT_NO_THROW(p.CreateMap(map3, map2, 1));

      CPPUNIT_ASSERT(fileUtils.FileExists(p.GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map2 + ".dtmap"));


      p.DeleteMap(map2);

      CPPUNIT_ASSERT( ! fileUtils.FileExists(p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map2 + ".dtmap"));

      CPPUNIT_ASSERT_MESSAGE("the map with the same file name in context 1 should not have been deleted.",
            fileUtils.FileExists(p.GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map2 + ".dtmap"));

      p.DeleteMap(map3);

      CPPUNIT_ASSERT( ! fileUtils.FileExists(p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map2 + ".dtmap"));

      CPPUNIT_ASSERT(fileUtils.FileExists(p.GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map1 + ".dtmap"));
      p.DeleteMap(map1);
      CPPUNIT_ASSERT( ! fileUtils.FileExists(p.GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map1 + ".dtmap"));
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   //    catch (const std::exception& ex)
   //    {
   //        CPPUNIT_FAIL(ex.what());
   //    }
}

void MapTests::TestSaveAsMultiContext()
{
   try
   {
      dtCore::Project& p = dtCore::Project::GetInstance();
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      const std::string map1("Orz");
      const std::string mapSecond("/Frumple/"); // slashes because it can't be in file name.

      dtCore::Map& map = p.CreateMap(map1, map1, 0);

      CPPUNIT_ASSERT(fileUtils.FileExists(p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map1 + ".dtmap"));
      CPPUNIT_ASSERT(!fileUtils.FileExists(p.GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map1 + ".dtmap"));

      p.SaveMapAs(map, mapSecond, map1, 1);

      CPPUNIT_ASSERT(fileUtils.FileExists(p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map1 + ".dtmap"));
      CPPUNIT_ASSERT(fileUtils.FileExists(p.GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + "maps" + dtUtil::FileUtils::PATH_SEPARATOR + map1 + ".dtmap"));
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   //    catch (const std::exception& ex)
   //    {
   //        CPPUNIT_FAIL(ex.what());
   //    }
}

//////////////////////////////////////////////////////////////////////////
void MapTests::TestParsingMapHeaderData()
{
   //Create a new Map, set its header data and save it. Then parse the map file
   //for its header data and validate.

   const std::string mapName("MyCoolMap");
   const std::string mapFileName("MyCoolMap.dtMap");

   //remove any leftovers, in case this test failed last time it ran
   if (dtUtil::FileUtils::GetInstance().FileExists(TESTS_DIR + "/data/ProjectContext/maps/" + mapFileName))
   {
      dtUtil::FileUtils::GetInstance().FileDelete(TESTS_DIR + "/data/ProjectContext/maps/" + mapFileName);
   }

   dtCore::Project::GetInstance().SetContext(TESTS_DIR + "/data/ProjectContext");

   //create Map
   dtCore::Map& map = dtCore::Project::GetInstance().CreateMap(mapName, mapFileName);   

   const std::string description("{d1ae0308-cf13-4d08-81d0-18bbb7f6f724}");
   const std::string author("{0e849f31-b1ff-4555-939a-dd83d7c0259d}");
   const std::string comment("{fe451321-99f8-41c3-8cc9-616f228b6596}");
   const std::string copyright("{1e653a73-0065-41d6-8e63-75871c9c88ad}");

   //set header data
   map.SetDescription(description);
   map.SetAuthor(author);
   map.SetComment(comment);
   map.SetCopyright(copyright);

   map.AddLibrary("Shouldn't load", "1.0");
   createActors(map); // Add actors to make sure they don't load.

   map.GetEventManager().AddEvent(*new dtCore::GameEvent("Shouldn't load", "Shouldn't be read in a header read."));

   //save/close Map
   dtCore::Project::GetInstance().SaveMap(mapName);
   dtCore::Project::GetInstance().CloseAllMaps(true);

   //Parse Map file's header data
   dtCore::RefPtr<dtCore::MapParser> parser = new dtCore::MapParser();

   dtCore::MapPtr mapHeader;
   try
   {
      mapHeader = parser->ParseMapHeaderData(TESTS_DIR + "/data/ProjectContext/maps/" + mapFileName);
   }
   catch (const dtCore::MapParsingException& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
   catch (const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e)
   {
      CPPUNIT_FAIL(dtUtil::XMLStringConverter(e.getMessage()).ToString());
   }

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Map header description didn't get parsed correctly", description, mapHeader->GetDescription());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Map header author didn't get parsed correctly", author, mapHeader->GetAuthor());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Map header comment didn't get parsed correctly", comment, mapHeader->GetComment());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Map header copyright didn't get parsed correctly", copyright, mapHeader->GetCopyright());

   CPPUNIT_ASSERT(mapHeader->GetAllProxies().empty());
   CPPUNIT_ASSERT(mapHeader->GetAllLibraries().empty());
   CPPUNIT_ASSERT(nullptr == mapHeader->GetEnvironmentActor());
   CPPUNIT_ASSERT_EQUAL(0U, mapHeader->GetEventManager().GetNumEvents());

   //delete the temp map file
   dtCore::Project::GetInstance().DeleteMap(mapName, true);
   dtCore::Project::GetInstance().ClearAllContexts();
}
