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
#include <stdio.h>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <osgDB/FileNameUtils>

#include <osgUtil/Optimizer>

#include <dtCore/globals.h>

#include "dtDAL/project.h"
#include "dtDAL/log.h"
#include "dtDAL/map.h"
#include "dtDAL/mapxml.h"
#include "dtDAL/datatype.h"
#include "dtDAL/fileutils.h"
#include "dtDAL/exception.h"
#include "dtDAL/stringtokenizer.h"
#include "dtDAL/librarymanager.h"

namespace dtDAL
{
    const std::string Project::LOG_NAME("Project.cpp");
    const std::string Project::MAP_DIRECTORY("maps");
    const std::string Project::MAP_BACKUP_SUB_DIRECTORY("backups");

    osg::ref_ptr<Project> Project::mInstance(NULL);

    //////////////////////////////////////////////////////////
    Project::Project() : mValidContext(false), mContext(""),
        mContextReadOnly(true), mResourcesIndexed(false)
    {
        MapParser::StaticInit();
        MapXMLConstants::StaticInit();

        mParser = new MapParser;
        mWriter = new MapWriter;
        libraryManager = &LibraryManager::GetInstance();
        mLogger = &Log::GetInstance(Project::LOG_NAME);
    }

    //////////////////////////////////////////////////////////
    Project::~Project() {
        MapXMLConstants::StaticShutdown();
        MapParser::StaticShutdown();
        //make sure the maps get closed before
        //the library manager is deleted
        mOpenMaps.clear();
    }

