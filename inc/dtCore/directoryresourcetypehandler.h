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
 * David Guthrie
 */

#ifndef DELTA_DIRECTORY_RESOURCE_TYPE_HANDLER
#define DELTA_DIRECTORY_RESOURCE_TYPE_HANDLER

#include "dtCore/resourcehelper.h"

namespace dtCore {

   /**
    * @class DirectoryResourceTypeHandler
    * @brief Resource type handler for importing, removing, and describing directory based resources.
    */
   class DT_CORE_EXPORT DirectoryResourceTypeHandler : public ResourceTypeHandler
   {
      public:
         DirectoryResourceTypeHandler(const DataType& dataType,
                                    const std::string& masterFile,
                                    const std::string& masterExtension,
                                    const std::string& resourceDirectoryExtension,
                                    const std::string& resourceDescription,
                                    std::vector<std::string> alternateMasterFiles);
      protected:
         virtual ~DirectoryResourceTypeHandler();
      public:
         /**
          * @param path The full path to the file or directory to check.
          * @param type The filetype, whether a directory or file.
          * @return true if the file name refers to an rbody file.
          */
         virtual bool HandlesFile(const std::string& path, dtUtil::FileType type) const;

         /**
          * Creates a resource descriptor based on the path to the resource.
          * @param category the category of the resource.
          * @param fileName the name of the file.
          * @return The resource descriptor base on the path to the resource.
          */
         virtual ResourceDescriptor CreateResourceDescriptor(const std::string& category, const std::string& fileName) const;

         /**
          * Import a resource into a project given the file, and the path.  THe path can be
          * retrieved by calling ResourceHelper::createResourceCategory
          * @see ResourceHelper#createResourceCategory
          * @param newName the name to give the resource.  This should not include the extension.
          * @param srcPath the path to the file or directory to import
          * @param destCategoryPath the full path to the destination.  This must exist already.
          * @throws ExceptionEnum::ProjectFileNotFound if source of destination files do not exist.
          * @throws ExceptionEnum::ProjectIOException if an error occurs copying the data.
          * @return the display fileName associated with this resource
          */
         virtual const std::string ImportResourceToPath(const std::string& newName,
                                                        const std::string& srcPath, const std::string& destCategoryPath) const;

         /**
          * removes a resource.  The current directory will be the root of the project when this
          * is called.
          * @param resourcePath path to the file the file getResourcePath returns
          */
         virtual void RemoveResource(const std::string& resourcePath) const;

         /**
          * @return true.  A directory will be imported.
          */
         virtual bool ImportsDirectory() const { return true; }

         /**
          * @return true.  Rbody resources are imported into a directory to hold all of the files.
          */
         virtual bool ResourceIsDirectory() const { return true; }

         /**
          * @return the extension that the resource directory will have when imported
          */
         virtual const std::string& GetResourceDirectoryExtension() const { return mResourceDirectoryExtension; };

         /**
          * @return a map of file extensions and their associated descriptions for rbody.
          */
         virtual const std::map<std::string, std::string>& GetFileFilters() const { return mFilters; }

         /**
          * @return A short description of the type of resources this imports.
          *         This is set in the constructor.
          */
         virtual const std::string& GetTypeHandlerDescription() const
         {
            return mDescription;
         }

         /**
          * @return The DataType for this instance.
          */
         virtual const DataType& GetResourceType() const { return *mDataType; };

      private:
         const DataType* mDataType;
         std::map<std::string, std::string> mFilters;
         const std::string mDescription;
         const std::string mMasterFile;
         const std::string mExtension;
         const std::string mResourceDirectoryExtension;
   };

}


#endif
