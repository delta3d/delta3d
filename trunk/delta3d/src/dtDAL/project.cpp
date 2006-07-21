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

#include <string>
#include <sstream>
#include <set>
#include <cstdio>
#include <cassert>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4267) // for warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#endif

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/XMLUni.hpp>

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include <osgDB/FileNameUtils>

#include <dtCore/globals.h>

#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>

#include "dtDAL/project.h"
#include "dtDAL/map.h"
#include "dtDAL/mapxml.h"
#include "dtDAL/datatype.h"
#include "dtDAL/exceptionenum.h"
#include "dtDAL/librarymanager.h"
#include "dtDAL/actorproxyicon.h"
#include "dtDAL/environmentactor.h"

#include <dtAI/waypointmanager.h>
#include <dtDAL/waypointactorproxy.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #ifndef snprintf
      #define snprintf _snprintf
   #endif
#endif

namespace dtDAL
{
   const std::string Project::LOG_NAME("project.cpp");
   const std::string Project::MAP_DIRECTORY("maps");
   const std::string Project::MAP_BACKUP_SUB_DIRECTORY("backups");

   dtCore::RefPtr<Project> Project::mInstance(NULL);

   //////////////////////////////////////////////////////////
   Project::Project() : mValidContext(false), mContext(""),
                        mContextReadOnly(true), mResourcesIndexed(false),
                        mEditMode(false)
   {
      MapParser::StaticInit();
      MapXMLConstants::StaticInit();

      mWriter = new MapWriter;
      libraryManager = &LibraryManager::GetInstance();
      mLogger = &dtUtil::Log::GetInstance(Project::LOG_NAME);
   }

   //////////////////////////////////////////////////////////
   Project::~Project()
   {
      MapXMLConstants::StaticShutdown();
      MapParser::StaticShutdown();
      //make sure the maps get closed before
      //the library manager is deleted
      mOpenMaps.clear();
   }

   //////////////////////////////////////////////////////////
   Project::Project(const Project&) {}
   //////////////////////////////////////////////////////////
   Project& Project::operator=(const Project&)
   {
      return *this;
   }

   //////////////////////////////////////////////////////////
   void Project::SetContext(const std::string& path, bool mOpenReadOnly)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      dtUtil::FileType ft = fileUtils.GetFileInfo(path).fileType;

      if (mValidContext)
      {
         mOpenMaps.clear();
         //clear the references to all the open maps
         mMapList.clear();
         mMapNames.clear();

         //clear out the list of mResources.
         mResources.clear();
         mResourcesIndexed = false;
      }

      //save the old context for later.
      std::string oldContext = mContext;

      //unset the current mContext.
      mValidContext = false;
      mContext = "";
      mContextReadOnly = true;

      //remove the old context from the data file path list.
      std::string searchPath = dtCore::GetDataFilePathList();
      size_t index = oldContext.empty() ? std::string::npos : searchPath.find(oldContext);
      if (index != std::string::npos)
      {
         searchPath.erase(index, oldContext.size());
         dtCore::SetDataFilePathList(searchPath);
      }

