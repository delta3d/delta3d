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

#include <osgDB/FileNameUtils>
#include <dtUtil/fileutils.h>
#include <dtUtil/stringutils.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/datatype.h>
#include <dtCore/resourcetreenode.h>
#include <dtCore/directoryresourcetypehandler.h>
#include <dtCore/resourcehelper.h>

namespace dtCore
{

   DefaultResourceTypeHandler::DefaultResourceTypeHandler(DataType& dataType, const std::string& description, const std::map<std::string, std::string>& fileFilters):
   mDataType(&dataType), mFileFilters(fileFilters), mDescription(description)
   {
   }

   DefaultResourceTypeHandler::~DefaultResourceTypeHandler()
   {
   }

   bool DefaultResourceTypeHandler::HandlesFile(const std::string& path, dtUtil::FileType type) const
   {
      if (type != dtUtil::REGULAR_FILE)
         return false;

      const std::string extension = osgDB::getLowerCaseFileExtension(path);
      return mFileFilters.find(extension) != mFileFilters.end() || mFileFilters.find("") != mFileFilters.end();
   }

   ResourceDescriptor DefaultResourceTypeHandler::CreateResourceDescriptor(const std::string& category, const std::string& fileName) const
   {
      std::string resultString = mDataType->GetName() + ResourceDescriptor::DESCRIPTOR_SEPARATOR;

      if (!category.empty())
      {
         resultString += category + ResourceDescriptor::DESCRIPTOR_SEPARATOR;
      }
      resultString += fileName;

      return ResourceDescriptor(resultString,resultString);
   }

   const std::string DefaultResourceTypeHandler::ImportResourceToPath(const std::string& newName, const std::string& srcPath,
                                                  const std::string& destCategoryPath) const
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      dtUtil::FileType ftype = fileUtils.GetFileInfo(srcPath).fileType;

      if (ftype != dtUtil::REGULAR_FILE)
      {
         throw dtCore::ProjectFileNotFoundException(
                std::string("No such file:\"") + srcPath + "\".", __FILE__, __LINE__);
      }

      std::string extension = osgDB::getFileExtension(srcPath);
      std::string resourceFileName = newName;
      if (osgDB::getFileExtension(newName) != extension)
         resourceFileName.append(".").append(extension);

