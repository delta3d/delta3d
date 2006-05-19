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

#ifndef DELTA_FILEUTILS
#define DELTA_FILEUTILS

#include <dtCore/refptr.h>
#include <osg/Referenced>
#include <osgDB/FileUtils>
#include "dtUtil/log.h"
#include "dtUtil/enumeration.h"
#include "dtUtil/exception.h"
#include "dtUtil/export.h"

namespace dtUtil
{
   typedef osgDB::DirectoryContents DirectoryContents;

   enum FileType
   {
      FILE_NOT_FOUND,
      REGULAR_FILE,
      DIRECTORY,
   };

   /**
    * @name FileInfo
    * @brief struct holding information about a file.
    */
   struct FileInfo
   {
      std::string fileName; //< the complete file name
      std::string path; //< the path to the file
      std::string baseName; //< the file name without the extension
      size_t size; //< the size of the file in bytes.
      time_t lastModified; //< when the file was last modified.
      FileType fileType;

      FileInfo(): size(0), lastModified(0), fileType(FILE_NOT_FOUND) {}
   };

   /**
    * @brief An enumeration of exception types that will be thrown by fileutils.
    */
   class DT_UTIL_EXPORT FileExceptionEnum : public dtUtil::Enumeration
   {
         DECLARE_ENUM(FileExceptionEnum);
      public:
         static FileExceptionEnum IOException;
         static FileExceptionEnum FileNotFound;

      protected:
         FileExceptionEnum(const std::string &name) : Enumeration(name)
         {
            AddInstance(this);
         }
   };

   /**
    * @name FileUtils
    * @brief Singleton class implementing basic file operations.
    */
   class DT_UTIL_EXPORT FileUtils : public osg::Referenced
   {
      public:

         static const char PATH_SEPARATOR; /// Character separating the parts of a file path.

         /**
          * @return the single instance of this class..
          */
         static FileUtils& GetInstance()
         {
            if (mInstance == NULL)
            {
               mInstance = new FileUtils();
            }
            return *mInstance;
         }

         /**
          * @param strFile the path to the file to check.
          * @return true if the file exists.
          */
         bool FileExists( const std::string& strFile ) const throw();

         /**
          * Copys a file.
          * @param strSrc The path to the source file.
          * @param strDest The path to the destination file or directory.
          * @param bOverwrite true if this call should overwrite the destination file if it exists.
          * @throws FileExceptionEnum::FileNotFound if the source file is not found.
          * @throws FileExceptionEnum::IOException if an error occurs copying the data or bOverwrite was false and the
          *                                           destination file exists.
          */
         void FileCopy( const std::string& strSrc, const std::string& strDest, bool bOverwrite ) const throw(dtUtil::Exception);

         /**
          * Moves a file. This call will attempt to move the file without moving the data, but if it can't,
          * the file will first be copied then the source file will be removed.
          * @param strSrc  The path to the source file
          * @param strDest The path to the destintion file or directory
          * @param bOverwrite true if this call should overwrite the destination file if it exists.
          * @throws FileExceptionEnum::FileNotFound if the source file is not found.
          * @throws FileExceptionEnum::IOException if an error occurs moving the data or bOverwrite was false and the
          *                                           destination file exists.
          */
         void FileMove( const std::string& strSrc, const std::string& strDest, bool bOverwrite ) const throw(dtUtil::Exception);

         /**
          * Deletes the given file
          * @param strFile the path to the file to delete
          * @throws FileExceptionEnum::IOException if an error occurs deleting the data
          */
         void FileDelete( const std::string& strFile ) const throw(dtUtil::Exception);

         /**
          * @note If the  file is not found, the fileType value will be set to FILE_NOT_FOUND and all other values
          *       will be undefined.
          * @return the fileInfo struct for the given file.
          * @see dtUtil::FileInfo
          */
         const struct FileInfo GetFileInfo( const std::string& strFile) const throw();

         /**
          * Changes the current directory to the one given in "path."
          * This will clear the stack of directories that is set by pushDirectory and popDirectory.
          * If this call fails, the stack will not be cleared.
          * @see pushDirectory
          * @see popDirectory
          * @param path The path to the new directory.
          * @throws FileExceptionEnum::FileNotFound if the path does not exist.
          */
         void ChangeDirectory(const std::string& path) throw(dtUtil::Exception);