    //////////////////////////////////////////////////////////
    void Project::SetContext(const std::string& path, bool mOpenReadOnly)
    {
        FileUtils& fileUtils = FileUtils::GetInstance();
        FileType ft = fileUtils.fileInfo(path).fileType;

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

        //unset the current mContext.
        mValidContext = false;
        mContext = "";
        mContextReadOnly = true;

        if (!mOpenReadOnly)
        {
            try
            {
                fileUtils.CreateDirectoryFromPath(path);
                ft = DIRECTORY;
            }
            catch (const Exception& ex)
            {
                std::ostringstream ss;
                ss << "Unable to create directory " << path << ". Error: " << ex.What();
                EXCEPT(ExceptionEnum::ProjectInvalidContext, ss.str());
            }
        }

        if (ft == FILE_NOT_FOUND)
        {
            char* fmt = "Directory %s does not exist";
            int size = strlen(fmt) + path.length();
            char* buffer = new char[size + 1];
            snprintf(buffer, size, fmt, path.c_str());
            std::string s(buffer);
            delete buffer;
            EXCEPT(ExceptionEnum::ProjectInvalidContext, s);
        }

        if (ft == REGULAR_FILE)
        {
            std::string s(path);
            s.append(" is not a directory");
            EXCEPT(ExceptionEnum::ProjectInvalidContext, s);
        }

        //from this point on, we know the path is a valid directory, so we now check the structure.

        std::string pPath = path;
        std::string::iterator last = pPath.end();
        --last;
        if (*last == FileUtils::PATH_SEPARATOR)
            pPath.erase(last);

        fileUtils.PushDirectory(path);

        try
        {
            const DirectoryContents contents = fileUtils.DirGetFiles(".");
            if (contents.empty())
            {
                if (mOpenReadOnly)
                {
                    std::string s(path);
                    s.append(" is not a valid project directory.");
                    EXCEPT(ExceptionEnum::ProjectInvalidContext, s);
                }
                else
                {
                    try
                    {
                        fileUtils.CreateDirectoryFromPath(Project::MAP_DIRECTORY);
                    }
                    catch(const Exception& ex)
                    {
                        std::ostringstream ss;
                        ss << "Unable to create directory " << Project::MAP_DIRECTORY << ". Error: " << ex.What();
                        EXCEPT(ExceptionEnum::ProjectInvalidContext, ss.str());
                    }
                }
            }
            else
            {
                std::set<std::string> contentsSet;
                contentsSet.insert(contents.begin(), contents.end());
                if (!mOpenReadOnly && contentsSet.find(Project::MAP_DIRECTORY) == contentsSet.end())
                    try
                    {
                        fileUtils.CreateDirectoryFromPath(Project::MAP_DIRECTORY);
                    }
                    catch (const Exception& ex)
                    {
                        std::ostringstream ss;
                        ss << "Unable to create directory " << Project::MAP_DIRECTORY << ". Error: " << ex.What();
                        EXCEPT(ExceptionEnum::ProjectInvalidContext, ss.str());
                    }
                else if (fileUtils.fileInfo(Project::MAP_DIRECTORY).fileType != DIRECTORY)
                {
                    std::string s(path);
                    s.append(" is not a valid project directory.  The ");
                    s.append(Project::MAP_DIRECTORY);
                    if (fileUtils.fileInfo(Project::MAP_DIRECTORY).fileType == REGULAR_FILE)
                        s.append(" is not a directory.");
                    else
                        s.append(" does not exist.");
                    EXCEPT(ExceptionEnum::ProjectInvalidContext, s);
                }
            }

            mValidContext = true;
            std::string oldContext = mContext;
            mContext = FileUtils::GetInstance().GetMyCurrentDirectory();
            mContextReadOnly = mOpenReadOnly;

            dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList() + ":" + mContext);

            GetMapNames();
        }
        catch (const Exception& ex)
        {
            FileUtils::GetInstance().PopDirectory();
            throw ex;
        }
        FileUtils::GetInstance().PopDirectory();

    }

    //////////////////////////////////////////////////////////
    void Project::Refresh()
    {
        if (!mValidContext)
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

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
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        if (mMapList.empty())
        {
            mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                               "The list of map names is empty, so the project is preparing to load the list.");
            FileUtils& fileUtils = FileUtils::GetInstance();
            fileUtils.PushDirectory(mContext);

            try
            {
                const DirectoryContents contents = fileUtils.DirGetFiles(Project::MAP_DIRECTORY);

                for (DirectoryContents::const_iterator i = contents.begin(); i < contents.end(); ++i)
                {
                    const std::string& f = *i;
                    std::string fp = Project::MAP_DIRECTORY + FileUtils::PATH_SEPARATOR + f;
                    if (fileUtils.fileInfo(fp).fileType == REGULAR_FILE)
                    {
                        try
                        {
                            const std::string& mapName = mParser->ParseMapName(fp);
                            mMapList.insert(make_pair(mapName, f));
                        }
                        catch (const Exception& e)
                        {
                            std::string error = "Unable to parse " + fp + " with error " + e.What();
                            mLogger->LogMessage(Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
                        }
                    }
                }
            }
            catch (const Exception& ex)
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
        FileUtils& fileUtils = FileUtils::GetInstance();
        fileUtils.PushDirectory(this->mContext);

        Map* map = NULL;
        try
        {
            if (fileUtils.fileInfo(fullPath).fileType != REGULAR_FILE)
                EXCEPT(ExceptionEnum::ProjectFileNotFound,
                    std::string("Map file \"") + fullPath + "\" not found.");

            map = mParser->Parse(fullPath);

            if (map == NULL)
            {
                EXCEPT(ExceptionEnum::MapLoadParsingError,
                    "Map loading didn't throw an exception, but the result is NULL");
            }

            mOpenMaps.insert(std::make_pair(name, osg::ref_ptr<Map>(map)));

            //Clearing the modified flag must be done because setting the
            //map properties at load will make the map look modified.
            //it must be done before adding the missing libraries and proxy
            //classes because clearing the modified flag clears those lists.
            if (clearModified)
                map->ClearModified();

            map->AddMissingLibraries(mParser->GetMissingLibraries());
            map->AddMissingActorTypes(mParser->GetMissingActorTypes());

        }
        catch (const Exception& e)
        {
            std::string error = "Unable to parse " + fullPath + " with error " + e.What();
            mLogger->LogMessage(Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
            fileUtils.PopDirectory();
            throw e;
        }
        fileUtils.PopDirectory();
        return *map;
    }

    //////////////////////////////////////////////////////////
    Map& Project::GetMap(const std::string& name)
    {
        std::map<std::string, osg::ref_ptr<Map> >::iterator openMapI = mOpenMaps.find(name);

        //map is already open.
        if (openMapI != mOpenMaps.end())
            return *(openMapI->second);

        std::map<std::string,std::string>::iterator i = mMapList.find(name);

        if (i == mMapList.end())
            EXCEPT(ExceptionEnum::ProjectFileNotFound,
                std::string("Map named ") + name + " does not exist.");

        const std::string& mapFileName = i->second;

        const std::string& fp = Project::MAP_DIRECTORY + FileUtils::PATH_SEPARATOR + mapFileName;

        Map& map = InternalLoadMap(name, fp, true);

        map.SetFileName(mapFileName);
        return map;
    }
    //////////////////////////////////////////////////////////
    Map& Project::OpenMapBackup(const std::string& name)
    {
        std::map<std::string, osg::ref_ptr<Map> >::iterator openMapI = mOpenMaps.find(name);

        //map is already open.
        if (openMapI != mOpenMaps.end())
        {
            //close the map if it's open.
            mOpenMaps.erase(openMapI);
        }

        std::map<std::string,std::string>::iterator i = mMapList.find(name);

        if (i == mMapList.end())
            EXCEPT(ExceptionEnum::ProjectFileNotFound,
                std::string("Map named ") + name + " does not exist.");

        const std::string& mapFileName = i->second;

        const std::string& fp = GetBackupDir() + FileUtils::PATH_SEPARATOR + mapFileName + ".backup";

        Map& map = InternalLoadMap(name, fp, false);
        map.SetFileName(mapFileName);
        map.SetSavedName(name);
        return map;

    }
    //////////////////////////////////////////////////////////
    Map& Project::CreateMap(const std::string& name, const std::string& fileName)
    {
        if (IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

        if (name == "")
            EXCEPT(ExceptionEnum::ProjectException,
                "Maps may not have an empty name.");

        if (fileName == "")
            EXCEPT(ExceptionEnum::ProjectException, std::string("Maps may not have an empty fileName."));

        //assign it to a refptr so that if I except, it will get deleted
        osg::ref_ptr<Map> map(new Map(fileName, name));

        for (std::map<std::string, std::string>::iterator i = mMapList.begin(); i != mMapList.end(); ++i)
        {
            if (i->first == name)
            {
                EXCEPT(ExceptionEnum::ProjectException,
                    std::string("Map named ") + name + " already exists.");
            }
            else if (i->second == map->GetFileName())
            {
                EXCEPT(ExceptionEnum::ProjectException,
                    std::string("A map with file name ") + fileName + " already exists.");
            }
        }


        InternalSaveMap(*map);

        mOpenMaps.insert(make_pair(name, osg::ref_ptr<Map>(map.get())));
        //The map can add extensions and such to the file name, so it
        //must be fetched back from the map object before being added to the name-file map.
        mMapList.insert(make_pair(name, map->GetFileName()));
        mMapNames.insert(name);

        return *map;

    }

    //////////////////////////////////////////////////////////
    void Project::LoadMapIntoScene(Map& map, dtCore::Scene& scene, bool addBillBoards)
    {
        CheckMapValidity(map, true);
        std::vector<osg::ref_ptr<ActorProxy> > container;
        map.GetAllProxies(container);
        for (std::vector<osg::ref_ptr<ActorProxy> >::iterator i = container.begin();
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
                    if (billBoard == NULL)
                    {
                        mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
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
                    //the proxy.
                }
            }
            else
            {
                //if we aren't drawing billboards, then the actors should always be in the scene.
                scene.AddDrawable(proxy.GetActor());
            }
        }

        osgUtil::Optimizer o;
        o.optimize(scene.GetSceneNode(), osgUtil::Optimizer::DEFAULT_OPTIMIZATIONS);
            //| osgUtil::Optimizer::SPATIALIZE_GROUPS);
    }

    //////////////////////////////////////////////////////////
    void Project::UnloadUnusedLibraries(Map& mapToClose)
    {
        //clear the proxies to make sure they are all freed.
        //one should not save AFTER calling this.
        mapToClose.ClearProxies();
        for (std::vector<std::string>::const_iterator i = mapToClose.GetAllLibraries().begin();
            i != mapToClose.GetAllLibraries().end(); ++i)
        {
            std::string libToClose = *i;
            bool libMayClose = true;
            for (std::map<std::string, osg::ref_ptr<Map> >::const_iterator j = mOpenMaps.begin(); j != mOpenMaps.end(); ++j)
            {
                const Map& toCheck = *j->second;
                if (&mapToClose != &toCheck && toCheck.HasLibrary(libToClose))
                {
                    if (mLogger->IsLevelEnabled(Log::LOG_INFO)) {
                        mLogger->LogMessage(Log::LOG_INFO, __FUNCTION__, __LINE__,
                            "Not unloading library %s because it is used by open map named %s.",
                            libToClose.c_str(), toCheck.GetName().c_str());
                    }
                    libMayClose = false;
                    break;
                }
            }
            if (libMayClose)
            {
                if (mLogger->IsLevelEnabled(Log::LOG_INFO))
                {
                    mLogger->LogMessage(Log::LOG_INFO, __FUNCTION__, __LINE__,
                        "About to unload library named %s.", libToClose.c_str());
                }
                LibraryManager::GetInstance().UnloadActorRegistry(libToClose);
            }

        }
    }

    //////////////////////////////////////////////////////////
    void Project::CloseMap(Map& map, bool unloadLibraries)
    {
        if (!mValidContext)
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        //bool
        std::map<std::string, osg::ref_ptr<Map> >::iterator j = mOpenMaps.find(map.GetSavedName());
        if (j == mOpenMaps.end() || (j->second.get() != &map))
        {
            EXCEPT(ExceptionEnum::ProjectInvalidContext,
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
            catch (const Exception& ex)
            {
                mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__, "Error clearing map backups when saving: %s", ex.What().c_str());
            }
            mOpenMaps.erase(j);
        }
    }

    //////////////////////////////////////////////////////////
    void Project::DeleteMap(Map& map, bool unloadLibraries)
    {
        if (!mValidContext)
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        if (IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

        CloseMap(map, unloadLibraries);

        std::string mapFileName = map.GetFileName();

        std::map<std::string, std::string>::iterator i = mMapList.find(map.GetSavedName());
        if (i == mMapList.end())
        {
            mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__, __LINE__,
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
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        if (IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

        std::map<std::string, osg::ref_ptr<Map> >::iterator j = mOpenMaps.find(mapName);
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
            EXCEPT(ExceptionEnum::ProjectFileNotFound, std::string("No such map: \"") + mapName + "\"");
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

        FileUtils& fileUtils = FileUtils::GetInstance();
        fileUtils.PushDirectory(this->mContext + FileUtils::PATH_SEPARATOR + Project::MAP_DIRECTORY);
        try
        {
            if (fileUtils.FileExists(mapFileName))
            {
                fileUtils.FileDelete(mapFileName);
            }
            else
            {
                mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__, __LINE__,
                    "Specified map was part of the project, but the map file did not exist.");
            }
        }
        catch (const Exception& ex)
        {
            fileUtils.PopDirectory();
            throw ex;
        }
        fileUtils.PopDirectory();
    }

    //////////////////////////////////////////////////////////
    void Project::SaveMap(Map& map)
    {
        CheckMapValidity(map);
        InternalSaveMap(map);
    }

    //////////////////////////////////////////////////////////
    void Project::SaveMapAs(const std::string& mapName, const std::string& newName, const std::string& newFileName)
    {
        if (!mValidContext)
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        if (IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

        //The map must be loaded to do a saveAs, so we call getMap();
        SaveMapAs(GetMap(mapName), newName, newFileName);

    }
    //////////////////////////////////////////////////////////
    void Project::SaveMapAs(Map& map, const std::string& newName, const std::string& newFileName)
    {
        CheckMapValidity(map);

        if (map.GetSavedName() == newName)
            EXCEPT(ExceptionEnum::ProjectException, std::string("Map named ")
                + map.GetSavedName() + " cannot be saved again as the same name");

        std::string newFileNameCopy(newFileName);
        std::string currentFileName(map.GetFileName());
        //compare the file name without the extension.
        if (currentFileName.substr(0, currentFileName.size() - Map::MAP_FILE_EXTENSION.size())
            == newFileNameCopy)
            EXCEPT(ExceptionEnum::ProjectException, std::string("Map named ") + map.GetSavedName()
                + " cannot be saved as a different map with the same file name.");

        for (std::map<std::string,std::string>::const_iterator i = mMapList.begin();
            i != mMapList.end(); ++i )
        {
            if (newFileNameCopy == i->second.substr(0, i->second.size() - Map::MAP_FILE_EXTENSION.size()))
                EXCEPT(ExceptionEnum::ProjectException, std::string("Map named ")
                    + map.GetSavedName() + " cannot be saved with file name "
                    + newFileName + " because it matches another map.");
        }

        std::string oldMapName = map.GetSavedName();
        std::string oldFileName = map.GetFileName();

        map.SetName(newName);
        map.SetFileName(newFileNameCopy);

        InternalSaveMap(map);
        //re-add the old map to the list of saved maps
        //since saving with a new name will remove the old entry.
        mMapList.insert(std::make_pair(oldMapName, oldFileName));
        mMapNames.insert(oldMapName);

        try
        {
            ClearBackup(oldMapName);
        }
        catch (const Exception& ex)
        {
            mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__, "Error clearing map backups when saving %s as %s: %s",
                oldMapName.c_str(), newName.c_str(), ex.What().c_str());
        }
    }

    //////////////////////////////////////////////////////////
    void Project::SaveMap(const std::string& mapName)
    {
        if (!mValidContext)
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        if (IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

        std::map<std::string, osg::ref_ptr<Map> >::iterator j = mOpenMaps.find(mapName);
        if (j == mOpenMaps.end())
        {
            return; //map is not in memory, so it doesn't need to be saved.
        }

        InternalSaveMap(*(j->second));
    }

    //////////////////////////////////////////////////////////
    void Project::CheckMapValidity(const Map& map, bool readonlyAllowed) const
    {
        if (!mValidContext)
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        if (!readonlyAllowed && IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

        std::map<std::string, std::string>::const_iterator i = mMapList.find(map.GetSavedName());
        if (i == mMapList.end())
        {
            EXCEPT(ExceptionEnum::ProjectFileNotFound, std::string("No such map: \"") + map.GetSavedName() + "\"");
        }

        std::map<std::string, osg::ref_ptr<Map> >::const_iterator j = mOpenMaps.find(map.GetSavedName());

        if (j == mOpenMaps.end())
        {
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("A map named \"") + map.GetSavedName()
                + "\" exists but is not currently open.");

        }
        else if (j->second.get() != &map)
        {
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("A map named \"") + map.GetSavedName()
                + "\" exists, but this is not the instance.");
        }
    }

    //////////////////////////////////////////////////////////
    void Project::InternalSaveMap(Map& map)
    {
        MapWriter& mw = *mWriter;

        if (map.GetSavedName() != map.GetName())
        {
            if (mMapList.find(map.GetName()) != mMapList.end())
            {
                EXCEPT(ExceptionEnum::ProjectException, "You may not save a map with a name that matches another map.");
            }
        }

        std::string fullPath = Project::MAP_DIRECTORY + FileUtils::PATH_SEPARATOR + map.GetFileName();
        std::string fullPathSaving = fullPath + ".saving";

        FileUtils& fileUtils = FileUtils::GetInstance();
        fileUtils.PushDirectory(mContext);
        try
        {
            //save the file to a separate name first so that
            //it won't blast the old one unless it is successful.
            mw.Save(map, fullPathSaving);
            //if it's successful, move it to the final file name
            fileUtils.FileMove(fullPathSaving, fullPath, true);
        }
        catch (const Exception& e)
        {
            mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__, e.What().c_str());
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

            osg::ref_ptr<Map> holder(&map);

            mOpenMaps.erase(mOpenMaps.find(map.GetSavedName()));
            mOpenMaps.insert(make_pair(map.GetName(), holder));
        }

        map.ClearModified();

        try
        {
            ClearBackup(map.GetSavedName());
        }
        catch (const Exception& ex)
        {
            //if the map in new, the following exception will be thrown
            //so don't print an error in that case.
            if (ex.TypeEnum() != ExceptionEnum::ProjectFileNotFound)
                mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
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

        std::string path = backupDir + FileUtils::PATH_SEPARATOR + map.GetFileName();

        FileUtils& fileUtils = FileUtils::GetInstance();
        fileUtils.PushDirectory(mContext);
        try
        {
            if (!fileUtils.DirExists(backupDir))
            {
                fileUtils.CreateDirectoryFromPath(backupDir);
            }

            std::string fileName = path + ".backupsaving";
            std::string finalFileName = path + ".backup";

            //save the file to a "saving" file so that if it blows or is killed while saving, the data
            //will not be lost.
            mw.Save(map, fileName);

            //when it completes, move the file to the final name.
            fileUtils.FileMove(fileName, finalFileName, true);
        }
        catch (const Exception& e)
        {
            mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__, e.What().c_str());
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
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        std::map< std::string, std::string >::const_iterator found = mMapList.find(mapName);
        if (found == mMapList.end())
            EXCEPT(ExceptionEnum::ProjectFileNotFound, std::string("No such map: \"") + mapName + "\"");

        const std::string& fileName = found->second;

        std::string backupDir = GetContext() + FileUtils::PATH_SEPARATOR + GetBackupDir();

        const std::string& backupFileName = backupDir + FileUtils::PATH_SEPARATOR + fileName + ".backup";

        return FileUtils::GetInstance().FileExists(backupFileName);
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
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        if (IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

        std::map< std::string, std::string >::iterator found = mMapList.find(mapName);
        if (found == mMapList.end())
            EXCEPT(ExceptionEnum::ProjectFileNotFound, std::string("No such map: \"") + mapName + "\"");

        std::string& fileName = found->second;

        std::string backupDir = GetContext() + FileUtils::PATH_SEPARATOR + GetBackupDir();

        FileUtils& fileUtils = FileUtils::GetInstance();

        if (!fileUtils.DirExists(backupDir))
           return;

        DirectoryContents dc = fileUtils.DirGetFiles(backupDir);

        size_t fileNameSize = fileName.size();
        for (DirectoryContents::const_iterator i = dc.begin(); i != dc.end(); ++i)
        {
            const std::string& file = *i;
            if (file.size() > fileNameSize
                && file.substr(0, fileNameSize) == fileName)
            {

                if (fileUtils.fileInfo(backupDir + FileUtils::PATH_SEPARATOR + file).fileType == REGULAR_FILE)
                    fileUtils.FileDelete(backupDir + FileUtils::PATH_SEPARATOR + file);
            }
        }

    }

    //////////////////////////////////////////////////////////
    void Project::GetHandlersForDataType(const DataType& resourceType, std::vector<osg::ref_ptr<const ResourceTypeHandler> >& toFill) const
    {
        mResourceHelper.GetHandlersForDataType(resourceType, toFill);
    }

    //////////////////////////////////////////////////////////
    void Project::RegisterResourceTypeHander(ResourceTypeHandler& handler)
    {
        mResourceHelper.RegisterResourceTypeHander(handler);
    }

    //////////////////////////////////////////////////////////
    const std::string Project::GetResourcePath(const ResourceDescriptor& resource) const
    {
        if (!mValidContext)
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        const std::string& path = mResourceHelper.GetResourcePath(resource);


        FileUtils& fileUtils = FileUtils::GetInstance();
        fileUtils.PushDirectory(this->mContext);

        try
        {
            FileType ftype = fileUtils.fileInfo(path).fileType;

            if (ftype != REGULAR_FILE)
            {
                if (ftype == FILE_NOT_FOUND)
                {
                    EXCEPT(ExceptionEnum::ProjectFileNotFound,
                        std::string("The specified resource was not found: ") + path);
                }
                else if (ftype == DIRECTORY)
                {
                    EXCEPT(ExceptionEnum::ProjectResourceError,
                        std::string("The resource identifier does not specify a resource file: ") + path);
                }
            }
        }
        catch (const Exception& ex)
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
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        if (IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

        if (!type.IsResource())
        {
            EXCEPT(ExceptionEnum::ProjectResourceError, std::string("Unable to add resource of type ") + type.GetName()
                    + ". It is not a resource type.");
        }

        FileUtils& fileUtils = FileUtils::GetInstance();
        fileUtils.PushDirectory(mContext);

        try
        {
            core::tree<ResourceTreeNode>* categoryInTree;
            core::tree<ResourceTreeNode>* dataTypeTree = NULL;

            if (mResourcesIndexed)
                dataTypeTree = &GetResourcesOfType(type);

            mResourceHelper.CreateResourceCategory(category, type, dataTypeTree, categoryInTree);
        }
        catch (const dtDAL::Exception& ex)
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
            EXCEPT(ExceptionEnum::ProjectInvalidContext,
                std::string("The context is not valid."));

        if (IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly,
                std::string("The context is readonly."));

        if (!type.IsResource())
        {
            EXCEPT(ExceptionEnum::ProjectResourceError, std::string("Unable to add resource of type ") + type.GetName()
                    + ". It is not a resource type.");
        }

        FileUtils& fileUtils = FileUtils::GetInstance();
        fileUtils.PushDirectory(mContext);

        bool result;

        try
        {
            core::tree<ResourceTreeNode>* dataTypeTree = NULL;
            if (mResourcesIndexed)
                dataTypeTree = &GetResourcesOfType(type);

            result = mResourceHelper.RemoveResourceCategory(category, type, recursive, dataTypeTree);
        }
        catch (const dtDAL::Exception& ex)
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
            EXCEPT(ExceptionEnum::ProjectInvalidContext,
                std::string("The context is not valid."));

        if (IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly,
                std::string("The context is readonly."));

        if (!type.IsResource())
        {
            EXCEPT(ExceptionEnum::ProjectResourceError,
                std::string("Unable to add resource of type ") + type.GetName()
                    + ". It is not a resource type.");
        }

        FileUtils& fileUtils = FileUtils::GetInstance();
        fileUtils.PushDirectory(mContext);

        ResourceDescriptor result;
        try
        {
            core::tree<ResourceTreeNode>* dataTypeTree = NULL;
            if (mResourcesIndexed)
                dataTypeTree = &GetResourcesOfType(type);

            result = mResourceHelper.AddResource(newName, pathToFile, category, type, dataTypeTree);

        }
        catch (const dtDAL::Exception& ex)
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
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));
        if (IsReadOnly())
            EXCEPT(ExceptionEnum::ProjectReadOnly, std::string("The context is readonly."));

        FileUtils& fileUtils = FileUtils::GetInstance();
        fileUtils.PushDirectory(mContext);
        try
        {

            core::tree<ResourceTreeNode>* resourceTree = NULL;
            if (mResourcesIndexed)
                resourceTree = &mResources;

            mResourceHelper.RemoveResource(resource, resourceTree);
        }
        catch (const Exception& ex)
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

        FileUtils& fileUtils = FileUtils::GetInstance();
        fileUtils.PushDirectory(GetContext());
        try
        {
            mResources.clear();
            mResourceHelper.IndexResources(mResources);
        }
        catch (const Exception& ex)
        {
            fileUtils.PopDirectory();
            throw ex;
        }
        fileUtils.PopDirectory();

        mResourcesIndexed = true;
    }

    //////////////////////////////////////////////////////////
    core::tree<ResourceTreeNode>& Project::GetResourcesOfType(const DataType& dataType) const
    {

        if (!mResourcesIndexed)
            IndexResources();

        ResourceTreeNode tr(dataType.GetName(), "");
        core::tree<ResourceTreeNode>::iterator it = mResources.find(tr);

        if (it  == mResources.end())
        {
            mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
                "ERROR: call getResourcesOfType ended with no valid results!");
            EXCEPT(ExceptionEnum::ProjectException, "ERROR: call GetResourcesOfType ended with no valid results!");
        }

        return it.tree_ref();

    }

    //////////////////////////////////////////////////////////
    void Project::GetResourcesOfType(const DataType& type, core::tree<ResourceTreeNode>& toFill) const
    {
        if (!mValidContext)
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));

        toFill.clear();
        if (!type.IsResource())
            return;

        toFill = GetResourcesOfType(type);

    }

//     void Project::findmResources(core::tree<ResourceTreeNode> toFill,
//         const DataType& type,
//         const std::string& partialName,
//         const std::string& extension) const {
//
//         toFill.clear();
//
//         core::tree<ResourceTreeNode>::const_iterator it = mResources.begin();
//         for (; it != mResources.end(); ++it) {
//             if (it->isCategory() && it->getNodeText() == type.getName()) {
//
//                 core::tree<ResourceTreeNode>* matchingBranch = getMatchingBranch(it.in(), type, partialName, extension);
//
//                 //core::tree<ResourceTreeNode>*
//             }
//         }
//     }


//Later
/*    core::tree<ResourceTreeNode>* Project::getMatchingBranch(
        core::tree<ResourceTreeNode>::const_iterator level,
        const DataType& type,
        const std::string& partialName,
        const std::string& extension) const {

        core::tree<ResourceTreeNode>::iterator it = level;
        for (; it != mResources.end(); ++it) {
            if (it->isCategory() && it->getNodeText() == type.getName()) {

                core::tree<ResourceTreeNode>* matchingBranch = getMatchingBranch(it.in(), type, partialName, extension);


            } else if ((extension != "" && it->getResource().getExtension() == extension) &&
                        (partialName != "" && it->getResource().getResourceName().find(partialName) < std::string::npos) ){
                core::tree<ResourceTreeNode>* node = new core::tree<ResourceTreeNode>;
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
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));
        return false;
    }

    //////////////////////////////////////////////////////////
    bool Project::IsReadOnly() const
    {
        if (!mValidContext)
            EXCEPT(ExceptionEnum::ProjectInvalidContext, std::string("The context is not valid."));
        return mContextReadOnly;
    }

}
