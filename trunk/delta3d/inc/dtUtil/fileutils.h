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

#ifndef DELTA_FILEUTILS
#define DELTA_FILEUTILS

#include <string>
#include <vector>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <osg/Referenced>
#include <dtUtil/enumeration.h>
#include <dtUtil/exception.h>
#include <dtUtil/export.h>

#include <osg/Version>
#include <osgDB/FileUtils>
#include <osgDB/ReaderWriter>

/// @cond DOXYGEN_SHOULD_SKIP_THIS

#if OPENSCENEGRAPH_MAJOR_VERSION < 3
namespace osgDB
{
   class ArchiveExtended;
}
#else
namespace osgDB
{
   typedef Archive ArchiveExtended;
}
#endif
/// @endcond

namespace dtUtil
{
   class Log;

   typedef std::vector<std::string> DirectoryContents;
   typedef std::vector<std::string> FileExtensionList;

   enum FileType
   {
      FILE_NOT_FOUND,
      REGULAR_FILE,
      DIRECTORY,
      ARCHIVE
   };

   /**
    * @name FileInfo
    * @brief struct holding information about a file.
    */
   struct FileInfo
   {
      std::string fileName; ///< the file name with full path
      std::string path; ///< the path to the file
      std::string baseName; ///< the file name with extension
      std::string extensionlessFileName; ///< the file name with extension
      std::string extension; ///< the file extension
      size_t size; ///< the size of the file in bytes.
      time_t lastModified; ///< when the file was last modified.
      FileType fileType; ///< The enum value specifying the type of file
      bool isInArchive; ///< true if the specified file lives within an archive

      FileInfo(): size(0), lastModified(0), fileType(FILE_NOT_FOUND), isInArchive(false) {}
   };

   class DT_UTIL_EXPORT FileUtilIOException : public dtUtil::Exception
   {
   public:
     FileUtilIOException(const std::string& message, const std::string& filename, unsigned int linenum);
     virtual ~FileUtilIOException() {};
   };

   class DT_UTIL_EXPORT FileNotFoundException : public dtUtil::Exception
   {
   public:
     FileNotFoundException(const std::string& message, const std::string& filename, unsigned int linenum);
     virtual ~FileNotFoundException() {};
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
       * @param caseInsensitive false for native, true for forced case insensitivity on platforms that normally are sensitive
       * @return true if the file exists.
       */
      bool FileExists(const std::string& strFile, bool caseInsensitive = false) const;

      /**
       * Copys a file.
       * @param strSrc The path to the source file.
       * @param strDest The path to the destination file or directory.
       * @param bOverwrite true if this call should overwrite the destination file if it exists.
       * @throws dtUtil::FileNotFoundException if the source file is not found.
       * @throws dtUtil::IOException if an error occurs copying the data or bOverwrite was false and the
       *                                           destination file exists.
       */
      void FileCopy(const std::string& strSrc, const std::string& strDest, bool bOverwrite) const;

      /**
       * Moves a file. This call will attempt to move the file without moving the data, but if it can't,
       * the file will first be copied then the source file will be removed.
       * @param strSrc  The path to the source file
       * @param strDest The path to the destintion file or directory
       * @param bOverwrite true if this call should overwrite the destination file if it exists.
       * @throws dtUtil::FileNotFoundException if the source file is not found.
       * @throws dtUtil::IOException if an error occurs moving the data or bOverwrite was false and the
       *                                           destination file exists.
       */
      void FileMove(const std::string& strSrc, const std::string& strDest, bool bOverwrite) const;

      /**
       * Deletes the given file
       * @param strFile the path to the file to delete
       * @throws dtUtil::IOException if an error occurs deleting the data
       */
      void FileDelete(const std::string& strFile) const;

      /**
       * @param caseInsensitive false for native behavior (default) or true to make it case insensitive on case sensitive
       *        systems.
       * @note If the  file is not found, the fileType value will be set to FILE_NOT_FOUND and all other values
       *       will be undefined.
       * @return the fileInfo struct for the given file.
       * @see dtUtil::FileInfo
       */
      const struct FileInfo GetFileInfo(const std::string& strFile, bool caseInsensitive = false) const;

