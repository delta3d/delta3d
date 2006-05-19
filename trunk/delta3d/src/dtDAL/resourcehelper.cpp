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
#include <iostream>

#include <osgDB/FileNameUtils>
#include <dtUtil/fileutils.h>

#include "dtDAL/resourcedescriptor.h"
#include "dtDAL/datatype.h"
#include "dtDAL/resourcetreenode.h"
#include "dtUtil/stringutils.h"
#include "dtDAL/rbodyresourcetypehandler.h"
#include "dtDAL/directoryresourcetypehandler.h"
#include "dtDAL/resourcehelper.h"

namespace dtDAL
{
   class DefaultResourceTypeHandler: public ResourceTypeHandler
   {
   public:
      DefaultResourceTypeHandler(DataType& dataType, const std::string& description, const std::map<std::string, std::string>& fileFilters):
      mDataType(&dataType), mFileFilters(fileFilters), mDescription(description)
      {
      }

      virtual ~DefaultResourceTypeHandler()
      {
      }

	  virtual bool HandlesFile(const std::string& path, dtUtil::FileType type) const
      {
         if (type != dtUtil::REGULAR_FILE)
            return false;

         const std::string extension = osgDB::getLowerCaseFileExtension(path);
         return mFileFilters.find(extension) != mFileFilters.end() || mFileFilters.find("") != mFileFilters.end();
      }

      virtual ResourceDescriptor CreateResourceDescriptor(
         const std::string& category, const std::string& fileName) const
      {
         const std::string resultString = mDataType->GetName() + ResourceDescriptor::DESCRIPTOR_SEPARATOR +
            category + ResourceDescriptor::DESCRIPTOR_SEPARATOR + fileName;
         return ResourceDescriptor(resultString,resultString);
      }

      virtual const std::string ImportResourceToPath(const std::string& newName, const std::string& srcPath,
                                                     const std::string& destCategoryPath) const
      {
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
		 dtUtil::FileType ftype = fileUtils.GetFileInfo(srcPath).fileType;

         if (ftype != dtUtil::REGULAR_FILE)
         {
            EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound,
                   std::string("No such file:\"") + srcPath + "\".");
         }

         std::string extension = osgDB::getLowerCaseFileExtension(srcPath);
         std::string resourceFileName = newName + '.' + extension;

         fileUtils.FileCopy(srcPath, destCategoryPath + dtUtil::FileUtils::PATH_SEPARATOR + resourceFileName, true);
         return resourceFileName;
      }

      virtual void RemoveResource(const std::string& resourcePath) const
      {
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         if (fileUtils.FileExists(resourcePath))
            fileUtils.FileDelete(resourcePath);
      }

      virtual bool ImportsDirectory() const { return false; }

      virtual bool ResourceIsDirectory() const { return false; }

      //this returns false because the resources is not a directory
      virtual const std::string& GetResourceDirectoryExtension() const { return mResourceDirExt; };

      virtual const std::map<std::string, std::string>& GetFileFilters() const
      {
         return mFileFilters;
      }

      virtual const std::string& GetTypeHandlerDescription() const
      {
         return mDescription;
      }

