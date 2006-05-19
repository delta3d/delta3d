/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author David Guthrie
*/
#include <vector>
#include <string>
#include <sstream>

#include <stdio.h>
#include <time.h>

#include <osg/Math>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <dtCore/dt.h>
#include <dtCore/globals.h>
#include <dtCore/timer.h>
#include <dtABC/application.h>

#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtUtil/log.h>
#include <dtDAL/mapxml.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/environmentactor.h>
#include <testActorLibrary/testActorLib.h>
#include <testActorLibrary/testdalenvironmentactor.h>

#include <rbody/config_error.h>

#include <cppunit/extensions/HelperMacros.h>

class MapTests : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( MapTests );
      CPPUNIT_TEST( testMapAddRemoveProxies );
      CPPUNIT_TEST( testMapProxySearch );
      CPPUNIT_TEST( testMapLibraryHandling );
      CPPUNIT_TEST( testLoadMapIntoScene );
      CPPUNIT_TEST( testMapSaveAndLoad );
      CPPUNIT_TEST( testLibraryMethods );
      CPPUNIT_TEST( testWildCard );
      CPPUNIT_TEST( testEnvironmentMapLoading );
      CPPUNIT_TEST( testLoadEnvironmentMapIntoScene );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testMapAddRemoveProxies();
    void testMapProxySearch();
    void testMapLibraryHandling();
    void testMapSaveAndLoad();
    void testLoadMapIntoScene();
    void testLibraryMethods();
    void testEnvironmentMapLoading();
    void testLoadEnvironmentMapIntoScene();
    void testWildCard();
private:
    static char* mExampleLibraryName;

    void createActors(dtDAL::Map& map);
    dtUtil::Log* logger;
    dtDAL::ActorProperty* getActorProperty(dtDAL::Map& map,
        const std::string& propName, dtDAL::DataType& type, unsigned which = 0);
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( MapTests );

const std::string DATA_DIR = dtCore::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR + "data";
const std::string TESTS_DIR = dtCore::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR + "tests";
const std::string MAPPROJECTCONTEXT = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtDAL" + dtUtil::FileUtils::PATH_SEPARATOR + "WorkingMapProject";
const std::string PROJECTCONTEXT = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtDAL" + dtUtil::FileUtils::PATH_SEPARATOR + "WorkingProject";

#if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
char* MapTests::mExampleLibraryName="testActorLibraryd";
#else
char* MapTests::mExampleLibraryName="testActorLibrary";
#endif


void MapTests::setUp()
{
    try
    {
        dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
        std::string logName("mapTest");

//        logger = &dtUtil::Log::GetInstance();
//        logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
//        logger = &dtUtil::Log::GetInstance("fileutils.cpp");
//        logger->SetLogLevel(dtUtil::Log::LOG_ERROR);
//        logger = &dtUtil::Log::GetInstance("mapxml.cpp");
//        logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);

        logger = &dtUtil::Log::GetInstance(logName);
//
//        logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
//        logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Log initialized.\n");
        dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
        std::string currentDir = fileUtils.CurrentDirectory();
        std::string projectDir("dtDAL");
        if (currentDir.substr(currentDir.size() - projectDir.size()) != projectDir)
            fileUtils.PushDirectory(projectDir);

        std::string rbodyToDelete("WorkingMapProject/Characters/marine/marine.rbody");

        if (fileUtils.DirExists(rbodyToDelete))
            fileUtils.DirDelete(rbodyToDelete, true);
        else if (fileUtils.FileExists(rbodyToDelete))
            fileUtils.FileDelete(rbodyToDelete);


        dtDAL::Project::GetInstance().SetContext("WorkingMapProject");
        //copy the vector because the act of deleting a map will reload the map names list.
        const std::set<std::string> v = dtDAL::Project::GetInstance().GetMapNames();

        for (std::set<std::string>::const_iterator i = v.begin(); i != v.end(); i++)
        {
            dtDAL::Project::GetInstance().DeleteMap(*i, true);
        }

    }
    catch (const dtUtil::Exception& e)
    {
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
}


void MapTests::tearDown()
{
    dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
    try
    {
        dtDAL::Project::GetInstance().SetContext("WorkingMapProject");
        //copy the vector because the act of deleting a map will reload the map names list.
        const std::set<std::string> v = dtDAL::Project::GetInstance().GetMapNames();

        //for (std::set<std::string>::const_iterator i = v.begin(); i != v.end(); i++) {
        //    dtDAL::Project::GetInstance().deleteMap(*i);
        //}

        std::string rbodyToDelete("WorkingMapProject/Characters/marine/marine.rbody");

        if (fileUtils.DirExists(rbodyToDelete))
            fileUtils.DirDelete(rbodyToDelete, true);
        else if (fileUtils.FileExists(rbodyToDelete))
           fileUtils.FileDelete(rbodyToDelete);

        if (dtDAL::LibraryManager::GetInstance().GetRegistry(mExampleLibraryName) != NULL)
        {
           dtDAL::LibraryManager::GetInstance().UnloadActorRegistry(mExampleLibraryName);
        }

    }
    catch (const dtUtil::Exception& e)
    {
        fileUtils.PopDirectory();
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
    fileUtils.PopDirectory();

}


void MapTests::createActors(dtDAL::Map& map)
{
   dtDAL::LibraryManager& libMgr = dtDAL::LibraryManager::GetInstance();
   std::vector<dtCore::RefPtr<dtDAL::ActorType> > actors;
   std::vector<dtDAL::ActorProperty *> props;
   
   libMgr.GetActorTypes(actors);
   
   int skippedActors = 0;
   int nameCounter = 0;
   char nameAsString[21];
   
   logger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, "Adding one of each proxy type to the map:");

   //only create half the actors :-)
   for (unsigned int i=0; i < actors.size(); ++i)
   {
      // In order to keep the tests fasts, we skip the nasty slow ones.
      if (actors[i]->GetName() == "Cloud Plane" || actors[i]->GetName() == "Environment" || 
          actors[i]->GetName() == "Test Environment Actor") 
      {
         skippedActors ++;
         continue; // go to next actor
      }
      
      dtCore::RefPtr<dtDAL::ActorProxy> proxy;
      // Test timing Stuff
      dtCore::Timer testClock;
      dtCore::Timer_t testClockStart = testClock.Tick();
      
      logger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                         "Creating actor proxy %s with category %s.", actors[i]->GetName().c_str(), actors[i]->GetCategory().c_str());
      
      proxy = libMgr.CreateActorProxy(*actors[i]);
      snprintf(nameAsString, 21, "%d", nameCounter);
      proxy->SetName(std::string(nameAsString));
      nameCounter++;
      
      logger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                         "Set proxy name to: %s", proxy->GetName().c_str());
      
      proxy->GetPropertyList(props);
      for (unsigned int j=0; j<props.size(); j++)
      {
         logger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                            "Property: Name: %s, Type: %s",
                            props[j]->GetName().c_str(), props[j]->GetPropertyType().GetName().c_str());
      }
      
      // Temporary timing for map tests...  when we get a slow one, we should exclude it from
      // the tests so that tests don't slow down universally.
      dtCore::Timer_t testClockDone = testClock.Tick();
      double timeToCreate = testClock.DeltaSec(testClockStart, testClockDone);
      if (timeToCreate > 0.5) // more than .5 seconds is too long for 1 object in a test
         logger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
                            "SLOW ACTOR CREATED IN TESTS - Type: %s, time[%f].  To ignore this slow actor in tests, modify these files (maptests.cpp, messagetests.cpp, proxytests.cpp, and gamemanagertests.cpp)", 
                            actors[i]->GetName().c_str(), timeToCreate);
      
      map.AddProxy(*proxy);
      
      CPPUNIT_ASSERT_MESSAGE("Proxy list has the wrong size.",
                             map.GetAllProxies().size() == (i + 1 - skippedActors));
      CPPUNIT_ASSERT_MESSAGE("Last proxy in the list should equal the new proxy.",
                             map.GetAllProxies().find(proxy->GetId())->second == proxy.get());
   }
   CPPUNIT_ASSERT_MESSAGE("The actors that should have been skipped when creating the actors for the test were not found.  This code is out of date.", 
      skippedActors >= 2);
}