      fileUtils.FileCopy(srcPath, destCategoryPath + dtUtil::FileUtils::PATH_SEPARATOR + resourceFileName, true);
      return resourceFileName;
   }

   void DefaultResourceTypeHandler::RemoveResource(const std::string& resourcePath) const
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      if (fileUtils.FileExists(resourcePath))
         fileUtils.FileDelete(resourcePath);
   }

   bool DefaultResourceTypeHandler::ImportsDirectory() const { return false; }

   bool DefaultResourceTypeHandler::ResourceIsDirectory() const { return false; }

   //this returns false because the resources is not a directory
   const std::string& DefaultResourceTypeHandler::GetResourceDirectoryExtension() const { return mResourceDirExt; };

   const std::map<std::string, std::string>& DefaultResourceTypeHandler::GetFileFilters() const
   {
      return mFileFilters;
   }

   const std::string& DefaultResourceTypeHandler::GetTypeHandlerDescription() const
   {
      return mDescription;
   }

   const DataType& DefaultResourceTypeHandler::GetResourceType() const { return *mDataType; }

   ////////////////////////////////////////////////////////////////////////////////
   class ToLowerClass
   {
   public:
      ToLowerClass(){}

      char operator() (char& elem) const
      {
         return tolower(elem);
      }
   };

   //////////////////////////////////////////////////////////
   ResourceHelper::ResourceHelper()
   {
      mLogger = &dtUtil::Log::GetInstance("resourcehelper.cpp");

      for (std::vector<dtCore::DataType*>::const_iterator i = DataType::EnumerateType().begin();
           i != DataType::EnumerateType().end(); ++i)
      {
         std::map<std::string, std::string> defFilter;
         //defFilter.insert(std::make_pair("*","Any File"));
         DataType& d = **i;

         if (d.IsResource())
         {
            std::map<std::string, std::string> extFilter;
            DefaultResourceTypeHandler* handler = NULL;
            std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> > extMap;

            std::string description;
            if (d == DataType::SOUND)
            {
               description = "Sound Files";

               extFilter.insert(std::make_pair("wav","Wave audio format."));
               handler = new DefaultResourceTypeHandler(d, "Wave audio format.", extFilter);
               extMap.insert(std::make_pair("wav", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.clear();
               extFilter.insert(std::make_pair("aiff","Audio Interchange File Format."));
               handler = new DefaultResourceTypeHandler(d, "Audio Interchange File Format.", extFilter);
               extMap.insert(std::make_pair("aiff", dtCore::RefPtr<ResourceTypeHandler>(handler)));
            }
            else if (d == DataType::STATIC_MESH)
            {
               std::map<std::string, std::string> extContainer;

               extContainer.insert(std::make_pair("3dc","3D Canvas file"));
               extContainer.insert(std::make_pair("ac","AC3D geometry format"));
               extContainer.insert(std::make_pair("dxf","AutoCAD file"));
               extContainer.insert(std::make_pair("ive","Open Scene Graph binary scene data (old)."));
               extContainer.insert(std::make_pair("lwo","LightWave Object"));
               extContainer.insert(std::make_pair("md2","Quake model file"));
               extContainer.insert(std::make_pair("mdl","MDL"));
               extContainer.insert(std::make_pair("obj","Wavefront Technologies OBJ format"));
               extContainer.insert(std::make_pair("flt","Open-Flight model."));
               extContainer.insert(std::make_pair("osg","Open Scene Graph ascii scene data (old)."));
               extContainer.insert(std::make_pair("osgt","Open Scene Graph ascii scene data."));
               extContainer.insert(std::make_pair("osgb","Open Scene Graph binary scene data."));
               extContainer.insert(std::make_pair("3ds","3D Studio Max."));
               extContainer.insert(std::make_pair("ai","Waypoint file."));
               extContainer.insert(std::make_pair("zip","Wrapping another file in a zip."));
               extContainer.insert(std::make_pair("dae","Collada"));
               extContainer.insert(std::make_pair("earth", "Earth file."));
               extContainer.insert(std::make_pair("ply", "Stanford point cloud."));
               extContainer.insert(std::make_pair("dtphys", "dtPhysics compiled physics mesh."));

               handler = new DefaultResourceTypeHandler(d, "Static Mesh Files", extContainer);

               std::map<std::string, std::string>::iterator extItr = extContainer.begin();
               while (extItr != extContainer.end())
               {
                  extMap.insert(std::make_pair(extItr->first, dtCore::RefPtr<ResourceTypeHandler>(handler)));
                  ++extItr;
               }
            }
            else if (d == DataType::SKELETAL_MESH)
            {
               description = "Skeletal Mesh Files";

               extFilter.insert(std::make_pair("dtchar","Delta Character."));
               extFilter.insert(std::make_pair("xml","Extensible Markup Language."));
               handler = new DefaultResourceTypeHandler(d, "Delta Character XML.", extFilter);
               extMap.insert(std::make_pair("xml", dtCore::RefPtr<ResourceTypeHandler>(handler)));
               extMap.insert(std::make_pair("dtchar", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.clear();
               extFilter.insert(std::make_pair("zip","Wrapping another file in a zip."));
               handler = new DefaultResourceTypeHandler(d, "Wrapping another file in a zip.", extFilter);
               extMap.insert(std::make_pair("zip", dtCore::RefPtr<ResourceTypeHandler>(handler)));
            }
            else if (d == DataType::TERRAIN)
            {
               description = "Static Mesh Terrain Files";

               extFilter.insert(std::make_pair("ive","Open Scene Graph binary terrain (old)."));
               extFilter.insert(std::make_pair("osgt","Open Scene Graph binary scene data."));
               extFilter.insert(std::make_pair("osgb","Open Scene Graph binary scene data."));
               extFilter.insert(std::make_pair("osg","Open Scene Graph ascii scene data (old)."));
               handler = new DefaultResourceTypeHandler(d, "Open Scene Graph scene data.", extFilter);
               extMap.insert(std::make_pair("ive", dtCore::RefPtr<ResourceTypeHandler>(handler)));
               extMap.insert(std::make_pair("osgt", dtCore::RefPtr<ResourceTypeHandler>(handler)));
               extMap.insert(std::make_pair("osgb", dtCore::RefPtr<ResourceTypeHandler>(handler)));
               extMap.insert(std::make_pair("osg", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.insert(std::make_pair("dae","Collada"));
               handler = new DefaultResourceTypeHandler(d, "Collada", extFilter);
               extMap.insert(std::make_pair("dae", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.clear();
               extFilter.insert(std::make_pair("zip","Wrapping another file in a zip."));
               handler = new DefaultResourceTypeHandler(d, "Wrapping another file in a zip.", extFilter);
               extMap.insert(std::make_pair("zip", dtCore::RefPtr<ResourceTypeHandler>(handler)));
            }
            else if (d == DataType::TEXTURE)
            {
               description = "Image Files";

               extFilter.insert(std::make_pair("png","Portable Network Graphics."));
               handler = new DefaultResourceTypeHandler(d, "Portable Network Graphics.", extFilter);
               extMap.insert(std::make_pair("png", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.clear();
               extFilter.insert(std::make_pair("gif","Graphics Interchange Format."));
               handler = new DefaultResourceTypeHandler(d, "Graphics Interchange Format.", extFilter);
               extMap.insert(std::make_pair("gif", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.clear();
               extFilter.insert(std::make_pair("tga","Targa Format."));
               handler = new DefaultResourceTypeHandler(d, "Targa Format.", extFilter);
               extMap.insert(std::make_pair("tga", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.clear();
               extFilter.insert(std::make_pair("jpg","Joint Photographic Group Format."));
               handler = new DefaultResourceTypeHandler(d, "Joint Photographic Group Format.", extFilter);
               extMap.insert(std::make_pair("jpg", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.clear();
               extFilter.insert(std::make_pair("bmp","Bitmap Format."));
               handler = new DefaultResourceTypeHandler(d, "Bitmap Format.", extFilter);
               extMap.insert(std::make_pair("bmp", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.clear();
               extFilter.insert(std::make_pair("tif","tiff Format."));
               extFilter.insert(std::make_pair("tiff","tiff Format."));
               handler = new DefaultResourceTypeHandler(d, "Tiff Format.", extFilter);
               extMap.insert(std::make_pair("tiff", dtCore::RefPtr<ResourceTypeHandler>(handler)));
               extMap.insert(std::make_pair("tif", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.clear();
               extFilter.insert(std::make_pair("rgb","Red Green Blue Format."));
               extFilter.insert(std::make_pair("rgba","Red Green Blue Alpha Format."));
               handler = new DefaultResourceTypeHandler(d, "Red Green Blue Format.", extFilter);
               extMap.insert(std::make_pair("rgb", dtCore::RefPtr<ResourceTypeHandler>(handler)));
               extMap.insert(std::make_pair("rgba", dtCore::RefPtr<ResourceTypeHandler>(handler)));

               extFilter.clear();
               extFilter.insert(std::make_pair("dds","GPU compatible compressed textures."));
               handler = new DefaultResourceTypeHandler(d, "GPU compatible compressed textures.", extFilter);
               extMap.insert(std::make_pair("dds", dtCore::RefPtr<ResourceTypeHandler>(handler)));
            }
            else if (d == DataType::PARTICLE_SYSTEM)
            {
               description = "Particle Files";

               extFilter.insert(std::make_pair("osg", "Open Scene Graph ascii scene data."));
               extFilter.insert(std::make_pair("osgb", "Open Scene Graph binary scene data."));
               extFilter.insert(std::make_pair("osgt", "Open Scene Graph ascii scene data."));
               handler = new DefaultResourceTypeHandler(d, "Open Scene Graph scene data.", extFilter);
               extMap.insert(std::make_pair("osg", dtCore::RefPtr<ResourceTypeHandler>(handler)));
               extMap.insert(std::make_pair("osgt", dtCore::RefPtr<ResourceTypeHandler>(handler)));
               extMap.insert(std::make_pair("osgb", dtCore::RefPtr<ResourceTypeHandler>(handler)));
            }
            else if (d == DataType::PREFAB)
            {
               description = "Prefab Resources";

               extFilter.insert(std::make_pair("dtprefab","Delta Prefab."));
               handler = new DefaultResourceTypeHandler(d, "Delta Prefab.", extFilter);
               extMap.insert(std::make_pair("dtprefab", dtCore::RefPtr<ResourceTypeHandler>(handler)));
            }
            else if (d == DataType::SHADER)
            {
               description = "Pixel Shaders";
               extFilter.insert(std::make_pair("dtshader", "Delta3D Shaders"));
               handler = new DefaultResourceTypeHandler(d, "Delta3D Shaders", extFilter);
               extMap.insert(std::make_pair("dtshader", dtCore::RefPtr<ResourceTypeHandler>(handler)));
            }
            else if (d == DataType::DIRECTOR)
            {
               description = "Director Graphs";
               extFilter.insert(std::make_pair("dtdirb", "Binary Director Scripts"));
               extFilter.insert(std::make_pair("dtdir", "XML Director Scripts"));
               handler = new DefaultResourceTypeHandler(d, "Director Scripts", extFilter);
               extMap.insert(std::make_pair("dtdir", dtCore::RefPtr<ResourceTypeHandler>(handler)));
               extMap.insert(std::make_pair("dtdirb", dtCore::RefPtr<ResourceTypeHandler>(handler)));
            }

            mTypeHandlers.insert(std::make_pair(&d, extMap));
         }

         std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> > extMap;
         mResourceDirectoryTypeHandlers.insert(std::make_pair(&d, extMap));
      }

      std::vector<std::string> fltMasterFiles;
      fltMasterFiles.push_back("main.flt");
      fltMasterFiles.push_back("terrain.flt");
      RegisterResourceTypeHander(*new DirectoryResourceTypeHandler(DataType::TERRAIN,
                                                                   "master.flt", "flt", "fltt", "Open Flight Terrains", fltMasterFiles));

      std::vector<std::string> terrexMasterFiles;
      terrexMasterFiles.push_back("main.txp");
      terrexMasterFiles.push_back("master.txp");
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
         LOGN_DEBUG("resourcehelper.cpp", "Examing handler for file: " + filePath);
         //file and directories handled by handlers MUST have extensions.
         std::string ext = osgDB::getLowerCaseFileExtension(filePath);

         //there is a bug in the function to get the extension when using relative paths
         //and the file has no extension.
         if (ext.find(dtUtil::FileUtils::PATH_SEPARATOR) != std::string::npos)
         {
            ext.clear();
         }

         //To work around a weird compiler bug...
         DataType* dt = const_cast<DataType*>(&resourceType);

         dtUtil::FileType fType = dtUtil::FileUtils::GetInstance().GetFileInfo(filePath).fileType;

         if (fType == dtUtil::REGULAR_FILE && !ext.empty())
         {
            const ResourceTypeHandler* handler = FindHandlerForDataTypeAndExtension(mTypeHandlers, *dt, ext);
            //ask the handler if it handles the given file.
            if (handler != NULL && handler->HandlesFile(filePath, fType))
            {
               return handler;
            }
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
               for (std::multimap<DataType*, dtCore::RefPtr<ResourceTypeHandler> >::const_iterator i = mDirectoryImportingTypeHandlers.find(dt);
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
         std::map<DataType*, dtCore::RefPtr<ResourceTypeHandler> >::const_iterator found =
            mDefaultTypeHandlers.find(dt);

         if (found != mDefaultTypeHandlers.end())
         {
            if (found->second.get()->HandlesFile(filePath, fType))
            {
               return found->second.get();
            }
         }
      }
      else
      {
         throw dtCore::ProjectResourceErrorException(
            "The datatype passed must be a resource type.", __FILE__, __LINE__);
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////
   const ResourceTypeHandler* ResourceHelper::FindHandlerForDataTypeAndExtension(
      const std::map<DataType*, std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> > >& mapToSearch,
      DataType& dt, std::string ext) const
   {
      std::map<DataType*, std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> > >::const_iterator found
         = mapToSearch.find(&dt);

      // Convert the extension to a lowercase value.
      std::transform(ext.begin(), ext.end(), ext.begin(), ToLowerClass());

      if (found != mapToSearch.end())
      {
         const std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> >& handlerMap = found->second;
         std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> >::const_iterator extFound = handlerMap.find(ext);
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
      std::vector<const ResourceTypeHandler*>& toFill) const
   {
      if (resourceType.IsResource())
      {
         toFill.clear();
         std::set<const ResourceTypeHandler* > tempSet;
         //so I can use it as a lookup key in the map.
         DataType* dt = const_cast<DataType*>(&resourceType);

         //insert the default handler
         if (mDefaultTypeHandlers.find(dt) != mDefaultTypeHandlers.end())
         {
            tempSet.insert(mDefaultTypeHandlers.find(dt)->second.get());
         }

         //lookup the handlers by type.
         std::map<DataType*, std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> > >::const_iterator found
            = mTypeHandlers.find(dt);

         if (found != mTypeHandlers.end())
         {
            const std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> >& extMap = found->second;
            for (std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> >::const_iterator i = extMap.begin();
                 i != extMap.end(); ++i)
            {
               tempSet.insert(i->second.get());
            }
         }

         //lookup directory handlers by type.
         found = mResourceDirectoryTypeHandlers.find(dt);

         if (found != mResourceDirectoryTypeHandlers.end())
         {
            const std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> >& extMap = found->second;
            for (std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> >::const_iterator i = extMap.begin();
                 i != extMap.end(); ++i)
            {
               tempSet.insert(i->second.get());
            }
         }

         //get all directory importers too.
         for (std::multimap<DataType*, dtCore::RefPtr<ResourceTypeHandler> >::const_iterator i = mDirectoryImportingTypeHandlers.find(dt);
              i != mDirectoryImportingTypeHandlers.end() && i->first == dt; ++i)
         {
            tempSet.insert(i->second.get());
         }

         toFill.insert(toFill.begin(), tempSet.begin(), tempSet.end());
      }
      else
      {
         throw dtCore::ProjectResourceErrorException(
            "The datatype passed must be a resource type.", __FILE__, __LINE__);
      }

   }

   void ResourceHelper::RegisterResourceTypeHander(ResourceTypeHandler& handler)
   {
      //so I can use it as a lookup key in the map.
      DataType* dt = const_cast<DataType*>(&handler.GetResourceType());

      if (!dt->IsResource())
         throw dtCore::ProjectResourceErrorException(
         "The datatype of resource handlers must a resource type.", __FILE__, __LINE__);

      //get the map for the
      std::map<DataType*, std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> > >::iterator found
         = mTypeHandlers.find(dt);

      if (found != mTypeHandlers.end())
      {
         std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> >& extMap = found->second;
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

            std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> >::iterator extFound = extMap.find(i->first);
            if (extFound == extMap.end())
            {
               //actually insert the handler.
               extMap.insert(std::make_pair(i->first, dtCore::RefPtr<ResourceTypeHandler>(&handler)));
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
         mDirectoryImportingTypeHandlers.insert(std::make_pair(dt, dtCore::RefPtr<ResourceTypeHandler>(&handler)));

      if (handler.ResourceIsDirectory())
      {
         //get the map for the
         std::map<DataType*, std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> > >::iterator foundRDMap = mResourceDirectoryTypeHandlers.find(dt);

         if (foundRDMap != mResourceDirectoryTypeHandlers.end())
         {
            std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> >& dirExtMap = foundRDMap->second;

            std::map<std::string, dtCore::RefPtr<ResourceTypeHandler> >::iterator dirExtFound = dirExtMap.find(handler.GetResourceDirectoryExtension());
            if (dirExtFound == dirExtMap.end())
            {
               //actually insert the handler.
               dirExtMap.insert(std::make_pair(handler.GetResourceDirectoryExtension(), dtCore::RefPtr<ResourceTypeHandler>(&handler)));
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
      {
         throw dtCore::ProjectResourceErrorException(
                std::string("Could not find data type to match ") + *tokens.begin() + ".", __FILE__, __LINE__);
      }

      DataType& type = static_cast<DataType&>(*e);

      //search the path until we find a handler.
      for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end(); ++i)
      {
         if (currentPath.empty())
         {
            currentPath = *i;
         }
         else
         {
            currentPath += dtUtil::FileUtils::PATH_SEPARATOR + *i;
         }

         //file and directories handled by handlers MUST have extensions.
         const std::string& ext = osgDB::getLowerCaseFileExtension(currentPath);

         if (!ext.empty())
         {
            handler = GetHandlerForFile(type, currentPath);
         }

         if (handler != NULL)
         {
            break;
         }
      }

      //it's only a problem that we don't have a handler if the file doesn't exist.
      if (handler == NULL && dtUtil::FileUtils::GetInstance().FileExists(currentPath))
      {
         throw dtCore::ProjectResourceErrorException(
                std::string("Could not find a resource handler for resource descriptor: ")
                + resource.GetResourceIdentifier() + " with type " + type.GetName() + ".", __FILE__, __LINE__);
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
                                                        const DataType& type, dtUtil::tree<ResourceTreeNode>*  dataTypeTree,
                                                        unsigned contextSlot) const
   {

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      dtUtil::FileType ftype = fileUtils.GetFileInfo(pathToFile).fileType;

      if (ftype == dtUtil::FILE_NOT_FOUND)
      {
         throw dtCore::ProjectFileNotFoundException(
                std::string("No such file:\"") + pathToFile + "\".", __FILE__, __LINE__);
      }

      dtUtil::tree<ResourceTreeNode>* categoryInTree;

      //create or get the path to the resource category
      std::string resourcePath = CreateResourceCategory(category, type, contextSlot,
                                                        dataTypeTree, categoryInTree);

      //get the handler for the file or directory to copy in.
      const ResourceTypeHandler* rth = GetHandlerForFile(type, pathToFile);

      if (rth == nullptr)
         throw dtCore::ProjectResourceErrorException(
                std::string("Could not find data type to match ") + pathToFile + ".", __FILE__, __LINE__);

      const std::string& resourceFileName = rth->ImportResourceToPath(newName, pathToFile, resourcePath);

      ResourceDescriptor result = rth->CreateResourceDescriptor(category, resourceFileName);

      if (categoryInTree != nullptr)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Adding new resource \"%s\" to tree.",
                                result.GetResourceIdentifier().c_str());
         categoryInTree->insert(ResourceTreeNode(resourceFileName, category, &result, contextSlot));

      }
      return result;
   }

   //////////////////////////////////////////////////////////
   const std::string ResourceHelper::CreateResourceCategory(const std::string& category, const DataType& type,
                                                            unsigned contextSlot,
                                                            dtUtil::tree<ResourceTreeNode>* dataTypeTree,
                                                            dtUtil::tree<ResourceTreeNode>*& categoryInTree) const
   {

      VerifyDirectoryExists(type.GetName(), contextSlot);

      dtUtil::tree<ResourceTreeNode>* currentLevelTree;
      if (dataTypeTree != NULL)
      {
         currentLevelTree = dataTypeTree;
      }
      else
      {
         currentLevelTree = NULL;
      }

      std::string sofar = type.GetName();

      if (!category.empty())
      {
         std::vector<std::string> tokens;
         dtUtil::StringTokenizer<IsCategorySeparator>::tokenize(tokens, category);
         std::string currentCategory;

         for (std::vector<std::string>::const_iterator i = tokens.begin(); i != tokens.end(); ++i)
         {
            if (currentCategory.empty())
               currentCategory += *i;
            else
               currentCategory += ResourceDescriptor::DESCRIPTOR_SEPARATOR + *i;

            sofar += dtUtil::FileUtils::PATH_SEPARATOR + *i;

            currentLevelTree = VerifyDirectoryExists(sofar, contextSlot, currentCategory, currentLevelTree);
         }
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

      dtUtil::tree<ResourceTreeNode>::iterator ti = resources.get_tree_iterator();

      for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end(); ++i)
      {
         if (ti == resources.end())
            return resources.end();

         //keep a full category string running
         //to create an accurate tree node to compare against.
         //Skip the first token because it is the datatype, not the category.
         std::string oldCategory = currentCategory;

         if (i != tokens.begin())
         {
            if (currentCategory == "")
            {
               currentCategory += *i;
            }
            else
            {
               currentCategory += ResourceDescriptor::DESCRIPTOR_SEPARATOR + *i;
            }
         }

         // The ref slot on the ResourceTreeNode isn't used in searching.
         dtUtil::tree<ResourceTreeNode>::iterator temp = ti.tree_ref().find(ResourceTreeNode(*i, currentCategory, NULL, 0));

         //if it hasn't been found, check to see if the current node represents a non-category
         //node
         if (temp == resources.end())
         {
            ResourceDescriptor searchTemp(currentCategory, id);
            //if this isn't found, we'll just return .end() anyway.
            return ti.tree_ref().find(ResourceTreeNode(*i, oldCategory, &searchTemp, 0));
         } //else {
         //    std::cout << temp->getNodeText() << " " << temp->getFullCategory() << std::endl;
         //}

         ti = temp;
      }
      return ti;
   }

   //////////////////////////////////////////////////////////
   dtUtil::tree<ResourceTreeNode>* ResourceHelper::VerifyDirectoryExists(
            const std::string& path,
            unsigned contextSlot,
            const std::string& category,
            dtUtil::tree<ResourceTreeNode>* parentTree) const
   {
      dtUtil::FileType ft = dtUtil::FileUtils::GetInstance().GetFileInfo(path).fileType;
      if (ft == dtUtil::REGULAR_FILE)
      {
         throw dtCore::ProjectResourceErrorException( std::string("File: \"")
                + path + "\" must be a directory.", __FILE__, __LINE__);
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
            throw dtCore::ProjectInvalidContextException(ss.str(), __FILE__, __LINE__);
         }

      }

      if (parentTree != NULL)
      {
         std::string lastPathPart = osgDB::getSimpleFileName(path);
         ResourceTreeNode newNode(lastPathPart, category, NULL, contextSlot);
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
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                (std::string("Couldn't find resource tree node matching resource: ")
                                 + resource.GetResourceIdentifier() + ".").c_str());
         }
      }
   }

   //////////////////////////////////////////////////////////
   void ResourceHelper::IndexResources(dtUtil::tree<ResourceTreeNode>& tree, unsigned referenceSlot) const
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      for (std::vector<dtCore::DataType *>::const_iterator i = DataType::EnumerateType().begin();
           i != DataType::EnumerateType().end(); ++i)
      {
         const DataType& dt = **i;
         if (dt.IsResource())
         {
            ResourceTreeNode newNode(dt.GetName(), "", NULL, referenceSlot);
            dtUtil::tree<ResourceTreeNode>::iterator dataTypeTree = tree.find(newNode);
            if (dataTypeTree == tree.end())
            {
               dataTypeTree = tree.insert(newNode);
            }

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
               IndexResources(dtUtil::FileUtils::GetInstance(), dataTypeTree, dt, std::string(""), std::string(""), referenceSlot);
            }
            catch (const dtUtil::Exception&)
            {
               fileUtils.PopDirectory();
               throw;
            }
            fileUtils.PopDirectory();
         }
      }
   }

   //////////////////////////////////////////////////////////
   void ResourceHelper::IndexResources(dtUtil::FileUtils& fileUtils, dtUtil::tree<ResourceTreeNode>::iterator& i,
                                       const DataType& dt, const std::string& categoryPath, const std::string& category,
                                       unsigned referenceSlot) const
   {
      std::string resourcePath = categoryPath;
      if (resourcePath.empty())
      {
         resourcePath = ".";
      }

      //push into the next subdirectory
      fileUtils.PushDirectory(osgDB::getSimpleFileName(resourcePath));
      try
      {
         dtUtil::DirectoryContents contents = fileUtils.DirGetFiles(fileUtils.CurrentDirectory());
         dtUtil::DirectoryContents folders(contents.size());
         dtUtil::DirectoryContents files(contents.size());
         for (dtUtil::DirectoryContents::const_iterator j = contents.begin(); j != contents.end(); ++j)
         {
            if (*j == "." || *j == "..")
            {
               continue;
            }

            if (*j == ".svn")
            {
               continue;
            }

            const std::string& currentFile = *j;

            // For the sake of the indexer, directories with an extension are treated like files.
            std::string::size_type dot = currentFile.find_last_of('.');
            if (dot == std::string::npos)
            {
               folders.push_back(currentFile);
            }
            else
            {
               files.push_back(currentFile);
            }
         }

         contents.swap(folders);
         // size was reserved above
         contents.insert(contents.end(), files.begin(), files.end());

         for (int fileIndex = 0; fileIndex < (int)contents.size(); fileIndex++)
         {
            const std::string& currentFile = contents[fileIndex];

            dtUtil::FileInfo fi = fileUtils.GetFileInfo(currentFile);

            const ResourceTypeHandler* handler = NULL;
            //only look for a handler if the file/dir has an extension.
            if (!osgDB::getLowerCaseFileExtension(currentFile).empty())
            {
               handler = GetHandlerForFile(dt, currentFile);
            }

            if (fi.fileType == dtUtil::DIRECTORY && handler == NULL)
            {
               //always put a path separator on the end.  The categoryPath should always
               //have a separator on both ends.
               std::string newCategoryPath;
               if (!categoryPath.empty())
               {
                  newCategoryPath = categoryPath + dtUtil::FileUtils::PATH_SEPARATOR + currentFile;
               }
               else
               {
                  newCategoryPath = currentFile;
               }

               std::string newCategory;
               if (!category.empty())
               {
                  newCategory = category + ResourceDescriptor::DESCRIPTOR_SEPARATOR + currentFile;
               }
               else
               {
                  newCategory = currentFile;
               }

               ResourceTreeNode newNode(currentFile,newCategory, NULL, referenceSlot);

               dtUtil::tree<ResourceTreeNode>::iterator subtree = i.tree_ref().find(newNode);

               if (subtree == i.tree_ref().end())
               {
                  subtree = i.tree_ref().insert(newNode);
               }

               IndexResources(fileUtils, subtree, dt, newCategoryPath, newCategory, referenceSlot);
            }
            else if (handler != NULL)
            {
               //the category will have a path separator on both ends.
               ResourceDescriptor rd = handler->CreateResourceDescriptor(category, currentFile);
               ResourceTreeNode newNode(currentFile, category, &rd, referenceSlot);

               dtUtil::tree<ResourceTreeNode>::iterator collidingNode = i.tree_ref().find(newNode);

               if (collidingNode == i.tree_ref().end())
               {
                  i.tree_ref().insert(newNode);
               }
            }
            else
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "No hander returned for file %s.",
                                      currentFile.c_str());
               }
            }
         }
      }
      catch (const dtUtil::Exception&)
      {
         fileUtils.PopDirectory();
         throw;
      }
      fileUtils.PopDirectory();
   }

}
