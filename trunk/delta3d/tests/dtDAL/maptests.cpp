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

#include <dtDAL/abstractenumactorproperty.h>
#include <dtDAL/actoractorproperty.h>
#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/colorrgbaactorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/doubleactorproperty.h>
#include <dtDAL/environmentactor.h>
#include <dtDAL/floatactorproperty.h>
#include <dtDAL/propertycontaineractorproperty.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/intactorproperty.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/namedactorparameter.h>
#include <dtDAL/namedbooleanparameter.h>
#include <dtDAL/nameddoubleparameter.h>
#include <dtDAL/namedenumparameter.h>
#include <dtDAL/namedfloatparameter.h>
#include <dtDAL/namedgameeventparameter.h>
#include <dtDAL/namedintparameter.h>
#include <dtDAL/namedlongintparameter.h>
#include <dtDAL/namedresourceparameter.h>
#include <dtDAL/namedrgbacolorparameter.h>
#include <dtDAL/namedstringparameter.h>
#include <dtDAL/namedvectorparameters.h>
#include <dtDAL/namedgroupparameter.h>
#include <dtDAL/namedgroupparameter.inl>
#include <dtDAL/physicalactorproxy.h>
#include <dtDAL/project.h>
#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/stringactorproperty.h>
#include <dtDAL/vectoractorproperties.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>

#include <cppunit/extensions/HelperMacros.h>

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
      CPPUNIT_TEST(TestLibraryMethods);
      CPPUNIT_TEST(TestWildCard);
      CPPUNIT_TEST(TestEnvironmentMapLoading);
      CPPUNIT_TEST(TestLoadEnvironmentMapIntoScene);
      CPPUNIT_TEST(TestActorProxyRemoveProperties);
      CPPUNIT_TEST(TestCreateMapsMultiContext);
      CPPUNIT_TEST(TestSaveAsMultiContext);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

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
      void TestIsMapFileValid();
      void TestLoadMapIntoScene();
      void TestLibraryMethods();
      void TestEnvironmentMapLoading();
      void TestLoadEnvironmentMapIntoScene();
      void TestWildCard();
      void TestActorProxyRemoveProperties();
      void TestCreateMapsMultiContext();
      void TestSaveAsMultiContext();

      static const std::string TEST_PROJECT_DIR;
      static const std::string TEST_PROJECT_DIR_2;

   private:
       static const std::string mExampleLibraryName;
       static const std::string mExampleGameLibraryName;

       void createActors(dtDAL::Map& map);
       dtDAL::ActorProperty* getActorProperty(dtDAL::Map& map,
         const std::string& propName, dtDAL::DataType& type, unsigned which = 0);

       dtUtil::Log* logger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(MapTests);

const std::string MapTests::mExampleLibraryName="testActorLibrary";
// TODO: this test uses a library that links to dtGame.  Is that okay?
const std::string MapTests::mExampleGameLibraryName="testGameActorLibrary";

const std::string MapTests::TEST_PROJECT_DIR="WorkingMapProject";
const std::string MapTests::TEST_PROJECT_DIR_2="WorkingMapProject2";

