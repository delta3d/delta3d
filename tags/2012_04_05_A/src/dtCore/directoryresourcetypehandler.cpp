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
#include <osgDB/FileNameUtils>
#include <dtUtil/fileutils.h>
#include <dtUtil/stringutils.h>

#include <dtCore/datatype.h>
#include <dtCore/directoryresourcetypehandler.h>
#include <dtCore/exceptionenum.h>

namespace dtCore
{

   DirectoryResourceTypeHandler::DirectoryResourceTypeHandler(const DataType& dataType,
                                                              const std::string& masterFile,
                                                              const std::string& masterExtension,
                                                              const std::string& resourceDirectoryExtension,
                                                              const std::string& resourceDescription,
                                                              std::vector<std::string> alternateMasterFiles)
      : mDataType(&dataType), mDescription(resourceDescription)
      , mMasterFile(masterFile), mExtension(masterExtension)
      , mResourceDirectoryExtension(resourceDirectoryExtension)
   {
      mFilters.insert(make_pair(masterFile, resourceDescription));

      for (std::vector<std::string>::iterator i = alternateMasterFiles.begin(); i != alternateMasterFiles.end(); ++i)
      {
         mFilters.insert(make_pair(*i, resourceDescription));
      }

      mFilters.insert(make_pair(masterExtension, resourceDescription));
   }

   DirectoryResourceTypeHandler::~DirectoryResourceTypeHandler() {}

   bool DirectoryResourceTypeHandler::HandlesFile(const std::string& path, dtUtil::FileType type) const
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      if (path.empty())
      {
         return false;
      }

      if (type == dtUtil::REGULAR_FILE)
      {
         //check for the file in a way that will handle both case sensitive and insensitive filesystems.
         if (dtUtil::StrCompare(osgDB::getSimpleFileName(path), mMasterFile, false) == 0)
         {
            return fileUtils.FileExists(path);
         }
         else if (osgDB::getLowerCaseFileExtension(path) == mExtension)
         {
            return true;
         }
      }
      else if (type == dtUtil::DIRECTORY)
      {
         // quick short circuit for speed in the resource directory.
         if (dtUtil::StrCompare(osgDB::getFileExtension(path), mResourceDirectoryExtension, false) == 0)
         {
            return true;
         }

         // look for any of the possible master files.
         for (std::map<std::string, std::string>::const_iterator i = mFilters.begin(); i != mFilters.end(); ++i)
         {
            // skip the extension
            if (i->first == mResourceDirectoryExtension)
               continue;

            if (fileUtils.FileExists(path + dtUtil::FileUtils::PATH_SEPARATOR + i->first))
               return true;

         }
      }

      return false;
   }

   ResourceDescriptor DirectoryResourceTypeHandler::CreateResourceDescriptor(const std::string& category,
                                                                             const std::string& fileName) const
   {
      std::string displayString = GetResourceType().GetName();
      std::string resultString;
      if (!category.empty())
      {
         displayString += ResourceDescriptor::DESCRIPTOR_SEPARATOR +
            category + ResourceDescriptor::DESCRIPTOR_SEPARATOR + fileName;
      }
      else
      {
         displayString += ResourceDescriptor::DESCRIPTOR_SEPARATOR + fileName;
      }

      resultString = displayString;

      if (dtUtil::StrCompare(osgDB::getSimpleFileName(fileName), mMasterFile, false) != 0 &&
               dtUtil::StrCompare(osgDB::getFileExtension(fileName), mResourceDirectoryExtension, false) == 0)
      {
         resultString += ResourceDescriptor::DESCRIPTOR_SEPARATOR + mMasterFile;
      }

      return ResourceDescriptor(displayString,resultString);
   }

   const std::string DirectoryResourceTypeHandler::ImportResourceToPath(const std::string& newName,
                                                                        const std::string& srcPath, const std::string& destCategoryPath) const
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      dtUtil::FileType ftype = fileUtils.GetFileInfo(srcPath).fileType;

      std::string mainSrcFileName;
      std::string dirToCopy;

      if (ftype == dtUtil::REGULAR_FILE)
      {
         mainSrcFileName = osgDB::getSimpleFileName(srcPath);
         dirToCopy = osgDB::getFilePath(srcPath);
      }
      else if (ftype == dtUtil::DIRECTORY)
      {
         //look for any of the possible master files.
         for (std::map<std::string, std::string>::const_iterator i = mFilters.begin(); i != mFilters.end(); ++i)
         {
            //skip the extension
            if (i->first == mResourceDirectoryExtension)
            {
               continue;
            }

            if (fileUtils.FileExists(srcPath + dtUtil::FileUtils::PATH_SEPARATOR + i->first))
            {
               mainSrcFileName = i->first;
               break;
            }

         }
         if (mainSrcFileName.empty())
            throw dtCore::ProjectFileNotFoundException(
                   std::string("No supported master file found in directory:\"") +
                   srcPath + "\".", __FILE__, __LINE__);
         dirToCopy = srcPath;
      }

      std::string resourceFileName = newName + '.' + mResourceDirectoryExtension;

      const std::string& destDir = destCategoryPath + dtUtil::FileUtils::PATH_SEPARATOR + resourceFileName;

      fileUtils.MakeDirectory(destDir);

      //don't overwrite so that we can see if any errors occurred.
      fileUtils.DirCopy(dirToCopy, destDir, false, true);

      //don't overwrite so that we can see if any errors occurred.
      if (mainSrcFileName != mMasterFile &&
          //This will catch the case where the names differ only by case, but the
          //filesystem is not case sensitive.
          !fileUtils.FileExists(destDir + dtUtil::FileUtils::PATH_SEPARATOR + mMasterFile))
         fileUtils.FileMove(destDir + dtUtil::FileUtils::PATH_SEPARATOR + mainSrcFileName, destDir + dtUtil::FileUtils::PATH_SEPARATOR + mMasterFile, false);

      return resourceFileName;
   }

   void DirectoryResourceTypeHandler::RemoveResource(const std::string& resourcePath) const
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      if (fileUtils.FileExists(resourcePath))
      {
         const std::string& path = osgDB::getFilePath(resourcePath);
         if (HandlesFile(path, dtUtil::DIRECTORY))
            fileUtils.DirDelete(path, true);
      }
   }


}
