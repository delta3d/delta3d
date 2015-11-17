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
 * David Guthrie
 */

#include <prefix/dtcoreprefix.h>
#include <string>
#include <sstream>
#include <set>
#include <cassert>

#include <osgDB/FileNameUtils>

#include <dtCore/scene.h>

#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/wrapperosgobject.h>

#include <dtCore/project.h>
#include <dtCore/projectconfig.h>
//just to init the constants.  being consistent on the way it works.
#include <dtCore/projectconfigxmlhandler.h>
#include <dtCore/map.h>
#include <dtCore/mapxml.h>
#include <dtCore/datatype.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/actorfactory.h>
#include <dtCore/actorproxyicon.h>
#include <dtCore/environmentactor.h>
#include <dtCore/gameevent.h>
#include <dtCore/actorproxy.h>
#include <dtCore/resourcedescriptor.h>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

namespace dtCore
{
   const std::string Project::LOG_NAME("project.cpp");
   const std::string Project::MAP_DIRECTORY("maps");
   const std::string Project::MAP_BACKUP_SUB_DIRECTORY(".backups");

   struct MapFileData
   {
      std::string mOrigName;
      std::string mFileName;
      Project::ContextSlot mSlotId;
   };

   class ProjectImpl {
   public:
      ProjectImpl()
      : mContextReadOnly(true)
      , mResourcesIndexed(false)
      , mEditMode(false)
      {
         libraryManager = &ActorFactory::GetInstance();
         mLogger = &dtUtil::Log::GetInstance(Project::LOG_NAME);
      }

      ~ProjectImpl()
      {
         //make sure the maps get closed before
         //the library manager is deleted
         mOpenMaps.clear();
         mMapTree.clear();
      }

      std::vector<std::string> mContexts;

      bool mContextReadOnly;
      mutable bool mResourcesIndexed;

      //set to true if we are running via stage - banderegg
      bool mEditMode;

      typedef std::map<std::string, MapFileData> MapListType;
      MapListType mMapList; //< The list of maps by name mapped to the file names.
      mutable std::set<std::string> mMapNames; //< The list of map names.
      dtCore::Project::MapTreeData mMapTree;

      std::map<std::string, dtCore::RefPtr<Map> > mOpenMaps; //< A vector of the maps currently loaded.
      mutable Project::ResourceTree mResources; //< a tree of all the resources.  This is more of a cache.

      dtCore::RefPtr<MapParser> mParser;

      //This is here to make sure the library manager is deleted AFTER the maps are closed.
      //so that libraries won't be closed and the proxies deleted out from under the map.
      dtCore::RefPtr<ActorFactory> libraryManager;
      ResourceHelper mResourceHelper;

      dtUtil::Log* mLogger;

      // Internal context add that doesn't refresh
      Project::ContextSlot InternalAddContext(const std::string& path);
      // Internal context remove that doesn't refresh
      void InternalRemoveContext(Project::ContextSlot slot);

      std::string InternalSaveMapOrPrefab(Map& map, const std::string& categoryPath, Project::ContextSlot slot, bool prefab);

      //internal handling for saving a map.
      void InternalSaveMap(Map& map, Project::ContextSlot slot);
      //internal handling for deleting a map.
      void InternalDeleteMap(const MapFileData& mapFileData);

      //internal handling for loading a map.
      Map& InternalLoadMap(const MapFileData& fileData, bool backup, bool clearModified);

      MapPtr InternalLoadPrefab(const std::string& fullPath, dtCore::ActorRefPtrVector& actorsOut);

      //internal handling of closing a sincle map.
      void InternalCloseMap(Map& map, bool unloadLibraries);

      void UnloadUnusedLibraries(Map& mapToClose);

      //internal method to get the pointer to the subtree for a given datatype.
      Project::ResourceTree& GetResourcesOfType(const DataType& datatype) const;
      //Checks to see if a map is actually one the current project knows about.
      Project::ContextSlot CheckMapValidity(const Map& map, bool allowReadonly = false) const;
      //re-reads the map names from project.
      void ReloadMapNames() const;
      //indexes all the resources in the project.
      void IndexResources() const;
      //recursive helper method for the other indexResources
      //The category AND the categoryPath are passed so that
      //they won't have to be converted on every recursive call.
      void IndexResources(dtUtil::FileUtils& fileUtils, Project::ResourceTree::iterator& i,
               const DataType& dt, const std::string& categoryPath,const std::string& category) const;

      //Gets the list of backup map files.
      void GetBackupMapFilesList(dtUtil::DirectoryContents& toFill) const;
      void ListMapsForContextDir(Project::ContextSlot slot);
      void ListMapsForDir(const std::string& mapsFolder, dtCore::Project::MapTreeData& treeData, dtUtil::FileExtensionList& extensions, dtCore::RefPtr<MapParser> parser, Project::ContextSlot slot);

      void GenerateMapList();

      //searches the resource tree for a category node and returns
      //an iterator to it or resources.end() if not found.
      Project::ResourceTree::iterator FindTreeNodeFromCategory(
               const DataType* dt, const std::string& category) const;


      const std::string GetBackupDir() const;

      dtUtil::FileInfo GetMapsDirectory(const std::string& contextPath, bool createIfNeeded);

      //dtUtil::FileInfo GetMapsDirectory(bool createIfNeeded);

      // for the get context method.
      static const std::string EMPTY_STRING;

      //Later
      /*    Project::ResourceTree* getMatchingBranch(
         Project::ResourceTree::iterator level,
         const DataType& type,
         const std::string& partialName,
         const std::string& extension) const;*/
   };
   const std::string ProjectImpl::EMPTY_STRING;

   dtCore::RefPtr<Project> Project::mInstance(NULL);