void MapTests::testMapAddRemoveProxies()
{
    try
    {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        project.SetContext("WorkingMapProject");

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
        for (unsigned int x = 0;  x < maxId; x++)
        {
            CPPUNIT_ASSERT_MESSAGE("Unable to remove item 0",
                map.RemoveProxy(*map.GetAllProxies().begin()->second));

            CPPUNIT_ASSERT_MESSAGE("Proxy list has the wrong size.",
                map.GetAllProxies().size() == (unsigned int)(maxId - (x + 1)));
        }
        map.RebuildProxyActorClassSet();
        CPPUNIT_ASSERT_MESSAGE("The set of actor classes should be empty.",
            map.GetProxyActorClasses().empty());

        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > proxies;
        map.GetAllProxies(proxies);

        map.AddLibrary(mExampleLibraryName, "1.0");
        dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);

        dtCore::RefPtr<dtDAL::ActorType> exampleType = dtDAL::LibraryManager::GetInstance().FindActorType("dtcore.examples", "Test All Properties");
        CPPUNIT_ASSERT_MESSAGE("The example type is NULL", exampleType.valid());

        dtCore::RefPtr<dtDAL::ActorProxy> proxy1 = dtDAL::LibraryManager::GetInstance().CreateActorProxy(*exampleType);
        CPPUNIT_ASSERT_MESSAGE("proxy1 is NULL", proxy1.valid());

        dtCore::RefPtr<dtDAL::ActorProxy> proxy2 = dtDAL::LibraryManager::GetInstance().CreateActorProxy(*exampleType);
        CPPUNIT_ASSERT_MESSAGE("proxy2 is NULL", proxy2.valid());

        dtCore::RefPtr<dtDAL::ActorProxy> proxy3 = dtDAL::LibraryManager::GetInstance().CreateActorProxy(*exampleType);
        CPPUNIT_ASSERT_MESSAGE("proxy3 is NULL", proxy3.valid());

        dtCore::RefPtr<dtDAL::ActorProxy> proxy4 = dtDAL::LibraryManager::GetInstance().CreateActorProxy(*exampleType);
        CPPUNIT_ASSERT_MESSAGE("proxy4 is NULL", proxy4.valid());

        map.AddProxy(*proxy1.get());
        map.AddProxy(*proxy2.get());
        map.AddProxy(*proxy3.get());
        map.AddProxy(*proxy4.get());

        static_cast<dtDAL::ActorActorProperty*>(proxy1->GetProperty("Test_Actor"))->SetValue(proxy2.get());
        static_cast<dtDAL::ActorActorProperty*>(proxy2->GetProperty("Test_Actor"))->SetValue(proxy4.get());
        static_cast<dtDAL::ActorActorProperty*>(proxy3->GetProperty("Test_Actor"))->SetValue(proxy4.get());

        map.RemoveProxy(*proxy4.get());

        CPPUNIT_ASSERT_MESSAGE("Proxy 1 is not linked to proxy2", static_cast<dtDAL::ActorActorProperty*>(proxy1->GetProperty("Test_Actor"))->GetValue() == proxy2.get());
        CPPUNIT_ASSERT_MESSAGE("Proxy 2 is linked still", static_cast<dtDAL::ActorActorProperty*>(proxy2->GetProperty("Test_Actor"))->GetValue() == NULL);
        CPPUNIT_ASSERT_MESSAGE("Proxy 3 is linked still", static_cast<dtDAL::ActorActorProperty*>(proxy3->GetProperty("Test_Actor"))->GetValue() == NULL);
    }
    catch (const dtUtil::Exception& e)
    {
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
}