      if (!mOpenReadOnly)
      {
         try
         {
            fileUtils.MakeDirectory(path);
            ft = dtUtil::DIRECTORY;
         }
         catch (const dtUtil::Exception& ex)
         {
            std::ostringstream ss;
            ss << "Unable to create directory " << path << ". Error: " << ex.What();
            EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, ss.str());
         }
      }

      if (ft == dtUtil::FILE_NOT_FOUND)
      {
         char* fmt = "Directory %s does not exist";
         int size = strlen(fmt) + path.length();
         char* buffer = new char[size + 1];
         snprintf(buffer, size, fmt, path.c_str());
         std::string s(buffer);
         delete buffer;
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, s);
      }

      if (ft == dtUtil::REGULAR_FILE)
      {
         std::string s(path);
         s.append(" is not a directory");
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, s);
      }

      //from this point on, we know the path is a valid directory, so we now check the structure.

      std::string pPath = path;
      std::string::iterator last = pPath.end();
      --last;
      if (*last == dtUtil::FileUtils::PATH_SEPARATOR)
         pPath.erase(last);

      fileUtils.PushDirectory(path);

      try
      {
         const dtUtil::DirectoryContents contents = fileUtils.DirGetFiles(".");
         if (contents.empty())
         {
            if (mOpenReadOnly)
            {
               std::string s(path);
               s.append(" is not a valid project directory.");
               EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, s);
            }
            else
            {
               try
               {
                  fileUtils.MakeDirectory(Project::MAP_DIRECTORY);
               }
               catch(const dtUtil::Exception& ex)
               {
                  std::ostringstream ss;
                  ss << "Unable to create directory " << Project::MAP_DIRECTORY << ". Error: " << ex.What();
                  EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, ss.str());
               }
            }
         }
         else
         {
            std::set<std::string> contentsSet;
            contentsSet.insert(contents.begin(), contents.end());
            if (!mOpenReadOnly && contentsSet.find(Project::MAP_DIRECTORY) == contentsSet.end())
            {
               try
               {
                  fileUtils.MakeDirectory(Project::MAP_DIRECTORY);
               }
               catch (const dtUtil::Exception& ex)
               {
                  std::ostringstream ss;
                  ss << "Unable to create directory " << Project::MAP_DIRECTORY << ". Error: " << ex.What();
                  EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, ss.str());
               }
            }
            else if (fileUtils.GetFileInfo(Project::MAP_DIRECTORY).fileType != dtUtil::DIRECTORY)
            {
               std::string s(path);
               s.append(" is not a valid project directory.  The ");
               s.append(Project::MAP_DIRECTORY);
               if (fileUtils.GetFileInfo(Project::MAP_DIRECTORY).fileType == dtUtil::REGULAR_FILE)
                  s.append(" is not a directory.");
               else
                  s.append(" does not exist.");
               EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, s);
            }
         }

         mValidContext = true;

         mContext = dtUtil::FileUtils::GetInstance().CurrentDirectory();
         mContextReadOnly = mOpenReadOnly;
         std::string searchPath = dtCore::GetDataFilePathList();

         if (searchPath.empty())
            searchPath = dtCore::GetDeltaDataPathList();

         dtCore::SetDataFilePathList(searchPath + ':' + mContext);


         if (mParser == NULL)
         {
            //create the parser after setting the context.
            //because the parser looks for map.xsd in the constructor.
            //that way users can put map.xsd in the project and not need
            //a "data" path.
            mParser = new MapParser;
         }

         GetMapNames();
      }
      catch (const dtUtil::Exception& ex)
      {
         dtUtil::FileUtils::GetInstance().PopDirectory();
         throw ex;
      }
      dtUtil::FileUtils::GetInstance().PopDirectory();

   }

   //////////////////////////////////////////////////////////
   void Project::Refresh()
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      //clear the references to all the open maps
      mMapList.clear();
      mMapNames.clear();
      GetMapNames();

      //clear out the list of mResources.
      mResources.clear();
      mResourcesIndexed = false;
   }

   //////////////////////////////////////////////////////////
   void Project::ReloadMapNames() const
   {
      mMapNames.clear();
      for (std::map<std::string,std::string>::const_iterator i = mMapList.begin(); i != mMapList.end(); ++i)
      {
         mMapNames.insert(i->first);
      }
   }

   //////////////////////////////////////////////////////////
   const std::set<std::string>& Project::GetMapNames()
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      if (mMapList.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The list of map names is empty, so the project is preparing to load the list.");
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.PushDirectory(mContext);

         try
         {
            const dtUtil::DirectoryContents contents = fileUtils.DirGetFiles(Project::MAP_DIRECTORY);

            for (dtUtil::DirectoryContents::const_iterator i = contents.begin(); i < contents.end(); ++i)
            {
               const std::string& f = *i;
               std::string fp = Project::MAP_DIRECTORY + dtUtil::FileUtils::PATH_SEPARATOR + f;
               if (fileUtils.GetFileInfo(fp).fileType == dtUtil::REGULAR_FILE)
               {
                  try
                  {
                     const std::string& mapName = mParser->ParseMapName(fp);
                     mMapList.insert(make_pair(mapName, f));
                  }
                  catch (const dtUtil::Exception& e)
                  {
                     std::string error = "Unable to parse " + fp + " with error " + e.What();
                     mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
                  }
               }
            }
         }
         catch (const dtUtil::Exception& ex)
         {
            fileUtils.PopDirectory();
            throw ex;
         }
         fileUtils.PopDirectory();

         ReloadMapNames();
      }

      return mMapNames;
   }

   //////////////////////////////////////////////////////////
   Map& Project::InternalLoadMap(const std::string& name, const std::string& fullPath, bool clearModified)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(this->mContext);

      Map* map = NULL;
      try
      {
         if (fileUtils.GetFileInfo(fullPath).fileType != dtUtil::REGULAR_FILE)
            EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound,
                   std::string("Map file \"") + fullPath + "\" not found.");

         map = mParser->Parse(fullPath);

         if (map == NULL)
         {
            EXCEPT(dtDAL::ExceptionEnum::MapLoadParsingError,
                   "Map loading didn't throw an exception, but the result is NULL");
         }

         mOpenMaps.insert(std::make_pair(name, dtCore::RefPtr<Map>(map)));

         //Clearing the modified flag must be done because setting the
         //map properties at load will make the map look modified.
         //it must be done before adding the missing libraries and proxy
         //classes because clearing the modified flag clears those lists.
         if (clearModified)
            map->ClearModified();

         map->AddMissingLibraries(mParser->GetMissingLibraries());
         map->AddMissingActorTypes(mParser->GetMissingActorTypes());

         //added support for waypoint files- banderegg 7-10-06
         if(!map->GetPathNodeFileName().empty())
         {
            dtAI::WaypointManager::GetInstance()->OnMapLoad(map->GetPathNodeFileName());
            
            //if we are running within stage we need to make proxies as well
            if(mEditMode) CreateWaypointActors(*map);
         }
      }
      catch (const dtUtil::Exception& e)
      {
         std::string error = "Unable to parse " + fullPath + " with error " + e.What();
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
         fileUtils.PopDirectory();
         throw e;
      }
      fileUtils.PopDirectory();
      return *map;
   }

   //////////////////////////////////////////////////////////
   Map& Project::GetMap(const std::string& name)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      std::map<std::string, dtCore::RefPtr<Map> >::iterator openMapI = mOpenMaps.find(name);

      //map is already open.
      if (openMapI != mOpenMaps.end())
         return *(openMapI->second);

      std::map<std::string,std::string>::iterator i = mMapList.find(name);

      if (i == mMapList.end())
         EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound,
                std::string("Map named ") + name + " does not exist.");

      const std::string& mapFileName = i->second;

      const std::string& fp = Project::MAP_DIRECTORY + dtUtil::FileUtils::PATH_SEPARATOR + mapFileName;

      Map& map = InternalLoadMap(name, fp, true);

      map.SetFileName(mapFileName);
      return map;
   }
   //////////////////////////////////////////////////////////
   Map& Project::OpenMapBackup(const std::string& name)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      std::map<std::string, dtCore::RefPtr<Map> >::iterator openMapI = mOpenMaps.find(name);

      //map is already open.
      if (openMapI != mOpenMaps.end())
      {
         //close the map if it's open.
         mOpenMaps.erase(openMapI);
      }

      std::map<std::string,std::string>::iterator i = mMapList.find(name);

      if (i == mMapList.end())
         EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound,
                std::string("Map named ") + name + " does not exist.");

      const std::string& mapFileName = i->second;

      const std::string& fp = GetBackupDir() + dtUtil::FileUtils::PATH_SEPARATOR + mapFileName + ".backup";

      Map& map = InternalLoadMap(name, fp, false);
      map.SetFileName(mapFileName);
      map.SetSavedName(name);
      return map;

   }

   //////////////////////////////////////////////////////////
   Map& Project::CreateMap(const std::string& name, const std::string& fileName)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      if (IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

      if (name == "")
         EXCEPT(dtDAL::ExceptionEnum::ProjectException,
                "Maps may not have an empty name.");

      if (fileName == "")
         EXCEPT(dtDAL::ExceptionEnum::ProjectException, std::string("Maps may not have an empty fileName."));

      //assign it to a refptr so that if I except, it will get deleted
      dtCore::RefPtr<Map> map(new Map(fileName, name));

      for (std::map<std::string, std::string>::iterator i = mMapList.begin(); i != mMapList.end(); ++i)
      {
         if (i->first == name)
         {
            EXCEPT(dtDAL::ExceptionEnum::ProjectException,
                   std::string("Map named ") + name + " already exists.");
         }
         else if (i->second == map->GetFileName())
         {
            EXCEPT(dtDAL::ExceptionEnum::ProjectException,
                   std::string("A map with file name ") + fileName + " already exists.");
         }
      }


      InternalSaveMap(*map, NULL);

      mOpenMaps.insert(make_pair(name, dtCore::RefPtr<Map>(map.get())));
      //The map can add extensions and such to the file name, so it
      //must be fetched back from the map object before being added to the name-file map.
      mMapList.insert(make_pair(name, map->GetFileName()));
      mMapNames.insert(name);

      return *map;

   }

   //////////////////////////////////////////////////////////
   void Project::LoadMapIntoScene(Map& map, dtCore::Scene& scene, bool addBillBoards, bool enablePaging)
   {
      CheckMapValidity(map, true);
      std::vector<dtCore::RefPtr<ActorProxy> > container;
      map.GetAllProxies(container);
      for (std::vector<dtCore::RefPtr<ActorProxy> >::iterator i = container.begin();
           i != container.end(); ++i)
      {
         ActorProxy& proxy = **i;

         //if we are adding billboards, then we need to check the render modes.
         if (addBillBoards)
         {
            const ActorProxy::RenderMode &renderMode = proxy.GetRenderMode();

            if (renderMode == ActorProxy::RenderMode::DRAW_BILLBOARD_ICON ||
                renderMode == ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
            {
               ActorProxyIcon *billBoard = proxy.GetBillBoardIcon();

               // Load the textures for the billboard and orientation arrow.
               // This is only done here so the files will only be loaded when
               // they are actually going to be rendered. Previously this was
               // done inside ActorProxyIcon's constructor, even if billboards
               // will never be displayed. It was broken out and placed here
               // to fix that. -osb
               billBoard->LoadImages();

               if (billBoard == NULL)
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                      "Proxy [%s] billboard was NULL.", proxy.GetName().c_str());
               }
               else
               {
                  scene.AddDrawable(billBoard->GetDrawable());
               }

            }

            if (renderMode == ActorProxy::RenderMode::DRAW_ACTOR ||
                renderMode == ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
            {
               scene.AddDrawable(proxy.GetActor());
            }


            if (renderMode == ActorProxy::RenderMode::DRAW_AUTO)
            {
               //If we got here, then the proxy wishes the system to determine how to display
               //the proxy. (Currently defaults to DRAW_ACTOR.
               if(map.GetEnvironmentActor() != NULL)
               {
                  EnvironmentActor *ea = dynamic_cast<EnvironmentActor*>(map.GetEnvironmentActor()->GetActor());
                  if(ea == NULL)
                     EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be of type dtDAL::EnvironmentActor");

                  ea->AddActor(proxy);
               }
               else
                  scene.AddDrawable(proxy.GetActor());
            }
         }
         else
         {
            //if we aren't drawing billboards, then the actors should always be in the scene.
            if(map.GetEnvironmentActor() != NULL)
            {
               EnvironmentActor *ea = dynamic_cast<EnvironmentActor*>(map.GetEnvironmentActor()->GetActor());
               if(ea == NULL)
                  EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be of type dtDAL::EnvironmentActor");

               // Hack to ensure the environment doesn't add itself from the map 
               if(proxy.GetActor() != dynamic_cast<dtCore::DeltaDrawable*>(ea))
                  ea->AddActor(proxy);
            }
            else
               scene.AddDrawable(proxy.GetActor());
         }
      }

      if(map.GetEnvironmentActor() != NULL)
      {
         scene.AddDrawable(map.GetEnvironmentActor()->GetActor());
      }

      if(enablePaging)
      {
         if(scene.IsPagingEnabled())
            scene.DisablePaging();

         scene.EnablePaging();
      }
   }

   //////////////////////////////////////////////////////////
   void Project::UnloadUnusedLibraries(Map& mapToClose)
   {

      std::vector<dtCore::RefPtr<ActorProxy> > proxies;
      mapToClose.GetAllProxies(proxies);
      std::vector<dtCore::RefPtr<ActorProxy> >::iterator i = proxies.begin();
      while (i != proxies.end())
      {
         dtCore::RefPtr<ActorProxy>& proxy = *i;
         //if this proxy has a reference count greater than 1
         //then its library may not close, but 2 is used here because
         //the vector has a referece to it now.
         if (proxy != NULL && proxy->referenceCount() <= 2)
         {
            i = proxies.erase(i);
         }
         else
         {
            ++i;
         }
      }

      //clear the proxies to make sure they are all freed.
      //one should not save AFTER calling this.
      mapToClose.ClearProxies();
      for (std::vector<std::string>::const_iterator i = mapToClose.GetAllLibraries().begin();
           i != mapToClose.GetAllLibraries().end(); ++i)
      {
         std::string libToClose = *i;
         bool libMayClose = true;
         for (std::map<std::string, dtCore::RefPtr<Map> >::const_iterator j = mOpenMaps.begin(); j != mOpenMaps.end(); ++j)
         {
            const Map& toCheck = *j->second;
            if (&mapToClose != &toCheck && toCheck.HasLibrary(libToClose))
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO)) {
                  mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                      "Not unloading library %s because it is used by open map named %s.",
                                      libToClose.c_str(), toCheck.GetName().c_str());
               }
               libMayClose = false;
               break;
            }
         }
         if (libMayClose)
         {
            ActorPluginRegistry* aprToClose =
               LibraryManager::GetInstance().GetRegistry(libToClose);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                   "About to unload library named %s.", libToClose.c_str());
            }

            //go through proxies still being held onto outside this library
            //and see if the currently library is the source of any.
            for (std::vector<dtCore::RefPtr<ActorProxy> >::iterator i = proxies.begin();
                 i != proxies.end(); ++i)
            {
               dtCore::RefPtr<ActorProxy>& proxy = *i;

               try
               {
                  ActorPluginRegistry* registry = LibraryManager::GetInstance().GetRegistryForType(proxy->GetActorType());
                  //the proxy is found in the library that is about to close.
                  if (aprToClose == registry)
                  {
                     libMayClose = false;
                     if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
                        mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                            "Library %s will not be closed because proxy with type %s and name %s has external references.",
                                            proxy->GetActorType().GetName().c_str(), libToClose.c_str(),
                                            proxy->GetName().c_str());
                  }
               }
               catch (const dtUtil::Exception& ex)
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                      "Error finding library for actor type %s: %s",
                                      proxy->GetActorType().GetName().c_str(), ex.What().c_str());
               }
            }

            //if the library may still close.
            if (libMayClose)
               LibraryManager::GetInstance().UnloadActorRegistry(libToClose);
         }

      }
   }

   //////////////////////////////////////////////////////////
   void Project::CloseMap(Map& map, bool unloadLibraries)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      //added support for waypoint files- banderegg 7/10/06
      if(!map.GetPathNodeFileName().empty())
      {
         dtAI::WaypointManager::GetInstance()->OnMapClose();
      }

      //bool
      std::map<std::string, dtCore::RefPtr<Map> >::iterator j = mOpenMaps.find(map.GetSavedName());
      if (j == mOpenMaps.end() || (j->second.get() != &map))
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext,
                std::string("A map named \"") + map.GetSavedName() + "\" exists, but this is not the instance.");
      }
      else
      {
         if (unloadLibraries)
         {
            UnloadUnusedLibraries(map);
         }
         try
         {
            ClearBackup(map.GetSavedName());
         }
         catch (const dtUtil::Exception& ex)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Error clearing map backups when saving: %s", ex.What().c_str());
         }
         mOpenMaps.erase(j);
      }
   }

   //////////////////////////////////////////////////////////
   void Project::DeleteMap(Map& map, bool unloadLibraries)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      if (IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

      // Do not access the map variable again after this line, it will be corrupted
      std::string mapSavedName = map.GetSavedName();

      CloseMap(map, unloadLibraries);

      std::map<std::string, std::string>::iterator i = mMapList.find(mapSavedName);
      if (i == mMapList.end())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                             "Map was found in the list of open maps, but not in map to fileName mapping");
      }
      else
      {
         mMapList.erase(i);
      }
   }

   //////////////////////////////////////////////////////////
   void Project::DeleteMap(const std::string& mapName, bool unloadLibraries)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      if (IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

      std::map<std::string, dtCore::RefPtr<Map> >::iterator j = mOpenMaps.find(mapName);
      if (j != mOpenMaps.end())
      {
         CloseMap(*j->second, unloadLibraries);
      }
      else
      {
         ClearBackup(mapName);
      }

      std::string mapFileName;

      std::map<std::string, std::string>::iterator i = mMapList.find(mapName);
      if (i == mMapList.end())
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound, std::string("No such map: \"") + mapName + "\"");
      }
      else
      {
         mapFileName = i->second;
         mMapList.erase(i);
      }

      InternalDeleteMap(mapFileName);

   }

   //////////////////////////////////////////////////////////
   void Project::InternalDeleteMap(const std::string& mapFileName)
   {
      ReloadMapNames();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(this->mContext + dtUtil::FileUtils::PATH_SEPARATOR + Project::MAP_DIRECTORY);
      try
      {
         if (fileUtils.FileExists(mapFileName))
         {
            fileUtils.FileDelete(mapFileName);
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                "Specified map was part of the project, but the map file did not exist.");
         }
      }
      catch (const dtUtil::Exception& ex)
      {
         fileUtils.PopDirectory();
         throw ex;
      }
      fileUtils.PopDirectory();
   }

   //////////////////////////////////////////////////////////
   void Project::SaveMap(Map& map, dtCore::Scene* pScene)
   {
      CheckMapValidity(map);
      InternalSaveMap(map, pScene);
   }

   //////////////////////////////////////////////////////////
   void Project::SaveMapAs(const std::string& mapName, dtCore::Scene* pScene, const std::string& newName, const std::string& newFileName)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      if (IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

      //The map must be loaded to do a saveAs, so we call getMap();
      SaveMapAs(GetMap(mapName), pScene, newName, newFileName);

   }
   //////////////////////////////////////////////////////////
   void Project::SaveMapAs(Map& map, dtCore::Scene* pScene, const std::string& newName, const std::string& newFileName)
   {
      CheckMapValidity(map);

      if (map.GetSavedName() == newName)
         EXCEPT(dtDAL::ExceptionEnum::ProjectException, std::string("Map named ")
                + map.GetSavedName() + " cannot be saved again as the same name");

      std::string newFileNameCopy(newFileName);
      std::string currentFileName(map.GetFileName());
      //compare the file name without the extension.
      if (currentFileName.substr(0, currentFileName.size() - Map::MAP_FILE_EXTENSION.size())
          == newFileNameCopy)
         EXCEPT(dtDAL::ExceptionEnum::ProjectException, std::string("Map named ") + map.GetSavedName()
                + " cannot be saved as a different map with the same file name.");

      for (std::map<std::string,std::string>::const_iterator i = mMapList.begin();
           i != mMapList.end(); ++i )
      {
         if (newFileNameCopy == i->second.substr(0, i->second.size() - Map::MAP_FILE_EXTENSION.size()))
            EXCEPT(dtDAL::ExceptionEnum::ProjectException, std::string("Map named ")
                   + map.GetSavedName() + " cannot be saved with file name "
                   + newFileName + " because it matches another map.");
      }

      std::string oldMapName = map.GetSavedName();
      std::string oldFileName = map.GetFileName();

      map.SetName(newName);
      map.SetFileName(newFileNameCopy);

      InternalSaveMap(map, pScene);
      //re-add the old map to the list of saved maps
      //since saving with a new name will remove the old entry.
      mMapList.insert(std::make_pair(oldMapName, oldFileName));
      mMapNames.insert(oldMapName);

      try
      {
         ClearBackup(oldMapName);
      }
      catch (const dtUtil::Exception& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Error clearing map backups when saving %s as %s: %s",
                             oldMapName.c_str(), newName.c_str(), ex.What().c_str());
      }
   }

   //////////////////////////////////////////////////////////
   void Project::SaveMap(const std::string& mapName, dtCore::Scene* pScene)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      if (IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

      std::map<std::string, dtCore::RefPtr<Map> >::iterator j = mOpenMaps.find(mapName);
      if (j == mOpenMaps.end())
      {
         return; //map is not in memory, so it doesn't need to be saved.
      }

      InternalSaveMap(*(j->second), pScene);
   }

   //////////////////////////////////////////////////////////
   void Project::CheckMapValidity(const Map& map, bool readonlyAllowed) const
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      if (!readonlyAllowed && IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

      std::map<std::string, std::string>::const_iterator i = mMapList.find(map.GetSavedName());
      if (i == mMapList.end())
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound, std::string("No such map: \"") + map.GetSavedName() + "\"");
      }

      std::map<std::string, dtCore::RefPtr<Map> >::const_iterator j = mOpenMaps.find(map.GetSavedName());

      if (j == mOpenMaps.end())
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("A map named \"") + map.GetSavedName()
                + "\" exists but is not currently open.");

      }
      else if (j->second.get() != &map)
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("A map named \"") + map.GetSavedName()
                + "\" exists, but this is not the instance.");
      }
   }

   //////////////////////////////////////////////////////////
   void Project::InternalSaveMap(Map& map, dtCore::Scene* pScene)
   {
      MapWriter& mw = *mWriter;

      if (map.GetSavedName() != map.GetName())
      {
         if (mMapList.find(map.GetName()) != mMapList.end())
         {
            EXCEPT(dtDAL::ExceptionEnum::ProjectException, "You may not save a map with a name that matches another map.");
         }
      }

      std::string fullPath = Project::MAP_DIRECTORY + dtUtil::FileUtils::PATH_SEPARATOR + map.GetFileName();
      std::string fullPathSaving = fullPath + ".saving";

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(mContext);
      try
      {
         //save the file to a separate name first so that
         //it won't blast the old one unless it is successful.
         mw.Save(map, fullPathSaving);
         //if it's successful, move it to the final file name
         fileUtils.FileMove(fullPathSaving, fullPath, true);

         //save the waypoint file- banderegg 7/10/06
         //if there is no filename given, we arent going to save any waypoints
         //lets check to see if there are waypoints in the scene and if so create 
         //a default waypoints filename for the user
         if(map.GetPathNodeFileName().empty() && (!dtAI::WaypointManager::GetInstance()->GetWaypoints().empty()))         
         {
            std::string mapName("Waypoints_");            
            mapName += map.GetName();
            mapName += ".ai";
            map.SetPathNodeFileName(mapName);
         }

         //alert the waypoint manager to save the waypoint file
         if(pScene && !map.GetPathNodeFileName().empty())
         {
            dtAI::WaypointManager::GetInstance()->OnMapSave(map.GetPathNodeFileName(), pScene);
         }
      }
      catch (const dtUtil::Exception& e)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, e.What().c_str());
         fileUtils.PopDirectory();
         throw e;
      }
      fileUtils.PopDirectory();

      //Update the internal lists to make sure that
      //map is keyed properly by name.
      if (map.GetSavedName() != "" && map.GetName() != map.GetSavedName())
      {
         if (!mMapList.empty())
         {
            mMapList.erase(mMapList.find(map.GetSavedName()));
            mMapList.insert(make_pair(map.GetName(), map.GetFileName()));
            ReloadMapNames();
         }

         dtCore::RefPtr<Map> holder(&map);

         mOpenMaps.erase(mOpenMaps.find(map.GetSavedName()));
         mOpenMaps.insert(make_pair(map.GetName(), holder));
      }

      map.ClearModified();

      try
      {
         ClearBackup(map.GetSavedName());
      }
      catch (const dtUtil::Exception& ex)
      {
         //if the map in new, the following exception will be thrown
         //so don't print an error in that case.
         if (ex.TypeEnum() != dtDAL::ExceptionEnum::ProjectFileNotFound)
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                "Error clearing map backups when saving: %s", ex.What().c_str());
      }
   }

   //////////////////////////////////////////////////////////
   void Project::SaveMapBackup(Map& map)
   {
      CheckMapValidity(map);

      MapWriter& mw = *mWriter;

      if (!map.IsModified())
         return;

      std::string backupDir = GetBackupDir();

      std::string path = backupDir + dtUtil::FileUtils::PATH_SEPARATOR + map.GetFileName();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(mContext);
      try
      {
         if (!fileUtils.DirExists(backupDir))
         {
            fileUtils.MakeDirectory(backupDir);
         }

         std::string fileName = path + ".backupsaving";
         std::string finalFileName = path + ".backup";

         //save the file to a "saving" file so that if it blows or is killed while saving, the data
         //will not be lost.
         mw.Save(map, fileName);

         //when it completes, move the file to the final name.
         fileUtils.FileMove(fileName, finalFileName, true);
      }
      catch (const dtUtil::Exception& e)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, e.What().c_str());
         fileUtils.PopDirectory();
         throw e;
      }
      fileUtils.PopDirectory();

   }

   //////////////////////////////////////////////////////////
   bool Project::HasBackup(Map& map) const
   {
      CheckMapValidity(map, true);
      return HasBackup(map.GetSavedName());
   }

   //////////////////////////////////////////////////////////
   bool Project::HasBackup(const std::string& mapName) const
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      std::map< std::string, std::string >::const_iterator found = mMapList.find(mapName);
      if (found == mMapList.end())
         EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound, std::string("No such map: \"") + mapName + "\"");

      const std::string& fileName = found->second;

      std::string backupDir = GetContext() + dtUtil::FileUtils::PATH_SEPARATOR + GetBackupDir();

      const std::string& backupFileName = backupDir + dtUtil::FileUtils::PATH_SEPARATOR + fileName + ".backup";

      return dtUtil::FileUtils::GetInstance().FileExists(backupFileName);
   }

   //////////////////////////////////////////////////////////
   void Project::ClearBackup(Map& map)
   {
      CheckMapValidity(map);
      ClearBackup(map.GetSavedName());
   }

   //////////////////////////////////////////////////////////
   void Project::ClearBackup(const std::string& mapName)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      if (IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

      std::map< std::string, std::string >::iterator found = mMapList.find(mapName);
      if (found == mMapList.end())
         EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound, std::string("No such map: \"") + mapName + "\"");

      std::string& fileName = found->second;

      std::string backupDir = GetContext() + dtUtil::FileUtils::PATH_SEPARATOR + GetBackupDir();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      if (!fileUtils.DirExists(backupDir))
         return;

      dtUtil::DirectoryContents dc = fileUtils.DirGetFiles(backupDir);

      size_t fileNameSize = fileName.size();
      for (dtUtil::DirectoryContents::const_iterator i = dc.begin(); i != dc.end(); ++i)
      {
         const std::string& file = *i;
         if (file.size() > fileNameSize
             && file.substr(0, fileNameSize) == fileName)
         {

            if (fileUtils.GetFileInfo(backupDir + dtUtil::FileUtils::PATH_SEPARATOR + file).fileType == dtUtil::REGULAR_FILE)
               fileUtils.FileDelete(backupDir + dtUtil::FileUtils::PATH_SEPARATOR + file);
         }
      }

   }

   //////////////////////////////////////////////////////////
   Map* Project::GetMapForActorProxy(const ActorProxy& proxy)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      std::map< std::string, dtCore::RefPtr<Map> >::iterator i = mOpenMaps.begin();
      while (i != mOpenMaps.end())
      {
         ActorProxy* ap = i->second->GetProxyById(proxy.GetId());
         if (ap != NULL)
            return i->second.get();
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////
   const Map* Project::GetMapForActorProxy(const ActorProxy& proxy) const
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      std::map< std::string, dtCore::RefPtr<Map> >::const_iterator i = mOpenMaps.begin();
      while (i != mOpenMaps.end())
      {
         const ActorProxy* ap = i->second->GetProxyById(proxy.GetId());
         if (ap != NULL)
            return i->second.get();
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////
   void Project::GetHandlersForDataType(const DataType& resourceType, std::vector<dtCore::RefPtr<const ResourceTypeHandler> >& toFill) const
   {
      mResourceHelper.GetHandlersForDataType(resourceType, toFill);
   }

   //////////////////////////////////////////////////////////
   void Project::RegisterResourceTypeHander(ResourceTypeHandler& handler)
   {
      mResourceHelper.RegisterResourceTypeHander(handler);
   }

   //////////////////////////////////////////////////////////
   const std::string Project::GetBackupDir() const
   {
      return Project::MAP_DIRECTORY + dtUtil::FileUtils::PATH_SEPARATOR + Project::MAP_BACKUP_SUB_DIRECTORY;
   }

   //////////////////////////////////////////////////////////
   const std::string Project::GetResourcePath(const ResourceDescriptor& resource) const
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      const std::string& path = mResourceHelper.GetResourcePath(resource);


      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(this->mContext);

      try
      {
		  dtUtil::FileType ftype = fileUtils.GetFileInfo(path).fileType;

         if (ftype != dtUtil::REGULAR_FILE)
         {
            if (ftype == dtUtil::FILE_NOT_FOUND)
            {
               EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound,
                      std::string("The specified resource was not found: ") + path);
            }
            else if (ftype == dtUtil::DIRECTORY)
            {
               EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError,
                      std::string("The resource identifier does not specify a resource file: ") + path);
            }
         }
      }
      catch (const dtUtil::Exception& ex)
      {
         //we have to make sure this happens before excepting
         fileUtils.PopDirectory();
         throw ex;
      }
      fileUtils.PopDirectory();

      return path;
   }


   //////////////////////////////////////////////////////////
   void Project::CreateResourceCategory(const std::string& category, const DataType& type)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      if (IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

      if (!type.IsResource())
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError, std::string("Unable to add resource of type ") + type.GetName()
                + ". It is not a resource type.");
      }

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(mContext);

      try
      {
         dtUtil::tree<ResourceTreeNode>* categoryInTree;
         dtUtil::tree<ResourceTreeNode>* dataTypeTree = NULL;

         if (mResourcesIndexed)
            dataTypeTree = &GetResourcesOfType(type);

         mResourceHelper.CreateResourceCategory(category, type, dataTypeTree, categoryInTree);
      }
      catch (const dtUtil::Exception& ex)
      {
         //we have to make sure this happens before excepting
         fileUtils.PopDirectory();
         throw ex;
      }
      fileUtils.PopDirectory();
   }

   //////////////////////////////////////////////////////////
   bool Project::RemoveResourceCategory(const std::string& category,
                                        const DataType& type, bool recursive)
   {

      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext,
                std::string("The context is not valid."));

      if (IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly,
                std::string("The context is readonly."));

      if (!type.IsResource())
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError, std::string("Unable to add resource of type ") + type.GetName()
                + ". It is not a resource type.");
      }

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(mContext);

      bool result;

      try
      {
         dtUtil::tree<ResourceTreeNode>* dataTypeTree = NULL;
         if (mResourcesIndexed)
            dataTypeTree = &GetResourcesOfType(type);

         result = mResourceHelper.RemoveResourceCategory(category, type, recursive, dataTypeTree);
      }
      catch (const dtUtil::Exception& ex)
      {
         //we have to make sure this happens before excepting
         fileUtils.PopDirectory();
         throw ex;
      }
      fileUtils.PopDirectory();

      return result;

   }

   //////////////////////////////////////////////////////////
   const ResourceDescriptor Project::AddResource(const std::string& newName,
                                                 const std::string& pathToFile, const std::string& category,
                                                 const DataType& type)
   {

      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext,
                std::string("The context is not valid."));

      if (IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly,
                std::string("The context is readonly."));

      if (!type.IsResource())
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError,
                std::string("Unable to add resource of type ") + type.GetName()
                + ". It is not a resource type.");
      }

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(mContext);

      ResourceDescriptor result;
      try
      {
         dtUtil::tree<ResourceTreeNode>* dataTypeTree = NULL;
         if (mResourcesIndexed)
            dataTypeTree = &GetResourcesOfType(type);

         result = mResourceHelper.AddResource(newName, pathToFile, category, type, dataTypeTree);

      }
      catch (const dtUtil::Exception& ex)
      {
         //we have to make sure this happens before excepting
         fileUtils.PopDirectory();
         throw ex;
      }
      fileUtils.PopDirectory();

      return result;
   }

   //////////////////////////////////////////////////////////
   void Project::RemoveResource(const ResourceDescriptor& resource)
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));
      if (IsReadOnly())
         EXCEPT(dtDAL::ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(mContext);
      try
      {

         dtUtil::tree<ResourceTreeNode>* resourceTree = NULL;
         if (mResourcesIndexed)
            resourceTree = &mResources;

         mResourceHelper.RemoveResource(resource, resourceTree);
      }
      catch (const dtUtil::Exception& ex)
      {
         fileUtils.PopDirectory();
         throw ex;
      }
      fileUtils.PopDirectory();
   }

   //////////////////////////////////////////////////////////
   void Project::IndexResources() const
   {
      if (mResourcesIndexed)
         return;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(GetContext());
      try
      {
         mResources.clear();
         mResourceHelper.IndexResources(mResources);
      }
      catch (const dtUtil::Exception& ex)
      {
         fileUtils.PopDirectory();
         throw ex;
      }
      fileUtils.PopDirectory();

      mResourcesIndexed = true;
   }

   //////////////////////////////////////////////////////////
   dtUtil::tree<ResourceTreeNode>& Project::GetResourcesOfType(const DataType& dataType) const
   {

      if (!mResourcesIndexed)
         IndexResources();

      ResourceTreeNode tr(dataType.GetName(), "");
      dtUtil::tree<ResourceTreeNode>::iterator it = mResources.find(tr);

      if (it  == mResources.end())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "ERROR: call getResourcesOfType ended with no valid results!");
         EXCEPT(dtDAL::ExceptionEnum::ProjectException, "ERROR: call GetResourcesOfType ended with no valid results!");
      }

      return it.tree_ref();

   }

   //////////////////////////////////////////////////////////
   void Project::GetResourcesOfType(const DataType& type, dtUtil::tree<ResourceTreeNode>& toFill) const
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

      toFill.clear();
      if (!type.IsResource())
         return;

      toFill = GetResourcesOfType(type);

   }