      /**
       * Ensure that the passed in string is fit for use as a file or dir string.
       * In our case we want all separators to be the '/' character and we do not want
       * a separator at the end.
       * @param strFileOrDir The string to cleanup.
       * @note The original string passed in is changed.
       */
      void CleanupFileString(std::string &strFileOrDir) const;

      /**
       * Query whether a given string is an absolute path or not.
       * @param strFileOrDir The path to check.
       * @return True if absolute, False if relative.
       * @note This maybe a subjective determination, may need to add to this later.
       */
      bool IsAbsolutePath(std::string strFileOrDir) const;

      /**
       * A more powerful version of the standard mkdir.  This function will check to
       * see if the directory exists first and only create if needed.  Also, it will
       * recursively create all subdirectories needed to create the final directory in
       * the passed in string.
       * @param strDir The directory to create.
       * @throws all the exceptions that MakeDirectory throws
       * @see ::MakeDirectory
       */
      void MakeDirectoryEX(std::string strDir);

      /**
       * Changes the current directory to the one given in "path."
       * This will clear the stack of directories that is set by pushDirectory and popDirectory.
       * If this call fails, the stack will not be cleared.
       * @see pushDirectory
       * @see popDirectory
       * @param path The path to the new directory.
       * @throws dtUtil::FileNotFoundException if the path does not exist.
       */
      void ChangeDirectory(const std::string& path);

      /**
       * @return the full path to the current directory.
       */
      const std::string& CurrentDirectory() const;

      /**
       * Changes the current directory to the one given in "path" and
       * adds the previous current directory to an internal stack so it can be returned
       * to via popDirectory. If this call fails, the stack will not be changed.
       * @see popDirectory
       * @param path The path to the new directory.
       * @throws dtUtil::FileNameFoundException if the path does not exist.
       */
      void PushDirectory(const std::string& path);

      /**
       * sets the current directory to the last directory on the stack.
       * @see pushDirectory
       * @throws dtUtil::FileNotFoundException if the previous directory no longer exists.
       */
      void PopDirectory();

      /**
       * Converts a relative path to an absolute path.
       * @param relativePath the relative path to convert to absolute.
       * @param removeFinalFile If this refers to a file, then remove the final file in the result.
       * @return the absolute path.
       * @throws dtUtil::FileNotFoundException if the path does not exist.
       */
      std::string GetAbsolutePath(const std::string& relativePath, bool removeFinalFile = false) const;

      /**
       * Note: throws exceptions of type dtUtil::Exception
       * @param path the path to the directory to list the contents of.
       * @param extensions Optional list of file extensions to filter on, including the "dot". (e.g., ".txt", ".xml")
       * @return a vector of file names.
       * @throws dtUtil::FileNotFoundException if the path does not exist.
       * @throws dtUtil::IOException if the path is not an actual directory
       */
      DirectoryContents DirGetFiles(const std::string& path,
                                    const FileExtensionList& extensions = FileExtensionList()) const;

      /**
       * @param path the path to the directory to get the subdirectories for.
       * @return a vector holding the list of subdirectories.
       * @throws dtUtil::FileNotFoundException if the path does not exist.
       */
      DirectoryContents DirGetSubs(const std::string& path) const;

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
       * @throws dtUtil::FileNotFoundException if the source file is not found.
       * @throws dtUtil::IOException if an error occurs copying the data or bOverwrite was false and a
       *                                           destination file exists.
       */
      void DirCopy(const std::string& srcPath, const std::string& destPath, bool bOverwrite, bool copyContentsOnly = false) const;

      /**
       * Deletes a directory.  If bRecursive is true, the directory and all it's contents will
       * be removed.  If it's false, the call will fail unless the directory is empty.
       * @param strDir The path of the directory to delete.
       * @param bRecursive true if the directory should be deleted recursively.
       * @return true if successful or false if the directory is NOT empty and bRecursive is false.
       * @throws dtUtil::FileNotFoundException if the path does not exist.
       * @throws dtUtil::IOException if an error occurs deleteting the directory
       */
      bool DirDelete(const std::string& strDir, bool bRecursive);

      /**
       * creates a new directory from a path.
       * @param strDir the directory to create.
       * @throws dtUtil::FileNotFoundException if the parent path does not exist.
       * @throws dtUtil::IOException if an error occurs creating the directory
       */
      void MakeDirectory(const std::string& strDir) const;