void MapTests::testMapProxySearch()
{
    try
    {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        project.SetContext("WorkingMapProject");

        dtDAL::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

        createActors(map);

        unsigned maxId = map.GetAllProxies().size();

        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > results;

        map.FindProxies(results, "", "dtcore", "");

        CPPUNIT_ASSERT_MESSAGE("dtcore category should include most of the base actor set and so should return most of the actors.",
            results.size() >= (map.GetAllProxies().size()/2));

        map.FindProxies(results, "", "dtc", "");

        CPPUNIT_ASSERT_MESSAGE("Only full category names should return results.", results.size() == 0);

        map.FindProxies(results, "", "dtcore.", "");
        CPPUNIT_ASSERT_MESSAGE("trailing dots should end with no results.", results.size() == 0);

        map.FindProxies(results, "", "", "","", dtDAL::Map::Placeable);
        for (std::vector<osg::ref_ptr<dtDAL::ActorProxy> >::iterator i = results.begin();
            i != results.end(); ++i)
        {
            CPPUNIT_ASSERT_MESSAGE(std::string("Proxy ") + (*i)->GetName()
                + " should not be in the results, it is not placeable",
                 (*i)->IsPlaceable());
        }

        map.FindProxies(results, "", "", "","", dtDAL::Map::NotPlaceable);
        for (std::vector<osg::ref_ptr<dtDAL::ActorProxy> >::iterator i = results.begin();
            i != results.end(); ++i)
        {
            CPPUNIT_ASSERT_MESSAGE(std::string("Proxy ") + (*i)->GetName()
                + " should not be in the results, it is placeable",
                !(*i)->IsPlaceable());
        }

        map.FindProxies(results, "", "", "","dtCore::Light");
        CPPUNIT_ASSERT_MESSAGE("There should be some lights in the results.", results.size() >= 3);


        for (std::vector<osg::ref_ptr<dtDAL::ActorProxy> >::iterator i = results.begin();
            i != results.end(); ++i)
        {
            CPPUNIT_ASSERT_MESSAGE("All results should be instances of dtCore::Light",
                (*i)->IsInstanceOf("dtCore::Light"));
        }

        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > proxies;

        map.GetAllProxies(proxies);

        for (unsigned x = 0;  x < proxies.size(); x++)
        {
            osg::ref_ptr<dtDAL::ActorProxy> proxyPTR = map.GetProxyById(proxies[x]->GetId());

            CPPUNIT_ASSERT_MESSAGE("Proxy should be found in the map by the project.", &map == dtDAL::Project::GetInstance().GetMapForActorProxy(*proxyPTR));

            CPPUNIT_ASSERT_MESSAGE((std::string("Proxy not found with id: ") + proxies[x]->GetId().ToString()).c_str()
                , proxyPTR != NULL);

            dtDAL::ActorProxy& proxy = *proxyPTR.get();

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
                *results.begin() == &proxy);

            map.FindProxies(results, proxyPTR->GetName());

            CPPUNIT_ASSERT_MESSAGE((std::string("Results should have exactly one proxy. Id:") +
                proxies[x]->GetId().ToString()).c_str() , results.size() == 1);
            CPPUNIT_ASSERT_MESSAGE((std::string("Correct proxy was not found with name search. Id:") +
                proxies[x]->GetId().ToString()).c_str() ,
                *results.begin() == &proxy);

            map.FindProxies(results, std::string(""), cat, typeName);

            CPPUNIT_ASSERT_MESSAGE((std::string("Results should have exactly proxy. Id:") +
                proxies[x]->GetId().ToString()).c_str() , results.size() == 1);
            CPPUNIT_ASSERT_MESSAGE((std::string("Correct proxy was not found with category and type search. Id:") +
                proxies[x]->GetId().ToString()).c_str() ,
                *results.begin() == &proxy);

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

dtDAL::ActorProperty* MapTests::getActorProperty(dtDAL::Map& map,
    const std::string& propName, dtDAL::DataType& type, unsigned which)
{
    for (std::map<dtCore::UniqueId, osg::ref_ptr<dtDAL::ActorProxy> >::const_iterator i = map.GetAllProxies().begin();
        i != map.GetAllProxies().end(); ++i)
    {

        dtDAL::ActorProxy* proxy = map.GetProxyById(i->first);

        CPPUNIT_ASSERT_MESSAGE("ERROR: Proxy is NULL", proxy!= NULL );

        if (propName != "")
        {
            dtDAL::ActorProperty* prop = proxy->GetProperty(propName);
            if (prop != NULL)
            {
                if (prop->GetPropertyType() == type && which-- == 0)
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
                if (prop->GetPropertyType() == type && which-- == 0)
                {
                    //std::cout << "Using prop " << prop->GetName() << " on actor with id " << proxy->GetId().ToString() << std::endl;
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


void MapTests::testLibraryMethods()
{
    try
    {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        project.SetContext("WorkingMapProject");

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

void MapTests::testMapLibraryHandling()
{
    try
    {
       dtDAL::Project& project = dtDAL::Project::GetInstance();

       project.SetContext("WorkingMapProject");

       std::string mapName("Neato Map");
       std::string mapFileName("neatomap");

       dtDAL::Map* map = &project.CreateMap(mapName, mapFileName);

       CPPUNIT_ASSERT_MESSAGE("neatomap.xml should be the name of the map file.", map->GetFileName() == "neatomap.xml");

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

       std::vector<osg::ref_ptr<dtDAL::ActorProxy> > proxies;
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

void MapTests::testMapSaveAndLoad()
{
    try
    {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        project.SetContext("WorkingMapProject");

        std::string mapName("Neato Map");
        std::string mapFileName("neatomap");

        dtDAL::Map* map = &project.CreateMap(mapName, mapFileName);

        CPPUNIT_ASSERT_MESSAGE("neatomap.xml should be the name of the map file.", map->GetFileName() == "neatomap.xml");

        project.SaveMapBackup(*map);

        //test both versions of the call.
        CPPUNIT_ASSERT_MESSAGE("Map was not modified.  There should be no backup saves.",
            !project.HasBackup(*map) && !project.HasBackup(mapName));

        map->SetDescription("Teague is league with a \"t\".");

        project.SaveMapBackup(*map);

        //test both versions of the call.
        CPPUNIT_ASSERT_MESSAGE("A backup was just saved.  The map should have backups.",
            project.HasBackup(*map) && project.HasBackup(mapName));

        project.ClearBackup(*map);

        //test both versions of the call.
        CPPUNIT_ASSERT_MESSAGE("Backups were cleared.  The map should have no backups.",
            !project.HasBackup(*map) && !project.HasBackup(mapName));

        project.SaveMapBackup(*map);

        //test both versions of the call.
        CPPUNIT_ASSERT_MESSAGE("A backup was just saved.  The map should have backups.",
            project.HasBackup(*map) && project.HasBackup(mapName));

        project.ClearBackup(mapName);

        //test both versions of the call.
        CPPUNIT_ASSERT_MESSAGE("Backups were cleared.  The map should have no backups.",
            !project.HasBackup(*map) && !project.HasBackup(mapName));

        map->AddLibrary(mExampleLibraryName, "1.0");
        dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);

#if !defined (WIN32) && !defined (_WIN32) && !defined (__WIN32__)
        dtDAL::ResourceDescriptor marineRD = project.AddResource("marine", DATA_DIR + "/marine/marine.rbody", "marine",
            dtDAL::DataType::CHARACTER);
#else
        LOG_ERROR("RBody unit tests fail on windows, not testing");
#endif

        dtDAL::ResourceDescriptor dirtRD = project.AddResource("dirt", 
           DATA_DIR + "/models/dirt.ive", "dirt", dtDAL::DataType::STATIC_MESH);

        createActors(*map);

        dtDAL::ActorProperty* ap;

        ap = getActorProperty(*map, "", dtDAL::DataType::STRING);
        ((dtDAL::StringActorProperty*)ap)->SetValue("2006-04-20T06:22:08");

        ap = getActorProperty(*map, "", dtDAL::DataType::BOOLEAN, 1);
        ((dtDAL::BooleanActorProperty*)ap)->SetValue(false);
        ap = getActorProperty(*map, "", dtDAL::DataType::BOOLEAN, 2);
        ((dtDAL::BooleanActorProperty*)ap)->SetValue(true);
        ap = getActorProperty(*map, "", dtDAL::DataType::FLOAT);
        ((dtDAL::FloatActorProperty*)ap)->SetValue(40.00f);
        ap = getActorProperty(*map, "", dtDAL::DataType::DOUBLE);
        ((dtDAL::DoubleActorProperty*)ap)->SetValue(39.70);


        //ActorActorProperty
        ap = getActorProperty(*map, "", dtDAL::DataType::ACTOR);
        dtDAL::ActorActorProperty* aap = static_cast<dtDAL::ActorActorProperty*>(ap);
        const std::string& className = aap->GetDesiredActorClass();
        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > toFill;
        //Do a search for the class name.
        map->FindProxies(toFill, "", "", "", className);

        CPPUNIT_ASSERT(toFill.size() > 0);
        //Set the value.
        aap->SetValue(toFill[0].get());
        //need to clear this because it could cause a segfault if this is not deleted before a
        //library is unloadad.
        toFill.clear();

        osg::Vec3 testVec3_1(33.5f, 12.25f, 49.125);
        osg::Vec3 testVec3_2(-34.75f, 96.03125f, 8.0f);
        osg::Vec3 testVec3_3(3.125, 90.25, 87.0625);

        ap = getActorProperty(*map, "Rotation", dtDAL::DataType::VEC3, 1);
        ((dtDAL::Vec3ActorProperty*)ap)->SetValue(testVec3_1);

        ap = getActorProperty(*map, "Translation", dtDAL::DataType::VEC3, 1);
        ((dtDAL::Vec3ActorProperty*)ap)->SetValue(testVec3_2);

        ap = getActorProperty(*map, "Scale", dtDAL::DataType::VEC3, 1);
        ((dtDAL::Vec3ActorProperty*)ap)->SetValue(testVec3_3);

        // Note - some properties, especially orientation ones, tend to mangle the 
        // values that we set so that we don't get back what we passed in.  To handle that
        // in the test, we get back whatever they think the value should be.
        ap = getActorProperty(*map, "", dtDAL::DataType::VEC3F);
        ((dtDAL::Vec3fActorProperty*)ap)->SetValue(testVec3_2);
        osg::Vec3 testVec3_2_actualValues = ((dtDAL::Vec3fActorProperty*)ap)->GetValue();

        ap = getActorProperty(*map, "", dtDAL::DataType::VEC3D);
        ((dtDAL::Vec3dActorProperty*)ap)->SetValue(testVec3_3);

        //osg::Vec2 testVec2_1(3.125, 90.25);
       // ap = GetActorProperty(*map, "Lat/Long", dtDAL::DataType::VEC2);
       // ((dtDAL::Vec2ActorProperty*)ap)->setValue(testVec2_1);


        osg::Vec4 testVec4_1(3.125, 90.25, 33.1, 73.64);
        osg::Vec4 testVec4_2(0.125, 33.25, 94.63, 11.211);
        osg::Vec4 testVec4_3(10.125f, 3.25f, 94.3f, 1.211f);

        osg::Vec4f testVec4f_1(0.125f, 33.25f, 94.63f, 11.211f);
        osg::Vec4d testVec4d_1(10.125, 3.25, 94.3, 1.211);

        ap = getActorProperty(*map, "", dtDAL::DataType::RGBACOLOR, 0);
        ((dtDAL::ColorRgbaActorProperty*)ap)->SetValue(testVec4_1);

        ap = getActorProperty(*map, "", dtDAL::DataType::RGBACOLOR, 1);
        ((dtDAL::ColorRgbaActorProperty*)ap)->SetValue(testVec4_2);

        ap = getActorProperty(*map, "", dtDAL::DataType::VEC4, 0);
        ((dtDAL::Vec4ActorProperty*)ap)->SetValue(testVec4_1);

        ap = getActorProperty(*map, "", dtDAL::DataType::VEC4F, 0);
        ((dtDAL::Vec4fActorProperty*)ap)->SetValue(testVec4f_1);

        ap = getActorProperty(*map, "", dtDAL::DataType::VEC4D, 0);
        ((dtDAL::Vec4dActorProperty*)ap)->SetValue(testVec4d_1);

        ap = getActorProperty(*map, "", dtDAL::DataType::INT);
        ((dtDAL::IntActorProperty*)ap)->SetValue(128);


        ap = getActorProperty(*map, "", dtDAL::DataType::ENUMERATION, 1);
        dtDAL::AbstractEnumActorProperty* eap = dynamic_cast<dtDAL::AbstractEnumActorProperty*>(ap);
        CPPUNIT_ASSERT(eap != NULL);

        if (eap->GetList().size() > 1)
           eap->SetEnumValue(const_cast<dtUtil::Enumeration&>(**(eap->GetList().begin()+1)));
        else
           logger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__, "Enum only has one value.");

#if !defined (WIN32) && !defined (_WIN32) && !defined (__WIN32__)
        ap = getActorProperty(*map, "model", dtDAL::DataType::CHARACTER);
        dtDAL::ResourceActorProperty& rap = static_cast<dtDAL::ResourceActorProperty&>(*ap);

		try {
        	rap.SetValue(&marineRD);

        	std::string marineStr = rap.GetStringValue();
        	rap.SetValue(NULL);
        	CPPUNIT_ASSERT(rap.GetValue() == NULL);
        	CPPUNIT_ASSERT(rap.SetStringValue(marineStr));
        	CPPUNIT_ASSERT(rap.GetValue() != NULL);
        	CPPUNIT_ASSERT(*rap.GetValue() == marineRD);
		} catch(const rbody::config_error& ex) {
            logger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Error loading character \"%s\": %s",
                marineRD.GetResourceIdentifier().c_str(), ex.what());
            CPPUNIT_FAIL("Error setting marine mesh.");
        }
#endif

        ap = getActorProperty(*map, "", dtDAL::DataType::STATIC_MESH);
        ((dtDAL::ResourceActorProperty*)ap)->SetValue(&dirtRD);

        unsigned numProxies = map->GetAllProxies().size();
        std::map<dtCore::UniqueId, std::string> names;
        for (std::map<dtCore::UniqueId, osg::ref_ptr<dtDAL::ActorProxy> >::const_iterator i = map->GetAllProxies().begin();
            i != map->GetAllProxies().end(); i++)
        {
            names.insert(std::make_pair(i->first, i->second->GetName()));
        }

        std::string newMapName("Weirdo Map");
        //set the name to make sure it can be changed.
        map->SetName(newMapName);

        CPPUNIT_ASSERT_MESSAGE(std::string("Map should have the new name - ") + newMapName,
            map->GetName() == newMapName);

        CPPUNIT_ASSERT_MESSAGE("Map should have the old saved name - \"Neato Map\".",
            map->GetSavedName() == "Neato Map");

        project.SaveMapBackup(*map);

        CPPUNIT_ASSERT_MESSAGE("A backup was just saved.  The map should have backups.",
            project.HasBackup(*map) && project.HasBackup(mapName));

        project.SaveMap(*map);

        //test both versions of the call.
        CPPUNIT_ASSERT_MESSAGE("Map was saved.  The map should have no backups.",
            !project.HasBackup(*map) && !project.HasBackup(newMapName));

        CPPUNIT_ASSERT_MESSAGE(std::string("Map should have the new saved name - ") + newMapName,
            map->GetSavedName() == newMapName);

        project.CloseMap(*map, true);

        map = &project.GetMap(newMapName);


        CPPUNIT_ASSERT_MESSAGE("Map should not have any loading errors.", !map->HasLoadingErrors());

        std::ostringstream ss;

        ss << "map has the wrong number of elements. It has "
            << map->GetAllProxies().size() << ". It should have been " << numProxies << ".";

        CPPUNIT_ASSERT_MESSAGE(ss.str(),
            map->GetAllProxies().size() == numProxies);

        for (std::map<dtCore::UniqueId, std::string>::const_iterator j = names.begin();
            j != names.end(); ++j)
        {
            dtDAL::ActorProxy* ap = map->GetProxyById(j->first);
            CPPUNIT_ASSERT(ap != NULL);
            CPPUNIT_ASSERT_MESSAGE(j->first.ToString() + " name should be " + j ->second, j->second == ap->GetName());
        }

        ap = getActorProperty(*map, "", dtDAL::DataType::STRING, 0);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 2006-04-20T06:22:08",
            ((dtDAL::StringActorProperty*)ap)->GetValue() == "2006-04-20T06:22:08");

        ap = getActorProperty(*map, "", dtDAL::DataType::BOOLEAN, 1);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be false",
            !((dtDAL::BooleanActorProperty*)ap)->GetValue());
        ap = getActorProperty(*map, "", dtDAL::DataType::BOOLEAN, 2);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be true",
            ((dtDAL::BooleanActorProperty*)ap)->GetValue());
        ap = getActorProperty(*map, "", dtDAL::DataType::FLOAT);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 40.00.",
            fabs(((dtDAL::FloatActorProperty*)ap)->GetValue() - 40.00f) < 0.0001);

        ap = getActorProperty(*map, "", dtDAL::DataType::DOUBLE);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 39.70.",
            fabs(((dtDAL::DoubleActorProperty*)ap)->GetValue() - 39.70) < 0.0001);

        ap = getActorProperty(*map, "Rotation", dtDAL::DataType::VEC3,1);

        if (logger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
        {
            osg::Vec3f val = ((dtDAL::Vec3ActorProperty*)ap)->GetValue();
            logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                "Vec3f Property values: %f, %f, %f", val[0], val[1], val[2]);
        }

        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 33.5f, 12.25f, 49.125f",
            osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[0], testVec3_1[0], 1e-2f)
            && osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[1], testVec3_1[1], 1e-2f )
            && osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[2], testVec3_1[2], 1e-2f )
            );

        ap = getActorProperty(*map, "Translation", dtDAL::DataType::VEC3, 1);

        if (logger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
        {
            osg::Vec3 val = ((dtDAL::Vec3ActorProperty*)ap)->GetValue();
            logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                "Vec3f Property values: %f, %f, %f", val[0], val[1], val[2]);
        }

        ss.str("");
        ss << ((dtDAL::Vec3ActorProperty*)ap)->GetValue()[0] << " " ;
        ss << ((dtDAL::Vec3ActorProperty*)ap)->GetValue()[1] << " " ;
        ss << ((dtDAL::Vec3ActorProperty*)ap)->GetValue()[2];
                CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be -34.75f, 96.03125f, 8.0f the value is " + ss.str(),
            osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[0], testVec3_2[0], 1e-2f)
            && osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[1], testVec3_2[1], 1e-2f)
            && osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[2], testVec3_2[2], 1e-2f)
            );

        // VEC3 
        ap = getActorProperty(*map, "", dtDAL::DataType::VEC3F, 0);
        if (logger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
        {
            osg::Vec3 val = ((dtDAL::Vec3fActorProperty*)ap)->GetValue();
            logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                "Vec3f Property values: %f, %f, %f", val[0], val[1], val[2]);
            logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Property: name [%s], label [%s], desc [%s], readonly[%d], tostring[%s], precision[%d]", 
               ap->GetName().c_str(), ap->GetLabel().c_str(), ap->GetDescription().c_str(), ap->IsReadOnly(),
               ap->ToString().c_str(), ap->GetNumberPrecision());
        }
        osg::Vec3 val3 = ((dtDAL::Vec3fActorProperty*)ap)->GetValue();
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be -34.75f, 96.03125f, 8.0f (unless mangled)",
            osg::equivalent(val3[0], testVec3_2_actualValues[0], 1e-2f)
            && osg::equivalent(val3[1], testVec3_2_actualValues[1], 1e-2f)
            && osg::equivalent(val3[2], testVec3_2_actualValues[2], 1e-2f)
            );

 
        ap = getActorProperty(*map, "", dtDAL::DataType::VEC3D, 0);
        if (logger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
        {
            osg::Vec3 val = ((dtDAL::Vec3dActorProperty*)ap)->GetValue();
            logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                "Vec3f Property values: %f, %f, %f", val[0], val[1], val[2]);
        }
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 3.125, 90.25, 87.0625",
            osg::equivalent((double)((dtDAL::Vec3dActorProperty*)ap)->GetValue()[0], (double)testVec3_3[0], (double)1e-2)
            && osg::equivalent((double)((dtDAL::Vec3dActorProperty*)ap)->GetValue()[1], (double)testVec3_3[1], (double)1e-2)
            && osg::equivalent((double)((dtDAL::Vec3dActorProperty*)ap)->GetValue()[2], (double)testVec3_3[2], (double)1e-2)
            );

        ap = getActorProperty(*map, "Scale", dtDAL::DataType::VEC3, 1);

        if (logger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
        {
            osg::Vec3f val = ((dtDAL::Vec3ActorProperty*)ap)->GetValue();
            logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                "Vec3f Property values: %f, %f, %f", val[0], val[1], val[2]);
        }

        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 3.125, 90.25, 87.0625",
             osg::equivalent((double)((dtDAL::Vec3ActorProperty*)ap)->GetValue()[0], (double)testVec3_3[0], (double)1e-2)
             && osg::equivalent((double)((dtDAL::Vec3ActorProperty*)ap)->GetValue()[1], (double)testVec3_3[1], (double)1e-2)
             && osg::equivalent((double)((dtDAL::Vec3ActorProperty*)ap)->GetValue()[2], (double)testVec3_3[2], (double)1e-2)
             );

        //ap = GetActorProperty(*map, "Lat/Long", dtDAL::DataType::VEC2);
        //CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 3.125, 90.25",
        //     osg::equivalent(((dtDAL::Vec2ActorProperty*)ap)->GetValue()[0], testVec2_1[0], 1e-2f)
        //     && osg::equivalent(((dtDAL::Vec2ActorProperty*)ap)->GetValue()[1], testVec2_1[1], 1e-2f)
        //     );


        ap = getActorProperty(*map, "", dtDAL::DataType::RGBACOLOR, 0);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 3.125, 90.25, 33.1, 73.64",
            osg::equivalent((double)((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[0], (double)testVec4_1[0], (double)1e-2f)
            && osg::equivalent((double)((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[1], (double)testVec4_1[1], (double)1e-2f)
            && osg::equivalent((double)((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[2], (double)testVec4_1[2], (double)1e-2f)
            && osg::equivalent((double)((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[3], (double)testVec4_1[3], (double)1e-2f)
            );

        ap = getActorProperty(*map, "", dtDAL::DataType::RGBACOLOR, 1);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 0.125, 33.25, 94.63, 11.211",
            osg::equivalent(((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[0], testVec4_2[0], 1e-2f)
            && osg::equivalent(((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[1], testVec4_2[1], 1e-2f)
            && osg::equivalent(((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[2], testVec4_2[2], 1e-2f)
            && osg::equivalent(((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[3], testVec4_2[3], 1e-2f)
            );

        ap = getActorProperty(*map, "", dtDAL::DataType::VEC4, 0);

        osg::Vec4 v4 = ((dtDAL::Vec4ActorProperty*)ap)->GetValue();

        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 3.125, 90.25, 33.1, 73.64",
           osg::equivalent(((dtDAL::Vec4ActorProperty*)ap)->GetValue()[0], testVec4_1[0], 1e-2f)
            && osg::equivalent(((dtDAL::Vec4ActorProperty*)ap)->GetValue()[1], testVec4_1[1], 1e-2f)
            && osg::equivalent(((dtDAL::Vec4ActorProperty*)ap)->GetValue()[2], testVec4_1[2], 1e-2f)
            && osg::equivalent(((dtDAL::Vec4ActorProperty*)ap)->GetValue()[3], testVec4_1[3], 1e-2f)
            );

        ap = getActorProperty(*map, "", dtDAL::DataType::VEC4F, 0);

        osg::Vec4f v4f = ((dtDAL::Vec4fActorProperty*)ap)->GetValue();

        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 10.125f, 3.25f, 94.3f, 1.211f",
            osg::equivalent(((dtDAL::Vec4fActorProperty*)ap)->GetValue()[0], testVec4f_1[0], 1e-2f)
            && osg::equivalent(((dtDAL::Vec4fActorProperty*)ap)->GetValue()[1], testVec4f_1[1], 1e-2f)
            && osg::equivalent(((dtDAL::Vec4fActorProperty*)ap)->GetValue()[2], testVec4f_1[2], 1e-2f)
            && osg::equivalent(((dtDAL::Vec4fActorProperty*)ap)->GetValue()[3], testVec4f_1[3], 1e-2f)
            );

        ap = getActorProperty(*map, "", dtDAL::DataType::VEC4D, 0);

        osg::Vec4d v4d = ((dtDAL::Vec4dActorProperty*)ap)->GetValue();

        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 0.125, 33.25, 94.63, 11.211",
            osg::equivalent(((dtDAL::Vec4dActorProperty*)ap)->GetValue()[0], testVec4d_1[0], 1e-2)
            && osg::equivalent(((dtDAL::Vec4dActorProperty*)ap)->GetValue()[1], testVec4d_1[1], 1e-2)
            && osg::equivalent(((dtDAL::Vec4dActorProperty*)ap)->GetValue()[2], testVec4d_1[2], 1e-2)
            && osg::equivalent(((dtDAL::Vec4dActorProperty*)ap)->GetValue()[3], testVec4d_1[3], 1e-2)
            );

        ap = getActorProperty(*map, "", dtDAL::DataType::INT);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 128",
             ((dtDAL::IntActorProperty*)ap)->GetValue() == 128);

        ap = getActorProperty(*map, "", dtDAL::DataType::ENUMERATION,1);
        eap = dynamic_cast<dtDAL::AbstractEnumActorProperty*>(ap);
        ap = eap->AsActorProperty();
        CPPUNIT_ASSERT_MESSAGE(std::string("Value should be ") + (*(eap->GetList().begin()+1))->GetName()
           + " but it is " + eap->GetEnumValue().GetName(),
           eap->GetEnumValue() == **(eap->GetList().begin()+1));

#if !defined (WIN32) && !defined (_WIN32) && !defined (__WIN32__)
        ap = getActorProperty(*map, "model", dtDAL::DataType::CHARACTER);
        dtDAL::ResourceDescriptor* rdVal = ((dtDAL::ResourceActorProperty*)ap)->GetValue();
        //testRD is declared in the setup section prior to the save and load.
        if (rdVal == NULL)
            CPPUNIT_FAIL("Character ResourceDescriptor should not be NULL.");
        CPPUNIT_ASSERT_MESSAGE("The resource Descriptor does not match.  Value is :" + rdVal->GetResourceIdentifier(),
            rdVal != NULL && *rdVal == marineRD);
#endif

        ap = getActorProperty(*map, "", dtDAL::DataType::STATIC_MESH);
        dtDAL::ResourceDescriptor* rdMeshVal = ((dtDAL::ResourceActorProperty*)ap)->GetValue();
        //testRD is declared in the setup section prior to the save and load.
        if (rdMeshVal == NULL)
            CPPUNIT_FAIL("Static Mesh ResourceDescriptor should not be NULL.");
        CPPUNIT_ASSERT_MESSAGE("The resource Descriptor does not match.  Value is :" + std::string(""), //rdVal->GetResourceIdentifier(),
            rdMeshVal != NULL && *rdMeshVal == dirtRD);

        const int value1 = 5, value2 = 27;
        ap = getActorProperty(*map, "Test_Read_Only_Int", dtDAL::DataType::INT);

        {
            CPPUNIT_ASSERT_MESSAGE("Test_Read_Only_Int should be in the map", ap != NULL);
            dtDAL::IntActorProperty *p = dynamic_cast<dtDAL::IntActorProperty*> (ap);
            CPPUNIT_ASSERT_MESSAGE("Test_Read_Only_Int is an IntActorProperty, dynamic_cast should have succeeded", p != NULL);
            p->SetReadOnly(true);
            p->SetValue(value2);
            CPPUNIT_ASSERT_MESSAGE("Property is read only, value should not have been set", p->GetValue() == value1);
            p->SetReadOnly(false);
            p->SetValue(value2);
            CPPUNIT_ASSERT_MESSAGE("Property is not read only, value should have been set", p->GetValue() == value2);
        }

        project.SaveMap(*map);
        project.CloseMap(*map);

        map = &project.GetMap(newMapName);
        ap = getActorProperty(*map, "Test_Read_Only_Int", dtDAL::DataType::INT);

        {
            CPPUNIT_ASSERT_MESSAGE("Test_Read_Only_Int should be in the map", ap != NULL);
            dtDAL::IntActorProperty *p = dynamic_cast<dtDAL::IntActorProperty*> (ap);
            CPPUNIT_ASSERT_MESSAGE("Test_Read_Only_Int is an IntActorProperty, dynamic_cast should have succeeded", p != NULL);
            CPPUNIT_ASSERT_MESSAGE("Test_Read_Only_Int should be readonly.", p->IsReadOnly());
            std::stringstream ss;
            ss << "Readonly int value should be the original readonly value " << value1 << " but it is " << p->GetValue();
            CPPUNIT_ASSERT_MESSAGE(ss.str(), p->GetValue() == value1);
        }

        ap = getActorProperty(*map, "", dtDAL::DataType::ACTOR);

        //aap is used above in the method
        aap = static_cast<dtDAL::ActorActorProperty*> (ap);

        const std::string& id = aap->GetStringValue();
        dtDAL::ActorProxy* p  = aap->GetValue();

        CPPUNIT_ASSERT_MESSAGE("The proxy should not be NULL", p != NULL);

        aap->SetValue(NULL);

        CPPUNIT_ASSERT_MESSAGE("GetValue should return NULL", aap->GetValue() == NULL);

        aap->SetStringValue(id);

        CPPUNIT_ASSERT_MESSAGE("The value should not be equal to the proxy", aap->GetValue() == p);

        std::string newAuthor("Dr. Eddie");

        map->SetAuthor(newAuthor);

        project.SaveMapBackup(*map);

        CPPUNIT_ASSERT_MESSAGE("neatomap.xml should be the name of the map file.", map->GetFileName() == "neatomap.xml");

        map = &project.OpenMapBackup(newMapName);

        CPPUNIT_ASSERT_MESSAGE("neatomap.xml should be the name of the map file.", map->GetFileName() == "neatomap.xml");
        CPPUNIT_ASSERT_MESSAGE(newAuthor + " should be the author of the map.", map->GetAuthor() == newAuthor);

        CPPUNIT_ASSERT_MESSAGE("Loading a backup map should load as modified.", map->IsModified());

        map->SetDescription("test 2");

        //test both versions of the call.
        CPPUNIT_ASSERT_MESSAGE("Map was loaded as a backup, so it should have backups.",
            project.HasBackup(*map) && project.HasBackup(newMapName));

        project.SaveMapBackup(*map);

        //test both versions of the call.
        CPPUNIT_ASSERT_MESSAGE("A backup was just saved.  The map should have a backup.",
            project.HasBackup(*map) && project.HasBackup(newMapName));

        try
        {
            project.SaveMapAs(*map, newMapName, mapFileName);
            CPPUNIT_FAIL("Calling SaveAs on a map with the same name and filename should fail.");
        }
        catch (const dtUtil::Exception&)
        {
            //correct
        }

        try
        {
            project.SaveMapAs(*map, mapName, mapFileName);
            CPPUNIT_FAIL("Calling SaveAs on a map with the same filename should fail.");
        }
        catch (const dtUtil::Exception&)
        {
            //correct
        }

        try
        {
            project.SaveMapAs(*map, newMapName, "oo");
            CPPUNIT_FAIL("Calling SaveAs on a map with the same name should fail.");
        }
        catch (const dtUtil::Exception&)
        {
            //correct
        }

        project.SaveMapAs(*map, mapName, "oo");

        //test both versions of the call.
        CPPUNIT_ASSERT_MESSAGE("Map was just saved AS.  The map should have no backups.",
            !project.HasBackup(*map) && !project.HasBackup(mapName));

        //test both versions of the call.
        CPPUNIT_ASSERT_MESSAGE("Map was just saved AS.  The old map should have no backups.",
            !project.HasBackup(newMapName));

        CPPUNIT_ASSERT_MESSAGE("Map file name should be oo.",
            map->GetFileName() == std::string("oo")+ dtDAL::Map::MAP_FILE_EXTENSION);
        CPPUNIT_ASSERT_MESSAGE(mapName + " should be the new map name.",
            map->GetName() == mapName && map->GetSavedName() == mapName);

        std::string newMapFilePath = project.GetContext() + dtUtil::FileUtils::PATH_SEPARATOR + "maps"
            + dtUtil::FileUtils::PATH_SEPARATOR + "oo" + dtDAL::Map::MAP_FILE_EXTENSION;

        CPPUNIT_ASSERT_MESSAGE(std::string("The new map file should exist: ") + newMapFilePath,
            dtUtil::FileUtils::GetInstance().FileExists(newMapFilePath));

        //set the map name before deleting it to make sure
        //I can delete with a changed name.
        map->SetName("some new name");

        project.DeleteMap(*map, true);
    }
    catch (const dtUtil::Exception& e)
    {
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
//    catch (const std::exception& ex) {
//        LOGN_ERROR("maptests.cpp", ex.what());
//        throw ex;
//    }
}

void MapTests::testWildCard()
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


void MapTests::testLoadMapIntoScene()
{
    try
    {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        project.SetContext("WorkingMapProject");

        dtDAL::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

        createActors(map);

        std::set<dtCore::UniqueId> ids;

        //add all the names of the actors that should be in the scene to set so we can track them.
        for (std::map<dtCore::UniqueId, osg::ref_ptr<dtDAL::ActorProxy> >::const_iterator i = map.GetAllProxies().begin();
            i != map.GetAllProxies().end(); ++i)
        {
            const dtDAL::ActorProxy::RenderMode &renderMode = const_cast<dtDAL::ActorProxy&>(*i->second).GetRenderMode();

            if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR ||
                renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
                ids.insert(i->first);
        }

        osg::ref_ptr<dtABC::Application> app(new dtABC::Application("config.xml"));
        dtCore::Scene& scene = *app->GetScene();
        //actually load the map into the scene.
        //TODO, test with the last param as false to make sure ALL proxies end up in the scene.
        project.LoadMapIntoScene(map, scene, true);

        //spin through the scene removing each actor found from the set.
        for (unsigned x = 0; x < (unsigned)scene.GetNumberOfAddedDrawable(); x++)
        {
            dtCore::DeltaDrawable* dd = scene.GetDrawable(x);
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

void MapTests::testEnvironmentMapLoading()
{
   try
   {
      dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);
      dtDAL::Project &project = dtDAL::Project::GetInstance();
      dtDAL::Map &map = project.CreateMap("TestEnvironmentMap", "TestEnvironmentMap");

      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > container;
      CPPUNIT_ASSERT(container.empty());

      const unsigned int numProxies = 4;
      for(unsigned int i = 0; i < numProxies; i++)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> p =
            dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtcore.examples", "Test All Properties");
         CPPUNIT_ASSERT(p.valid());
         container.push_back(p);
      }

      CPPUNIT_ASSERT(container.size() == numProxies);

      for(unsigned int i = 0; i < container.size(); i++)
         map.AddProxy(*container[i]);

      std::vector<osg::ref_ptr<dtDAL::ActorProxy> > mapProxies;
      map.GetAllProxies(mapProxies);
      CPPUNIT_ASSERT_MESSAGE("The map should have the correct number of proxies", mapProxies.size() == numProxies);

      dtCore::RefPtr<dtDAL::ActorProxy> envProxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy("Test Environment Actor", "Test Environment Actor");
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

void MapTests::testLoadEnvironmentMapIntoScene()
{
   dtDAL::LibraryManager::GetInstance().LoadActorRegistry(mExampleLibraryName);
   dtDAL::Project &project = dtDAL::Project::GetInstance();
   dtDAL::Map &map = project.CreateMap("TestEnvironmentMap", "TestEnvironmentMap");
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene;

   scene->RemoveAllDrawables();
   CPPUNIT_ASSERT(scene->GetNumberOfAddedDrawable() == 0);

   std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > container;
   CPPUNIT_ASSERT(container.empty());

   const unsigned int numProxies = 4;
   for(unsigned int i = 0; i < numProxies; i++)
   {
      dtCore::RefPtr<dtDAL::ActorProxy> p =
         dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtcore.examples", "Test All Properties");
      CPPUNIT_ASSERT(p.valid());
      container.push_back(p);
   }

   CPPUNIT_ASSERT(container.size() == numProxies);

   for(unsigned int i = 0; i < container.size(); i++)
      map.AddProxy(*container[i]);

   std::vector<osg::ref_ptr<dtDAL::ActorProxy> > mapProxies;
   map.GetAllProxies(mapProxies);
   CPPUNIT_ASSERT_MESSAGE("The map should have the correct number of proxies", mapProxies.size() == numProxies);

   dtCore::RefPtr<dtDAL::ActorProxy> envProxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy("Test Environment Actor", "Test Environment Actor");
   CPPUNIT_ASSERT(envProxy.valid());

   project.LoadMapIntoScene(map, *scene);
   unsigned int numDrawables = scene->GetNumberOfAddedDrawable();
   CPPUNIT_ASSERT_MESSAGE("The number of drawables should equal the number of proxies", numDrawables == numProxies);
   scene->RemoveAllDrawables();
   CPPUNIT_ASSERT(scene->GetNumberOfAddedDrawable() == 0);

   dtDAL::EnvironmentActor *tea = dynamic_cast<dtDAL::EnvironmentActor*>(envProxy->GetActor());
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