//     void Project::findmResources(dtUtil::tree<ResourceTreeNode> toFill,
//         const DataType& type,
//         const std::string& partialName,
//         const std::string& extension) const {
//
//         toFill.clear();
//
//         dtUtil::tree<ResourceTreeNode>::const_iterator it = mResources.begin();
//         for (; it != mResources.end(); ++it) {
//             if (it->isCategory() && it->getNodeText() == type.getName()) {
//
//                 dtUtil::tree<ResourceTreeNode>* matchingBranch = getMatchingBranch(it.in(), type, partialName, extension);
//
//                 //dtUtil::tree<ResourceTreeNode>*
//             }
//         }
//     }


//Later
/*    dtUtil::tree<ResourceTreeNode>* Project::getMatchingBranch(
      dtUtil::tree<ResourceTreeNode>::const_iterator level,
      const DataType& type,
      const std::string& partialName,
      const std::string& extension) const {

      dtUtil::tree<ResourceTreeNode>::iterator it = level;
      for (; it != mResources.end(); ++it) {
      if (it->isCategory() && it->getNodeText() == type.getName()) {

      dtUtil::tree<ResourceTreeNode>* matchingBranch = getMatchingBranch(it.in(), type, partialName, extension);


      } else if ((extension != "" && it->getResource().getExtension() == extension) &&
      (partialName != "" && it->getResource().getResourceName().find(partialName) < std::string::npos) ){
      dtUtil::tree<ResourceTreeNode>* node = new dtUtil::tree<ResourceTreeNode>;
      node->data(*it);
      return node;
      }
      }


      }*/
   //void archiveProject(const std::string& targetPath);
   //void unarchiveProject(const std::string& targetPath);

   //////////////////////////////////////////////////////////
   bool Project::IsArchive() const
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));
      return false;
   }

   //////////////////////////////////////////////////////////
   bool Project::IsReadOnly() const
   {
      if (!mValidContext)
         EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));
      return mContextReadOnly;
   }

   //////////////////////////////////////////////////////////
   void Project::SetEditMode(bool pInStage)
   {
      mEditMode = pInStage;
   }

   //////////////////////////////////////////////////////////////////////////
   void Project::CreateWaypointActors(Map& pMap)
   {
      if(dtAI::WaypointManager::GetInstance()->ObtainLock())
      {
         dtAI::WaypointManager::WaypointMap pWaypoints = dtAI::WaypointManager::GetInstance()->GetWaypoints();
         dtAI::WaypointManager::WaypointIterator iter = pWaypoints.begin();
         dtAI::WaypointManager::WaypointIterator endOfVector = pWaypoints.end();

         unsigned counter = 0;

         while(iter != endOfVector)
         {
            osg::ref_ptr<dtDAL::ActorProxy> proxy =
               dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtai.waypoint", "Waypoint").get();

            if (proxy.valid())
            {            
               dtAI::WaypointActor* pActor = dynamic_cast<dtAI::WaypointActor*>(proxy->GetActor());
               assert(pActor);

               osg::Vec3 vec = (*iter).second->GetPosition();

               dtDAL::WaypointActorProxy* pActorProxy = dynamic_cast<dtDAL::WaypointActorProxy*>(proxy.get());
               assert(pActorProxy);

               //note.. this will crash if we dont set the index first
               //cause setting the translation will trigger a callback in our move waypoint function
               pActor->SetIndex(counter);
               pActorProxy->SetTranslation(vec);            

               pMap.AddProxy(*proxy);    
               ++counter;
            }

            ++iter;
         }
      }

      dtAI::WaypointManager::GetInstance()->ReleaseLock();
   }
}
