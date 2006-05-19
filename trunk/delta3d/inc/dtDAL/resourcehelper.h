/* -*-c++-*-
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
#ifndef DELTA_RESOURCE_HELPER
#define DELTA_RESOURCE_HELPER

#include <string>
#include <map>

#include <osg/Referenced>
#include <osg/ref_ptr>

#include <dtUtil/fileutils.h>
#include <dtUtil/tree.h>
#include <dtDAL/exceptionenum.h>
#include "dtDAL/resourcetreenode.h"
#include "dtDAL/export.h"

namespace dtDAL
{
   class ResourceDescriptor;
   class DataType;

   /**
    * @class ResourceTypeHandler
    * @brief pure virtual class that handles a kind of resource.
    */
   class DT_DAL_EXPORT ResourceTypeHandler: public osg::Referenced
   {
      public:
         ResourceTypeHandler() {}
      protected:
         virtual ~ResourceTypeHandler() {}
      public:
         /**
          * @note the file type is provided so that implementations that only need to
          *       check extensions will not have to stat the file
          *       to see if it's a regular file or directory.
          * @param path The full path to the file or directory to check.
          * @param type The filetype, whether a directory or file.
          * @return true if the file name is a file that would be handled by this handler.
          */
		 virtual bool HandlesFile(const std::string& path, dtUtil::FileType type) const  = 0;

         /**
          * Creates a resource descriptor based on the path to the resource.
          * @param category the category of the resource.
          * @param fileName the name of the file.
          * @return The resource descriptor base on the path to the resource.
          */
         virtual ResourceDescriptor CreateResourceDescriptor(const std::string& category, const std::string& fileName) const = 0;

         /**
          * Import a resource into a project given the file, and the path.  The path can be
          * retrieved by calling ResourceHelper.createResourceCategory
          * @see ResourceHelper#createResourceCategory
          * @param newName the name to give the resource.  This should not include the extension.
          * @param srcPath the path to the file or directory to import
          * @param destCategoryPath the full path to the destination.  This must exist already.
          * @throws ExceptionEnum::ProjectFileNotFound if source of destination files do not exist.
          * @throws ExceptionEnum::ProjectIOException if an error occurs copying the data.
          * @return the display fileName associated with this resource
          */
         virtual const std::string ImportResourceToPath(const std::string& newName,
                                                        const std::string& srcPath, const std::string& destCategoryPath) const = 0;

         /**
          * removes a resource.  The current directory will be the root of the project when this
          * is called.
          * @param resourcePath path to the file the file getResourcePath returns
          */
         virtual void RemoveResource(const std::string& resourcePath) const = 0;

         /**
          * @return true if the resource imports directories.
          */
         virtual bool ImportsDirectory() const = 0;

         /**
          * @return true if the imported resource is a directory.
          */
         virtual bool ResourceIsDirectory() const = 0;

         /**
          * @return the extension string or undefined if ResourceIsDirectory if false
          */
         virtual const std::string& GetResourceDirectoryExtension() const = 0;
           
         /**
          * @note the extensions should not have the "*" in front of them so that they can be used internally.
          * @return a map of file extensions and their associated descriptions that will match this helper.
          */
         virtual const std::map<std::string, std::string>& GetFileFilters() const = 0;

         /**
          * @return A description for this suitable for answering the question "What does this import?"
          */
         virtual const std::string& GetTypeHandlerDescription() const = 0;
           
         /**
          * @return the resource data type this helper is used with.
          */
         virtual const DataType& GetResourceType() const = 0;
           
      private:
         //Hide the copy constructor and operator=
         ResourceTypeHandler(const ResourceTypeHandler&){}
         ResourceTypeHandler& operator=(const ResourceTypeHandler&) { return *this; }

   };

   /**
    * @class ResourceHelper
    * @brief This class is a registry of file types with handlers to allow file type specifice resource handling.
    */
   class DT_DAL_EXPORT ResourceHelper: public osg::Referenced
   {
      public:
         ResourceHelper();
         ~ResourceHelper();

         /**
          * Converts a resource descriptor into relative file path to the main file of the given resource.
          * It does not validate if the path is valid, nor does it have the context to do so.
          * @param descriptor the resource descriptor to convert.
          * @return the relative file path for the descriptor.
          */
         const std::string GetResourcePath(const ResourceDescriptor& descriptor) const;

         /**
          * @param resourceType the type of resource.  The datatype passed in MUST be a resource type.
          * @param filePath the handler for this file name will be returned.  This should specify a full path.
          * @return The resource type that will handle the give file and resource type or NULL if there is no match.
          * @throws ExceptionEnum::ProjectResourceError if the datatype is a primitive type, not a resource type.
          */
         const ResourceTypeHandler* GetHandlerForFile(const DataType& resourceType, const std::string& filePath) const;

         /**
          * @param resourceType the type of resource.  The datatype passed in MUST be a resource type.
          * @param toFill the vector to fill with the handlers.
          * @return The resource types that are available for the given handler.
          * @throws ExceptionEnum::ProjectResourceError if the datatype is a primitive type, not a resource type.
          */
         void GetHandlersForDataType(const DataType& resourceType, std::vector<osg::ref_ptr<const ResourceTypeHandler> >& toFill) const;

         /**
          * Registers a new type handler.  The method will get the datatype and applicable filters from
          * the handler itself.
          * @param handler The resource type handler to register.
          * @throws ExceptionEnum::ProjectResourceError if the datatype of the handler is a primitive type, not a resource type.
          */
         void RegisterResourceTypeHander(ResourceTypeHandler& handler);

         /**
          * Indexes the resources for a project.  It will use the handlers to index all of the different types of resources.
          * The current directory should be the top of the project when this is called.
          * @note The current directory must be the top of the project.
          * @param tree the tree of resources to fill.
          */
         void IndexResources(dtUtil::tree<ResourceTreeNode>& tree) const;

         /**
          * Creates a resource category.  The current directory should be
          * the project context when this is called.
          * @note The current directory must be the top of the project.
          * @param category The category of the resource.
          * @param type the datatype of the category
          * @param dataTypeTree a pointer to the resource tree for the datatype, or null to ignore resource indexing.
          * @param categoryInTree a output parameter that will be the resource tree for the newly created category.
          *                       This value is undefined if the dataTypeTree parameter is NULL.
          * @return the file path to the category relative to the project context.
          */
         const std::string CreateResourceCategory(const std::string& category, const DataType& type,
                                                  dtUtil::tree<ResourceTreeNode>* dataTypeTree,
                                                  dtUtil::tree<ResourceTreeNode>*& categoryInTree) const;

         /**
          * Removes a category. This method expects the current directory to be the root of the project
          * context.
          * @note The current directory must be the top of the project.
          * @param category The category to remove.
          * @param type The datatype the category is in.
          * @param recursive true to recursively delete the category and all subcategories and resources.
          * @param dataTypeTree a pointer to the resource tree for the datatype, or null to ignore resource indexing.
          * @return true if the category was removed, or didn't exist.  false if the category could not be removed because
          *         recursive was false and the category was not empty.
          * @throws ExceptionEnum::ProjectIOException if the could not complete because of some sort of IO exception.
          * @throws ExceptionEnum::ProjectResourceError if the resource could not be removed for reasons other than file io.
          * @throws ExceptionEnum::ProjectFileNotFound on rare occasion, this could possibly be thrown if the file contents
          *              are changed while the recusive delete is occuring.
          */
         bool RemoveResourceCategory(const std::string& category,
                                     const DataType& type, bool recursive, dtUtil::tree<ResourceTreeNode>* dataTypeTree) const;

         /**
          * Removes a resource and updates the tree.
          * @note The current directory must be the top of the project.
          * @param resource the resource descriptor of the resource to remove.
          * @param resourceTree the tree to update or NULL to ignore.
          */
         void RemoveResource(const ResourceDescriptor& resource,
                             dtUtil::tree<ResourceTreeNode>* resourceTree) const;

         /**
          * Adds a new resource to the current project and updates the tree.
          * @note The current directory must be the top of the project.
          * @param newName the new name of the resource.  This must NOT have an extension on it.
          * @param pathToFile the path to the file or directory to import.
          * @param category the category for the new resource.
          * @param type the datatype of the resource.
          * @param dataTypeTree pointer to the tree for the dataType to update, or NULL to ignore
          * @return the resource descriptor for the newly added resource.
          */
         const ResourceDescriptor AddResource(const std::string& newName,
                                              const std::string& pathToFile, const std::string& category,
                                              const DataType& type, dtUtil::tree<ResourceTreeNode>* dataTypeTree) const;

         /**
          * Removes a resource from the tree.  This is called by removeResource.
          * @param resourceTree the tree of resources.
          * @param resource the resource descriptor for the resource to remove.
          */
         void RemoveResourceFromTree(dtUtil::tree<ResourceTreeNode>& resourceTree,
                                    const ResourceDescriptor& resource) const;

      private:
         static osg::ref_ptr<ResourceHelper> mInstance;
         std::map<DataType*, osg::ref_ptr<ResourceTypeHandler> > mDefaultTypeHandlers;
         std::map<DataType*, std::map<std::string, osg::ref_ptr<ResourceTypeHandler> > > mTypeHandlers;
         //map if directory based resources based on the directory extension.
         std::map<DataType*, std::map<std::string, osg::ref_ptr<ResourceTypeHandler> > > mResourceDirectoryTypeHandlers;
         //multimap of resource handlers that import directories.
         std::multimap<DataType*, osg::ref_ptr<ResourceTypeHandler> > mDirectoryImportingTypeHandlers;
         dtUtil::Log* mLogger;

         //Hide the copy constructor and operator=
         ResourceHelper(const ResourceHelper&){}
         ResourceHelper& operator=(const ResourceHelper&) { return *this; }

         void IndexResources(dtUtil::FileUtils& fileUtils, dtUtil::tree<ResourceTreeNode>::iterator& i,
                           const DataType& dt, const std::string& categoryPath, const std::string& category) const;

         dtUtil::tree<ResourceTreeNode>* VerifyDirectoryExists(const std::string& path,
                                                               const std::string& category = "", dtUtil::tree<ResourceTreeNode>* parentTree = NULL) const;

         const ResourceTypeHandler* FindHandlerForDataTypeAndExtension(
            const std::map<DataType*, std::map<std::string, osg::ref_ptr<ResourceTypeHandler> > >& mapToSearch, 
            DataType& dt, const std::string& ext) const;
           
         static dtUtil::tree<ResourceTreeNode>::iterator FindTreeNodeFor(
            dtUtil::tree<ResourceTreeNode>& resources, const std::string& id);
   };

}
#endif //DELTA_RESOURCE_HELPER