      /**
       * @param strDir The directory to check.
       * @param caseInsensitive false for native, true for forced case insensitivity on platforms that normally are sensitive
       * @return true if the path exists and is a directory
       */
      bool DirExists(const std::string& strDir, bool caseInsensitive = false) const;

      /**
       * Helper function that returns the relative path between absolutePath and file
       * @param absolutePath The absolute path to search
       * @param file The absolute path to the file
       * @return The relative path or empty string for failure
       * @note This function assumes that directory separators are equal for both paths
       */
      std::string RelativePath(const std::string& absolutePath, const std::string& file) const;

      /**
       *  Converts an absolute path to a relative path based on the current working directory.
       *  @param absPath the absolute path to process
       *  @param relPath output parameter that will contain the relative path.
       */
      //void AbsoluteToRelative(const std::string &absPath, std::string& relPath);

      /**
       *  It is possible for two different path strings to point at the same
       *  file on disk.
       *  (Things like relative paths and filesystem links make this possible).
       *
       *  This function makes absolutely certain that the two files aren't the same
       *  by checking the inodes of the two files -- preventing things like having
       *  the FileUtils::FileCopy accidentally blow away a file by copying it
       *  onto itself.
       *
       *  @param file1 -- Path to first file.
       *  @param file2 -- Path to second file.
       *  @return True if inodes match, false otherwise (if one or both files are inaccessible, returns false).
       */
      bool IsSameFile(const std::string& file1, const std::string& file2) const;

      /// Concatenates two paths adding a path separator in between if necessary.
      static std::string ConcatPaths(const std::string& left, const std::string& right);


      osg::Object* ReadObject(const std::string& filename, osgDB::ReaderWriter::Options* options = NULL);
      osg::Node* ReadNode(const std::string& filename, osgDB::ReaderWriter::Options* options = NULL);
      
      /**
       * Used to search for archives by filename.
       */
      osgDB::ArchiveExtended* FindArchive(const std::string& archiveFileName) const;
      
   private:

      static const int PATH_BUFFER_SIZE = 512;

      FileUtils();
      virtual ~FileUtils();

      void ChangeDirectoryInternal(const std::string& path);
      void InternalDirCopy(const std::string& srcPath,
         const std::string& destPath, bool bOverwrite) const;

      void RecursDeleteDir(bool bRecursive);

      /**
       * Splits a filename for a file within an archive into two parts, the archive filename and the actual
       *    filename relative to the archive root.
       */
      bool SplitArchiveFilename(const std::string& fullFilename, std::string& archiveFilename, std::string& fileInArchive) const;

      /**
       * Internal function used to find files within an archive subdirectory.
       */
      void DirGetFilesInArchive(const osgDB::ArchiveExtended& a, const std::string& path, DirectoryContents& result) const;

      FileType GetFileTypeForFileInArchive(const osgDB::ArchiveExtended& a, const std::string& path) const;
      FileInfo GetFileInfoForFileInArchive(const osgDB::ArchiveExtended& a, const std::string& path) const;

      FileType GetFileTypeFromOSGDBFileType(osgDB::FileType ft) const;
      
      bool ContainsArchiveExtension(const std::string& path) const;  

      std::string ArchiveRelativeToAbsolute(const std::string& relativeFile) const;

      const struct FileInfo GetFileInfo_Internal(const std::string& strFile, bool caseInsensitive) const;
      bool IsSameFile_Internal(const std::string& file1, const std::string& file2) const;


      static dtCore::RefPtr<FileUtils> mInstance;

      dtUtil::Log* mLogger;

      std::string mCurrentDirectory;
      std::vector<std::string> mStackOfDirectories;

   };

   /// Simple class to change directory where it automatically pop back out on destruction.  To be used on the stack.
   class DT_UTIL_EXPORT DirectoryPush
   {
   public:
      DirectoryPush(const std::string& dir);
      ~DirectoryPush();

      bool GetSucceeded();

      const std::string& GetError();
   private:
      bool mSucceeded;
      std::string mError;
   };

   /**
    * For tokenizing paths.
    */
   class IsPathSeparator : public std::unary_function<char, bool>
   {
       public:
           bool operator()(char c) const { return c == '/' || c == '\\' || c == FileUtils::PATH_SEPARATOR; }
   };


} // namespace dtUtil

#endif // DELTA_FILEUTILS