   /////////////////////////////////////////////////////////////////////////////
   Project::Project()
   : mImpl(new ProjectImpl())
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   Project::~Project()
   {
      delete mImpl;
      mImpl = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   Project::Project(const Project&) {}

   /////////////////////////////////////////////////////////////////////////////
   Project& Project::operator=(const Project&)
   {
      return *this;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::SetupFromProjectConfig(const ProjectConfig& config)
   {
      ClearAllContexts();

      SetReadOnly(config.GetReadOnly());

      try
      {
         dtUtil::DirectoryPush dPush(config.GetBasePath());
         // Should be relative to the base path.
         for (unsigned i = 0; i < config.GetNumContextData(); ++i)
         {
            AddContext(config.GetContextData(i).GetPath());
         }
      }
      catch (const dtUtil::Exception&)
      {
         // Clear everything if any part of the config fails.
         ClearAllContexts();
         throw;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ProjectConfig> Project::SetupFromProjectConfigFile(const std::string& path)
   {
      dtCore::RefPtr<ProjectConfig> result = LoadProjectConfigFile(path);
      if (result.valid())
      {
         SetupFromProjectConfig(*result);
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ProjectConfig> Project::LoadProjectConfigFile(const std::string& path)
   {
      if (!dtUtil::FileUtils::GetInstance().FileExists(path))
      {
         throw XMLLoadParsingException(std::string("Unable to save project config file \"") + path + "\", the file already exists.", __FILE__, __LINE__);
      }

      dtCore::RefPtr<osgDB::ReaderWriter::Options> options = NULL;
      if (osgDB::Registry::instance()->getOptions() != NULL)
      {
         options = dynamic_cast<osgDB::ReaderWriter::Options*>(osgDB::Registry::instance()->getOptions()->clone(osg::CopyOp::DEEP_COPY_ALL));
      }
      else
      {
         options = new osgDB::ReaderWriter::Options();
      }

      options->setObjectCacheHint(osgDB::ReaderWriter::Options::CacheHintOptions(options->getObjectCacheHint() | osgDB::ReaderWriter::Options::CACHE_ARCHIVES));

      dtCore::RefPtr<osg::Object> result = osgDB::readObjectFile(path, options.get());
      if (result.valid())
      {
         ProjectConfig* pc = dynamic_cast<ProjectConfig*>(result->getUserData());
         if (pc != NULL)
         {
            return pc;
         }
      }

      throw XMLLoadParsingException(std::string("Unable to load project config file \"") + path + "\".", __FILE__, __LINE__);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::SaveProjectConfigFile(ProjectConfig& projectConfig, const std::string& path)
   {
      if (dtUtil::FileUtils::GetInstance().FileExists(path))
      {
         throw ProjectConfigSaveException(std::string("Unable to save project config file \"") + path + "\", the file already exists.", __FILE__, __LINE__);
      }

      dtCore::RefPtr<osgDB::ReaderWriter::Options> options = NULL;
      if (osgDB::Registry::instance()->getOptions() != NULL)
      {
         options = dynamic_cast<osgDB::ReaderWriter::Options*>(osgDB::Registry::instance()->getOptions()->clone(osg::CopyOp::DEEP_COPY_ALL));
      }
      else
      {
         options = new osgDB::ReaderWriter::Options();
      }

      options->setObjectCacheHint(osgDB::ReaderWriter::Options::CacheHintOptions(options->getObjectCacheHint() | osgDB::ReaderWriter::Options::CACHE_ARCHIVES));

      dtCore::RefPtr<dtUtil::WrapperOSGObject> wrapObj = new dtUtil::WrapperOSGObject;
      wrapObj->setUserData(&projectConfig);
      if (!osgDB::writeObjectFile(*wrapObj, path, options))
      {
         throw ProjectConfigSaveException(std::string("Unable to save project config file \"") + path + "\".", __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::CreateContext(const std::string& path, bool createMapsDir)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      dtUtil::FileType ft = fileUtils.GetFileInfo(path).fileType;
      if (ft == dtUtil::FILE_NOT_FOUND)
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
            throw dtCore::ProjectInvalidContextException(ss.str(), __FILE__, __LINE__);
         }
      }
      else if (ft == dtUtil::REGULAR_FILE)
      {
         std::string s(path);
         s.append(" is not a directory");
         throw dtCore::ProjectInvalidContextException(s, __FILE__, __LINE__);
      }

      // from this point on, we know we have a valid directory
      std::string pPath = path;
      std::string::iterator last = pPath.end();
      --last;

      if (*last == dtUtil::FileUtils::PATH_SEPARATOR)
      {
         pPath.erase(last);
      }

      mImpl->GetMapsDirectory(path, createMapsDir);

   }

   /////////////////////////////////////////////////////////////////////////////
   bool Project::IsContextValid(ContextSlot slot) const
   {
      if (slot == DEFAULT_SLOT_VALUE)
      {
         return !mImpl->mContexts.empty();
      }

      return GetContextSlotCount()  > slot;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::SetContext(const std::string& path, bool openReadOnly)
   {
      ClearAllContexts();

      SetReadOnly(openReadOnly);

      AddContext(path);
   }

   /////////////////////////////////////////////////////////////////////////////
   Project::ContextSlot Project::AddContext(const std::string& path)
   {
      Project::ContextSlot slot = mImpl->InternalAddContext(path);
      Refresh();
      return slot;
   }

   /////////////////////////////////////////////////////////////////////////////
   Project::ContextSlot ProjectImpl::InternalAddContext(const std::string& path)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      dtUtil::FileType ft = fileUtils.GetFileInfo(path).fileType;

      if (ft == dtUtil::FILE_NOT_FOUND)
      {
         std::ostringstream ss;
         ss << "Directory \"" << path << "\" does not exist";
         throw dtCore::ProjectInvalidContextException(ss.str(), __FILE__, __LINE__);
      }

      if (ft == dtUtil::REGULAR_FILE)
      {
         std::string s(path);
         s.append(" is not a directory");
         throw dtCore::ProjectInvalidContextException(s, __FILE__, __LINE__);
      }

      //from this point on, we know the path is a valid directory, so we now check the structure.

      std::string pPath = path;
      std::string::iterator last = pPath.end();
      --last;

      if (*last == dtUtil::FileUtils::PATH_SEPARATOR)
      {
         pPath.erase(last);
      }

      GetMapsDirectory(path, false);

      dtUtil::DirectoryPush dp(path);

      // Check not to see that the resource directories exist, but that they aren't regular files
      // and can therefore be created if needed.
      dtCore::DataType::EnumerateListType::const_iterator i, iend;
      i = dtCore::DataType::EnumerateType().begin();
      iend = dtCore::DataType::EnumerateType().end();
      for (;i != iend; ++i)
      {
         dtCore::DataType& dt = **i;
         if (dt.IsResource())
         {
            if (fileUtils.GetFileInfo(dt.GetName()).fileType == dtUtil::REGULAR_FILE)
            {
               std::string s(path);
               s.append(" is not a valid project directory.  The ");
               s.append(dt.GetName());
               s.append(" file is not a directory.  It doesn't have to exist, but it may not be a regular file.");
            }
         }
      }

      mContexts.push_back(dtUtil::FileUtils::GetInstance().CurrentDirectory());
      const std::string& context = mContexts.back();
      std::string searchPath = dtUtil::GetDataFilePathList();

      if (searchPath.empty())
      {
         searchPath = dtUtil::GetDeltaDataPathList();
      }

      dtUtil::SetDataFilePathList(searchPath + ':' + context);

      return mContexts.size() - 1U;

   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::RemoveContext(ContextSlot slot)
   {
      if (slot < GetContextSlotCount() )
      {
         mImpl->InternalRemoveContext(slot);
         Refresh();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ProjectImpl::InternalRemoveContext(Project::ContextSlot slot)
   {
      if (slot < mContexts.size())
      {
         //remove the old context from the data file path list.
         // TODO, this has a potential threading issue.  The get and set data path lock internally, but it could
         // be changed while the string manipulation is happening.
         std::string searchPath = dtUtil::GetDataFilePathList();
         const std::string& oldContext = mContexts[slot];
         size_t index = oldContext.empty() ? std::string::npos : searchPath.find(oldContext);
         if (index != std::string::npos)
         {
            searchPath.erase(index, oldContext.size());
            dtUtil::SetDataFilePathList(searchPath);
         }
         mContexts.erase(mContexts.begin() + slot);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::ClearAllContexts()
   {
      mImpl->mOpenMaps.clear();
      //clear the references to all the open maps
      mImpl->mMapList.clear();
      mImpl->mMapNames.clear();

      //clear out the list of mResources.
      mImpl->mResources.clear();
      mImpl->mResourcesIndexed = false;

      while (!mImpl->mContexts.empty())
      {
         mImpl->InternalRemoveContext(0);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& Project::GetContext(Project::ContextSlot slot) const
   {
      if (slot == DEFAULT_SLOT_VALUE)
      {
         slot = 0U;
      }

      if (slot < GetContextSlotCount())
      {
         return mImpl->mContexts[slot];
      }

      return ProjectImpl::EMPTY_STRING;
   }

   /////////////////////////////////////////////////////////////////////////////
   size_t Project::GetContextSlotCount() const
   {
      return mImpl->mContexts.size();
   }

   /////////////////////////////////////////////////////////////////////////////
   Project::ContextSlot Project::GetContextSlotForPath(const std::string& path) const
   {
      ContextSlot result = DEFAULT_SLOT_VALUE;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      std::string pathAbs = path;
      if (!fileUtils.IsAbsolutePath(path))
      {
         pathAbs = fileUtils.GetAbsolutePath(path);
      }

      unsigned slotCount = GetContextSlotCount();

      for (unsigned i = 0; i < slotCount; ++i)
      {
         const std::string& curCont = mImpl->mContexts[i];
         if (pathAbs.size() >= curCont.size() && pathAbs.substr(0, curCont.size()) == curCont)
         {
            // The context is a substr, we just have to make sure the next character is a separator
            // because /dir/dir2/dir3/hello.png is not a subpath to /dir/dir2/dir, but it would pass
            // the above check.  Another way to handle this would be to tokenize both paths, but why bother.
            if (pathAbs.size() == curCont.size() ||
                     (pathAbs[curCont.size()] == '/' || pathAbs[curCont.size()] == dtUtil::FileUtils::PATH_SEPARATOR))
            {
               result = i;
               break;
            }
         }
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::Refresh()
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      //clear the references to all the open maps
      mImpl->mMapList.clear();
      mImpl->mMapNames.clear();
      mImpl->mMapTree.clear();
      GetMapNames();

      //clear out the list of mResources.
      mImpl->mResources.clear();
      mImpl->mResourcesIndexed = false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ProjectImpl::ReloadMapNames() const
   {
      mMapNames.clear();
      for (MapListType::const_iterator i = mMapList.begin(); i != mMapList.end(); ++i)
      {
         mMapNames.insert(i->first);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Project::IsValidMapFile(const std::string& mapFileName)
   {
      std::string fullFilePath = dtUtil::FindFileInPathList(mapFileName);

      // Memory for the last map is maintained by the MapContentHandler
      Map* map = NULL;
      bool isMapValid = true;

      dtCore::RefPtr<MapParser> parser = new MapParser();
      try
      {
         parser->Parse(fullFilePath, &map);
      }
      catch (dtCore::XMLLoadParsingException&)
      {
         isMapValid = false;
      }

      return isMapValid;
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::set<std::string>& Project::GetMapNames()
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (mImpl->mMapList.empty())
      {
         mImpl->GenerateMapList();
      }

      return mImpl->mMapNames;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Project::MapTreeData& Project::GetMapTree()
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
            std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (mImpl->mMapList.empty())
      {
         mImpl->GenerateMapList();
      }

      return mImpl->mMapTree;
   }

   ////////////////////////////////////////////////////////////////////////////////
   MapPtr Project::GetMapHeader(const std::string& mapName)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
            std::string("There is no current project context.  Unable to get map headers."), __FILE__, __LINE__);
      }

      if (mImpl->mMapList.empty())
      {
         mImpl->GenerateMapList();
      }

      ProjectImpl::MapListType::iterator iter = mImpl->mMapList.find(mapName);

      MapPtr headerOnly;

      if (iter != mImpl->mMapList.end())
      {
         dtCore::RefPtr<MapParser> parser = new MapParser();

         Project::ContextSlot slotID = mImpl->mMapList[mapName].mSlotId;

         std::string& fileName = mImpl->mMapList[mapName].mFileName;
         std::string fullPath = mImpl->GetMapsDirectory(mImpl->mContexts[slotID], false).fileName + dtUtil::FileUtils::PATH_SEPARATOR + fileName;

         headerOnly = parser->ParseMapHeaderData(fullPath);
      }
      else
      {
         throw dtUtil::FileNotFoundException("No such map exists: " + mapName, __FILE__, __LINE__);
      }

      return headerOnly;
   }

   /////////////////////////////////////////////////////////////////////////////
   MapPtr Project::GetPrefabHeader(const dtCore::ResourceDescriptor& prefabResource)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
            std::string("There is no current project context.  Unable to get map headers."), __FILE__, __LINE__);
      }
      MapPtr result;

      const std::string fullPath = GetResourcePath(prefabResource);
      dtCore::RefPtr<MapParser> parser = new MapParser();
      result = parser->ParseMapHeaderData(fullPath, true);

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ProjectImpl::ListMapsForContextDir(Project::ContextSlot slot)
   {
      dtUtil::FileExtensionList extensions; ///list of acceptable file extensions
      extensions.push_back("." + dtCore::Map::MAP_FILE_EXTENSION);
      extensions.push_back(".xml");
      extensions.push_back(""); // allow for folder recursion.

      dtUtil::FileInfo fi = GetMapsDirectory(mContexts[slot], false);
      // It may not have a maps directory, so we have to check.
      if (fi.fileType == dtUtil::DIRECTORY)
      {
         dtCore::RefPtr<MapParser> parser = new MapParser();

         // Now recurse through this folder and all sub-folders for all maps.
         ListMapsForDir(fi.fileName, mMapTree, extensions, parser, slot);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ProjectImpl::ListMapsForDir(const std::string& mapsFolder, dtCore::Project::MapTreeData& treeData, dtUtil::FileExtensionList& extensions, dtCore::RefPtr<MapParser> parser, Project::ContextSlot slot)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      std::string dirPath = mapsFolder;
      if (!treeData.categoryName.empty())
      {
         dirPath += dtUtil::FileUtils::PATH_SEPARATOR + treeData.categoryName;
      }

      const dtUtil::DirectoryContents contents = fileUtils.DirGetFiles(dirPath, extensions);

      for (dtUtil::DirectoryContents::const_iterator fileIter = contents.begin(); fileIter < contents.end(); ++fileIter)
      {
         const std::string& filename = *fileIter;
         std::string filePath;
         std::string fullPath;

         if (!treeData.categoryName.empty())
         {
            filePath = treeData.categoryName + dtUtil::FileUtils::PATH_SEPARATOR + filename;
         }
         else
         {
            filePath = filename;
         }

         fullPath = mapsFolder + dtUtil::FileUtils::PATH_SEPARATOR + filePath;

         dtUtil::FileInfo fileInfo = fileUtils.GetFileInfo(fullPath);

         // If this is a regular file, assume it is a map and attempt to load it.
         if (fileInfo.fileType == dtUtil::REGULAR_FILE)
         {
            try
            {
               MapPtr headerOnly = parser->ParseMapHeaderData(fullPath);
               std::string mapName = headerOnly->GetName();

               MapFileData fileData;
               fileData.mOrigName = mapName;
               fileData.mFileName = filePath;
               fileData.mSlotId = slot;

               // resolve name collisions.
               std::string mapNameBuffer = mapName;
               int suffix = 1;
               while (mMapList.find(mapNameBuffer) != mMapList.end())
               {
                  mapNameBuffer = mapName;
                  dtUtil::MakeIndexString(suffix, mapNameBuffer, 0);
                  ++suffix;
               }

               mMapList.insert(make_pair(mapNameBuffer, fileData));

               treeData.mapList.insert(mapName);
            }
            catch (const dtUtil::Exception& e)
            {
               std::string error = "Unable to parse " + filePath + " with error " + e.What();
               mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
            }
         }
         // If we found a sub-directory, recurse into it.
         else if (fileInfo.fileType == dtUtil::DIRECTORY)
         {
            treeData.subCategories.push_back(dtCore::Project::MapTreeData());
            dtCore::Project::MapTreeData& subData = treeData.subCategories.back();

            subData.clear();
            subData.categoryName = filePath;
            ListMapsForDir(mapsFolder, subData, extensions, parser, slot);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ProjectImpl::GenerateMapList()
   {
      mMapList.clear();
      mMapTree.clear();
      mMapNames.clear();

      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
         "The project is preparing to load the map list.");

      for (Project::ContextSlot i = 0; i < mContexts.size(); ++i)
      {
         ListMapsForContextDir(i);
      }

      ReloadMapNames();
   }

   /////////////////////////////////////////////////////////////////////////////
   MapPtr ProjectImpl::InternalLoadPrefab(const std::string& fullPath, dtCore::ActorRefPtrVector& actorsOut)
   {
      // This really should be impossible because the code shouldn't call this unless it already validated the path.
      if (fullPath.empty()) return MapPtr(nullptr);

      //create the parser after setting the context.
      //because the parser looks for map.xsd in the constructor.
      //that way users can put map.xsd in the project and not need
      //a "data" path.
      if (!mParser.valid())
      {
         mParser = new MapParser();
      }

      MapPtr mapToUse;
      try
      {
         Map* mapRawPointer = nullptr;
         mParser->Parse(fullPath,&mapRawPointer, true);
         mapToUse = mapRawPointer;
         if (!mapToUse.valid())
         {
            throw dtCore::MapParsingException(
               "Prefab loading didn't throw an exception, but the result is NULL", __FILE__, __LINE__);
         }
         mapToUse->GetAllProxies(actorsOut);
         std::string filename = osgDB::getSimpleFileName(fullPath);
         mapToUse->SetName(filename);
         mapToUse->SetFileName(filename);
      }
      catch (const dtUtil::Exception& e)
      {
         mParser = NULL;
         std::string error = "Unable to parse \"" + fullPath + "\" with error \"" + e.What() + "\"";
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
         throw e;
      }

      mParser = NULL; //done using this MapParser, delete it
      return mapToUse;
   }

   /////////////////////////////////////////////////////////////////////////////
   Map& ProjectImpl::InternalLoadMap(const MapFileData& fileData, bool backup, bool clearModified)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      std::string fullPath = GetMapsDirectory(mContexts[fileData.mSlotId], true).fileName;

      if (backup)
      {
         fullPath += dtUtil::FileUtils::PATH_SEPARATOR + GetBackupDir();
      }

      fullPath += dtUtil::FileUtils::PATH_SEPARATOR + fileData.mFileName;
      if (backup)
      {
         fullPath += ".backup";
      }

      Map* map = NULL;

      //create the parser after setting the context.
      //because the parser looks for map.xsd in the constructor.
      //that way users can put map.xsd in the project and not need
      //a "data" path.
      if (!mParser.valid())
      {
         mParser = new MapParser();
      }

      try
      {
         if (fileUtils.GetFileInfo(fullPath).fileType != dtUtil::REGULAR_FILE)
         {
            throw dtCore::ProjectFileNotFoundException(
                   std::string("Map file \"") + fullPath + "\" not found.", __FILE__, __LINE__);
         }

         if (!mParser->Parse(fullPath, &map) || map == NULL)
         {
            throw dtCore::MapParsingException(
               "Map loading didn't throw an exception, but the result is NULL", __FILE__, __LINE__);
         }

         // TODO the name here should be the disabiguated name?
         mOpenMaps.insert(std::make_pair(fileData.mOrigName, dtCore::RefPtr<Map>(map)));

         //Clearing the modified flag must be done because setting the
         //map properties at load will make the map look modified.
         //it must be done before adding the missing libraries and proxy
         //classes because clearing the modified flag clears those lists.
         if (clearModified)
         {
            map->ClearModified();
         }

         // If the map has a temporary property, we should mark it modified.
         if (mParser->HasDeprecatedProperty())
         {
            map->SetModified(true);
         }

         map->AddMissingLibraries(mParser->GetMissingLibraries());
         map->AddMissingActorTypes(mParser->GetMissingActorTypes());
      }
      catch (const dtUtil::Exception& e)
      {
         mParser = NULL;
         std::string error = "Unable to parse \"" + fullPath + "\" with error \"" + e.What() + "\"";
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
         throw e;
      }

      mParser = NULL; //done using this MapParser, delete it

      return *map;
   }

   /////////////////////////////////////////////////////////////////////////////
   Map& Project::GetMap(const std::string& name)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      std::map<std::string, dtCore::RefPtr<Map> >::iterator openMapI = mImpl->mOpenMaps.find(name);

      //map is already open.
      if (openMapI != mImpl->mOpenMaps.end())
      {
         return *(openMapI->second);
      }

      ProjectImpl::MapListType::iterator mapIter = mImpl->mMapList.find(name);

      if (mapIter == mImpl->mMapList.end())
      {
         throw dtCore::ProjectFileNotFoundException(
                std::string("Map named ") + name + " does not exist.", __FILE__, __LINE__);
      }

      MapFileData& fileData = mapIter->second;

      Map& map = mImpl->InternalLoadMap(fileData, false, true);

      map.SetFileName(fileData.mFileName);
      return map;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Project::IsMapOpen(const std::string& name)
   {
      return mImpl->mOpenMaps.find(name) != mImpl->mOpenMaps.end();
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<Map*> Project::GetOpenMaps()
   {
      std::vector<Map*> maps;
      std::map<std::string, dtCore::RefPtr<Map> >::iterator iter;
      for (iter = mImpl->mOpenMaps.begin(); iter != mImpl->mOpenMaps.end(); ++iter)
      {
         maps.push_back(iter->second);
      }
      return maps;
   }

   /////////////////////////////////////////////////////////////////////////////
   Map& Project::OpenMapBackup(const std::string& name)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      std::map<std::string, dtCore::RefPtr<Map> >::iterator openMapI = mImpl->mOpenMaps.find(name);

      //map is already open.
      if (openMapI != mImpl->mOpenMaps.end())
      {
         //close the map if it's open.
         mImpl->mOpenMaps.erase(openMapI);
      }

      ProjectImpl::MapListType::iterator mapIter = mImpl->mMapList.find(name);

      if (mapIter == mImpl->mMapList.end())
      {
         throw dtCore::ProjectFileNotFoundException(
                std::string("Map named ") + name + " does not exist.", __FILE__, __LINE__);
      }

      const MapFileData& fileData = mapIter->second;

      Map& map = mImpl->InternalLoadMap(fileData, true, false);
      map.SetFileName(fileData.mFileName);
      map.SetSavedName(name);
      return map;

   }

   /////////////////////////////////////////////////////////////////////////////
   Map& Project::CreateMap(const std::string& name, const std::string& fileName, ContextSlot slot)
   {
      if (slot == DEFAULT_SLOT_VALUE)
      {
         slot = 0;
      }

      if (!IsContextValid(slot))
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (IsReadOnly())
      {
         throw dtCore::ProjectReadOnlyException(
         std::string("The context is readonly."), __FILE__, __LINE__);
      }

      if (name.empty())
      {
         throw dtCore::ProjectException(
                "Maps may not have an empty name.", __FILE__, __LINE__);
      }

      if (fileName.empty())
      {
         throw dtCore::ProjectException(
         std::string("Maps may not have an empty fileName."), __FILE__, __LINE__);
      }

      //assign it to a refptr so that if I except, it will get deleted
      dtCore::RefPtr<Map> map(new Map(fileName, name));

      for (ProjectImpl::MapListType::iterator mapIter = mImpl->mMapList.begin(); mapIter != mImpl->mMapList.end(); ++mapIter)
      {
         if (mapIter->first == name || mapIter->second.mOrigName == name)
         {
            throw dtCore::ProjectException(
                   std::string("Map named ") + name + " already exists.", __FILE__, __LINE__);
         }
         else if (mapIter->second.mFileName == map->GetFileName() && mapIter->second.mSlotId == slot)
         {
            throw dtCore::ProjectException(
                   std::string("A map with file name ") + fileName + " in context \"" + mImpl->mContexts[slot] + "\" already exists.",
                   __FILE__, __LINE__);
         }
      }

      mImpl->InternalSaveMap(*map, slot);

      mImpl->mOpenMaps.insert(make_pair(name, dtCore::RefPtr<Map>(map.get())));
      //The map can add extensions and such to the file name, so it
      //must be fetched back from the map object before being added to the name-file map.
      MapFileData fileData;
      fileData.mFileName = map->GetFileName();
      fileData.mSlotId = slot;
      fileData.mOrigName = name;
      mImpl->mMapList.insert(make_pair(name, fileData));
      mImpl->mMapNames.insert(name);

      return *map;

   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseActorObject> Project::LoadPrefab(const dtCore::PrefabActorType& actorType)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }
      std::string fullPath = GetResourcePath(actorType.GetPrefabResource());

      return NULL;
      //return mImpl->InternalLoadPrefab(fullPath, actorType);
   }

   /////////////////////////////////////////////////////////////////////////////
   MapPtr Project::LoadPrefab(const dtCore::ResourceDescriptor& rd, dtCore::ActorRefPtrVector& actorsOut)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      std::string fullPath = GetResourcePath(rd);

      return mImpl->InternalLoadPrefab(fullPath, actorsOut);
   }

   /////////////////////////////////////////////////////////////////////////////
   ResourceDescriptor Project::SavePrefab(const std::string& name, const std::string& category, const ActorRefPtrVector& actorList,
         const std::string& description,
         const std::string& iconFile, ContextSlot slot)
   {
      if (slot == DEFAULT_SLOT_VALUE)
      {
         slot = 0U;
      }

      if (!IsContextValid(slot))
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      std::string fileName = name, curExt;
      curExt = osgDB::getLowerCaseFileExtension(fileName);
      if (curExt != Map::PREFAB_FILE_EXTENSION)
      {
         fileName.append(".").append(Map::PREFAB_FILE_EXTENSION);
      }

      dtCore::MapPtr tempMap = new dtCore::Map(fileName, name);
      tempMap->SetDescription(description);
      tempMap->SetIconFile(iconFile);
      std::for_each(actorList.begin(), actorList.end(), [&tempMap](const ActorPtr& actor)
            {
         tempMap->AddProxy(*actor, false);
            });

      // Save the temp file in the project context slot root.
      std::string savedPath = mImpl->InternalSaveMapOrPrefab(*tempMap, mImpl->mContexts[slot], slot, true);
      std::string removedSavingExtension = osgDB::getNameLessExtension(savedPath);
      dtUtil::FileUtils::GetInstance().FileMove(savedPath, removedSavingExtension, true);
      // then add it as a resource.
      ResourceDescriptor result = AddResource(name, removedSavingExtension, category, dtCore::DataType::PREFAB, slot);
      try
      {
         dtUtil::FileUtils::GetInstance().FileDelete(removedSavingExtension);
      }
      catch(const dtUtil::Exception& ex)
      {
         // If there is an error deleting the temp file, then it didn't really fail, it just didn't clean up after itself, like the plumber...
         ex.LogException(dtUtil::Log::LOG_ERROR, mImpl->mLogger->GetName());
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   Map& Project::LoadMapIntoScene(const std::string& name, dtCore::Scene& scene, bool addBillBoards)
   {
      Map& m = GetMap(name);
      LoadMapIntoScene(m, scene, addBillBoards);
      return m;
   }

   /////////////////////////////////////////////////////////////////////////////
   void NormalAddChild(dtCore::Map& map, dtCore::BaseActorObject& actor, dtCore::Scene& scene)
   {
      if (actor.GetDrawable() != nullptr && actor.GetDrawable()->GetParent() == nullptr)
      {
         //If we got here, then the proxy wishes the system to determine how to display
         //the proxy. (Currently defaults to DRAW_ACTOR.
         if (map.GetEnvironmentActor() != nullptr)
         {
            IEnvironmentActor *ea = dynamic_cast<IEnvironmentActor*>(map.GetEnvironmentActor()->GetDrawable());
            if (ea == nullptr)
            {
               throw dtCore::InvalidActorException(
                     "Actor should be of type dtCore::EnvironmentActor", __FILE__, __LINE__);
            }
            // Hack to ensure the environment doesn't add itself from the map
            if (actor.GetDrawable() != dynamic_cast<dtCore::DeltaDrawable*>(ea))
            {
               ea->AddActor(*actor.GetDrawable());
            }
         }
         else
         {
            scene.AddChild(actor.GetDrawable());
         }
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::LoadMapIntoScene(Map& map, dtCore::Scene& scene, bool addBillBoards)
   {
      mImpl->CheckMapValidity(map, true);
      std::vector<dtCore::RefPtr<BaseActorObject> > container;
      map.GetAllProxies(container);

      for (std::vector<dtCore::RefPtr<BaseActorObject> >::iterator proxyIter = container.begin();
           proxyIter != container.end(); ++proxyIter)
      {
         BaseActorObject& actor = **proxyIter;

         //if we are adding billboards, then we need to check the render modes.
         if (addBillBoards)
         {
            const BaseActorObject::RenderMode& renderMode = actor.GetRenderMode();

            if (renderMode == BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON ||
                renderMode == BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
            {
               ActorProxyIcon *billBoard = actor.GetBillBoardIcon();

               // Load the textures for the billboard and orientation arrow.
               // This is only done here so the files will only be loaded when
               // they are actually going to be rendered. Previously this was
               // done inside ActorProxyIcon's constructor, even if billboards
               // will never be displayed. It was broken out and placed here
               // to fix that. -osb
               billBoard->LoadImages();

               if (billBoard == nullptr)
               {
                  mImpl->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                      "Proxy [%s] billboard was NULL.", actor.GetName().c_str());
               }
               else
               {
                  if (actor.GetDrawable()->GetParent() != nullptr)
                  {
                     actor.GetDrawable()->GetParent()->AddChild(billBoard->GetDrawable());
                  }
                  else
                  {
                     scene.AddChild(billBoard->GetDrawable());
                  }
               }

            }

            if (renderMode == BaseActorObject::RenderMode::DRAW_ACTOR ||
                renderMode == BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
                renderMode == BaseActorObject::RenderMode::DRAW_AUTO)
            {
               NormalAddChild(map, actor, scene);
            }
         }
         else
         {
            NormalAddChild(map, actor, scene);
         }
      }

      if (map.GetEnvironmentActor() != nullptr)
      {
         scene.AddChild(map.GetEnvironmentActor()->GetDrawable());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ProjectImpl::UnloadUnusedLibraries(Map& mapToClose)
   {
      std::vector<dtCore::RefPtr<BaseActorObject> > proxies;
      mapToClose.GetAllProxies(proxies);

      // Scoped for proxyIter
      {
         std::vector<dtCore::RefPtr<BaseActorObject> >::iterator proxyIter = proxies.begin();

         while (proxyIter != proxies.end())
         {
            dtCore::RefPtr<BaseActorObject>& proxy = *proxyIter;
            //if this proxy has a reference count greater than 1
            //then its library may not close, but 2 is used here because
            //the vector has a referece to it now.
            if (proxy != NULL && proxy->referenceCount() <= 2)
            {
               proxyIter = proxies.erase(proxyIter);
            }
            else
            {
               ++proxyIter;
            }
         }
      }

      //clear the proxies to make sure they are all freed.
      //one should not save AFTER calling this.
      mapToClose.ClearProxies();

      for (std::vector<std::string>::const_iterator proxyIter = mapToClose.GetAllLibraries().begin();
           proxyIter != mapToClose.GetAllLibraries().end(); ++proxyIter)
      {
         std::string libToClose = *proxyIter;
         bool libMayClose = true;
         for (std::map<std::string, dtCore::RefPtr<Map> >::const_iterator mapIter = mOpenMaps.begin(); mapIter != mOpenMaps.end(); ++mapIter)
         {
            const Map& toCheck = *mapIter->second;
            if (&mapToClose != &toCheck && toCheck.HasLibrary(libToClose))
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
               {
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
               ActorFactory::GetInstance().GetRegistry(libToClose);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                   "About to unload library named %s.", libToClose.c_str());
            }

            //go through proxies still being held onto outside this library
            //and see if the currently library is the source of any.
            for (std::vector<dtCore::RefPtr<BaseActorObject> >::iterator proxyIter = proxies.begin();
                 proxyIter != proxies.end(); ++proxyIter)
            {
               dtCore::RefPtr<BaseActorObject>& proxy = *proxyIter;

               try
               {
                  ActorPluginRegistry* registry = ActorFactory::GetInstance().GetRegistryForType(proxy->GetActorType());
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
            {
               ActorFactory::GetInstance().UnloadActorRegistry(libToClose);
            }
         }

      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ProjectImpl::InternalCloseMap(Map& map, bool unloadLibraries)
   {
      if (unloadLibraries)
      {
         UnloadUnusedLibraries(map);
      }

      if (!mContextReadOnly)
      {
         try
         {
            Project::mInstance->ClearBackup(map.GetSavedName());
         }
         catch (const dtUtil::Exception& ex)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Error clearing Map backups when closing: %s", ex.What().c_str());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::CloseMap(Map& map, bool unloadLibraries)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      std::map<std::string, dtCore::RefPtr<Map> >::iterator mapIter = mImpl->mOpenMaps.find(map.GetSavedName());
      if (mapIter == mImpl->mOpenMaps.end() || (mapIter->second.get() != &map))
      {
         throw dtCore::ProjectInvalidContextException(
                std::string("A map named \"") + map.GetSavedName() + "\" exists, but this is not the instance.", __FILE__, __LINE__);
      }
      else
      {
         mImpl->InternalCloseMap(map, unloadLibraries);
         mImpl->mOpenMaps.erase(mapIter);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::CloseAllMaps(bool unloadLibraries)
   {
      std::map<std::string, dtCore::RefPtr<Map> >::iterator mapIter = mImpl->mOpenMaps.begin();
      std::map<std::string, dtCore::RefPtr<Map> >::iterator mapIterEnd = mImpl->mOpenMaps.end();
      while (mapIter != mapIterEnd)
      {
         std::map<std::string, dtCore::RefPtr<Map> >::iterator inext = mapIter;
         ++inext;
         mImpl->InternalCloseMap(*mapIter->second, unloadLibraries);
         mImpl->mOpenMaps.erase(mapIter);
         mapIter = inext;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::DeleteMap(Map& map, bool unloadLibraries)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (IsReadOnly())
      {
         throw dtCore::ProjectReadOnlyException(
         std::string("The context is readonly."), __FILE__, __LINE__);
      }

      // TODO There is an issue with maps having the same name and getting numbers tacked on the end.
      std::string mapSavedName = map.GetSavedName();

      // Do not access the map variable again after this line, it will be corrupted
      CloseMap(map, unloadLibraries);

      ProjectImpl::MapListType::iterator mapIter = mImpl->mMapList.find(mapSavedName);
      if (mapIter == mImpl->mMapList.end())
      {
         mImpl->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                             "Map was found in the list of open maps, but not in map to fileName mapping");
      }
      else
      {
         mImpl->mMapList.erase(mapIter);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::DeleteMap(const std::string& mapName, bool unloadLibraries)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (IsReadOnly())
      {
         throw dtCore::ProjectReadOnlyException(
         std::string("The context is readonly."), __FILE__, __LINE__);
      }

      std::map<std::string, dtCore::RefPtr<Map> >::iterator mapIter = mImpl->mOpenMaps.find(mapName);
      if (mapIter != mImpl->mOpenMaps.end())
      {
         CloseMap(*mapIter->second, unloadLibraries);
      }
      else
      {
         ClearBackup(mapName);
      }

      MapFileData mapFileData;

      ProjectImpl::MapListType::iterator i = mImpl->mMapList.find(mapName);
      if (i == mImpl->mMapList.end())
      {
         throw dtCore::ProjectFileNotFoundException(
            std::string("No such map: \"") + mapName + "\"", __FILE__, __LINE__);
      }
      else
      {
         mapFileData = i->second;
         mImpl->mMapList.erase(i);
      }

      // TODO need the context path.
      mImpl->InternalDeleteMap(mapFileData);

   }

   /////////////////////////////////////////////////////////////////////////////
   void ProjectImpl::InternalDeleteMap(const MapFileData& mapFileData)
   {
      ReloadMapNames();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      dtUtil::DirectoryPush dp(GetMapsDirectory(mContexts[mapFileData.mSlotId], false).fileName);
      if (fileUtils.FileExists(mapFileData.mFileName))
      {
         fileUtils.FileDelete(mapFileData.mFileName);
      }
      else
      {
         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                             "Specified map was part of the project, but the map file did not exist.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::SaveMap(Map& map)
   {
      Project::ContextSlot slot = mImpl->CheckMapValidity(map);
      mImpl->InternalSaveMap(map, slot);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::SaveMapAs(const std::string& mapName, const std::string& newName, const std::string& newFileName, ContextSlot slot)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (IsReadOnly())
      {
         throw dtCore::ProjectReadOnlyException(
         std::string("The context is readonly."), __FILE__, __LINE__);
      }

      //The map must be loaded to do a saveAs, so we call getMap();
      SaveMapAs(GetMap(mapName), newName, newFileName);

   }
   //////////////////////////////////////////////////////////
   void Project::SaveMapAs(Map& map, const std::string& newName, const std::string& newFileName, ContextSlot slot)
   {
      mImpl->CheckMapValidity(map);

      // JPH: Why not?  Sometimes we just want to change the description.
      //if (map.GetSavedName() == newName)
      //{
      //   throw dtCore::ProjectException( std::string("Map named ")
      //          + map.GetSavedName() + " cannot be saved again as the same name", __FILE__, __LINE__);
      //}

      // Unless we are overwriting our current map file, we must not allow
      // the same named map to be saved as another file name (eg. in another category folder).
      if (map.GetName() == newName && osgDB::getNameLessExtension(map.GetFileName()) != newFileName)
      {
         throw dtCore::ProjectException( std::string("Map named ")
                + map.GetName() + " cannot be saved again as the same name while under a new category name", __FILE__, __LINE__);
      }

      //std::string newFileNameCopy(newFileName);
      std::string currentFileName(map.GetFileName());

      std::string oldMapName = map.GetSavedName();

      MapFileData oldFileData = mImpl->mMapList[oldMapName];
      if (slot == DEFAULT_SLOT_VALUE)
      {
         slot = oldFileData.mSlotId;
      }

      // JPH: Same as above, overwriting the same map should be allowed.
      //compare the file name without the extension.
      //if (currentFileName.substr(0, currentFileName.size() - Map::MAP_FILE_EXTENSION.size())
      //    == newFileName && oldFileData.mSlotId == slot)
      //{
      //   throw dtCore::ProjectException( std::string("Map named ") + map.GetSavedName()
      //          + " cannot be saved as a different map with the same file name.", __FILE__, __LINE__);
      //}

      // If we are not changing our current maps file name, don't check for overwrite.
      if (newFileName != osgDB::getNameLessExtension(map.GetFileName()))
      {
         for (ProjectImpl::MapListType::const_iterator i = mImpl->mMapList.begin();
            i != mImpl->mMapList.end(); ++i )
         {
            const std::string& mapFileNameRef = i->second.mFileName;
            if (newFileName == mapFileNameRef.substr(0, mapFileNameRef.size() - (Map::MAP_FILE_EXTENSION.size() + 1))
               && i->second.mSlotId == slot)
            {
               throw dtCore::ProjectException( std::string("Map named ")
                  + map.GetSavedName() + " cannot be saved with file name "
                  + newFileName + " because it matches another map.", __FILE__, __LINE__);
            }
         }
      }

      map.SetName(newName);
      map.SetFileName(newFileName);

      mImpl->InternalSaveMap(map, slot);

      //re-add the old map to the list of saved maps
      //since saving with a new name will remove the old entry.
      mImpl->mMapList.insert(std::make_pair(oldMapName, oldFileData));
      mImpl->mMapNames.insert(oldMapName);

      try
      {
         ClearBackup(oldMapName);
      }
      catch (const dtUtil::Exception& ex)
      {
         mImpl->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Error clearing map backups when saving %s as %s: %s",
                             oldMapName.c_str(), newName.c_str(), ex.What().c_str());
      }

      // Now re-load our map listing.
      mImpl->GenerateMapList();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::SaveMap(const std::string& mapName)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (IsReadOnly())
      {
         throw dtCore::ProjectReadOnlyException(
         std::string("The context is readonly."), __FILE__, __LINE__);
      }

      std::map<std::string, dtCore::RefPtr<Map> >::iterator mapIter = mImpl->mOpenMaps.find(mapName);
      if (mapIter == mImpl->mOpenMaps.end())
      {
         return; //map is not in memory, so it doesn't need to be saved.
      }

      Map& m = *(mapIter->second);
      mImpl->InternalSaveMap(m, mImpl->CheckMapValidity(m));
   }

   /////////////////////////////////////////////////////////////////////////////
   Project::ContextSlot ProjectImpl::CheckMapValidity(const Map& map, bool readonlyAllowed) const
   {
      if (!Project::mInstance->IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (!readonlyAllowed && Project::mInstance->IsReadOnly())
      {
         throw dtCore::ProjectReadOnlyException(
         std::string("The context is readonly."), __FILE__, __LINE__);
      }

      ProjectImpl::MapListType::const_iterator mapListIter = mMapList.find(map.GetSavedName());
      if (mapListIter == mMapList.end())
      {
         throw dtCore::ProjectFileNotFoundException(
            std::string("No such map: \"") + map.GetSavedName() + "\"", __FILE__, __LINE__);
      }

      Project::ContextSlot slot = mapListIter->second.mSlotId;

      std::map<std::string, dtCore::RefPtr<Map> >::const_iterator mapIter = mOpenMaps.find(map.GetSavedName());

      if (mapIter == mOpenMaps.end())
      {
         throw dtCore::ProjectInvalidContextException(
            std::string("A map named \"") + map.GetSavedName()
                + "\" exists but is not currently open.", __FILE__, __LINE__);

      }
      else if (mapIter->second.get() != &map)
      {
         throw dtCore::ProjectInvalidContextException(std::string("A map named \"") + map.GetSavedName()
                + "\" exists, but this is not the instance.", __FILE__, __LINE__);
      }

      return slot;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string ProjectImpl::InternalSaveMapOrPrefab(Map& map, const std::string& categoryPath, Project::ContextSlot slot, bool prefab)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      std::string fullPath = categoryPath + dtUtil::FileUtils::PATH_SEPARATOR + map.GetFileName();
      std::string fullPathSaving = fullPath + ".saving";

      //make sure the category directory exists.
      std::string folderPath = osgDB::getFilePath(fullPath);
      fileUtils.MakeDirectoryEX(folderPath);

      //save the file to a separate name first so that
      //it won't blast the old one unless it is successful.
      dtCore::RefPtr<MapWriter> writer = new MapWriter();
      writer->Save(map, fullPathSaving, prefab);
      return fullPathSaving;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ProjectImpl::InternalSaveMap(Map& map, Project::ContextSlot slot)
   {
      bool isNew = map.GetSavedName().empty();

      if (map.GetSavedName() != map.GetName())
      {
         if (mMapList.find(map.GetName()) != mMapList.end())
         {
            throw dtCore::ProjectException(
               "You may not save a map with a name that matches another map.", __FILE__, __LINE__);
         }
      }

      std::string mapDir = GetMapsDirectory(mContexts[slot], true).fileName;

      std::string finalPath = mapDir + dtUtil::FileUtils::PATH_SEPARATOR + map.GetFileName();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.FileMove(InternalSaveMapOrPrefab(map, mapDir, slot, false), finalPath, true);

      //Update the internal lists to make sure that
      //map is keyed properly by name.
      if (!map.GetSavedName().empty() && map.GetName() != map.GetSavedName())
      {
         if (!mMapList.empty())
         {
            MapFileData fileData;
            fileData.mFileName = map.GetFileName();
            fileData.mOrigName = map.GetName();
            fileData.mSlotId = slot;
            mMapList.erase(mMapList.find(map.GetSavedName()));
            mMapList.insert(make_pair(map.GetName(), fileData));
            ReloadMapNames();
         }

         dtCore::RefPtr<Map> holder(&map);

         mOpenMaps.erase(mOpenMaps.find(map.GetSavedName()));
         mOpenMaps.insert(make_pair(map.GetName(), holder));
      }

      map.ClearModified();

      if (!isNew)
      {
         try
         {
            Project::mInstance->ClearBackup(map.GetSavedName());
         }
         catch (const dtCore::ProjectFileNotFoundException& ex)
         {
            //if the map is new, the following exception will be thrown
            //so don't print an error in that case.
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                               "Error clearing map backups when saving: %s", ex.What().c_str());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::SaveMapBackup(Map& map)
   {
      Project::ContextSlot slot = mImpl->CheckMapValidity(map);

      if (!map.IsModified())
      {
         return;
      }

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      //dtUtil::DirectoryPush dp();
      std::string backupDir = mImpl->GetMapsDirectory(mImpl->mContexts[slot], true).fileName + dtUtil::FileUtils::PATH_SEPARATOR + mImpl->GetBackupDir();

      std::string path = backupDir + dtUtil::FileUtils::PATH_SEPARATOR + map.GetFileName();

      std::string dir = osgDB::getFilePath(path);
      if (!fileUtils.DirExists(dir))
      {
         fileUtils.MakeDirectory(dir);
      }

      std::string fileName = path + ".backupsaving";
      std::string finalFileName = path + ".backup";

      //save the file to a "saving" file so that if it blows or is killed while saving, the data
      //will not be lost.
      dtCore::RefPtr<MapWriter> writer = new MapWriter();
      writer->Save(map, fileName);


      //when it completes, move the file to the final name.
      fileUtils.FileMove(fileName, finalFileName, true);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Project::HasBackup(Map& map) const
   {
      mImpl->CheckMapValidity(map, true);
      return HasBackup(map.GetSavedName());
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Project::HasBackup(const std::string& mapName) const
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      ProjectImpl::MapListType::const_iterator found = mImpl->mMapList.find(mapName);
      if (found == mImpl->mMapList.end())
      {
         throw dtCore::ProjectFileNotFoundException(
         std::string("No such map: \"") + mapName + "\"", __FILE__, __LINE__);
      }

      const std::string& fileName = found->second.mFileName;

      bool result = false;

      dtUtil::FileInfo fi = mImpl->GetMapsDirectory(mImpl->mContexts[found->second.mSlotId], false);
      if (fi.fileType == dtUtil::DIRECTORY)
      {
         std::string backupDir =  fi.fileName + dtUtil::FileUtils::PATH_SEPARATOR + mImpl->GetBackupDir();

         const std::string& backupFileName = backupDir + dtUtil::FileUtils::PATH_SEPARATOR + fileName + ".backup";

         result = dtUtil::FileUtils::GetInstance().FileExists(backupFileName);
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::ClearBackup(Map& map)
   {
      mImpl->CheckMapValidity(map);
      ClearBackup(map.GetSavedName());
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::ClearBackup(const std::string& mapName)
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      ProjectImpl::MapListType::iterator found = mImpl->mMapList.find(mapName);
      if (found == mImpl->mMapList.end())
      {
         throw dtCore::ProjectFileNotFoundException(
                  std::string("No such map: \"") + mapName + "\"", __FILE__, __LINE__);
      }

      const std::string& fileName = found->second.mFileName;

      dtUtil::FileInfo fiMaps = mImpl->GetMapsDirectory(mImpl->mContexts[found->second.mSlotId], false);

      if (fiMaps.fileType == dtUtil::FILE_NOT_FOUND)
      {
         return;
      }

      std::string backupDir = fiMaps.fileName + dtUtil::FileUtils::PATH_SEPARATOR + mImpl->GetBackupDir();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      if (!fileUtils.DirExists(backupDir))
      {
         return;
      }

      dtUtil::DirectoryContents dc = fileUtils.DirGetFiles(backupDir);

      size_t fileNameSize = fileName.size();
      for (dtUtil::DirectoryContents::const_iterator i = dc.begin(); i != dc.end(); ++i)
      {
         const std::string& file = *i;
         if (file.size() > fileNameSize
             && file.substr(0, fileNameSize) == fileName)
         {

            if (fileUtils.GetFileInfo(backupDir + dtUtil::FileUtils::PATH_SEPARATOR + file).fileType == dtUtil::REGULAR_FILE)
            {
               //If we encountered a map backup and this context is read-only, then
               //we have a problem.
               if (IsReadOnly())
               {
                  throw dtCore::ProjectReadOnlyException(
                     std::string("The context is readonly."), __FILE__, __LINE__);
               }

               fileUtils.FileDelete(backupDir + dtUtil::FileUtils::PATH_SEPARATOR + file);
            }
         }
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   Map* Project::GetMapForActor(const dtCore::UniqueId& id)
   {
      if (!IsContextValid())
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);

      if (mImpl->mParser.valid() && mImpl->mParser->IsParsing())
      {
         Map* m = mImpl->mParser->GetMapBeingParsed();
         if (m != NULL)
         {
            BaseActorObject* ap = m->GetProxyById(id);
            if (ap != NULL)
               return m;
         }
      }

      std::map< std::string, dtCore::RefPtr<Map> >::iterator i = mImpl->mOpenMaps.begin();
      while (i != mImpl->mOpenMaps.end())
      {
         BaseActorObject* ap = i->second->GetProxyById(id);
         if (ap != NULL)
            return i->second.get();

         i++;
      }
      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   const Map* Project::GetMapForActor(const dtCore::UniqueId& id) const
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
            std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (mImpl->mParser.valid() && mImpl->mParser->IsParsing())
      {
         const Map* m = mImpl->mParser->GetMapBeingParsed();
         if (m != NULL)
         {
            const BaseActorObject* ap = m->GetProxyById(id);
            if (ap != NULL)
               return m;
         }
      }

      std::map< std::string, dtCore::RefPtr<Map> >::const_iterator i = mImpl->mOpenMaps.begin();
      while (i != mImpl->mOpenMaps.end())
      {
         const BaseActorObject* ap = i->second->GetProxyById(id);
         if (ap != NULL)
         {
            return i->second.get();
         }

         i++;
      }
      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   Map* Project::GetMapForActor(const BaseActorObject& proxy)
   {
      return GetMapForActor(proxy.GetId());
   }

   /////////////////////////////////////////////////////////////////////////////
   const Map* Project::GetMapForActor(const BaseActorObject& proxy) const
   {
      return GetMapForActor(proxy.GetId());
   }

   ////////////////////////////////////////////////////////////////////////////////
   GameEvent* Project::GetGameEvent(const dtCore::UniqueId& id)
   {
      GameEvent* e = GameEventManager::GetInstance().FindEvent(id);
      if (e != NULL)
      {
         return e;
      }

      if (!IsContextValid())
      {
         return NULL;
      }

      std::map<std::string, dtCore::RefPtr<Map> >::iterator i = mImpl->mOpenMaps.begin();
      for (; i != mImpl->mOpenMaps.end(); ++i)
      {
         dtCore::RefPtr<Map>& map = i->second;

         e = map->GetEventManager().FindEvent(id);
         if (e)
         {
            return e;
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   GameEvent* Project::GetGameEvent(const std::string& eventName)
   {
      GameEvent* e = GameEventManager::GetInstance().FindEvent(eventName);
      if (e)
      {
         return e;
      }

      if (!IsContextValid())
      {
         return NULL;
      }

      std::map<std::string, dtCore::RefPtr<Map> >::iterator i = mImpl->mOpenMaps.begin();
      for (; i != mImpl->mOpenMaps.end(); ++i)
      {
         dtCore::RefPtr<Map>& map = i->second;

         e = map->GetEventManager().FindEvent(eventName);
         if (e)
         {
            return e;
         }
      }

      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::GetHandlersForDataType(const DataType& resourceType, std::vector<const ResourceTypeHandler* >& toFill) const
   {
      mImpl->mResourceHelper.GetHandlersForDataType(resourceType, toFill);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::RegisterResourceTypeHander(ResourceTypeHandler& handler)
   {
      mImpl->mResourceHelper.RegisterResourceTypeHander(handler);
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string ProjectImpl::GetBackupDir() const
   {
      return Project::MAP_BACKUP_SUB_DIRECTORY;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtUtil::FileInfo ProjectImpl::GetMapsDirectory(const std::string& contextPath, bool createIfNeeded)
   {
      dtUtil::FileInfo fileInfo;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      dtUtil::StrCompareFunc cmpfunc(Project::MAP_DIRECTORY, false);

      const dtUtil::DirectoryContents contents = fileUtils.DirGetFiles(contextPath);

      dtUtil::DirectoryContents::const_iterator dirIt = std::find_if(contents.begin(), contents.end(), cmpfunc);
      bool hasMapsDir = dirIt != contents.end();

      if (!hasMapsDir)
      {
         std::string mapsDirFull = contextPath + dtUtil::FileUtils::PATH_SEPARATOR + Project::MAP_DIRECTORY;
         if (createIfNeeded)
         {
            try
            {
               fileUtils.MakeDirectory(mapsDirFull);
               fileInfo = fileUtils.GetFileInfo(mapsDirFull);
            }
            catch(const dtUtil::Exception& ex)
            {
               std::ostringstream ss;
               ss << "Unable to create directory " << mapsDirFull << ". Error: " << ex.What();
               throw dtCore::ProjectInvalidContextException(
                        ss.str(), __FILE__, __LINE__);
            }
         }
         else
         {
            // Should result in a file not found fileInfo.
            fileInfo = fileUtils.GetFileInfo(mapsDirFull);
         }
      }
      else
      {
         fileInfo = fileUtils.GetFileInfo(contextPath + dtUtil::FileUtils::PATH_SEPARATOR + *dirIt);

         if (fileInfo.fileType != dtUtil::DIRECTORY)
         {
            std::string s(contextPath + dtUtil::FileUtils::PATH_SEPARATOR);
            s.append(" is not a valid project directory.  The file \"");
            s.append(*dirIt);
            s.append("\" is not a directory.  Delete or rename the file.");

            throw dtCore::ProjectInvalidContextException(s, __FILE__, __LINE__);
         }
      }
      return fileInfo;
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string Project::GetResourcePath(const ResourceDescriptor& resource, bool isCategory) const
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (resource == ResourceDescriptor::NULL_RESOURCE)
      {
         throw ProjectFileNotFoundException(
            std::string("An empty resource was passed in to process: [") + resource.GetResourceIdentifier() + "]", __FILE__, __LINE__);
      }

      const std::string& path = mImpl->mResourceHelper.GetResourcePath(resource);


      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      // Init to file not found.
      dtUtil::FileType ftype = dtUtil::FILE_NOT_FOUND;

      dtUtil::FileType expectedType = dtUtil::REGULAR_FILE;
      if (isCategory)
      {
         expectedType = dtUtil::DIRECTORY;
      }

      //for proper error handling.
      bool foundADir = false;

      dtUtil::FileInfo resultInfo;

      std::vector<std::string>::const_iterator i, iend;
      i = mImpl->mContexts.begin();
      iend = mImpl->mContexts.end();
      for (; i != iend && ftype != expectedType; ++i)
      {
         resultInfo = fileUtils.GetFileInfo(*i + dtUtil::FileUtils::PATH_SEPARATOR + path, true);
         ftype = resultInfo.fileType;

         if (ftype == dtUtil::DIRECTORY)
         {
            if (!isCategory)
            {
               // didn't find the resource, but found a directory with that same name.
               // This is only an error if no file is found in a later path.
               foundADir = true;
            }
         }

         if (ftype == dtUtil::REGULAR_FILE)
         {
            if (isCategory)
            {
               break;
            }
         }
      }

      if (ftype != expectedType)
      {

         if (!isCategory)
         {
            if (!foundADir)
            {
               throw ProjectFileNotFoundException(
                  std::string("The specified resource was not found: [") + path + "]", __FILE__, __LINE__);
            }
            else
            {
               throw ProjectResourceErrorException(
                      std::string("The resource identifier specifies a category or directory: ") + path, __FILE__, __LINE__);
            }
         }
         else
         {
            if (ftype == dtUtil::FILE_NOT_FOUND)
            {
               throw ProjectFileNotFoundException(
                  std::string("The specified resource was not found: [") + path + "]", __FILE__, __LINE__);
            }
            else
            {
               throw ProjectResourceErrorException(
                      std::string("The resource identifier specifies a regular file, not a category/directory: ") + path, __FILE__, __LINE__);
            }
         }

      }

      return resultInfo.fileName;
   }


   /////////////////////////////////////////////////////////////////////////////
   void Project::CreateResourceCategory(const std::string& category, const DataType& type, ContextSlot slot)
   {
      if (!IsContextValid(slot))
      {
         throw dtCore::ProjectInvalidContextException(
                  std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (IsReadOnly())
      {
         throw dtCore::ProjectReadOnlyException(
                  std::string("The project is readonly."), __FILE__, __LINE__);
      }

      if (!type.IsResource())
      {
         throw dtCore::ProjectResourceErrorException( std::string("Unable to add resource of type ") + type.GetName()
                + ". It is not a resource type.", __FILE__, __LINE__);
      }

      std::vector<std::string>::const_iterator i, iend;
      if (slot == Project::DEFAULT_SLOT_VALUE)
      {
         i = mImpl->mContexts.begin();
         iend = mImpl->mContexts.end();
      }
      else
      {
         //if you specify a slot, just loop over the one index.
         i = mImpl->mContexts.begin() + slot;
         iend = mImpl->mContexts.begin() + slot + 1;
      }

      ContextSlot curSlot = 0;
      for (; i != iend; ++i)
      {
         dtUtil::DirectoryPush dt(*i);
         Project::ResourceTree* categoryInTree;
         Project::ResourceTree* dataTypeTree = NULL;

         if (mImpl->mResourcesIndexed)
         {
            dataTypeTree = &mImpl->GetResourcesOfType(type);
         }

         // TODO see what this does if it thinks it has this resource already.
         mImpl->mResourceHelper.CreateResourceCategory(category, type, curSlot, dataTypeTree, categoryInTree);
         ++curSlot;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Project::RemoveResourceCategory(const std::string& category,
                                        const DataType& type, bool recursive, ContextSlot slot)
   {

      if (!IsContextValid(slot))
      {
         throw dtCore::ProjectInvalidContextException(
                std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (IsReadOnly())
      {
         throw dtCore::ProjectReadOnlyException(
                std::string("The context is readonly."), __FILE__, __LINE__);
      }

      if (!type.IsResource())
      {
         throw dtCore::ProjectResourceErrorException( std::string("Unable to add resource of type ") + type.GetName()
                + ". It is not a resource type.", __FILE__, __LINE__);
      }

      std::vector<std::string>::const_iterator i, iend;
      if (slot == Project::DEFAULT_SLOT_VALUE)
      {
         // Default remove from instances.
         i = mImpl->mContexts.begin();
         iend = mImpl->mContexts.end();
      }
      else
      {
         //if you specify a slot, just loop over the one index.
         i = mImpl->mContexts.begin() + slot;
         iend = mImpl->mContexts.begin() + slot + 1;
      }

      // default to true because it's only false if it was unable to remove the category
      // due to a directory not being empty.
      // TODO not sure what to do if a category exists in several but is not empty in only
      // some.
      bool result = true;

      for (; i != iend; ++i)
      {
         dtUtil::DirectoryPush dp(*i);
         Project::ResourceTree* dataTypeTree = NULL;
         if (mImpl->mResourcesIndexed)
         {
            dataTypeTree = &mImpl->GetResourcesOfType(type);
         }

         // TODO see what it does if it things it's already removed it.
         result = result && mImpl->mResourceHelper.RemoveResourceCategory(category, type, recursive, dataTypeTree);
      }
      return result;

   }

   /////////////////////////////////////////////////////////////////////////////
   const Project::ResourceTree& Project::GetAllResources() const
   {
      if (!mImpl->mResourcesIndexed)
      {
         mImpl->IndexResources();
      }

      return mImpl->mResources;
   }

   /////////////////////////////////////////////////////////////////////////////
   const ResourceDescriptor Project::AddResource(const std::string& newName,
                                                 const std::string& pathToFile, const std::string& category,
                                                 const DataType& type,
                                                 ContextSlot slot)
   {

      if (slot == Project::DEFAULT_SLOT_VALUE) { slot = 0; }
      if (!IsContextValid(slot))
      {
         throw dtCore::ProjectInvalidContextException(
                std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (IsReadOnly())
      {
         throw dtCore::ProjectReadOnlyException(
                std::string("The context is readonly."), __FILE__, __LINE__);
      }

      if (!type.IsResource())
      {
         throw dtCore::ProjectResourceErrorException(
                std::string("Unable to add resource of type ") + type.GetName()
                + ". It is not a resource type.", __FILE__, __LINE__);
      }

      dtUtil::DirectoryPush dp(mImpl->mContexts[slot]);
      ResourceDescriptor result;
      Project::ResourceTree* dataTypeTree = NULL;
      if (mImpl->mResourcesIndexed)
         dataTypeTree = &mImpl->GetResourcesOfType(type);

      result = mImpl->mResourceHelper.AddResource(newName, pathToFile, category, type, dataTypeTree, slot);

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::RemoveResource(const ResourceDescriptor& resource, ContextSlot slot)
   {
      if (!IsContextValid(slot))
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      if (IsReadOnly())
      {
         throw dtCore::ProjectReadOnlyException(
         std::string("The context is readonly."), __FILE__, __LINE__);
      }


      ContextSlot first = slot, last = slot + 1;
      if (slot == Project::DEFAULT_SLOT_VALUE)
      {
         first = 0;
         last = GetContextSlotCount();
      }

      for (ContextSlot i = first; i < last; ++i)
      {
         dtUtil::DirectoryPush dp(mImpl->mContexts[i]);
         Project::ResourceTree* resourceTree = NULL;
         if (mImpl->mResourcesIndexed)
         {
            resourceTree = &mImpl->mResources;
         }

         mImpl->mResourceHelper.RemoveResource(resource, resourceTree);
      }
   }

   //////////////////////////////////////////////////////////
   void ProjectImpl::IndexResources() const
   {
      if (mResourcesIndexed)
      {
         return;
      }

      mResources.clear();

      for (Project::ContextSlot i = 0; i != mContexts.size(); ++i)
      {
         dtUtil::DirectoryPush dp(mContexts[i]);
         // TODO ? break up the resources somehow by context rather than merging them all?
         mResourceHelper.IndexResources(mResources, i);
      }

      mResourcesIndexed = true;
   }

   /////////////////////////////////////////////////////////////////////////////
   Project::ResourceTree& ProjectImpl::GetResourcesOfType(const DataType& dataType) const
   {
      if (!mResourcesIndexed)
      {
         IndexResources();
      }

      ResourceTreeNode tr(dataType.GetName(), "", NULL, 0);
      Project::ResourceTree::iterator it = mResources.find(tr);

      if (it  == mResources.end())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "ERROR: call getResourcesOfType ended with no valid results!");
         throw dtCore::ProjectException(
            "ERROR: call GetResourcesOfType ended with no valid results!", __FILE__, __LINE__);
      }

      return it.tree_ref();

   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::GetResourcesOfType(const DataType& type, Project::ResourceTree& toFill) const
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      toFill.clear();
      if (!type.IsResource())
      {
         return;
      }

      toFill = mImpl->GetResourcesOfType(type);

   }

//     void Project::findmResources(Project::ResourceTree toFill,
//         const DataType& type,
//         const std::string& partialName,
//         const std::string& extension) const {
//
//         toFill.clear();
//
//         Project::ResourceTree::const_iterator it = mResources.begin();
//         for (; it != mResources.end(); ++it) {
//             if (it->isCategory() && it->getNodeText() == type.getName()) {
//
//                 Project::ResourceTree* matchingBranch = getMatchingBranch(it.in(), type, partialName, extension);
//
//                 //Project::ResourceTree*
//             }
//         }
//     }


//Later
/*    Project::ResourceTree* Project::getMatchingBranch(
      Project::ResourceTree::const_iterator level,
      const DataType& type,
      const std::string& partialName,
      const std::string& extension) const {

      Project::ResourceTree::iterator it = level;
      for (; it != mResources.end(); ++it) {
      if (it->isCategory() && it->getNodeText() == type.getName()) {

      Project::ResourceTree* matchingBranch = getMatchingBranch(it.in(), type, partialName, extension);


      } else if ((extension != "" && it->getResource().getExtension() == extension) &&
      (partialName != "" && it->getResource().getResourceName().find(partialName) < std::string::npos) ){
      Project::ResourceTree* node = new Project::ResourceTree;
      node->data(*it);
      return node;
      }
      }


      }*/
   //void archiveProject(const std::string& targetPath);
   //void unarchiveProject(const std::string& targetPath);

   /////////////////////////////////////////////////////////////////////////////
   bool Project::IsArchive() const
   {
      if (!IsContextValid())
      {
         throw dtCore::ProjectInvalidContextException(
         std::string("The context is not valid."), __FILE__, __LINE__);
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Project::IsReadOnly() const
   {
      return mImpl->mContextReadOnly;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::SetReadOnly(bool newReadOnly)
   {
      mImpl->mContextReadOnly = newReadOnly;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Project::SetEditMode(bool pInStage)
   {
      mImpl->mEditMode = pInStage;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Project::GetEditMode() const
   {
      return mImpl->mEditMode;
   }

   /////////////////////////////////////////////////////////////////////////////
   MapParser* Project::GetCurrentMapParser()
   {
      return mImpl->mParser.get();
   }
}