const std::string DATA_DIR = dtUtil::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR + "examples/data";
const std::string TESTS_DIR = dtUtil::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR + "tests";
const std::string MAPPROJECTCONTEXT = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtDAL" + dtUtil::FileUtils::PATH_SEPARATOR + MapTests::TEST_PROJECT_DIR;
const std::string PROJECTCONTEXT = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtDAL" + dtUtil::FileUtils::PATH_SEPARATOR + "WorkingProject";

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::setUp()
{
    try
    {
        dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
        std::string logName("mapTest");

        logger = &dtUtil::Log::GetInstance(logName);

        dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
        std::string currentDir = fileUtils.CurrentDirectory();
        std::string projectDir("dtDAL");
        if (currentDir.substr(currentDir.size() - projectDir.size()) != projectDir)
        {
            fileUtils.PushDirectory(projectDir);
        }

        std::string rbodyToDelete("WorkingMapProject/Characters/marine/marine.rbody");

        if (fileUtils.DirExists(rbodyToDelete))
        {
            fileUtils.DirDelete(rbodyToDelete, true);
        }
        else if (fileUtils.FileExists(rbodyToDelete))
        {
            fileUtils.FileDelete(rbodyToDelete);
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
        dtDAL::Project::GetInstance().CreateContext(TEST_PROJECT_DIR, false);
        dtDAL::Project::GetInstance().SetContext(TEST_PROJECT_DIR);
        dtDAL::Project::GetInstance().CreateContext(TEST_PROJECT_DIR_2, true);
        dtDAL::Project::GetInstance().AddContext(TEST_PROJECT_DIR_2);

        CPPUNIT_ASSERT_MESSAGE("Context 0 should have no maps dir",
                 !fileUtils.DirExists(dtDAL::Project::GetInstance().GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + "maps"));
        CPPUNIT_ASSERT_MESSAGE("Context 1 should have a maps dir",
                 fileUtils.DirExists(dtDAL::Project::GetInstance().GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + "maps"));

        //copy the vector because the act of deleting a map will reload the map names list.
        const std::set<std::string> v = dtDAL::Project::GetInstance().GetMapNames();

        for (std::set<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
        {
            dtDAL::Project::GetInstance().DeleteMap(*i, true);
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
   std::string projectDir("dtDAL");
   shouldPopDir = currentDir.substr(currentDir.size() - projectDir.size()) == projectDir;

   try
   {
      dtDAL::Project::GetInstance().SetContext(TEST_PROJECT_DIR);
      dtDAL::Project::GetInstance().AddContext(TEST_PROJECT_DIR_2);
      //copy the vector because the act of deleting a map will reload the map names list.
      const std::set<std::string> v = dtDAL::Project::GetInstance().GetMapNames();

      for (std::set<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
      {
         dtDAL::Project::GetInstance().DeleteMap(*i);
      }

      std::string rbodyToDelete("WorkingMapProject/Characters/marine/marine.rbody");

      if (fileUtils.DirExists(rbodyToDelete))
      {
         fileUtils.DirDelete(rbodyToDelete, true);
      }
      else if (fileUtils.FileExists(rbodyToDelete))
      {
         fileUtils.FileDelete(rbodyToDelete);
      }

      dtDAL::Project::GetInstance().ClearAllContexts();

      if (fileUtils.DirExists("WorkingProject"))
      {
         fileUtils.DirDelete("WorkingProject", true);
      }
      if (fileUtils.DirExists("WorkingProject2"))
      {
         fileUtils.DirDelete("WorkingProject2", true);
      }

      if (dtDAL::LibraryManager::GetInstance().GetRegistry(mExampleLibraryName) != NULL)
      {
         dtDAL::LibraryManager::GetInstance().UnloadActorRegistry(mExampleLibraryName);
      }

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
void MapTests::createActors(dtDAL::Map& map)
{
   dtDAL::LibraryManager& libMgr = dtDAL::LibraryManager::GetInstance();
   std::vector<const dtDAL::ActorType*> actorTypes;
   std::vector<dtDAL::ActorProperty*> props;

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

      dtCore::RefPtr<dtDAL::BaseActorObject> proxy;
      // Test timing Stuff
      dtCore::Timer testClock;
      dtCore::Timer_t testClockStart = testClock.Tick();

      logger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                         "Creating actor proxy with type \"" + actorTypes[i]->GetFullName() + "\"." );

      proxy = libMgr.CreateActorProxy(*actorTypes[i]);
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
void MapTests::TestMapAddRemoveProxies()
{
    try
    {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        dtDAL::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

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

        std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > proxies;
        map.GetAllProxies(proxies);

        map.AddLibrary(mExampleLibraryName, "1.0");
        dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);

        dtCore::RefPtr<const dtDAL::ActorType> exampleType = dtDAL::LibraryManager::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
        CPPUNIT_ASSERT_MESSAGE("The example type is NULL", exampleType.valid());

        dtCore::RefPtr<dtDAL::BaseActorObject> proxy1 = dtDAL::LibraryManager::GetInstance().CreateActorProxy(*exampleType);
        CPPUNIT_ASSERT_MESSAGE("proxy1 is NULL", proxy1.valid());

        dtCore::RefPtr<dtDAL::BaseActorObject> proxy2 = dtDAL::LibraryManager::GetInstance().CreateActorProxy(*exampleType);
        CPPUNIT_ASSERT_MESSAGE("proxy2 is NULL", proxy2.valid());

        dtCore::RefPtr<dtDAL::BaseActorObject> proxy3 = dtDAL::LibraryManager::GetInstance().CreateActorProxy(*exampleType);
        CPPUNIT_ASSERT_MESSAGE("proxy3 is NULL", proxy3.valid());

        dtCore::RefPtr<dtDAL::BaseActorObject> proxy4 = dtDAL::LibraryManager::GetInstance().CreateActorProxy(*exampleType);
        CPPUNIT_ASSERT_MESSAGE("proxy4 is NULL", proxy4.valid());

        map.AddProxy(*proxy1.get());
        map.AddProxy(*proxy2.get());
        map.AddProxy(*proxy3.get());
        map.AddProxy(*proxy4.get());

        dynamic_cast<dtDAL::ActorIDActorProperty*>(proxy1->GetProperty("Test_Actor"))->SetValue(proxy2->GetId());
        dynamic_cast<dtDAL::ActorIDActorProperty*>(proxy2->GetProperty("Test_Actor"))->SetValue(proxy4->GetId());
        dynamic_cast<dtDAL::ActorIDActorProperty*>(proxy3->GetProperty("Test_Actor"))->SetValue(proxy4->GetId());

        map.RemoveProxy(*proxy4.get());

        CPPUNIT_ASSERT_MESSAGE("Proxy 1 should still be linked to proxy2", dynamic_cast<dtDAL::ActorIDActorProperty*>(proxy1->GetProperty("Test_Actor"))->GetValue() == proxy2->GetId());
        CPPUNIT_ASSERT_MESSAGE("Proxy 2 is linked still", dynamic_cast<dtDAL::ActorIDActorProperty*>(proxy2->GetProperty("Test_Actor"))->GetValue().ToString().empty());
        CPPUNIT_ASSERT_MESSAGE("Proxy 3 is linked still", dynamic_cast<dtDAL::ActorIDActorProperty*>(proxy3->GetProperty("Test_Actor"))->GetValue().ToString().empty());
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
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        project.SetContext(TEST_PROJECT_DIR);

        dtDAL::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

        createActors(map);

        unsigned maxId = map.GetAllProxies().size();

        std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > results;

        map.FindProxies(results, "", "dtcore", "");

        CPPUNIT_ASSERT_MESSAGE("dtcore category should include most of the base actor set and so should return most of the actors.",
            results.size() >= (map.GetAllProxies().size()/2));

        map.FindProxies(results, "", "dtc", "");

        CPPUNIT_ASSERT_MESSAGE("Only full category names should return results.", results.size() == 0);

        map.FindProxies(results, "", "dtcore.", "");
        CPPUNIT_ASSERT_MESSAGE("trailing dots should end with no results.", results.size() == 0);

        map.FindProxies(results, "", "", "","", dtDAL::Map::Placeable);
        for (std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> >::iterator i = results.begin();
            i != results.end(); ++i)
        {
            CPPUNIT_ASSERT_MESSAGE(std::string("Proxy ") + (*i)->GetName()
                + " should not be in the results, it is not placeable",
                 (*i)->IsPlaceable());
        }

        map.FindProxies(results, "", "", "","", dtDAL::Map::NotPlaceable);
        for (std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> >::iterator i = results.begin();
            i != results.end(); ++i)
        {
            CPPUNIT_ASSERT_MESSAGE(std::string("Proxy ") + (*i)->GetName()
                + " should not be in the results, it is placeable",
                !(*i)->IsPlaceable());
        }

        map.FindProxies(results, "", "", "","dtCore::Light");
        CPPUNIT_ASSERT_MESSAGE("There should be some lights in the results.", results.size() >= 3);


        for (std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> >::iterator i = results.begin();
            i != results.end(); ++i)
        {
            CPPUNIT_ASSERT_MESSAGE("All results should be instances of dtCore::Light",
                (*i)->IsInstanceOf("dtCore::Light"));
        }

        std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > proxies;

        map.GetAllProxies(proxies);

        for (unsigned x = 0;  x < proxies.size(); ++x)
        {
            dtCore::RefPtr<dtDAL::BaseActorObject> proxyPTR = map.GetProxyById(proxies[x]->GetId());

            CPPUNIT_ASSERT_MESSAGE("Proxy should be found in the map by the project.", &map == dtDAL::Project::GetInstance().GetMapForActorProxy(*proxyPTR));

            CPPUNIT_ASSERT_MESSAGE((std::string("Proxy not found with id: ") + proxies[x]->GetId().ToString()).c_str()
                , proxyPTR != NULL);

            dtDAL::BaseActorObject& proxy = *proxyPTR.get();

            CPPUNIT_ASSERT_MESSAGE((std::string("Proxy has the wrong id. ") + proxy.GetId().ToString()).c_str() ,
                proxy.GetId() == proxies[x]->GetId());

            const std::string& cat = proxy.GetActorType().GetCategory();
            const std::string& typeName = proxy.GetActorType().GetName();

            map.FindProxies(results, proxy.GetName(), cat, typeName, "",
                proxy.IsPlaceable() ? dtDAL::Map::Placeable : dtDAL::Map::NotPlaceable);

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
               const dtDAL::ActorType& at = results[j]->GetActorType();
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
dtDAL::ActorProperty* MapTests::getActorProperty(dtDAL::Map& map,
    const std::string& propName, dtDAL::DataType& type, unsigned which)
{
    for (std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::BaseActorObject> >::const_iterator i = map.GetAllProxies().begin();
        i != map.GetAllProxies().end(); ++i)
    {

        dtDAL::BaseActorObject* proxy = map.GetProxyById(i->first);

        CPPUNIT_ASSERT_MESSAGE("ERROR: Proxy is NULL", proxy!= NULL );

        if (propName != "")
        {
            dtDAL::ActorProperty* prop = proxy->GetProperty(propName);
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
            std::vector<dtDAL::ActorProperty*> props;
            proxy->GetPropertyList(props);
            for (std::vector<dtDAL::ActorProperty*>::iterator j = props.begin(); j<props.end(); ++j)
            {
                dtDAL::ActorProperty* prop = *j;
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
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        dtDAL::Map* map = &project.CreateMap("Neato Map", "neatomap");

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
       dtDAL::Project& project = dtDAL::Project::GetInstance();

       const std::string mapName("Neato Map");
       const std::string mapFileName("neatomap");

       dtDAL::Map* map = &project.CreateMap(mapName, mapFileName);
       const std::string fileNameToTest = mapFileName + dtDAL::Map::MAP_FILE_EXTENSION;

       CPPUNIT_ASSERT_EQUAL_MESSAGE("A newly created Map doesn't have the expected generated filename.",
                                    fileNameToTest, map->GetFileName());

       map->AddLibrary(mExampleLibraryName, "1.0");
       dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);

       createActors(*map);

       dtDAL::ActorPluginRegistry* reg = dtDAL::LibraryManager::GetInstance().GetRegistry(mExampleLibraryName);
       CPPUNIT_ASSERT_MESSAGE("Registry for testActorLibrary should not be NULL.", reg != NULL);

       project.SaveMap(*map);

       project.CloseMap(*map, true);

       reg = dtDAL::LibraryManager::GetInstance().GetRegistry(mExampleLibraryName);
       CPPUNIT_ASSERT_MESSAGE("testActorLibrary should have been closed.", reg == NULL);

       map = &project.GetMap(mapName);

       reg = dtDAL::LibraryManager::GetInstance().GetRegistry(mExampleLibraryName);
       CPPUNIT_ASSERT_MESSAGE("Registry for testActorLibrary should not be NULL.", reg != NULL);

       std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > proxies;
       //hold onto all the proxies so that the actor libraries can't be closed.
       map->GetAllProxies(proxies);

       project.CloseMap(*map, true);

       reg = dtDAL::LibraryManager::GetInstance().GetRegistry(mExampleLibraryName);
       CPPUNIT_ASSERT_MESSAGE("Registry for testActorLibrary should not be NULL.", reg != NULL);

       //cleanup the proxies
       proxies.clear();

       map = &project.GetMap(mapName);
       //create a new map that will ALSO use the same libraries
       project.CreateMap(mapName + "1", mapFileName + "1").AddLibrary(mExampleLibraryName, "1.0");

       createActors(project.GetMap(mapName + "1"));

       project.CloseMap(*map, true);

       reg = dtDAL::LibraryManager::GetInstance().GetRegistry(mExampleLibraryName);
       CPPUNIT_ASSERT_MESSAGE("Registry for testActorLibrary should not be NULL.", reg != NULL);

       //when the second map is closed, the libraries should not close if false is passed.
       project.CloseMap(project.GetMap(mapName + "1"), false);

       reg = dtDAL::LibraryManager::GetInstance().GetRegistry(mExampleLibraryName);
       CPPUNIT_ASSERT_MESSAGE("Registry for testActorLibrary should not be NULL.", reg != NULL);

       //reopen the map and close it with true to make sure the libraries close.
       project.CloseMap(project.GetMap(mapName), true);

       reg = dtDAL::LibraryManager::GetInstance().GetRegistry(mExampleLibraryName);
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
      dtDAL::Project& project = dtDAL::Project::GetInstance();
      dtDAL::Map& map = project.CreateMap("Neato Map", "neatomap");

      CPPUNIT_ASSERT(!map.IsModified());

      dtCore::RefPtr<dtDAL::GameEvent> event = new dtDAL::GameEvent("jojo", "helo");
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
   dtDAL::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data/demoMap");

   std::string validFile("maps/MyCoolMap.xml");
   std::string invalidFile("SkeletalMeshes/marine_test.xml");

   dtDAL::Project& project = dtDAL::Project::GetInstance();
   CPPUNIT_ASSERT(project.IsValidMapFile(validFile));
   CPPUNIT_ASSERT(!project.IsValidMapFile(invalidFile));
}

//////////////////////////////////////////////////////////////////////////
template<class PropertyType>
PropertyType* GetActorProperty(dtDAL::ActorProxy& proxy, dtDAL::DataType& type)
{
   std::vector<dtDAL::ActorProperty*> props;
   proxy.GetPropertyList(props);

   std::vector<dtDAL::ActorProperty*>::iterator propItr = props.begin();
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

   dtDAL::Map* map = NULL;

   try
   {
      //create new map
      map = &dtDAL::Project::GetInstance().CreateMap(mapName, mapFileName);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.ToString());
   }

   map->AddLibrary(mExampleLibraryName, "1.0");

   dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);

   //create test actor
   dtCore::RefPtr<dtDAL::BaseActorObject> proxy;
   try
   {
      proxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy(*ExampleActorLib::TEST_ACTOR_PROPERTY_TYPE.get());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.ToString());
   }

   map->AddProxy(*proxy);

   //dtDAL::ActorProperty* prop(NULL);

   const std::string TEST_STRING("test123  !@# < hello > ");
   const bool TEST_BOOL(true);
   const float TEST_FLOAT(12345.12345f);
   const double TEST_DOUBLE(12345.54321);
   const int TEST_INT(123345);
   const osg::Vec3 TEST_VEC3(1.f, 2.f, 3.f);
   const osg::Vec3 TEST_VEC3F(123.123f, 456.456f, 789.789f);
   const osg::Vec3d TEST_VEC3D(123.123, 456.456, 789.789);
   const osg::Vec4f TEST_VEC4F(1.f, 2.f, 3.f, 4.f);
   const osg::Vec4d TEST_VEC4D(2.0, 3.0, 4.0, 5.0);
   const osg::Vec4 TEST_RGBA(255.f, 245.f, 235.f, 1235.f);
   const dtCore::UniqueId TEST_ACTORID;
   const dtDAL::ResourceDescriptor TEST_RESOURCE("test", "somethingelse");

   //string
   {
      dtDAL::StringActorProperty* prop = GetActorProperty<dtDAL::StringActorProperty>(*proxy, dtDAL::DataType::STRING);
      prop->SetValue(TEST_STRING);
   }

   //boolean
   {
      dtDAL::BooleanActorProperty* prop = GetActorProperty<dtDAL::BooleanActorProperty>(*proxy, dtDAL::DataType::BOOLEAN);
      prop->SetValue(TEST_BOOL);
   }

   //float
   {
      dtDAL::FloatActorProperty* prop = GetActorProperty<dtDAL::FloatActorProperty>(*proxy, dtDAL::DataType::FLOAT);
      prop->SetValue(TEST_FLOAT);
   }

   //double
   {
      dtDAL::DoubleActorProperty* prop = GetActorProperty<dtDAL::DoubleActorProperty>(*proxy, dtDAL::DataType::DOUBLE);
      prop->SetValue(TEST_DOUBLE);
   }

   //int
   {
      dtDAL::IntActorProperty* prop = GetActorProperty<dtDAL::IntActorProperty>(*proxy, dtDAL::DataType::INT);
      prop->SetValue(TEST_INT);
   }


   //vec3 / vec3f
   {
      dtDAL::Vec3ActorProperty* prop = GetActorProperty<dtDAL::Vec3ActorProperty>(*proxy, dtDAL::DataType::VEC3);
      prop->SetValue(TEST_VEC3);
   }


   //vec3d
   {
      dtDAL::Vec3dActorProperty* prop = GetActorProperty<dtDAL::Vec3dActorProperty>(*proxy, dtDAL::DataType::VEC3D);
      prop->SetValue(TEST_VEC3D);
   }

   //vec4f
   {
      dtDAL::Vec4fActorProperty* prop = GetActorProperty<dtDAL::Vec4fActorProperty>(*proxy, dtDAL::DataType::VEC4F);
      prop->SetValue(TEST_VEC4F);
   }

   //vec4d
   {
      dtDAL::Vec4dActorProperty* prop = GetActorProperty<dtDAL::Vec4dActorProperty>(*proxy, dtDAL::DataType::VEC4D);
      prop->SetValue(TEST_VEC4D);
   }

   //rgbacolor
   {
      dtDAL::ColorRgbaActorProperty* prop = GetActorProperty<dtDAL::ColorRgbaActorProperty>(*proxy, dtDAL::DataType::RGBACOLOR);
      prop->SetValue(TEST_RGBA);
   }

   // ActorIDProperty
   {
      dtDAL::ActorIDActorProperty* prop = GetActorProperty<dtDAL::ActorIDActorProperty>(*proxy, dtDAL::DataType::ACTOR);
      prop->SetValue(TEST_ACTORID);
   }

   //enumeration
   {
      dtDAL::AbstractEnumActorProperty* prop = GetActorProperty<dtDAL::AbstractEnumActorProperty>(*proxy, dtDAL::DataType::ENUMERATION);
      prop->SetEnumValue( *(*prop->GetList().begin()) );
   }

   //Resource
   {
      dtDAL::ResourceActorProperty* prop = GetActorProperty<dtDAL::ResourceActorProperty>(*proxy, dtDAL::DataType::SOUND);
      prop->SetValue(TEST_RESOURCE);
   }

   //save map
   dtDAL::Project::GetInstance().SaveMap(*map);

   //close map
   dtDAL::Project::GetInstance().CloseMap(*map, true);
   map = NULL;

   //reopen map
   map = &dtDAL::Project::GetInstance().GetMap(mapName);

   const std::string kAssertMsg("Property value returned from Map doesn't match what was set");

   //string
   {
      dtDAL::StringActorProperty* prop = GetActorProperty<dtDAL::StringActorProperty>(*proxy, dtDAL::DataType::STRING);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_STRING, prop->GetValue());
   }

   //boolean
   {
      dtDAL::BooleanActorProperty* prop = GetActorProperty<dtDAL::BooleanActorProperty>(*proxy, dtDAL::DataType::BOOLEAN);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_BOOL, prop->GetValue());
   }

   //float
   {
      dtDAL::FloatActorProperty* prop = GetActorProperty<dtDAL::FloatActorProperty>(*proxy, dtDAL::DataType::FLOAT);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_FLOAT, prop->GetValue());
   }

   //double
   {
      dtDAL::DoubleActorProperty* prop = GetActorProperty<dtDAL::DoubleActorProperty>(*proxy, dtDAL::DataType::DOUBLE);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_DOUBLE, prop->GetValue());
   }

   //int
   {
      dtDAL::IntActorProperty* prop = GetActorProperty<dtDAL::IntActorProperty>(*proxy, dtDAL::DataType::INT);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_INT, prop->GetValue());
   }


   //vec3 /vec3f
   {
      dtDAL::Vec3ActorProperty* prop = GetActorProperty<dtDAL::Vec3ActorProperty>(*proxy, dtDAL::DataType::VEC3);
      CPPUNIT_ASSERT_MESSAGE(kAssertMsg, dtUtil::Equivalent(TEST_VEC3, prop->GetValue(), 1e-2f));
   }

   //vec3d
   {
      dtDAL::Vec3dActorProperty* prop = GetActorProperty<dtDAL::Vec3dActorProperty>(*proxy, dtDAL::DataType::VEC3D);
      CPPUNIT_ASSERT_MESSAGE(kAssertMsg, dtUtil::Equivalent(TEST_VEC3D, prop->GetValue(), 1e-2));
   }

   //vec4f
   {
      dtDAL::Vec4fActorProperty* prop = GetActorProperty<dtDAL::Vec4fActorProperty>(*proxy, dtDAL::DataType::VEC4F);
      CPPUNIT_ASSERT_MESSAGE(kAssertMsg, dtUtil::Equivalent(TEST_VEC4F, prop->GetValue(), 1e-2f));
   }

   //vec4d
   {
      dtDAL::Vec4dActorProperty* prop = GetActorProperty<dtDAL::Vec4dActorProperty>(*proxy, dtDAL::DataType::VEC4D);
      CPPUNIT_ASSERT_MESSAGE(kAssertMsg, dtUtil::Equivalent(TEST_VEC4D, prop->GetValue(), 1e-2));
   }

   //rgbacolor
   {
      dtDAL::ColorRgbaActorProperty* prop = GetActorProperty<dtDAL::ColorRgbaActorProperty>(*proxy, dtDAL::DataType::RGBACOLOR);
      CPPUNIT_ASSERT_MESSAGE(kAssertMsg, dtUtil::Equivalent(TEST_RGBA, prop->GetValue(), 1e-2f));
   }

   // ActoridProperty
   {
      dtDAL::ActorIDActorProperty* prop = GetActorProperty<dtDAL::ActorIDActorProperty>(*proxy, dtDAL::DataType::ACTOR);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_ACTORID, prop->GetValue());
   }

   //enumeration
   {
      dtDAL::AbstractEnumActorProperty* prop = GetActorProperty<dtDAL::AbstractEnumActorProperty>(*proxy, dtDAL::DataType::ENUMERATION);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, (*prop->GetList().begin())->GetName(), prop->GetEnumValue().GetName());
   }

   //Resource
   {
      dtDAL::ResourceActorProperty* prop = GetActorProperty<dtDAL::ResourceActorProperty>(*proxy, dtDAL::DataType::SOUND);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(kAssertMsg, TEST_RESOURCE.GetResourceIdentifier(), prop->GetValue().GetResourceIdentifier());
   }

   dtDAL::Project::GetInstance().DeleteMap(*map, true);
}


///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestMapSaveAndLoadEvents()
{
   try
   {
      dtDAL::Project& project = dtDAL::Project::GetInstance();

      const std::string mapName("Neato Map");
      const std::string mapFileName("neatomap");

      dtDAL::Map* map = &project.CreateMap(mapName, mapFileName);

      map->SetDescription("Teague is league with a \"t\".");

      const unsigned eventCount = 6;

      std::vector<dtCore::RefPtr<dtDAL::GameEvent> > events;
      for (unsigned i = 0; i < eventCount; ++i)
      {
         std::ostringstream ss;
         ss << "name" << i;
         dtCore::RefPtr<dtDAL::GameEvent> ge = new dtDAL::GameEvent(ss.str(), "Test Description");
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
         dtDAL::GameEvent& expectedEvent = *events[i];
         dtDAL::GameEvent* ge = map->GetEventManager().FindEvent(expectedEvent.GetUniqueId());

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
      dtDAL::Project& project = dtDAL::Project::GetInstance();

      std::string mapName("Neato Map");
      std::string mapFileName("neatomap");

      dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);

      const dtDAL::ActorType* at = dtDAL::LibraryManager::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT(at != NULL);

      dtCore::RefPtr<dtDAL::NamedGroupParameter> expectedResult = new dtDAL::NamedGroupParameter("TestGroup");
      dtCore::RefPtr<dtDAL::NamedGroupParameter> secondInternalGroup = static_cast<dtDAL::NamedGroupParameter*>(expectedResult->AddParameter("FloatGroup", dtDAL::DataType::GROUP));
    
      {
         dtDAL::Map* map = &project.CreateMap(mapName, mapFileName);
         map->AddLibrary(mExampleLibraryName, "1.0");

         dtCore::RefPtr<dtDAL::BaseActorObject> proxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy(*at);
       
         dtDAL::GroupActorProperty* groupProp;
         proxy->GetProperty("TestGroup", groupProp);

         const dtDAL::ResourceDescriptor rd("StaticMeshes:Chicken:Horse.ive", "StaticMeshes:Chicken:Horse.ive");

         dtDAL::GameEvent* ge = new dtDAL::GameEvent("cow", "chicken");
         map->GetEventManager().AddEvent(*ge);

         expectedResult->AddValue("SillyInt", 24);
         expectedResult->AddValue("SillyLong", 37L);
         expectedResult->AddValue("SillyString", std::string("Jojo"));
         static_cast<dtDAL::NamedResourceParameter&>(*expectedResult->AddParameter("SillyResource1", dtDAL::DataType::STATIC_MESH)).SetValue(rd);
         static_cast<dtDAL::NamedResourceParameter&>(*expectedResult->AddParameter("SillyResource2", dtDAL::DataType::TEXTURE)).SetValue(dtDAL::ResourceDescriptor::NULL_RESOURCE);

         dtDAL::NamedGroupParameter& internalGroup = static_cast<dtDAL::NamedGroupParameter&>(*expectedResult->AddParameter("SillyGroup", dtDAL::DataType::GROUP));
         internalGroup.AddValue("CuteString", std::string("Just a string"));
         static_cast<dtDAL::NamedGameEventParameter&>(*internalGroup.AddParameter("CuteEvent", dtDAL::DataType::GAME_EVENT)).SetValue(ge->GetUniqueId());
         static_cast<dtDAL::NamedActorParameter&>(*internalGroup.AddParameter("CuteActor", dtDAL::DataType::ACTOR)).SetValue(proxy->GetId());
         internalGroup.AddValue("CuteBool", true);

         static_cast<dtDAL::NamedVec2Parameter&>(*secondInternalGroup->AddParameter("CuteVec2", dtDAL::DataType::VEC2)).SetValue(osg::Vec2(1.0f, 1.3f));
         static_cast<dtDAL::NamedVec3Parameter&>(*secondInternalGroup->AddParameter("CuteVec3", dtDAL::DataType::VEC3)).SetValue(osg::Vec3(1.0f, 1.3f, 34.7f));
         static_cast<dtDAL::NamedVec4Parameter&>(*secondInternalGroup->AddParameter("CuteVec4", dtDAL::DataType::VEC4)).SetValue(osg::Vec4(1.0f, 1.3f, 34.7f, 77.6f));
         static_cast<dtDAL::NamedRGBAColorParameter&>(*secondInternalGroup->AddParameter("CuteColor", dtDAL::DataType::RGBACOLOR)).SetValue(osg::Vec4(1.0f, 0.6f, 0.3f, 0.11f));
         static_cast<dtDAL::NamedFloatParameter&>(*secondInternalGroup->AddParameter("CuteFloat", dtDAL::DataType::FLOAT)).SetValue(3.8f);
         static_cast<dtDAL::NamedDoubleParameter&>(*secondInternalGroup->AddParameter("CuteDouble", dtDAL::DataType::DOUBLE)).SetValue(3.8f);

         groupProp->SetValue(*expectedResult);

         //remove the floats so that they can compared separately using epsilons.
         expectedResult->RemoveParameter(secondInternalGroup->GetName());

         map->AddProxy(*proxy);

         project.SaveMap(*map);
         project.CloseMap(*map);
      }

      dtDAL::Map* map = &project.GetMap(mapName);

      std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > toFill;
      map->GetAllProxies(toFill);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The map was saved with one proxy.  It should have one when loaded.", toFill.size(), size_t(1));

      dtCore::RefPtr<dtDAL::BaseActorObject> proxy = toFill[0];
 
      dtDAL::GroupActorProperty* groupProp;
      proxy->GetProperty("TestGroup", groupProp);

      dtCore::RefPtr<dtDAL::NamedGroupParameter> actualResult = groupProp->GetValue();
      CPPUNIT_ASSERT(actualResult.valid());

      dtCore::RefPtr<dtDAL::NamedParameter> floatGroup = actualResult->RemoveParameter(secondInternalGroup->GetName());
      dtCore::RefPtr<dtDAL::NamedGroupParameter> actualFloatGroup(dynamic_cast<dtDAL::NamedGroupParameter*>(floatGroup.get()));

      CPPUNIT_ASSERT_MESSAGE("The loaded result parameter should have group filled with floats.", actualFloatGroup.valid());

      CPPUNIT_ASSERT_MESSAGE("The returned group parameter doesn't match the actual\n" + actualResult->ToString() + " \n\n " + expectedResult->ToString(),
                              *expectedResult == *actualResult);

      std::vector<dtDAL::NamedParameter*> savedFloatParams;
      secondInternalGroup->GetParameters(savedFloatParams);
      for (unsigned i = 0; i < savedFloatParams.size(); ++i)
      {
         dtDAL::NamedParameter* np = actualFloatGroup->GetParameter(savedFloatParams[i]->GetName());
         CPPUNIT_ASSERT_MESSAGE(np->GetName() + " should be a parameter in the FloatGroup parameter group loaded from the map." , np != NULL);
         CPPUNIT_ASSERT_MESSAGE(np->GetName() + " parameter should have the same data type as it did before it was saved in a map." ,
            np->GetDataType() == savedFloatParams[i]->GetDataType());
      }

      std::string valueString = actualFloatGroup->ToString() + "\n\n" + secondInternalGroup->ToString();

      CPPUNIT_ASSERT_MESSAGE("The loaded vec2 parameter should match the one saved: \n" + valueString,
         dtUtil::Equivalent(
            static_cast<dtDAL::NamedVec2Parameter*>(secondInternalGroup->GetParameter("CuteVec2"))->GetValue(),
            static_cast<dtDAL::NamedVec2Parameter*>(actualFloatGroup->GetParameter("CuteVec2"))->GetValue(), 1e-3f));

      CPPUNIT_ASSERT_MESSAGE("The loaded vec3 parameter should match the one saved: \n" + valueString,
         dtUtil::Equivalent(
            static_cast<dtDAL::NamedVec3Parameter*>(secondInternalGroup->GetParameter("CuteVec3"))->GetValue(),
            static_cast<dtDAL::NamedVec3Parameter*>(actualFloatGroup->GetParameter("CuteVec3"))->GetValue(), 1e-3f));

      CPPUNIT_ASSERT_MESSAGE("The loaded vec4 parameter should match the one saved: \n" + valueString,
         dtUtil::Equivalent(
            static_cast<dtDAL::NamedVec4Parameter*>(secondInternalGroup->GetParameter("CuteVec4"))->GetValue(),
            static_cast<dtDAL::NamedVec4Parameter*>(actualFloatGroup->GetParameter("CuteVec4"))->GetValue(), 1e-3f));

      CPPUNIT_ASSERT_MESSAGE("The loaded color parameter should match the one saved: \n" + valueString,
         dtUtil::Equivalent(
            static_cast<dtDAL::NamedRGBAColorParameter*>(secondInternalGroup->GetParameter("CuteColor"))->GetValue(),
            static_cast<dtDAL::NamedRGBAColorParameter*>(actualFloatGroup->GetParameter("CuteColor"))->GetValue(), 1e-3f));

      CPPUNIT_ASSERT_MESSAGE("The loaded float parameter should match the one saved: \n" + valueString,
         osg::equivalent(
            static_cast<dtDAL::NamedFloatParameter*>(secondInternalGroup->GetParameter("CuteFloat"))->GetValue(),
            static_cast<dtDAL::NamedFloatParameter*>(actualFloatGroup->GetParameter("CuteFloat"))->GetValue(), 1e-3f));
      CPPUNIT_ASSERT_MESSAGE("The loaded double parameter should match the one saved: \n" + valueString,
         osg::equivalent(
            static_cast<dtDAL::NamedDoubleParameter*>(secondInternalGroup->GetParameter("CuteDouble"))->GetValue(),
            static_cast<dtDAL::NamedDoubleParameter*>(actualFloatGroup->GetParameter("CuteDouble"))->GetValue(), 1e-3));

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
      dtDAL::Project& project = dtDAL::Project::GetInstance();

      std::string mapName("Neato Map");
      std::string mapFileName("neatomap");

      dtDAL::Map* map = &project.CreateMap(mapName, mapFileName);
      map->AddLibrary(mExampleLibraryName, "1.0");
      dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);

      const dtDAL::ActorType* at = dtDAL::LibraryManager::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT(at != NULL);

      dtCore::RefPtr<dtDAL::BaseActorObject> actor = dtDAL::LibraryManager::GetInstance().CreateActor(*at);
      map->AddProxy(*actor);
      dtCore::UniqueId idToSave = actor->GetId();

      dtDAL::BasePropertyContainerActorProperty* bpc = NULL;
      actor->GetProperty("TestPropertyContainer", bpc);
      CPPUNIT_ASSERT(bpc != NULL);

      dtDAL::PropertyContainer* pc = bpc->GetValue();
      CPPUNIT_ASSERT(pc != NULL);

      const float testFloat(37.36f);
      const double testDouble(-393.334);
      const bool testBool(true);
      const int testInt(-347);

      dtDAL::FloatActorProperty* fap;
      pc->GetProperty("Test_Float", fap);
      CPPUNIT_ASSERT(fap != NULL);
      fap->SetValue(testFloat);

      dtDAL::DoubleActorProperty* dap;
      pc->GetProperty("Test_Double", dap);
      CPPUNIT_ASSERT(dap != NULL);
      dap->SetValue(testDouble);

      dtDAL::BooleanActorProperty* bap;
      pc->GetProperty("Test_Boolean", bap);
      CPPUNIT_ASSERT(bap != NULL);
      bap->SetValue(testBool);

      dtDAL::IntActorProperty* iap;
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
      dtDAL::Project& project = dtDAL::Project::GetInstance();

      std::string mapName("Neato Map");
      std::string mapFileName("neatomap");

      dtDAL::Map* map = &project.CreateMap(mapName, mapFileName);
      map->AddLibrary(mExampleGameLibraryName, "1.0");
      dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleGameLibraryName);

      const dtDAL::ActorType* at = dtDAL::LibraryManager::GetInstance().FindActorType("ExampleActors", "TestGamePropertyActor");
      CPPUNIT_ASSERT(at != NULL);

      dtCore::RefPtr<dtDAL::BaseActorObject> actor = dtDAL::LibraryManager::GetInstance().CreateActor(*at);
      map->AddProxy(*actor);
      dtCore::UniqueId idToSave = actor->GetId();

      dtDAL::BasePropertyContainerActorProperty* bpc = NULL;
      actor->GetProperty("TestPropertyContainer", bpc);
      CPPUNIT_ASSERT(bpc != NULL);

      dtDAL::PropertyContainer* pc = bpc->GetValue();
      CPPUNIT_ASSERT(pc != NULL);

      dtDAL::ArrayActorPropertyBase* arrayProp = NULL;
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

      dtDAL::BasePropertyContainerActorProperty* bpcD1 = dynamic_cast<dtDAL::BasePropertyContainerActorProperty*>(arrayProp->GetArrayProperty());
      CPPUNIT_ASSERT(bpcD1 != NULL);

      // Get the new property container at index 1 on the depth 1 array.
      dtDAL::PropertyContainer* pcD1i1 = bpcD1->GetValue();
      dtDAL::ArrayActorPropertyBase* arrayPropD1 = NULL;
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

      bpcD1 = dynamic_cast<dtDAL::BasePropertyContainerActorProperty*>(arrayProp->GetArrayProperty());
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
//when calling Project::GetMapForActorProxy.  It also tests the mParsing flag on the dtDAL::MapParser
//and it tests the SubTasks property on the task actors.
void MapTests::TestMapSaveAndLoadActorGroups()
{
   try
   {
      dtDAL::Project& project = dtDAL::Project::GetInstance();

      std::string mapName("Neato Map");
      std::string mapFileName("neatomap");

      dtDAL::Map* map = &project.CreateMap(mapName, mapFileName);

      dtDAL::LibraryManager& libraryManager = dtDAL::LibraryManager::GetInstance();

      const dtDAL::ActorType* at = libraryManager.FindActorType("dtcore.Tasks", "Task Actor");
      CPPUNIT_ASSERT(at != NULL);

      dtCore::RefPtr<dtDAL::BaseActorObject> proxy = libraryManager.CreateActorProxy(*at);

      map->AddProxy(*proxy);

      std::vector<dtCore::UniqueId> subTasks;
      //all of these actors are added to the map AFTER the main proxy
      //so they won't be loaded yet when the map is loaded.
      //This tests that the group property will load correctly regardless of actor ordering.
      std::ostringstream ss;
      for (unsigned i = 0; i < 10; ++i)
      {
         dtCore::RefPtr<dtDAL::BaseActorObject> proxy = libraryManager.CreateActorProxy(*at);
         subTasks.push_back(proxy->GetId());
         map->AddProxy(*proxy);
         ss.str("");
         ss << i;
      }

      dtDAL::ArrayActorProperty<dtCore::UniqueId>* arrayProp = NULL;
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
void MapTests::TestWildCard()
{
   CPPUNIT_ASSERT(dtDAL::Map::WildMatch("*", "sthsthsth"));
   CPPUNIT_ASSERT(dtDAL::Map::WildMatch("sth*", "sthsthsth"));
   CPPUNIT_ASSERT(dtDAL::Map::WildMatch("a*eda", "aeeeda"));
   CPPUNIT_ASSERT(dtDAL::Map::WildMatch("a*eda", "aedededa"));
   CPPUNIT_ASSERT(dtDAL::Map::WildMatch("a*eda", "aedededa"));
   CPPUNIT_ASSERT(dtDAL::Map::WildMatch("*Cur?is? *Murphy*", "Curtiss Murphy"));
   CPPUNIT_ASSERT(dtDAL::Map::WildMatch("?????", "12345"));

   CPPUNIT_ASSERT(!dtDAL::Map::WildMatch("*Cur?i? *Murphy*", "Curtiss Murphy"));
   CPPUNIT_ASSERT(!dtDAL::Map::WildMatch("??????", "12345"));
   CPPUNIT_ASSERT(!dtDAL::Map::WildMatch("a?eda", "aedededa"));
}


///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestLoadMapIntoScene()
{
    try
    {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        dtDAL::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

        createActors(map);

        std::set<dtCore::UniqueId> ids;

        //add all the names of the actors that should be in the scene to set so we can track them.
        for (std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::BaseActorObject> >::const_iterator i = map.GetAllProxies().begin();
            i != map.GetAllProxies().end(); ++i)
        {
            const dtDAL::BaseActorObject::RenderMode &renderMode = const_cast<dtDAL::BaseActorObject&>(*i->second).GetRenderMode();

            if (renderMode == dtDAL::BaseActorObject::RenderMode::DRAW_ACTOR ||
                renderMode == dtDAL::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
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

        std::ostringstream ostream;
        ostream << "All drawables should have been found in the delta 3d list of drawables and removed. There are " << ids.size() <<
        "not found.";

        //Make sure all drawables have been removed.
        CPPUNIT_ASSERT_MESSAGE(ostream.str() ,
            ids.size() == 0);

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
      dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);
      dtDAL::Project &project = dtDAL::Project::GetInstance();
      dtDAL::Map &map = project.CreateMap("TestEnvironmentMap", "TestEnvironmentMap");

      std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > container;
      CPPUNIT_ASSERT(container.empty());

      const unsigned int numProxies = 4;
      for (unsigned int i = 0; i < numProxies; ++i)
      {
         dtCore::RefPtr<dtDAL::BaseActorObject> p =
            dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtcore.examples", "Test All Properties");
         CPPUNIT_ASSERT(p.valid());
         container.push_back(p);
      }

      CPPUNIT_ASSERT(container.size() == numProxies);

      for (unsigned int i = 0; i < container.size(); ++i)
      {
         map.AddProxy(*container[i]);
      }

      std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > mapProxies;
      map.GetAllProxies(mapProxies);
      CPPUNIT_ASSERT_MESSAGE("The map should have the correct number of proxies", mapProxies.size() == numProxies);

      dtCore::RefPtr<dtDAL::BaseActorObject> envProxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy("Test Environment Actor", "Test Environment Actor");
      CPPUNIT_ASSERT(envProxy.valid());

      map.SetEnvironmentActor(envProxy.get());
      mapProxies.clear();

      map.GetAllProxies(mapProxies);
      CPPUNIT_ASSERT_MESSAGE("The map should have the correct number of proxies + the environment actor", mapProxies.size() == numProxies + 1);
      CPPUNIT_ASSERT_MESSAGE("GetEnvironmentActor should return what was set", map.GetEnvironmentActor() == envProxy.get());

      std::string mapName = map.GetName();
      project.SaveMap(mapName);
      project.CloseMap(map);

      dtDAL::Map &savedMap = project.GetMap(mapName);
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
   dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);
   dtDAL::Project &project = dtDAL::Project::GetInstance();
   dtDAL::Map &map = project.CreateMap("TestEnvironmentMap", "TestEnvironmentMap");
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene;

   scene->RemoveAllDrawables();
   CPPUNIT_ASSERT(scene->GetNumberOfAddedDrawable() == 0);

   std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > container;
   CPPUNIT_ASSERT(container.empty());

   const unsigned int numProxies = 4;
   for (unsigned int i = 0; i < numProxies; ++i)
   {
      dtCore::RefPtr<dtDAL::BaseActorObject> p =
         dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT(p.valid());
      container.push_back(p);
   }

   CPPUNIT_ASSERT(container.size() == numProxies);

   for (unsigned int i = 0; i < container.size(); ++i)
   {
      map.AddProxy(*container[i]);
   }

   std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > mapProxies;
   map.GetAllProxies(mapProxies);
   CPPUNIT_ASSERT_MESSAGE("The map should have the correct number of proxies", mapProxies.size() == numProxies);

   dtCore::RefPtr<dtDAL::BaseActorObject> envProxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy("Test Environment Actor", "Test Environment Actor");
   CPPUNIT_ASSERT(envProxy.valid());

   project.LoadMapIntoScene(map, *scene);
   unsigned int numDrawables = scene->GetNumberOfAddedDrawable();
   CPPUNIT_ASSERT_MESSAGE("The number of drawables should equal the number of proxies", numDrawables == numProxies);
   scene->RemoveAllDrawables();
   CPPUNIT_ASSERT(scene->GetNumberOfAddedDrawable() == 0);

   dtDAL::IEnvironmentActor *tea = dynamic_cast<dtDAL::IEnvironmentActor*>(envProxy->GetActor());
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

class OverriddenActorProxy : public dtDAL::TransformableActorProxy
{
   //dtCore::RefPtr<dtCore::Transformable> mActor;
public:
   OverriddenActorProxy() {SetClassName("OverriddenActorProxy");}
   void CreateActor(){SetActor(*new dtCore::Transformable());}
   bool RemoveTheProperty(std::string& stringToRemove)
   {
      // not in the list
      if (GetProperty(stringToRemove) == false) return false;
      // is in the list
      RemoveProperty(stringToRemove);
      return (GetProperty(stringToRemove) == NULL);
   }

protected:
   virtual ~OverriddenActorProxy() {}
};

///////////////////////////////////////////////////////////////////////////////////////
void MapTests::TestActorProxyRemoveProperties()
{
   dtCore::RefPtr<OverriddenActorProxy> actorProxy = new OverriddenActorProxy;
   std::string NameToRemove = dtDAL::TransformableActorProxy::PROPERTY_ROTATION;
   std::string DoesntExist = "TeagueHasAHawtMom";
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property before initialized should have returned false", actorProxy->RemoveTheProperty(NameToRemove) == false );
   actorProxy->CreateActor();
   actorProxy->BuildPropertyMap();
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property after initialized should have returned true", actorProxy->RemoveTheProperty(NameToRemove) == true );
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property after initialized for a second time should have returned false", actorProxy->RemoveTheProperty(NameToRemove) == false );
   CPPUNIT_ASSERT_MESSAGE("Tried to remove a property that we know doesnt exist should have returned false", actorProxy->RemoveTheProperty(DoesntExist) == false );
}

void MapTests::TestCreateMapsMultiContext()
{
   try
   {
      dtDAL::Project& p = dtDAL::Project::GetInstance();
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
      dtDAL::Project& p = dtDAL::Project::GetInstance();
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      const std::string map1("Orz");
      const std::string mapSecond("/Frumple/"); // slashes because it can't be in file name.

      dtDAL::Map& map = p.CreateMap(map1, map1, 0);

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