         /**
          * @return the full path to the current directory.
          */
         const std::string& CurrentDirectory() const throw();

         /**
          * Changes the current directory to the one given in "path" and
          * adds the previous current directory to an internal stack so it can be returned
          * to via popDirectory. If this call fails, the stack will not be changed.
          * @see popDirectory
          * @param path The path to the new directory.
          * @throws FileExceptionEnum::FileNotFound if the path does not exist.
          */
         void PushDirectory(const std::string& path) throw(dtUtil::Exception);

         /**
          * sets the current directory to the last directory on the stack.
          * @see pushDirectory
          * @throws FileExceptionEnum::FileNotFound if the previous directory no longer exists.
          */
         void PopDirectory() throw(dtUtil::Exception);

         /**
          * Converts a relative path to an absolute path.
          * @param relativePath the relative path to convert to absolute.
          * @return the absolute path.
          * @throws FileExceptionEnum::FileNotFound if the path does not exist.
          */
         const std::string GetAbsolutePath(const std::string& relativePath) const throw(dtUtil::Exception);

         /**
          * @param path the path to the directory to list the contents of.
          * @return a vector of file names.
          * @throws FileExceptionEnum::FileNotFound if the path does not exist.
          */
         DirectoryContents DirGetFiles( const std::string& path ) const throw(dtUtil::Exception);

         /**
          * @param path the path to the directory to get the subdirectories for.
          * @return a vector holding the list of subdirectories.
          * @throws FileExceptionEnum::FileNotFound if the path does not exist.
          */
         DirectoryContents DirGetSubs( const std::string& path ) const throw(dtUtil::Exception);

         /**
          * Copys an entire directory.  If destPath exists, then a subdirectory will be created in
          * destPath with the same name as srcPath unless copyContentsOnly is true, in which case the contents
          * of srcPath will be copied into destPath.  If destPath does not exist, destPath will be created if
          * the parent exists and the contents of srcPath will be copied to destPath whether copyContentsOnly
          * is true or false.
          *
          * @param srcPath the source directory to copy.
          * @param destPath the destination directory.
          * @param bOverwrite true if this call should overwrite the destination file if it exists.
          * @param copyContentsOnly true if the contents of srcPath should be copied into destPath
          *                         rather than create a subdirectory
          * @throws FileExceptionEnum::FileNotFound if the source file is not found.
          * @throws FileExceptionEnum::IOException if an error occurs copying the data or bOverwrite was false and a
          *                                           destination file exists.
          */
         void DirCopy(const std::string& srcPath, const std::string& destPath, bool bOverwrite, bool copyContentsOnly = false) const throw(dtUtil::Exception);

         /**
          * Deletes a directory.  If bRecursive is true, the directory and all it's contents will
          * be removed.  If it's false, the call will fail unless the directory is empty.
          * @param strDir The path of the directory to delete.
          * @param bRecursive true if the directory should be deleted recursively.
          * @return true if successful or false if the directory is NOT empty and bRecursive is false.
          * @throws FileExceptionEnum::FileNotFound if the path does not exist.
          * @throws FileExceptionEnum::IOException if an error occurs deleteting the directory
          */
         bool DirDelete( const std::string& strDir, bool bRecursive ) throw(dtUtil::Exception);

         /**
          * creates a new directory from a path.
          * @param strDir the directory to create.
          * @throws FileExceptionEnum::FileNotFound if the parent path does not exist.
          * @throws FileExceptionEnum::IOException if an error occurs creating the directory
          */
         void MakeDirectory(const std::string& strDir) const throw(dtUtil::Exception);

         /**
          * @param strDir The directory to check.
          * @return true if the path exists and is a directory
          */
         bool DirExists( const std::string& strDir ) const throw();
           
      private:
         FileUtils();

         virtual ~FileUtils();

         void RecursDeleteDir( bool bRecursive ) throw(dtUtil::Exception);

         static dtCore::RefPtr<FileUtils> mInstance;

         dtUtil::Log* mLogger;

         std::string mCurrentDirectory;
         std::vector<std::string> mStackOfDirectories;
         static const int PATH_BUFFER_SIZE = 512;
         void ChangeDirectoryInternal(const std::string& path) throw(dtUtil::Exception);
         void InternalDirCopy(const std::string& srcPath,
                              const std::string& destPath, bool bOverwrite) const throw(dtUtil::Exception);
   };

}


#endif
