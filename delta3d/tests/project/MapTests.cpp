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
#include <dtABC/application.h>

#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/log.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/exception.h>
#include <dtDAL/fileutils.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxy.h>


#include "MapTests.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( MapTests );


void MapTests::setUp() {
    try {
        dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
        std::string logName("mapTest");

        logger = &dtDAL::Log::GetInstance();
        logger->SetLogLevel(dtDAL::Log::LOG_DEBUG);
        logger = &dtDAL::Log::GetInstance("fileutils.cpp");
        logger->SetLogLevel(dtDAL::Log::LOG_ERROR);
        logger = &dtDAL::Log::GetInstance("mapxml.cpp");
        logger->SetLogLevel(dtDAL::Log::LOG_DEBUG);

        logger = &dtDAL::Log::GetInstance(logName);

        logger->SetLogLevel(dtDAL::Log::LOG_DEBUG);
        logger->LogMessage(dtDAL::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Log initialized.\n");
        dtDAL::FileUtils& fileUtils = dtDAL::FileUtils::GetInstance();
        fileUtils.PushDirectory(std::string("project"));

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
    catch (const dtDAL::Exception& e)
    {
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
}


void MapTests::tearDown()
{
    dtDAL::FileUtils& fileUtils = dtDAL::FileUtils::GetInstance();
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

    } catch (const dtDAL::Exception& e) {
        fileUtils.PopDirectory();
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
    fileUtils.PopDirectory();

}


void MapTests::createActors(dtDAL::Map& map) {
  dtDAL::LibraryManager& libMgr = dtDAL::LibraryManager::GetInstance();
  std::vector<osg::ref_ptr<dtDAL::ActorType> > actors;
  std::vector<dtDAL::ActorProperty *> props;

  libMgr.GetActorTypes(actors);

  int nameCounter = 0;
  char nameAsString[21];

  logger->LogMessage(dtDAL::Log::LOG_INFO, __FUNCTION__, __LINE__, "Adding one of each proxy type to the map:");

  for (unsigned int i=0; i< actors.size(); i++) 
  {
    osg::ref_ptr<dtDAL::ActorProxy> proxy;

    logger->LogMessage(dtDAL::Log::LOG_INFO, __FUNCTION__, __LINE__,
      "Creating actor proxy %s with category %s.", actors[i]->GetName().c_str(), actors[i]->GetCategory().c_str());

    proxy = libMgr.CreateActorProxy(*actors[i]);
    snprintf(nameAsString, 21, "%d", nameCounter);
    proxy->SetName(std::string(nameAsString));
    nameCounter++;

    logger->LogMessage(dtDAL::Log::LOG_INFO, __FUNCTION__, __LINE__,
      "Set proxy name to: %s", proxy->GetName().c_str());


    proxy->GetPropertyList(props);
    for (unsigned int j=0; j<props.size(); j++) {
      logger->LogMessage(dtDAL::Log::LOG_INFO, __FUNCTION__, __LINE__,
        "Property: Name: %s, Type: %s",
            props[j]->GetName().c_str(), props[j]->GetPropertyType().GetName().c_str());
    }

    map.AddProxy(*proxy);

    CPPUNIT_ASSERT_MESSAGE("Proxy list has the wrong size.",
        map.GetAllProxies().size() == i + 1);
    CPPUNIT_ASSERT_MESSAGE("Last proxy in the list should equal the new proxy.",
        map.GetAllProxies().find(proxy->GetId())->second == proxy);
  }
}

void MapTests::testMapAddRemoveProxies() {
    try {
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

        unsigned maxId = map.GetAllProxies().size();
        for (unsigned x = 0;  x < maxId; x++) {
            CPPUNIT_ASSERT_MESSAGE("Unable to remove item 0",
                map.RemoveProxy(*map.GetAllProxies().begin()->second));

            CPPUNIT_ASSERT_MESSAGE("Proxy list has the wrong size.",
                map.GetAllProxies().size() == (unsigned)(maxId - (x + 1)));
        }
        map.RebuildProxyActorClassSet();
        CPPUNIT_ASSERT_MESSAGE("The set of actor classes should be empty.",
            map.GetProxyActorClasses().empty());

    } catch (const dtDAL::Exception& e) {
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
}

void MapTests::testMapProxySearch() {
    try {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        project.SetContext("WorkingMapProject");

        dtDAL::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

        createActors(map);

        unsigned maxId = map.GetAllProxies().size();

        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > results;

        map.FindProxies(results, "", "dtcore", "");

        CPPUNIT_ASSERT_MESSAGE("dtcore category should include the entire base actor set and should return all the actors.",
            results.size() == map.GetAllProxies().size());

        map.FindProxies(results, "", "dtc", "");

        CPPUNIT_ASSERT_MESSAGE("Only full category names should return results.", results.size() == 0);

        map.FindProxies(results, "", "dtcore.", "");
        CPPUNIT_ASSERT_MESSAGE("trailing dots should end with no results.", results.size() == 0);

        map.FindProxies(results, "", "", "","", dtDAL::Map::Placeable);
        for (std::vector<osg::ref_ptr<dtDAL::ActorProxy> >::iterator i = results.begin();
            i != results.end(); ++i) {
            CPPUNIT_ASSERT_MESSAGE(std::string("Proxy ") + (*i)->GetName()
                + " should not be in the results, it is not placeable",
                 (*i)->IsPlaceable());
        }

        map.FindProxies(results, "", "", "","", dtDAL::Map::NotPlaceable);
        for (std::vector<osg::ref_ptr<dtDAL::ActorProxy> >::iterator i = results.begin();
            i != results.end(); ++i) {
            CPPUNIT_ASSERT_MESSAGE(std::string("Proxy ") + (*i)->GetName()
                + " should not be in the results, it is placeable",
                !(*i)->IsPlaceable());
        }

        map.FindProxies(results, "", "", "","dtCore::Light");
        CPPUNIT_ASSERT_MESSAGE("There should be some lights in the results.", results.size() >= 3);


        for (std::vector<osg::ref_ptr<dtDAL::ActorProxy> >::iterator i = results.begin();
            i != results.end(); ++i) {
            CPPUNIT_ASSERT_MESSAGE("All results should be instances of dtCore::Light",
                (*i)->IsInstanceOf("dtCore::Light"));
        }

        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > proxies;

        map.GetAllProxies(proxies);

        for (unsigned x = 0;  x < proxies.size(); x++) {

            osg::ref_ptr<dtDAL::ActorProxy> proxyPTR = map.GetProxyById(proxies[x]->GetId());

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
    } catch (const dtDAL::Exception& e) {
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
}

dtDAL::ActorProperty* MapTests::getActorProperty(dtDAL::Map& map,
    const std::string& propName, dtDAL::DataType& type, unsigned which) {
    for (std::map<dtCore::UniqueId, osg::ref_ptr<dtDAL::ActorProxy> >::const_iterator i = map.GetAllProxies().begin();
        i != map.GetAllProxies().end(); ++i) {

        dtDAL::ActorProxy* proxy = map.GetProxyById(i->first);

        CPPUNIT_ASSERT_MESSAGE("ERROR: Proxy is NULL", proxy!= NULL );

        if (propName != "") {
            dtDAL::ActorProperty* prop = proxy->GetProperty(propName);
            if (prop != NULL) {
                if (prop->GetPropertyType() == type && which-- == 0)
                    return prop;
            }
        } else  {
            std::vector<dtDAL::ActorProperty*> props;
            proxy->GetPropertyList(props);
            for (std::vector<dtDAL::ActorProperty*>::iterator j = props.begin(); j<props.end(); ++j) {
                dtDAL::ActorProperty* prop = *j;
                if (prop->GetPropertyType() == type && which-- == 0)
                    return prop;
            }
        }
    }

    CPPUNIT_FAIL(std::string("No property found with name \"")
        + propName + "\", type \"" + type.GetName() + "\".");
    //This line will never be reached because fail will throw and exception.
    return NULL;
}


void MapTests::testLibraryMethods() {
    try {
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

    } catch (const dtDAL::Exception& e) {
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
}

void MapTests::testMapLibraryHandling() {
    try {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        project.SetContext("WorkingMapProject");

        std::string mapName("Neato Map");
        std::string mapFileName("neatomap");

        dtDAL::Map* map = &project.CreateMap(mapName, mapFileName);

        CPPUNIT_ASSERT_MESSAGE("neatomap.xml should be the name of the map file.", map->GetFileName() == "neatomap.xml");

        map->AddLibrary("dtCreateActors", "1.0");
        dtDAL::LibraryManager::GetInstance().LoadActorRegistry("dtCreateActors");

        createActors(*map);
    
        dtDAL::ActorPluginRegistry* reg = dtDAL::LibraryManager::GetInstance().GetRegistry("dtCreateActors");
        CPPUNIT_ASSERT_MESSAGE("Registry for dtCreateActors should not be NULL.", reg != NULL);
        
        project.SaveMap(*map);
        
        project.CloseMap(*map, true);

        reg = dtDAL::LibraryManager::GetInstance().GetRegistry("dtCreateActors");
        CPPUNIT_ASSERT_MESSAGE("dtCreateActors should have been closed.", reg == NULL);

        map = &project.GetMap(mapName);
        
        reg = dtDAL::LibraryManager::GetInstance().GetRegistry("dtCreateActors");
        CPPUNIT_ASSERT_MESSAGE("Registry for dtCreateActors should not be NULL.", reg != NULL);
        
        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > proxies;
        //hold onto all the proxies so that the actor libraries can't be closed.
        map->GetAllProxies(proxies);
        
        project.CloseMap(*map, true);
        
        reg = dtDAL::LibraryManager::GetInstance().GetRegistry("dtCreateActors");
        CPPUNIT_ASSERT_MESSAGE("Registry for dtCreateActors should not be NULL.", reg != NULL);

        //cleanup the proxies
        proxies.clear();
        
        map = &project.GetMap(mapName);
        //create a new map that will ALSO use the same libraries
        project.CreateMap(mapName + "1", mapFileName + "1").AddLibrary("dtCreateActors", "1.0");
        
        createActors(project.GetMap(mapName + "1"));
        
        project.CloseMap(*map, true);
        
        reg = dtDAL::LibraryManager::GetInstance().GetRegistry("dtCreateActors");
        CPPUNIT_ASSERT_MESSAGE("Registry for dtCreateActors should not be NULL.", reg != NULL);

        //when the second map is closed, the libraries should not close if false is passed.
        project.CloseMap(project.GetMap(mapName + "1"), false);

        reg = dtDAL::LibraryManager::GetInstance().GetRegistry("dtCreateActors");
        CPPUNIT_ASSERT_MESSAGE("Registry for dtCreateActors should not be NULL.", reg != NULL);

        //reopen the map and close it with true to make sure the libraries close.
        project.CloseMap(project.GetMap(mapName), true);

        reg = dtDAL::LibraryManager::GetInstance().GetRegistry("dtCreateActors");
        CPPUNIT_ASSERT_MESSAGE("dtCreateActors should have been closed.", reg == NULL);        
    } catch (const dtDAL::Exception& e) {
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
}

void MapTests::testMapSaveAndLoad() {
    try {
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

        map->AddLibrary("dtCreateActors", "1.0");
        dtDAL::LibraryManager::GetInstance().LoadActorRegistry("dtCreateActors");

        dtDAL::ResourceDescriptor marineRD = project.AddResource("marine", "../../../data/marine/marine.rbody", "marine",
            dtDAL::DataType::CHARACTER);

        dtDAL::ResourceDescriptor dirtRD = project.AddResource("dirt", "../../../data/models/dirt.ive", "dirt",
            dtDAL::DataType::STATIC_MESH);

        createActors(*map);

        dtDAL::ActorProperty* ap;

        ap = getActorProperty(*map, "", dtDAL::DataType::STRING);
        ((dtDAL::StringActorProperty*)ap)->SetValue("hello");

        ap = getActorProperty(*map, "", dtDAL::DataType::BOOLEAN, 1);
        ((dtDAL::BooleanActorProperty*)ap)->SetValue(false);
        ap = getActorProperty(*map, "", dtDAL::DataType::BOOLEAN, 2);
        ((dtDAL::BooleanActorProperty*)ap)->SetValue(true);
        ap = getActorProperty(*map, "", dtDAL::DataType::FLOAT);
        ((dtDAL::FloatActorProperty*)ap)->SetValue(40.00f);
        ap = getActorProperty(*map, "", dtDAL::DataType::DOUBLE);
        ((dtDAL::DoubleActorProperty*)ap)->SetValue(39.70);

        osg::Vec3 testVec3_1(33.5f, 12.25f, 49.125);
        osg::Vec3 testVec3_2(-34.75f, 96.03125f, 8.0f);
        osg::Vec3 testVec3_3(3.125, 90.25, 87.0625);

        ap = getActorProperty(*map, "Rotation", dtDAL::DataType::VEC3, 1);
        ((dtDAL::Vec3ActorProperty*)ap)->SetValue(testVec3_1);

        ap = getActorProperty(*map, "Translation", dtDAL::DataType::VEC3, 1);
        ((dtDAL::Vec3ActorProperty*)ap)->SetValue(testVec3_2);

        ap = getActorProperty(*map, "Scale", dtDAL::DataType::VEC3, 1);
        ((dtDAL::Vec3ActorProperty*)ap)->SetValue(testVec3_3);

        //osg::Vec2 testVec2_1(3.125, 90.25);
       // ap = GetActorProperty(*map, "Lat/Long", dtDAL::DataType::VEC2);
       // ((dtDAL::Vec2ActorProperty*)ap)->setValue(testVec2_1);


        osg::Vec4 testVec4_1(3.125, 90.25, 33.1, 73.64);
        osg::Vec4 testVec4_2(0.125, 33.25, 94.63, 11.211);

        ap = getActorProperty(*map, "", dtDAL::DataType::RGBACOLOR, 0);
        ((dtDAL::ColorRgbaActorProperty*)ap)->SetValue(testVec4_1);

        ap = getActorProperty(*map, "", dtDAL::DataType::RGBACOLOR, 1);
        ((dtDAL::ColorRgbaActorProperty*)ap)->SetValue(testVec4_2);

        ap = getActorProperty(*map, "", dtDAL::DataType::VEC4, 0);
        ((dtDAL::Vec4ActorProperty*)ap)->SetValue(testVec4_1);

        ap = getActorProperty(*map, "", dtDAL::DataType::INT);
        ((dtDAL::IntActorProperty*)ap)->SetValue(128);


        ap = getActorProperty(*map, "", dtDAL::DataType::ENUMERATION, 1);
        dtDAL::AbstractEnumActorProperty* eap = dynamic_cast<dtDAL::AbstractEnumActorProperty*>(ap);
        CPPUNIT_ASSERT(eap != NULL);

        if (eap->GetList().size() > 1)
           eap->SetEnumValue(const_cast<dtUtil::Enumeration&>(**(eap->GetList().begin()+1)));
        else
           std::cout << "Enum only has one value." << std::endl;

        ap = getActorProperty(*map, "model", dtDAL::DataType::CHARACTER);
        dtDAL::ResourceActorProperty& rap = static_cast<dtDAL::ResourceActorProperty&>(*ap);
        rap.SetValue(&marineRD);

        std::string marineStr = rap.GetStringValue();
        rap.SetValue(NULL);
        CPPUNIT_ASSERT(rap.GetValue() == NULL);
        CPPUNIT_ASSERT(rap.SetStringValue(marineStr));
        CPPUNIT_ASSERT(rap.GetValue() != NULL);
        CPPUNIT_ASSERT(*rap.GetValue() == marineRD);


        ap = getActorProperty(*map, "", dtDAL::DataType::STATIC_MESH);
        ((dtDAL::ResourceActorProperty*)ap)->SetValue(&dirtRD);


        unsigned numProxies = map->GetAllProxies().size();
        std::map<dtCore::UniqueId, std::string> names;
        for (std::map<dtCore::UniqueId, osg::ref_ptr<dtDAL::ActorProxy> >::const_iterator i = map->GetAllProxies().begin();
            i != map->GetAllProxies().end(); i++) {

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
            j != names.end(); ++j) {
            dtDAL::ActorProxy* ap = map->GetProxyById(j->first);
            CPPUNIT_ASSERT(ap != NULL);
            CPPUNIT_ASSERT_MESSAGE(j->first.ToString() + " name should be " + j ->second, j->second == ap->GetName());
        }

        ap = getActorProperty(*map, "", dtDAL::DataType::STRING, 0);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be hello",
            ((dtDAL::StringActorProperty*)ap)->GetValue() == "hello");

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

        if (logger->IsLevelEnabled(dtDAL::Log::LOG_DEBUG)) {
            osg::Vec3f val = ((dtDAL::Vec3ActorProperty*)ap)->GetValue();
            logger->LogMessage(dtDAL::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                "Vec3f Property values: %f, %f, %f", val[0], val[1], val[2]);
        }

        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 33.5f, 12.25f, 49.125f",
            osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[0], testVec3_1[0], 1e-2f)
            && osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[1], testVec3_1[1], 1e-2f )
            && osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[2], testVec3_1[2], 1e-2f )
            );

        ap = getActorProperty(*map, "Translation", dtDAL::DataType::VEC3, 1);

        if (logger->IsLevelEnabled(dtDAL::Log::LOG_DEBUG)) {
            osg::Vec3 val = ((dtDAL::Vec3ActorProperty*)ap)->GetValue();
            logger->LogMessage(dtDAL::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                "Vec3f Property values: %f, %f, %f", val[0], val[1], val[2]);
        }

        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be -34.75f, 96.03125f, 8.0f",
            osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[0], testVec3_2[0], 1e-2f)
            && osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[1], testVec3_2[1], 1e-2f)
            && osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[2], testVec3_2[2], 1e-2f)
            );
        ap = getActorProperty(*map, "Scale", dtDAL::DataType::VEC3, 1);

        if (logger->IsLevelEnabled(dtDAL::Log::LOG_DEBUG)) {
            osg::Vec3f val = ((dtDAL::Vec3ActorProperty*)ap)->GetValue();
            logger->LogMessage(dtDAL::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                "Vec3f Property values: %f, %f, %f", val[0], val[1], val[2]);
        }

        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 3.125, 90.25, 87.0625",
             osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[0], testVec3_3[0], 1e-2f)
             && osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[1], testVec3_3[1], 1e-2f)
             && osg::equivalent(((dtDAL::Vec3ActorProperty*)ap)->GetValue()[2], testVec3_3[2], 1e-2f)
             );

        //ap = GetActorProperty(*map, "Lat/Long", dtDAL::DataType::VEC2);
        //CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 3.125, 90.25",
        //     osg::equivalent(((dtDAL::Vec2ActorProperty*)ap)->GetValue()[0], testVec2_1[0], 1e-2f)
        //     && osg::equivalent(((dtDAL::Vec2ActorProperty*)ap)->GetValue()[1], testVec2_1[1], 1e-2f)
        //     );


        ap = getActorProperty(*map, "", dtDAL::DataType::RGBACOLOR, 0);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 3.125, 90.25, 33.1, 73.64",
            osg::equivalent(((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[0], testVec4_1[0], 1e-2f)
            && osg::equivalent(((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[1], testVec4_1[1], 1e-2f)
            && osg::equivalent(((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[2], testVec4_1[2], 1e-2f)
            && osg::equivalent(((dtDAL::ColorRgbaActorProperty*)ap)->GetValue()[3], testVec4_1[3], 1e-2f)
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

        ap = getActorProperty(*map, "", dtDAL::DataType::INT);
        CPPUNIT_ASSERT_MESSAGE(ap->GetName() + " value should be 128",
             ((dtDAL::IntActorProperty*)ap)->GetValue() == 128);

        ap = getActorProperty(*map, "", dtDAL::DataType::ENUMERATION,1);
        eap = dynamic_cast<dtDAL::AbstractEnumActorProperty*>(ap);
        ap = eap->AsActorProperty();
        CPPUNIT_ASSERT_MESSAGE(std::string("Value should be ") + (*(eap->GetList().begin()+1))->GetName()
           + " but it is " + eap->GetEnumValue().GetName(),
           eap->GetEnumValue() == **(eap->GetList().begin()+1));

        ap = getActorProperty(*map, "model", dtDAL::DataType::CHARACTER);
        dtDAL::ResourceDescriptor* rdVal = ((dtDAL::ResourceActorProperty*)ap)->GetValue();
        //testRD is declared in the setup section prior to the save and load.
        if (rdVal == NULL)
            CPPUNIT_FAIL("Character ResourceDescriptor should not be NULL.");
        CPPUNIT_ASSERT_MESSAGE("The resource Descriptor does not match.  Value is :" + rdVal->GetResourceIdentifier(),
            rdVal != NULL && *rdVal == marineRD);

        ap = getActorProperty(*map, "", dtDAL::DataType::STATIC_MESH);
        dtDAL::ResourceDescriptor* rdMeshVal = ((dtDAL::ResourceActorProperty*)ap)->GetValue();
        //testRD is declared in the setup section prior to the save and load.
        if (rdMeshVal == NULL)
            CPPUNIT_FAIL("Static Mesh ResourceDescriptor should not be NULL.");
        CPPUNIT_ASSERT_MESSAGE("The resource Descriptor does not match.  Value is :" + rdVal->GetResourceIdentifier(),
            rdMeshVal != NULL && *rdMeshVal == dirtRD);

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

        try {
            project.SaveMapAs(*map, newMapName, mapFileName);
            CPPUNIT_FAIL("Calling SaveAs on a map with the same name and filename should fail.");
        } catch (const dtDAL::Exception& e) {
            //correct
        }

        try {
            project.SaveMapAs(*map, mapName, mapFileName);
            CPPUNIT_FAIL("Calling SaveAs on a map with the same filename should fail.");
        } catch (const dtDAL::Exception& e) {
            //correct
        }

        try {
            project.SaveMapAs(*map, newMapName, "oo");
            CPPUNIT_FAIL("Calling SaveAs on a map with the same name should fail.");
        } catch (const dtDAL::Exception& e) {
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

        std::string newMapFilePath = project.GetContext() + dtDAL::FileUtils::PATH_SEPARATOR + "maps"
            + dtDAL::FileUtils::PATH_SEPARATOR + "oo" + dtDAL::Map::MAP_FILE_EXTENSION;

        CPPUNIT_ASSERT_MESSAGE(std::string("The new map file should exist: ") + newMapFilePath,
            dtDAL::FileUtils::GetInstance().FileExists(newMapFilePath));

        //set the map name before deleting it to make sure
        //I can delete with a changed name.
        map->SetName("some new name");

        project.DeleteMap(*map, true);
    } catch (const dtDAL::Exception& e) {
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
}

void MapTests::testLoadErrorHandling() {
    try {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        project.SetContext("WorkingMapProject");

        dtDAL::Map* map = &project.CreateMap("Neato Map", "neatomap");

        createActors(*map);

        //LibraryManager::GetInstance().loadActorRegistry("dtCreateActors");

    } catch (const dtDAL::Exception& e) {
        CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
    }
}

void MapTests::testWildCard() {
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


void MapTests::testLoadMapIntoScene() {
    try {
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        project.SetContext("WorkingMapProject");

        dtDAL::Map& map = project.CreateMap(std::string("Neato Map"), std::string("neatomap"));

        createActors(map);

        std::set<dtCore::UniqueId> ids;

        //add all the names of the actors that should be in the scene to set so we can track them.
        for (std::map<dtCore::UniqueId, osg::ref_ptr<dtDAL::ActorProxy> >::const_iterator i = map.GetAllProxies().begin();
            i != map.GetAllProxies().end(); ++i) {
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
        for (unsigned x = 0; x < (unsigned)scene.GetNumberOfAddedDrawable(); x++) {
            dtCore::DeltaDrawable* dd = scene.GetDrawable(x);
            std::set<dtCore::UniqueId>::iterator found = ids.find(dd->GetUniqueId());
            //Need to check to see if the actor exists in the set before removing it
            //because this is a test and because the scene could add drawables itself.
            if (found != ids.end()) {
                ids.erase(found);
            }
        }

        std::ostringstream ostream;
        ostream << "All drawables should have been found in the delta 3d list of drawables and removed. There are " << ids.size() <<
        "not found.";

        //Make sure all drawables have been removed.
        CPPUNIT_ASSERT_MESSAGE(ostream.str() ,
            ids.size() == 0);

    } catch (dtDAL::Exception& ex) {
        CPPUNIT_FAIL((std::string("Error: ") + ex.What()).c_str());
    }
}