      virtual const DataType& GetResourceType() const { return *mDataType; }
   private:
      DataType* mDataType;
      std::map<std::string, std::string> mFileFilters;
      const std::string mResourceDirExt;
      const std::string mDescription;
   };


   //////////////////////////////////////////////////////////
   ResourceHelper::ResourceHelper()
   {
      mLogger = &dtUtil::Log::GetInstance("resourcehelper.cpp");

      for (std::vector<dtUtil::Enumeration*>::const_iterator i = DataType::Enumerate().begin();
           i != DataType::Enumerate().end(); ++i)
      {
         std::map<std::string, std::string> defFilter;
         defFilter.insert(std::make_pair("*","Any File"));
         DataType& d = *static_cast<DataType*>(*i);
         if (d.IsResource())
         {
            std::string description;
            if (d == DataType::SOUND)
            {
               description = "Sound Files";
               defFilter.insert(std::make_pair("wav","Wave audio format"));
               defFilter.insert(std::make_pair("aiff","Audio Interchange File Format"));
            }
            else if (d == DataType::STATIC_MESH)
            {
               description = "Static Mesh Files";
               defFilter.insert(std::make_pair("ive","Open Scene Graph binary scene data."));
               defFilter.insert(std::make_pair("flt","Open-Flight model"));
               defFilter.insert(std::make_pair("3ds","3D Studio Max"));
            }
            else if (d == DataType::TERRAIN)
            {
               description = "Static Mesh Terrain Files";
               defFilter.insert(std::make_pair("ive","Open Scene Graph binary terrain."));
            }
            else if (d == DataType::TEXTURE)
            {
               description = "Image Files";
               defFilter.insert(std::make_pair("png","Portable Network Graphics"));
               defFilter.insert(std::make_pair("gif","Graphics Interchange Format"));
               defFilter.insert(std::make_pair("tga","Targa"));
            }
            DefaultResourceTypeHandler* def = new DefaultResourceTypeHandler(d, description, defFilter);
            mDefaultTypeHandlers.insert(std::make_pair(&d,
                                                       osg::ref_ptr<ResourceTypeHandler>(def)));
         }
         std::map<std::string, osg::ref_ptr<ResourceTypeHandler> > extMap;

         mTypeHandlers.insert(std::make_pair(&d, extMap));

         mResourceDirectoryTypeHandlers.insert(std::make_pair(&d, extMap));
      }

      //Not yet working properly
      RegisterResourceTypeHander(*new RBodyResourceTypeHandler);

      std::vector<std::string> fltMasterFiles;
      fltMasterFiles.push_back("main.flt");
      fltMasterFiles.push_back("terrain.flt");
      RegisterResourceTypeHander(*new DirectoryResourceTypeHandler(DataType::TERRAIN,
                                                                   "master.flt", "flt", "fltt", "Open Flight Terrains", fltMasterFiles));

      std::vector<std::string> terrexMasterFiles;
      fltMasterFiles.push_back("main.txp");
      fltMasterFiles.push_back("master.txp");
      RegisterResourceTypeHander(*new DirectoryResourceTypeHandler(DataType::TERRAIN,
                                                                   "archive.txp", "txp", "terrex", "Terrex Terra Page Terrains", terrexMasterFiles));

      std::vector<std::string> threeDStudioMaxMasterFiles;
      threeDStudioMaxMasterFiles.push_back("main.3ds");
      threeDStudioMaxMasterFiles.push_back("master.3ds");
      //3d studio files often have upper-case extensions.
      threeDStudioMaxMasterFiles.push_back("main.3DS");
      threeDStudioMaxMasterFiles.push_back("master.3DS");
      threeDStudioMaxMasterFiles.push_back("terrain.3DS");
      RegisterResourceTypeHander(*new DirectoryResourceTypeHandler(DataType::TERRAIN,
                                                                   "terrain.3ds", "3ds", "3dst", "3D Studio Max Terrains", threeDStudioMaxMasterFiles));
   }

   //////////////////////////////////////////////////////////
   ResourceHelper::~ResourceHelper() {}

   //////////////////////////////////////////////////////////
   const ResourceTypeHandler* ResourceHelper::GetHandlerForFile(
      const DataType& resourceType, const std::string& filePath) const
   {
      if (resourceType.IsResource())
      {

         //file and directories handled by handlers MUST have extensions.
         std::string ext = osgDB::getLowerCaseFileExtension(filePath);

         //there is a bug in the function to get the extension when using relative paths
         //and the file has no extension.
         if (ext.find(dtUtil::FileUtils::PATH_SEPARATOR) != std::string::npos)
            ext.clear();

         //To work around a weird compiler bug...
         DataType* dt = const_cast<DataType*>(&resourceType);

		 dtUtil::FileType fType = dtUtil::FileUtils::GetInstance().GetFileInfo(filePath).fileType;

         if (fType == dtUtil::REGULAR_FILE && !ext.empty())
         {
            const ResourceTypeHandler* handler = FindHandlerForDataTypeAndExtension(mTypeHandlers, *dt, ext);
            //ask the handler if it handles the given file.
            if (handler != NULL && handler->HandlesFile(filePath, fType))
               return handler;

         }
         else
         {
            //the file doesn't exist, so we obviously can't check for a datatype.
            if (fType == dtUtil::FILE_NOT_FOUND)
            {
               return NULL;

            }
            else if (fType == dtUtil::DIRECTORY)
            {
               //if we have an extension, look for it in the ResourceDirectoryTypeHandlers
               if (!ext.empty())
               {
                  const ResourceTypeHandler* handler = FindHandlerForDataTypeAndExtension(mResourceDirectoryTypeHandlers, *dt, ext);
                  //ask the handler if it handles the given file.
                  if (handler != NULL && handler->HandlesFile(filePath, fType))
                     return handler;
               }

               //if ext is empty or the ResourceDirectoryTypeHandlers map had no matches, look at all
               //the directory importers to see if there is a match.
               for (std::multimap<DataType*, osg::ref_ptr<ResourceTypeHandler> >::const_iterator i = mDirectoryImportingTypeHandlers.find(dt);
                    i != mDirectoryImportingTypeHandlers.end() && i->first == dt; ++i)
               {
                  if (i->second->HandlesFile(filePath, dtUtil::DIRECTORY))
                  {
                     return i->second.get();
                  }
               }
            }
         }
         //otherwise get the handler default handler.
         return mDefaultTypeHandlers.find(dt)->second.get();
      }
      else
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError, "The datatype passed must be a resource type.");
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////
   const ResourceTypeHandler* ResourceHelper::FindHandlerForDataTypeAndExtension(
      const std::map<DataType*, std::map<std::string, osg::ref_ptr<ResourceTypeHandler> > >& mapToSearch,
      DataType& dt, const std::string& ext) const
   {
      std::map<DataType*, std::map<std::string, osg::ref_ptr<ResourceTypeHandler> > >::const_iterator found
         = mapToSearch.find(&dt);

      if (found != mapToSearch.end())
      {
         const std::map<std::string, osg::ref_ptr<ResourceTypeHandler> >& handlerMap = found->second;
         std::map<std::string, osg::ref_ptr<ResourceTypeHandler> >::const_iterator extFound = handlerMap.find(ext);
         if (extFound != handlerMap.end())
         {
            return extFound->second.get();
         }
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////
   void ResourceHelper::GetHandlersForDataType(
      const DataType& resourceType,
      std::vector<osg::ref_ptr<const ResourceTypeHandler> >& toFill) const
   {
      if (resourceType.IsResource())
      {
         toFill.clear();
         std::set<osg::ref_ptr<const ResourceTypeHandler> > tempSet;
         //so I can use it as a lookup key in the map.
         DataType* dt = const_cast<DataType*>(&resourceType);

         //insert the default handler
         tempSet.insert(osg::ref_ptr<const ResourceTypeHandler>(mDefaultTypeHandlers.find(dt)->second.get()));

         //lookup the handlers by type.
         std::map<DataType*, std::map<std::string, osg::ref_ptr<ResourceTypeHandler> > >::const_iterator found
            = mTypeHandlers.find(dt);

         if (found != mTypeHandlers.end())
         {
            const std::map<std::string, osg::ref_ptr<ResourceTypeHandler> >& extMap = found->second;
            for (std::map<std::string, osg::ref_ptr<ResourceTypeHandler> >::const_iterator i = extMap.begin();
                 i != extMap.end(); ++i)
            {
               tempSet.insert(osg::ref_ptr<const ResourceTypeHandler>(i->second.get()));
            }
         }

         //lookup directory handlers by type.
         found = mResourceDirectoryTypeHandlers.find(dt);

         if (found != mResourceDirectoryTypeHandlers.end())
         {
            const std::map<std::string, osg::ref_ptr<ResourceTypeHandler> >& extMap = found->second;
            for (std::map<std::string, osg::ref_ptr<ResourceTypeHandler> >::const_iterator i = extMap.begin();
                 i != extMap.end(); ++i)
            {
               tempSet.insert(osg::ref_ptr<const ResourceTypeHandler>(i->second.get()));
            }
         }

         //get all directory importers too.
         for (std::multimap<DataType*, osg::ref_ptr<ResourceTypeHandler> >::const_iterator i = mDirectoryImportingTypeHandlers.find(dt);
              i != mDirectoryImportingTypeHandlers.end() && i->first == dt; ++i)
         {
            tempSet.insert(osg::ref_ptr<const ResourceTypeHandler>(i->second.get()));
         }

         toFill.insert(toFill.begin(), tempSet.begin(), tempSet.end());
      }
      else
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError, "The datatype passed must be a resource type.");
      }

   }

   void ResourceHelper::RegisterResourceTypeHander(ResourceTypeHandler& handler)
   {
      //so I can use it as a lookup key in the map.
      DataType* dt = const_cast<DataType*>(&handler.GetResourceType());

      if (!dt->IsResource())
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError, "The datatype of resource handlers must a resource type.");

      //get the map for the
      std::map<DataType*, std::map<std::string, osg::ref_ptr<ResourceTypeHandler> > >::iterator found
         = mTypeHandlers.find(dt);

      if (found != mTypeHandlers.end())
      {
         std::map<std::string, osg::ref_ptr<ResourceTypeHandler> >& extMap = found->second;
         const std::map<std::string, std::string>& filters = handler.GetFileFilters();

         for (std::map<std::string, std::string>::const_iterator i = filters.begin(); i != filters.end(); ++i)
         {
            if (i->first.empty() || i->first == "*")
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                   "Attempting to insert new default handler by using extension \"%s\" with description \"%s\". Ignoring.",
                                   i->first.c_str(), i->second.c_str() );

               continue;
            }

            std::map<std::string, osg::ref_ptr<ResourceTypeHandler> >::iterator extFound = extMap.find(i->first);
            if (extFound == extMap.end())
            {
               //actually insert the handler.
               extMap.insert(std::make_pair(i->first, osg::ref_ptr<ResourceTypeHandler>(&handler)));
            }
            else
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                   "Not inserting new handler for extension \"%s\" with description \"%s\" because a handler is already registered for it.",
                                   i->first.c_str(), i->second.c_str() );
            }
         }

      }
      else
      {
         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                             "Unable to register type handler because no extension map exists for datatype \"%s\"",
                             dt->GetDisplayName().c_str() );
      }

      if (handler.ImportsDirectory())
         mDirectoryImportingTypeHandlers.insert(std::make_pair(dt, osg::ref_ptr<ResourceTypeHandler>(&handler)));

      if (handler.ResourceIsDirectory())
      {
         //get the map for the
         std::map<DataType*, std::map<std::string, osg::ref_ptr<ResourceTypeHandler> > >::iterator foundRDMap = mResourceDirectoryTypeHandlers.find(dt);

         if (foundRDMap != mResourceDirectoryTypeHandlers.end())
         {
            std::map<std::string, osg::ref_ptr<ResourceTypeHandler> >& dirExtMap = foundRDMap->second;

            std::map<std::string, osg::ref_ptr<ResourceTypeHandler> >::iterator dirExtFound = dirExtMap.find(handler.GetResourceDirectoryExtension());
            if (dirExtFound == dirExtMap.end())
            {
               //actually insert the handler.
               dirExtMap.insert(std::make_pair(handler.GetResourceDirectoryExtension(), osg::ref_ptr<ResourceTypeHandler>(&handler)));
            }
            else
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                   "Not inserting new handler for resource directory extension \"%s\" because a handler is already registered for it.",
                                   handler.GetResourceDirectoryExtension().c_str());
            }

         }

      }
   }

   //////////////////////////////////////////////////////////
   const std::string ResourceHelper::GetResourcePath(const ResourceDescriptor& resource) const
   {
      std::string path = resource.GetResourceIdentifier();
      unsigned int x = 0;
      //converting a descriptor to a string involves simply replacing the separators.
      for (std::string::iterator i = path.begin(); i != path.end(); ++i)
      {
         if (*i == ResourceDescriptor::DESCRIPTOR_SEPARATOR)
         {
            path[x] = dtUtil::FileUtils::PATH_SEPARATOR;
         }
         x++;
      }
      return path;
   }


   //////////////////////////////////////////////////////////
   void ResourceHelper::RemoveResource(const ResourceDescriptor& resource,
                                       dtUtil::tree<ResourceTreeNode>* resourceTree) const
   {

      std::vector<std::string> tokens;
      dtUtil::StringTokenizer<IsCategorySeparator>::tokenize(tokens, resource.GetResourceIdentifier());

      std::string currentPath;

      const ResourceTypeHandler* handler = NULL;

      //Finding out the datatype for the resource descriptor.
      dtUtil::Enumeration* e = DataType::GetValueForName(*tokens.begin());
      if (e == NULL)
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError,
                std::string("Could not find data type to match ") + *tokens.begin() + ".");

      DataType& type = static_cast<DataType&>(*e);

      //search the path until we find a handler.
      for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end(); ++i)
      {
         if (currentPath.empty())
         {
            currentPath = *i;
         }
         else
            currentPath += dtUtil::FileUtils::PATH_SEPARATOR + *i;

         //file and directories handled by handlers MUST have extensions.
         const std::string& ext = osgDB::getLowerCaseFileExtension(currentPath);

         if (!ext.empty())
            handler = GetHandlerForFile(type, currentPath);

         if (handler != NULL)
            break;
      }

      //it's only a problem that we don't have a handler if the file doesn't exist.
      if (handler == NULL && dtUtil::FileUtils::GetInstance().FileExists(currentPath))
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError,
                std::string("Could not find a resource handler for resource descriptor: ")
                + resource.GetResourceIdentifier() + " with type " + type.GetName() + ".");
      }
      else if (handler != NULL)
         handler->RemoveResource(GetResourcePath(resource));

      if (resourceTree != NULL)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Removing resource \"%s\" from the resource tree.",
                                resource.GetResourceIdentifier().c_str());
         RemoveResourceFromTree(*resourceTree, resource);
      }
   }

   //////////////////////////////////////////////////////////
   const ResourceDescriptor ResourceHelper::AddResource(const std::string& newName,
                                                        const std::string& pathToFile, const std::string& category,
                                                        const DataType& type, dtUtil::tree<ResourceTreeNode>*  dataTypeTree) const
   {

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

	  dtUtil::FileType ftype = fileUtils.GetFileInfo(pathToFile).fileType;

      if (ftype == dtUtil::FILE_NOT_FOUND)
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound,
                std::string("No such file:\"") + pathToFile + "\".");
      }

      dtUtil::tree<ResourceTreeNode>* categoryInTree;

      //create or get the path to the resource category
      std::string resourcePath = CreateResourceCategory(category, type,
                                                        dataTypeTree, categoryInTree);

      //get the handler for the file or directory to copy in.
      const ResourceTypeHandler* rth = GetHandlerForFile(type, pathToFile);

      if (rth == NULL)
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError,
                std::string("Could not find data type to match ") + pathToFile + ".");

      const std::string& resourceFileName = rth->ImportResourceToPath(newName, pathToFile, resourcePath);

      ResourceDescriptor result = rth->CreateResourceDescriptor(category, resourceFileName);

      if (categoryInTree != NULL)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Adding new resource \"%s\" to tree.",
                                result.GetResourceIdentifier().c_str());
         categoryInTree->insert(ResourceTreeNode(resourceFileName, category, &result));

      }
      return result;
   }

   //////////////////////////////////////////////////////////
   const std::string ResourceHelper::CreateResourceCategory(const std::string& category, const DataType& type,
                                                            dtUtil::tree<ResourceTreeNode>* dataTypeTree,
                                                            dtUtil::tree<ResourceTreeNode>*& categoryInTree) const
   {

      VerifyDirectoryExists(type.GetName());

      dtUtil::tree<ResourceTreeNode>* currentLevelTree;
      if (dataTypeTree != NULL)
      {
         currentLevelTree = dataTypeTree;
      }
      else
         currentLevelTree = NULL;

      std::string sofar = type.GetName();

      std::vector<std::string> tokens;
      dtUtil::StringTokenizer<IsCategorySeparator>::tokenize(tokens, category);
      std::string currentCategory;

      for (std::vector<std::string>::const_iterator i = tokens.begin(); i != tokens.end(); ++i)
      {
         if (currentCategory == "")
            currentCategory += *i;
         else
            currentCategory += ResourceDescriptor::DESCRIPTOR_SEPARATOR + *i;

         sofar += dtUtil::FileUtils::PATH_SEPARATOR + *i;

         currentLevelTree = VerifyDirectoryExists(sofar, currentCategory, currentLevelTree);
      }

      categoryInTree = currentLevelTree;

      return sofar;
   }

   //////////////////////////////////////////////////////////
   bool ResourceHelper::RemoveResourceCategory(const std::string& category,
                                               const DataType& type, bool recursive,
                                               dtUtil::tree<ResourceTreeNode>* dataTypeTree) const
   {
      bool result = true;
      //start with the datetype name
      std::string sofar = type.GetName() + dtUtil::FileUtils::PATH_SEPARATOR;

      //replace all
      for (std::string::const_iterator i = category.begin(); i != category.end(); ++i)
      {
         if (*i == ResourceDescriptor::DESCRIPTOR_SEPARATOR)
         {
            sofar += dtUtil::FileUtils::PATH_SEPARATOR;
         }
         else
            sofar += *i;
      }

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      if (fileUtils.DirExists(sofar))
      {
         //this only returns false if recursive is false and the directory is not empty.
         //otherwise it throws an exception.
         result = fileUtils.DirDelete(sofar, recursive);
         if (result && dataTypeTree != NULL)
         {
            dtUtil::tree<ResourceTreeNode>::iterator treeIt = FindTreeNodeFor(*dataTypeTree, category);
            if (treeIt != dataTypeTree->end())
               //get the tree above this one, and remove the iterator from it.
               treeIt.out().tree_ref().erase(treeIt);
            else
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                      "Attempting to remove category \"%s:%s\" from cached tree, but it wasn't found.",
                                      type.GetName().c_str(), category.c_str());
         }
      }
      return result;
   }

   //////////////////////////////////////////////////////////
   dtUtil::tree<ResourceTreeNode>::iterator ResourceHelper::FindTreeNodeFor(
      dtUtil::tree<ResourceTreeNode>& resources, const std::string& id)
   {

      std::vector<std::string> tokens;
      dtUtil::StringTokenizer<IsCategorySeparator>::tokenize(tokens, id);

      std::string currentCategory;

      dtUtil::tree<ResourceTreeNode>::iterator ti = resources.tree_iterator();

      for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end(); ++i)
      {
         if (ti == resources.end())
            return resources.end();

         //keep a full category string running
         //to create an accurate tree node to compare against.
         //Skip the first token because it is the datatype, not the category.
         std::string oldCategory = currentCategory;

         if (i != tokens.begin())
            if (currentCategory == "")
               currentCategory += *i;
            else
               currentCategory += ResourceDescriptor::DESCRIPTOR_SEPARATOR + *i;

         dtUtil::tree<ResourceTreeNode>::iterator temp = ti.tree_ref().find(ResourceTreeNode(*i, currentCategory));

         //if it hasn't been found, check to see if the current node represents a non-category
         //node
         if (temp == resources.end())
         {
            ResourceDescriptor searchTemp(currentCategory, id);
            //if this isn't found, we'll just return .end() anyway.
            return ti.tree_ref().find(ResourceTreeNode(*i, oldCategory, &searchTemp));
         } //else {
         //    std::cout << temp->getNodeText() << " " << temp->getFullCategory() << std::endl;
         //}

         ti = temp;
      }
      return ti;
   }

   //////////////////////////////////////////////////////////
   dtUtil::tree<ResourceTreeNode>* ResourceHelper::VerifyDirectoryExists(const std::string& path,
                                                                         const std::string& category, dtUtil::tree<ResourceTreeNode>* parentTree) const
   {
	   dtUtil::FileType ft = dtUtil::FileUtils::GetInstance().GetFileInfo(path).fileType;
      if (ft == dtUtil::REGULAR_FILE)
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError, std::string("File: \"")
                + path + "\" must be a directory.");
      }
      else if (ft == dtUtil::FILE_NOT_FOUND)
      {
         try
         {
            dtUtil::FileUtils::GetInstance().MakeDirectory(path);
         }
         catch (const dtUtil::Exception& ex)
         {
            std::ostringstream ss;
            ss << "Unable to create directory " << path << ". Error: " << ex.What();
            EXCEPT(dtDAL::ExceptionEnum::ProjectInvalidContext, ss.str());
         }

      }

      if (parentTree != NULL)
      {
         std::string lastPathPart = osgDB::getSimpleFileName(path);
         ResourceTreeNode newNode(lastPathPart, category);
         dtUtil::tree<ResourceTreeNode>::iterator iter= parentTree->find(newNode);
         if (iter == parentTree->end())
            return parentTree->insert(newNode).tree_ptr();
         else
            return iter.tree_ptr();
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////
   void ResourceHelper::RemoveResourceFromTree(
      dtUtil::tree<ResourceTreeNode>& resourceTree,
      const ResourceDescriptor& resource) const
   {

      //find the category node from that string.
      dtUtil::tree<ResourceTreeNode>::iterator resIt = FindTreeNodeFor(resourceTree, resource.GetResourceIdentifier());

      if (resIt != resourceTree.end() && !resIt->isCategory())
      {
         //find the resource node in category
         //Note: this is to remove the "const"
         dtUtil::tree<ResourceTreeNode>::iterator treeIt = resIt.out();
         treeIt.tree_ref().erase(resIt);
      }
      else
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                (std::string("Couldn't find resource tree node matching resource: ")
                                 + resource.GetResourceIdentifier() + ".").c_str());
      }
   }

   //////////////////////////////////////////////////////////
   void ResourceHelper::IndexResources(dtUtil::tree<ResourceTreeNode>& tree) const
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      for (std::vector<dtUtil::Enumeration *>::const_iterator i = DataType::Enumerate().begin();
           i != DataType::Enumerate().end(); ++i)
      {
         const DataType& dt = static_cast<DataType&>(**i);
         if (dt.IsResource())
         {
            ResourceTreeNode newNode(dt.GetName(), "");
            dtUtil::tree<ResourceTreeNode>::iterator dataTypeTree = tree.insert(newNode);

            //make sure the directory exists before even attempting to parse it.
            if (!fileUtils.DirExists(dt.GetName()))
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                      (std::string("Resource directory does not exist: ") + dt.GetName() + ".").c_str());
               continue;
            }

            fileUtils.PushDirectory(dt.GetName());
            try
            {
               IndexResources(dtUtil::FileUtils::GetInstance(), dataTypeTree, dt, std::string(""), std::string(""));
            }
            catch (const dtUtil::Exception& ex)
            {
               fileUtils.PopDirectory();
               throw ex;
            }
            fileUtils.PopDirectory();
         }
      }
   }

   //////////////////////////////////////////////////////////
   void ResourceHelper::IndexResources(dtUtil::FileUtils& fileUtils, dtUtil::tree<ResourceTreeNode>::iterator& i,
                                       const DataType& dt, const std::string& categoryPath, const std::string& category) const
   {
      std::string resourcePath = categoryPath;
      if (resourcePath.empty())
         resourcePath = ".";

      //push into the next subdirectory
      fileUtils.PushDirectory(osgDB::getSimpleFileName(resourcePath));
      try
      {
         dtUtil::DirectoryContents contents = fileUtils.DirGetFiles(fileUtils.CurrentDirectory());
         for (dtUtil::DirectoryContents::const_iterator j = contents.begin(); j != contents.end(); ++j)
         {
            if (*j == "." || *j == "..")
               continue;

            const std::string& currentFile = *j;

            dtUtil::FileInfo fi = fileUtils.GetFileInfo(currentFile);

            const ResourceTypeHandler* handler = NULL;
            //only look for a handler if the file/dir has an extension.
            if (!osgDB::getLowerCaseFileExtension(currentFile).empty())
               handler = GetHandlerForFile(dt, currentFile);

            if (fi.fileType == dtUtil::DIRECTORY && handler == NULL)
            {
               //always put a path separator on the end.  The categoryPath should always
               //have a separator on both ends.
               std::string newCategoryPath;
               if (!categoryPath.empty())
                  newCategoryPath = categoryPath + dtUtil::FileUtils::PATH_SEPARATOR + currentFile;
               else
                  newCategoryPath = currentFile;

               std::string newCategory;
               if (!category.empty())
                  newCategory = category + ResourceDescriptor::DESCRIPTOR_SEPARATOR + currentFile;
               else
                  newCategory = currentFile;

               ResourceTreeNode newNode(currentFile,newCategory);

               dtUtil::tree<ResourceTreeNode>::iterator subTree = i.tree_ref().insert(newNode);

               IndexResources(fileUtils, subTree, dt, newCategoryPath, newCategory);
            }
            else if (handler != NULL)
            {
               //the category will have a path separator on both ends.
               ResourceDescriptor rd = handler->CreateResourceDescriptor(category, currentFile);
               ResourceTreeNode newNode(currentFile, category, &rd);
               i.tree_ref().insert(newNode);
            }
            else
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "No hander returned for file %s.",
                                      currentFile.c_str());
            }
         }
      }
      catch (const dtUtil::Exception& ex)
      {
         fileUtils.PopDirectory();
         throw ex;
         
      }
      fileUtils.PopDirectory();
   }

}
