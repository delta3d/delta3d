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

#include <prefix/dtutilprefix.h>
#include <dtUtil/mswinmacros.h>
#include <dtUtil/macros.h> //for DTUNREFERENCED_PARAMETER
#include <osg/Version>
#include <algorithm>

#ifdef DELTA_WIN32
#   include <dtUtil/mswin.h>
#   include <direct.h>
#   include <io.h>
#   include <errno.h>
#   define stat64 _stati64
#   define mkdir(x,y) _mkdir((x))

#   if defined(_MT) || defined(_DLL)
_CRTIMP extern int * __cdecl _errno(void);
#      define errno   (*_errno())
#   else   // ndef _MT && ndef _DLL
_CRTIMP extern int errno;
#   endif  // _MT || _DLL

//Linux and Mac OS X
#else
#   include <sys/param.h>
#   include <errno.h>
#   include <unistd.h>
#endif


#include <osg/Notify>
#include <stack>

#include <dtUtil/fileutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/exception.h>
#include <dtUtil/log.h>
#include <osgDB/ReadFile>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

#include <osgDB/Archive>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

//we only want to NOT use stat64, if it's not defined.
#ifndef stat64
#define stat64 stat
#endif


#ifndef S_ISREG
#define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif

#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif

#ifdef MAX_PATH
   #undef MAX_PATH
#endif

#define MAX_PATH 1024

#include <stack>
#include <queue>


namespace dtUtil
{
   dtCore::RefPtr<FileUtils> FileUtils::mInstance;

#ifdef DELTA_WIN32
   const char FileUtils::PATH_SEPARATOR = '\\';
#else
   const char FileUtils::PATH_SEPARATOR = '/';
#endif

   // copy of function from osg to make osg 2.8 still work
   static std::string GetPathRoot(const std::string& path)
   {
       // Test for unix root
       if (path.empty()) return "";
       if (path[0] == '/') return "/";
       // Now test for Windows root
       if (path.length()<2) return "";
       if (path[1] == ':') return path.substr(0, 2);        // We should check that path[0] is a letter, but as ':' is invalid in paths in other cases, that's not
       return "";
   }

   // temporary copy of osgDB::makeDirectory because of some bugs in it.
   static bool iMakeDirectory(const std::string& path)
   {
      if (path.empty())
      {
         osg::notify(osg::DEBUG_INFO) << "osgDB::makeDirectory():  cannot create an empty directory" << std::endl;
         return false;
      }

      struct stat64 stbuf;
      if (stat64(path.c_str(), &stbuf) == 0)
      {
         if (S_ISDIR(stbuf.st_mode))
         {
            return true;
         }
         else
         {
            osg::notify(osg::DEBUG_INFO) << "osgDB::makeDirectory():  "  <<
               path << " already exists and is not a  directory!" << std::endl;
            return false;
         }
      }

      std::string dir = path;
      std::stack<std::string> paths;
      while (true)
      {
         if (dir.empty())
         {
            break;
         }

         if (stat64(dir.c_str(), &stbuf) < 0)
         {
            switch (errno)
            {
            case ENOENT:
            case ENOTDIR:
               paths.push(dir);
               break;

            default:
               osg::notify(osg::DEBUG_INFO) <<  "osgDB::makeDirectory(): "  << strerror(errno) << std::endl;
               return false;
            }
         }
         else
         {
            break;
         }
         dir = osgDB::getFilePath(std::string(dir));
      }

      while (!paths.empty())
      {
         std::string dir2 = paths.top();

         if (mkdir(dir2.c_str(), 0755) < 0)
         {
            osg::notify(osg::DEBUG_INFO) << "osgDB::makeDirectory():  " << strerror(errno) << std::endl;
            return false;
         }
         paths.pop();
      }
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool FileUtils::FileExists(const std::string& strFile, bool caseInsensitive) const
   {
      return GetFileInfo(strFile, caseInsensitive).fileType != FILE_NOT_FOUND;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::FileCopy(const std::string& strSrc, const std::string& strDest, bool bOverwrite) const
   {
      FILE* pSrcFile;
      FILE* pDestFile;

      struct stat tagStat;

      // Make absolutely certain these two strings don't point to the same file.
      if (!IsSameFile(strSrc, strDest))
      {
         if (!FileExists(strSrc))
         {
            throw dtUtil::FileNotFoundException(
                   std::string("Source file does not exist: \"") + strSrc + "\"", __FILE__, __LINE__);
         }

         // Open the source file for reading.
         pSrcFile = fopen(strSrc.c_str(), "rb");
         if (pSrcFile == NULL)
         {
            throw dtUtil::FileUtilIOException(
                   std::string("Unable to open source file for reading: \"") + strSrc + "\"", __FILE__, __LINE__);
         }

         try
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Source file exists.");
            }

            std::string destFile = strDest;

            FileType ft = GetFileInfo(strDest).fileType;

            // Check to see if the destination is a file or directory.
            if (ft == DIRECTORY)
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination is a directory.");
               }

               // If the file is a directory, append the base name of the source file to the destination
               // to make the new file name.
               if (strDest[strDest.size()-1] != FileUtils::PATH_SEPARATOR)
               {
                  destFile = strDest + FileUtils::PATH_SEPARATOR + osgDB::getSimpleFileName(strSrc);
               }
               else
               {
                  destFile = strDest + osgDB::getSimpleFileName(strSrc);
               }
            }
            else
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination is a file.");
               }
            }

            if (FileExists(destFile) && !bOverwrite)
            {
               throw dtUtil::FileUtilIOException(
                      std::string("Destination file exists, but overwriting is turned off: \"") + destFile + "\"", __FILE__, __LINE__);
            }

            pDestFile = fopen(destFile.c_str(), "wb");

            if (pDestFile == NULL)
            {
               // make sure to close the source file.
               throw dtUtil::FileUtilIOException(
                      std::string("Unable to open destination for writing: \"") + destFile + "\"", __FILE__, __LINE__);
            }

            try
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, "Destination opened for reading.");
               }

               stat(strSrc.c_str(), &tagStat);
               long i = 0;
               char buffer[4096];
               while (i < tagStat.st_size)
               {
                  size_t readCount = fread(buffer, 1, 4096, pSrcFile);
                  if (readCount > 0)
                  {
                     size_t numWritten = fwrite(buffer, 1, readCount, pDestFile);
                     if (numWritten < readCount)
                     {
                        throw dtUtil::FileUtilIOException(
                               std::string("Unable to write to destinate file: \"") + destFile + "\"", __FILE__, __LINE__);
                     }
                     i += readCount;
                  }
               }
               fclose(pDestFile);
               fclose(pSrcFile);
            }
            catch (dtUtil::Exception&)
            {
               fclose(pDestFile);
               throw;
            }
         }
         catch (dtUtil::Exception&)
         {
            fclose(pSrcFile);
            throw;
         }

      }

      // if the source equals the destination, this method is really a noop.
      // (Not to mention the fact that if you attempt to copy a file onto itself
      // in this manner then you will end up blowing it away).
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::FileMove(const std::string& strSrc, const std::string& strDest, bool bOverwrite) const
   {
      if (GetFileInfo(strSrc).fileType != REGULAR_FILE)
      {
         throw dtUtil::FileNotFoundException(
                std::string("Source file was not found or is a Directory: \"") + strSrc + "\"", __FILE__, __LINE__);
      }

      FileType ft = GetFileInfo(strDest).fileType;

      std::string destFile = strDest;

      // Check to see if the destination is a directory.
      if (ft == DIRECTORY)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination is a directory.");
         }

         // Check to see if the destination is a file or directory.
         if (strDest[strDest.size()-1] != FileUtils::PATH_SEPARATOR)
         {
            destFile = strDest + FileUtils::PATH_SEPARATOR + osgDB::getSimpleFileName(strSrc);
         }
         else
         {
            destFile = strDest + osgDB::getSimpleFileName(strSrc);
         }

         ft = GetFileInfo(destFile).fileType;
      }
      else
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination is a file.");
         }
      }

      if (ft != FILE_NOT_FOUND && !bOverwrite)
      {
         throw dtUtil::FileUtilIOException(
                std::string("Destination file exists and the call was not set to overwrite: \"") + strDest + "\"", __FILE__, __LINE__);
      }

      // first check to see if the file can be moved without copying it.
      if (rename(strSrc.c_str(), destFile.c_str()) == 0)
      {
         return;
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, "Rename failed, attempting to copy file and delete the source");
      }

      // copy the file
      FileCopy(strSrc, strDest, bOverwrite);

      // attempt to delete the original file.
      if (unlink(strSrc.c_str()) != 0)
      {
         throw dtUtil::FileUtilIOException(
                std::string("Unable to delete \"") + strSrc + "\" but file copied to new location.", __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::FileDelete(const std::string& strFile) const
   {
      FileType ft = GetFileInfo(strFile).fileType;

      // If the file does not exist, then ignore.
      if (ft == FILE_NOT_FOUND)
      {
         return;
      }

      if (ft != REGULAR_FILE)
      {
         throw dtUtil::FileUtilIOException(
                std::string("File \"") + strFile + "\" is a directory.", __FILE__, __LINE__);
      }

      if (unlink(strFile.c_str()) != 0)
      {
         throw dtUtil::FileUtilIOException(
                std::string("Unable to delete \"") + strFile + "\".", __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const struct FileInfo FileUtils::GetFileInfo(const std::string& strFile, bool caseInsensitive) const
   {
      struct FileInfo info;

      if(!strFile.empty())
      {
         std::string filename = strFile;
         CleanupFileString(filename);

         //we have a CWD in an archive and we have specified a relative path
         if (ContainsArchiveExtension(filename) || (!IsAbsolutePath(filename) && ContainsArchiveExtension(mCurrentDirectory)))
         {
             if(!IsAbsolutePath(filename))
             {
                filename = ArchiveRelativeToAbsolute(filename);
             }

            info = GetFileInfo_Internal(filename, caseInsensitive);
         }
         else
         {
            info = GetFileInfo_Internal(filename, caseInsensitive);
         }
      }
      return info;
   }

      //we use cleanup string on the path so this accept becomes very simple
      class IsFileSeparator: public std::unary_function<char, bool>
      {
      public:
         IsFileSeparator() {}
         bool operator()(char c) const { return c == '/'; }

      };

   //-----------------------------------------------------------------------
   std::string FileUtils::ArchiveRelativeToAbsolute(const std::string& relativeFile) const
   {


      std::string absDir = mCurrentDirectory;
      CleanupFileString(absDir);

      std::vector<std::string> tokens;
      dtUtil::StringTokenizer<IsFileSeparator>::tokenize(tokens, relativeFile);


      std::vector<std::string>::iterator iter = tokens.begin();
      std::vector<std::string>::iterator iterEnd = tokens.end();
      for(;iter != iterEnd; ++iter)
      {
         std::string& curToken = *iter;

         if(curToken == ".")
         {
            //do nothing
         }
         else if(curToken == "..")
         {
            size_t lastSlash = absDir.find_last_of('/');
            if(lastSlash != std::string::npos)
            {
               absDir = absDir.substr(0, lastSlash);
            }

         }
         else
         {
            absDir.append("/");
            absDir.append(curToken);
         }
      }

      return absDir;
   }

   //-----------------------------------------------------------------------
   const struct FileInfo FileUtils::GetFileInfo_Internal(const std::string& strFile, bool caseInsensitive) const
   {
      struct FileInfo info;
      struct stat tagStat;

      std::string archiveName;
      std::string fileInArchive;

      bool isInArchive = SplitArchiveFilename(strFile, archiveName, fileInArchive);

      if(isInArchive)
      {
         const osgDB::ArchiveExtended* a = FindArchive(archiveName);
         if(a == NULL)
         {
            a = FindArchive(archiveName);
         }

         if(a != NULL)
         {
            info = GetFileInfoForFileInArchive(*a, fileInArchive);
         }
      }
      else
      {
         // chop trailing slashes off
         std::string choppedStr = strFile;
         if (strFile.size() > 0 && (strFile[strFile.size() - 1] == '\\' ||
                                    strFile[strFile.size() - 1] == '/'))
         {
            choppedStr = strFile.substr(0, strFile.length() - 1);
         }

         if (caseInsensitive)
         {
            choppedStr = osgDB::findFileInDirectory(choppedStr, "", osgDB::CASE_INSENSITIVE);
         }

         if (stat(choppedStr.c_str(), &tagStat) != 0)
         {
            // throw dtUtil::FileNotFoundException( std::string("Cannot open file ") + choppedStr);
            info.fileType = FILE_NOT_FOUND;
            return info;
         }

         info.extensionlessFileName = osgDB::getStrippedName(choppedStr);
         info.path = osgDB::getFilePath(choppedStr);
         info.baseName = osgDB::getSimpleFileName(choppedStr);
         info.extension = osgDB::getFileExtension(choppedStr);
         if (info.path.empty())
         {
            info.fileName = info.baseName;
         }
         else
         {
            info.fileName = info.path + PATH_SEPARATOR + info.baseName;
         }

         info.size = tagStat.st_size;
         info.lastModified = tagStat.st_mtime;

         if (S_ISDIR(tagStat.st_mode))
         {
            info.fileType = DIRECTORY;
         }
         else if(!archiveName.empty())
         {
            info.fileType = ARCHIVE;
         }
         else
         {
            // Anything else is a regular file, including special files
            // this is incomplete, but not a case that we deemed necessary to handle.
            // Symbolic links should NOT show up because stat was called, not lstat.
            info.fileType = REGULAR_FILE;
         }
      }

      return info;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::CleanupFileString(std::string& strFileOrDir) const
   {
      if (strFileOrDir.empty())
      {
         return;
      }

      // remove duplicate path separators
      for (unsigned int i = 0; i < strFileOrDir.length(); ++i)
      {
         if (strFileOrDir[i] == '/' || strFileOrDir[i] == '\\')
         {
            if(i > 0 && (strFileOrDir[i-1] == '/' || strFileOrDir[i-1] == '\\'))
            {
               strFileOrDir.erase(strFileOrDir.begin() + i);
               --i;
            }
            strFileOrDir[i] = PATH_SEPARATOR;
         }
      }

      // get rid of trailing separators
      if (strFileOrDir[strFileOrDir.length()-1] == PATH_SEPARATOR)
      {
         strFileOrDir.erase(strFileOrDir.end() - 1);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool FileUtils::IsAbsolutePath(std::string strFileOrDir) const
   {
      // just in case, make sure we are using a valid path
      CleanupFileString(strFileOrDir);

      // handle unix-style paths
      if (strFileOrDir.length() > 0 && strFileOrDir[0] == '/')
      {
         return true;
      }

      // handle windows-style paths
      else if (strFileOrDir.length() > 1 && strFileOrDir[1] == ':')
      {
         return true;
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::MakeDirectoryEX(std::string strDir)
   {
      if (strDir.empty())
      {
         return;
      }

      // first make sure the directory string is clean
      CleanupFileString(strDir);

      // this generated string is used to check each subdirectory in the path
      std::string dir;

      // prepare tokenizer
      char* buffer = new char[strDir.length()+1];
      strcpy(buffer, strDir.c_str());
      char* tok = strtok(buffer, "/");

      // setup root directory
      if (strDir[0] == '/')
      {
         dir = "/";
         dir += tok;
      }
      else
      {
         dir = tok;
         dir += "/";
      }

      // traverse each subdirectory in path and create directories as needed
      while (tok)
      {
         // debug
         //std::cout << tok << std::endl;

         // create directory if necessary
         if (!DirExists(dir))
         {
            MakeDirectory(dir);
         }

         // get next subdirectory
         tok = strtok(NULL, "/");
         if (tok)
         {
            if (dir[dir.length()-1] != '/') { dir += '/'; }
            dir += tok;
         }
      }

      // cleanup
      delete [] buffer;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::ChangeDirectory(const std::string& path)
   {
      if(!path.empty())
      {
         std::string filename = path;
         CleanupFileString(filename);

         //we have a CWD in an archive and we have specified a relative path
         if (ContainsArchiveExtension(filename) || (!IsAbsolutePath(filename) && ContainsArchiveExtension(mCurrentDirectory)))
         {
            if(!IsAbsolutePath(filename))
            {
               filename = ArchiveRelativeToAbsolute(filename);
            }

            ChangeDirectoryInternal(filename);
         }
         else
         {
            ChangeDirectoryInternal(filename);
         }

         mStackOfDirectories.clear();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& FileUtils::CurrentDirectory() const
   {
      return mCurrentDirectory;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::ChangeDirectoryInternal(const std::string& path)
   {
      std::string archiveName;
      std::string fileInArchive;

      bool inArchive = SplitArchiveFilename(path, archiveName, fileInArchive);

      if(inArchive || !archiveName.empty())
      {
         osgDB::ArchiveExtended* a = FindArchive(archiveName);
         if(a != NULL)
         {
            if(inArchive)
            {
               FileInfo info = GetFileInfoForFileInArchive(*a, fileInArchive);
               if(info.fileType == DIRECTORY)
               {
                  mCurrentDirectory = archiveName + "/" + fileInArchive;
               }
               else
               {
                  throw dtUtil::FileNotFoundException( std::string("Cannot change directory into path: ") + path, __FILE__, __LINE__);
               }
            }
            else
            {
               //it is the archive itself
               mCurrentDirectory = osgDB::getRealPath(archiveName);
            }
         }
         else
         {
            throw dtUtil::FileNotFoundException( std::string("Cannot find valid archive for path: ") + path, __FILE__, __LINE__);
         }
      }
      else
      {
         if (chdir(path.c_str()) == -1)
         {
            throw dtUtil::FileNotFoundException( std::string("Cannot open directory ") + path, __FILE__, __LINE__);
         }
         char buf[512];
         char* bufAddress = getcwd(buf, 512);
         if (buf != bufAddress)
         {
            throw dtUtil::FileUtilIOException( std::string("Cannot get current working directory"), __FILE__, __LINE__);
         }

         mCurrentDirectory = buf;
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::string message("Changed directory to \"");
         message += mCurrentDirectory;
         message += "\".";
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, message.c_str());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::PushDirectory(const std::string& path)
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::string message("Pushing Directory \"");
         message += path;
         message += "\".";
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, message.c_str());
      }
      std::string old = mCurrentDirectory;
      ChangeDirectoryInternal(path);
      mStackOfDirectories.push_back(old);
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::PopDirectory()
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Popping Directory.");
      }

      if (mStackOfDirectories.empty())
      {
         return;
      }

      ChangeDirectoryInternal(mStackOfDirectories.back());
      mStackOfDirectories.pop_back();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string FileUtils::GetAbsolutePath(const std::string& relativePath, bool removeFinalFile) const
   {
      //todo- add archive support here

      FileInfo fi = GetFileInfo(relativePath);
      if (fi.fileType == FILE_NOT_FOUND)
      {
         throw dtUtil::FileNotFoundException( std::string("Cannot open file or directory ") + relativePath, __FILE__, __LINE__);
      }
      else if (fi.fileType == ARCHIVE || fi.isInArchive)
      {
         throw dtUtil::FileUtilIOException( std::string("Unable to get the absolute path to a file in an archive. ") + relativePath, __FILE__, __LINE__);
      }

      std::string dir = relativePath;
      if (fi.fileType == REGULAR_FILE)
      {
         dir = fi.path;
      }

      if (dir.empty())
      {
         dir =".";
      }

      std::string result;
      DirectoryPush dp(dir);
      if (dp.GetSucceeded())
      {
         result = mCurrentDirectory;
         if (!removeFinalFile && fi.fileType == REGULAR_FILE)
         {
            result += PATH_SEPARATOR + fi.baseName;
         }
      }
      else
      {
         throw dtUtil::FileUtilIOException( std::string("Cannot get absolute path for file.  Cannot enter directory: ") + relativePath, __FILE__, __LINE__);
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string getFileExtensionIncludingDot(const std::string& fileName)
   {
#if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR <= 2  && OSG_VERSION_MINOR <= 6
      //copied from osgDB/FileNameUtils.cpp (v2.8.0)
      std::string::size_type dot = fileName.find_last_of('.');
      if (dot==std::string::npos) return std::string("");
      return std::string(fileName.begin()+dot,fileName.end());
#else
      return osgDB::getFileExtensionIncludingDot(fileName);
#endif
   }

   /////////////////////////////////////////////////////////////////////////////
   class ToLowerClass
   {
   public:
      ToLowerClass(){}

      char operator() (char& elem) const
      {
         return tolower(elem);
      }
   };

   /////////////////////////////////////////////////////////////////////////////
   DirectoryContents FileUtils::DirGetFiles(const std::string& path,
                                            const FileExtensionList& extensions) const
   {
      DirectoryContents dirContents;

      FileInfo ft = GetFileInfo(path);
      if (ft.fileType == FILE_NOT_FOUND)
      {
         throw dtUtil::FileNotFoundException(
                std::string("Path not Found: \"") + path + "\".", __FILE__, __LINE__);
      }
      else if (ft.fileType == REGULAR_FILE)
      {
         throw dtUtil::FileUtilIOException(
                std::string("Path does not specify a directory: \"") + path + "\".", __FILE__, __LINE__);
      }
      else if(ft.fileType == ARCHIVE || ft.isInArchive)
      {
         std::string archiveName;
         std::string fileInArchiveName;
         std::string absoluteFilename = path;
         if(!IsAbsolutePath(path))
         {
            absoluteFilename = ArchiveRelativeToAbsolute(path);
         }

         SplitArchiveFilename(absoluteFilename, archiveName, fileInArchiveName);

         const osgDB::ArchiveExtended* a = FindArchive(archiveName);
         if(a != NULL)
         {
             DirGetFilesInArchive(*a, absoluteFilename, dirContents);
         }
         else
         {
            throw dtUtil::FileUtilIOException(
               std::string("Archive must be opened to search into: \"") + path + "\".", __FILE__, __LINE__);
         }
      }
      else //should be a directory
      {
         dirContents = osgDB::getDirectoryContents(path);
      }

      if(extensions.empty())
      {
         return dirContents;
      }

      DirectoryContents filteredContents;
      // iterate over contents, looking for files that have the requested extensions
      DirectoryContents::iterator dirItr = dirContents.begin();
      while (dirItr != dirContents.end())
      {
         FileExtensionList::const_iterator extItr = extensions.begin();
         while (extItr != extensions.end())
         {
            std::string testExt = getFileExtensionIncludingDot((*dirItr));
            std::string validExt = (*extItr);

            if (dtUtil::StrCompare(testExt, validExt, false) == 0)
            {
               filteredContents.push_back((*dirItr));
               // stop when we find at least one match, to avoid duplicate file entries, in
               // case extensions contains duplicate entries.
               break;
            }
            ++extItr;
         }
         ++dirItr;
      }

      return filteredContents;
   }

   /////////////////////////////////////////////////////////////////////////////
   DirectoryContents FileUtils::DirGetSubs(const std::string& path) const
   {
      DirectoryContents vec;

      DirectoryContents dirCont = DirGetFiles(path);

      for (DirectoryContents::const_iterator i = dirCont.begin(); i != dirCont.end(); ++i)
      {
         if (GetFileInfo(path + PATH_SEPARATOR + *i).fileType == DIRECTORY && (*i != ".") && (*i != ".."))
         {
            vec.push_back(*i);
         }
      }

      return vec;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::InternalDirCopy(const std::string& srcPath,
                                   const std::string& destPath, bool bOverwrite) const
   {
      FileType destFileType = GetFileInfo(destPath).fileType;
      //std::cout << "Copying " << srcPath << " to " << destPath << std::endl;

      if (destFileType == REGULAR_FILE)
      {
         throw dtUtil::FileNotFoundException(
                std::string("The destination path must be a directory: \"") + destPath + "\"", __FILE__, __LINE__);
      }

      if (destFileType == FILE_NOT_FOUND)
      {
         MakeDirectory(destPath);
      }

      DirectoryContents contents = DirGetFiles(srcPath);
      for (DirectoryContents::iterator i = contents.begin(); i != contents.end(); ++i)
      {
         if (*i == "." || *i == "..")
         {
            continue;
         }
         const std::string newSrcPath = srcPath + PATH_SEPARATOR + *i;
         const std::string newDestPath = destPath + PATH_SEPARATOR + *i;
         FileInfo fi = GetFileInfo(newSrcPath);
         if (fi.fileType == DIRECTORY)
         {
            if (destPath.size() >= newSrcPath.size() &&
                destPath.substr(0, newSrcPath.size()) == newSrcPath &&
                (destPath.size() == newSrcPath.size() || destPath[newSrcPath.size()] == PATH_SEPARATOR))
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__, "Can't copy %s into itself.", newSrcPath.c_str());
               }
            }
            else
            {
               InternalDirCopy(newSrcPath, newDestPath, bOverwrite);
            }
         }
         else
         {
            FileCopy(newSrcPath, newDestPath, bOverwrite);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::DirCopy(const std::string& srcPath,
                           const std::string& destPath, bool bOverwrite, bool copyContentsOnly) const
   {
      if (!DirExists(srcPath))
      {
         throw dtUtil::FileNotFoundException(
                std::string("Source directory does not exist: \"") + srcPath + "\"", __FILE__, __LINE__);
      }

      FileType destFileType = GetFileInfo(destPath).fileType;

      if (destFileType == REGULAR_FILE)
      {
         throw dtUtil::FileNotFoundException(
                std::string("The destination path must be a directory: \"") + destPath + "\"", __FILE__, __LINE__);
      }

      bool createDest = destFileType == FILE_NOT_FOUND;

      std::string fullSrcPath = GetAbsolutePath(srcPath);
      // from here, the code can assume srcPath exists.
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Source directory \"%s\" exists.", fullSrcPath.c_str());
      }

      std::string fullDestPath;

      if (createDest)
      {
         MakeDirectory(destPath);
         fullDestPath = GetAbsolutePath(destPath);
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination directory \"%s\" has been created.",
                                fullDestPath.c_str());
         }
      }
      else
      {
         fullDestPath = GetAbsolutePath(destPath);
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination directory \"%s\" exists.",
                                fullDestPath.c_str());
         }

         if ((copyContentsOnly && fullSrcPath == fullDestPath)
              || (!copyContentsOnly && osgDB::getFilePath(fullSrcPath) == fullDestPath))
         {
            throw dtUtil::FileUtilIOException(
                   std::string("The source equals the destination: \"") + srcPath + "\"", __FILE__, __LINE__);
         }

         if (!copyContentsOnly)
         {
            const std::string& srcName = osgDB::getSimpleFileName(fullSrcPath);
            fullDestPath += PATH_SEPARATOR + srcName;

            if (DirExists(fullDestPath) && !bOverwrite)
            {
               throw dtUtil::FileUtilIOException(
                           std::string("Cannot overwrite directory (overwrite flag is false): \"") + srcPath + "\"",
                           __FILE__, __LINE__);
            }

            if (!DirExists(fullDestPath))
            {
               MakeDirectory(fullDestPath);
            }

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Destination directory \"%s\" created - copyContentsOnly is false.",
                                   fullDestPath.c_str());
            }
         }
         else
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Destination directory \"%s\" exists - copyContentsOnly is true.",
                                   fullDestPath.c_str());
            }
         }
      }

      InternalDirCopy(fullSrcPath, fullDestPath, bOverwrite);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool FileUtils::DirDelete(const std::string& strDir, bool bRecursive)
   {
      if (bRecursive)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Attempting to recursively delete %s.", strDir.c_str());
         }
         try
         {
            PushDirectory(strDir);
            try
            {
               RecursDeleteDir(true);
            }
            catch(const dtUtil::Exception&)
            {
               PopDirectory();
               throw;
            }

            PopDirectory();
         }
         catch (const dtUtil::FileNotFoundException& ex)
         {
            DTUNREFERENCED_PARAMETER(ex); //squelch "unreferenced" warning

            // if we get a file not found trying to recurse into the top directory
            // then the directory does not exist, so there is no need to throw an exception.
            if (!DirExists(strDir))
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                     "Directory %s doesn't exist to delete. Ignoring.", strDir.c_str());
               }
               return true;
           }
         }
         catch (const dtUtil::Exception&)
         {
            throw;
         }
      }
      else
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Attempting to delete %s, but not recursively.", strDir.c_str());
         }
      }

      errno = 0;
      if (rmdir(strDir.c_str()) != 0)
      {
         if (!bRecursive && errno == ENOTEMPTY)
         {
            return false;
         }
         else if (errno == ENOENT)
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Directory %s doesn't exist to delete. Ignoring.", strDir.c_str());
            }
            return true;
         }
         else
         {
            throw dtUtil::FileUtilIOException(
                   std::string("Unable to delete directory \"") + strDir + "\":" + strerror(errno), __FILE__, __LINE__);
         }
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::MakeDirectory(const std::string& strDir) const
   {
      if (!iMakeDirectory(strDir))
      {
         FileType ft = GetFileInfo(strDir).fileType;
         if (ft == REGULAR_FILE)
         {
            throw dtUtil::FileUtilIOException( std::string("Cannot create directory. ")
                   + strDir + " is an existing non-directory file.", __FILE__, __LINE__);
         }
         else if (ft == DIRECTORY)
         {
            return;
         }

         if (!DirExists(osgDB::getFilePath(strDir)))
         {
            throw dtUtil::FileNotFoundException( std::string("Cannot create directory ")
                   + strDir + ". Parent directory doesn't exist.", __FILE__, __LINE__);
         }
         else
         {
            throw dtUtil::FileUtilIOException( std::string("Cannot create directory ") + strDir + ".", __FILE__, __LINE__);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool FileUtils::DirExists(const std::string& strDir, bool caseInsensitive) const
   {
      return GetFileInfo(strDir, caseInsensitive).fileType == DIRECTORY;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string FileUtils::RelativePath(const std::string& absolutePath, const std::string& file) const
   {
      if (!IsAbsolutePath(absolutePath) || !IsAbsolutePath(file))
      {
         return file;
      }

      std::string relativePath;

      if (file.empty() || absolutePath.empty())
      {
         return file;
      }

      const std::string PS(PATH_SEPARATOR, 1);

      std::string root1, root2;
      root1 = GetPathRoot(file);
      root2 = GetPathRoot(absolutePath);

      if (root1 != root2)
      {
         return file;
      }

      std::vector<std::string> absPathVec, filePathVec;
      IsPathSeparator pathSepFunc;

      dtUtil::StringTokenizer<IsPathSeparator>::tokenize(absPathVec, absolutePath, pathSepFunc);
      dtUtil::StringTokenizer<IsPathSeparator>::tokenize(filePathVec, file, pathSepFunc);

      size_t pointOfDivergence = 0;

      for (size_t i = 0; i < filePathVec.size() && i < absPathVec.size(); ++i)
      {
         if (filePathVec[i] != absPathVec[i])
         {
            break;
         }
         ++pointOfDivergence;
      }

      // Add ../ for each of the rest of the items in the absolute path.
      for (size_t i = pointOfDivergence; i < absPathVec.size(); ++i)
      {
         relativePath.append("../");
      }

      // Add the non matching items from the file path.
      for (size_t i = pointOfDivergence; i < filePathVec.size(); ++i)
      {
         relativePath.append(filePathVec[i]);
         // only put the slash in if it's not the last one.
         if (i < filePathVec.size() - 1)
         {
            relativePath.append("/");
         }
      }



      return relativePath;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool FileUtils::IsSameFile(const std::string& file1, const std::string& file2) const
   {
      bool result = false;

      FileInfo fileInfo1 = GetFileInfo(file1);
      FileInfo fileInfo2 = GetFileInfo(file2);

      if(!fileInfo1.isInArchive && !fileInfo2.isInArchive)
      {
         std::string tmp1 = file1;
         std::string tmp2 = file2;

         try
         {
            if ( ! fileInfo1.path.empty())
            {
               tmp1 = GetAbsolutePath(file1);
            }
            if ( ! fileInfo2.path.empty())
            {
               tmp2 = GetAbsolutePath(file2);
            }
         }
         catch(...)
         {
            // This query method is not a good place to allow an exception through to the caller.
         }

         //the former, and non archive route..
         result = IsSameFile_Internal(tmp1, tmp2);
      }
      else
      {
         std::string file1ArchiveName = file1;
         std::string file2ArchiveName = file2;
         std::string file1InArchive, file2InArchive;


         if(!IsAbsolutePath(file1))
         {
            file1ArchiveName = ArchiveRelativeToAbsolute(file1);
         }
         if(!IsAbsolutePath(file2))
         {
            file2ArchiveName = ArchiveRelativeToAbsolute(file2);
         }

         SplitArchiveFilename(file1ArchiveName, file1ArchiveName, file1InArchive);
         SplitArchiveFilename(file2ArchiveName, file2ArchiveName, file2InArchive);

         //compare the actual archives first
         if(IsSameFile_Internal(file1ArchiveName, file2ArchiveName))
         {
            result = (file1InArchive == file2InArchive);
         }

      }

      return result;
   }

   //-----------------------------------------------------------------------
   bool FileUtils::IsSameFile_Internal(const std::string& file1, const std::string& file2) const
   {
      // If path names are different, we still could be pointing at the same file
      // on disk.
      struct stat stat1;
      struct stat stat2;
      memset(&stat1, 0, sizeof(struct stat));
      memset(&stat2, 0, sizeof(struct stat));

      if (stat(file1.c_str(), &stat1) != 0)
      {
         return false;
      }

      if (stat(file2.c_str(), &stat2) != 0)
      {
         return false;
      }

// only Unix variants support inodes
#ifndef WIN32
      if (stat1.st_ino == stat2.st_ino)
      {
         return true;
      }
#else // WIN32 -- No inodes in Windows -- we'll have to imitate an inode's functionality.
      if (stat1.st_atime == stat2.st_atime &&
          stat1.st_ctime == stat2.st_ctime &&
          stat1.st_mtime == stat2.st_mtime &&
          stat1.st_gid   == stat2.st_gid   &&
          stat1.st_uid   == stat2.st_uid   &&
          stat1.st_mode  == stat2.st_mode  &&
          stat1.st_size  == stat2.st_size)
      {
         // also make sure the file names (NOT paths) are the same
         std::string file1Name = file1.substr(file1.find_last_of("\\/"));
         // Windows is case insensitive
         std::transform(file1Name.begin(), file1Name.end(), file1Name.begin(), tolower);
         std::string file2Name = file2.substr(file2.find_last_of("\\/"));
         std::transform(file2Name.begin(), file2Name.end(), file2Name.begin(), tolower);

         if (file1Name == file2Name)
         {
            return true;
         }
      }
#endif

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FileUtils::RecursDeleteDir(bool bRecursive)
   {
      // this method assumes one is IN the directory that you want to delete.
      DirectoryContents dirCont = DirGetFiles(mCurrentDirectory);

      // iterate over all of the directory contents.
      for (DirectoryContents::const_iterator i = dirCont.begin(); i != dirCont.end(); ++i)
      {
         FileType ft = GetFileInfo(*i).fileType;
         if (ft == REGULAR_FILE)
         {
            // Delete regular files.
            errno = 0;
            if (unlink(i->c_str()) < 0)
            {
               throw dtUtil::FileUtilIOException(
                      std::string("Unable to delete directory \"") + *i + "\":" + strerror(errno), __FILE__, __LINE__);
            }
         }
         else if ((*i != ".") && (*i != "..") && ft == DIRECTORY && bRecursive)
         {
            // if it's a directory and it's not the "." or ".." special directories,
            // change into that directory and recurse.
            ChangeDirectoryInternal(*i);
            RecursDeleteDir(true);
            // now that the directory is empty, remove it.
            errno = 0;
            if (rmdir(i->c_str()) != 0)
            {
               if (errno == ENOENT)
               {
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  {
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                         "Directory %s doesn't exist to delete. Ignoring.", (mCurrentDirectory + PATH_SEPARATOR + *i).c_str());
                  }
                  return;
               }
               else
               {
                  throw dtUtil::FileUtilIOException(
                         std::string("Unable to delete directory \"") + mCurrentDirectory + PATH_SEPARATOR + *i + "\":" + strerror(errno), __FILE__, __LINE__);

               }
            }
         }
      }

      // change up so that when the method ends, the directory is ready to be removed.
      ChangeDirectoryInternal(std::string(".."));
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string FileUtils::ConcatPaths(const std::string& left, const std::string& right)
   {
#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION <= 2 && defined(OPENSCENEGRAPH_MINOR_VERSION) && OPENSCENEGRAPH_MINOR_VERSION <= 8 && defined(OPENSCENEGRAPH_PATCH_VERSION) && OPENSCENEGRAPH_PATCH_VERSION < 3
      if (left.empty())
      {
         return right;
      }
#endif

      return osgDB::concatPaths(left, right);
   }

   /////////////////////////////////////////////////////////////////////////////
   FileUtils::FileUtils()
   {
      mLogger = &dtUtil::Log::GetInstance(std::string("fileutils.cpp"));
      // assign the current directory
      ChangeDirectory(".");

      //adding zip extension for use as archive
      osgDB::Registry::instance()->addArchiveExtension("zip");
   }

   /////////////////////////////////////////////////////////////////////////////
   FileUtils::~FileUtils() {}


   ////////////////////////////////////////////////////////////////////////////////
   bool FileUtils::SplitArchiveFilename(const std::string& fullFilename, std::string& archiveFilename, std::string& fileInArchive) const
   {

      std::string filename = fullFilename;
      CleanupFileString(filename);


//osgDB::Registry::getArchiveExtensions() was submitted and should be released in OSG 3.0
#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 3

      osgDB::Registry* reg = osgDB::Registry::instance();
      const osgDB::Registry::ArchiveExtensionList& extensions = reg->getArchiveExtensions();

      osgDB::Registry::ArchiveExtensionList::const_iterator iter = extensions.begin();
      osgDB::Registry::ArchiveExtensionList::const_iterator iterEnd = extensions.end();
      for(;iter != iterEnd; ++iter)
      {
         const std::string& archiveExtension = *iter;
#else
         std::string archiveExtension = ".zip";
         {
#endif

         std::string::size_type positionArchive = filename.find(archiveExtension+'/');
         if (positionArchive==std::string::npos) positionArchive = filename.find(archiveExtension+'\\');
         if (positionArchive!=std::string::npos)
         {
            std::string::size_type endArchive = positionArchive + archiveExtension.length();
            //copy filename -in case the function was called with filename and archivefilename the same string
            std::string filenameCopy = filename;
            archiveFilename = filenameCopy.substr(0,endArchive);
            fileInArchive = filenameCopy.substr(endArchive+1,std::string::npos);
            return true;
         }
         else //maybe it is an archive
         {
            std::string::size_type positionArchive2 = filename.find(archiveExtension);
            if (positionArchive2 != std::string::npos)
            {
               archiveFilename = filename;
               return false;
            }
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtUtil::FileType FileUtils::GetFileTypeForFileInArchive(const osgDB::ArchiveExtended& a, const std::string& path) const
   {
      return GetFileTypeFromOSGDBFileType(a.getFileType(path));
   }


   ////////////////////////////////////////////////////////////////////////////////
   void FileUtils::DirGetFilesInArchive(const osgDB::ArchiveExtended& a, const std::string& path, DirectoryContents& result) const
   {

      std::string archiveName;
      std::string fileInArchive;

      SplitArchiveFilename(path, archiveName, fileInArchive);

      result = a.getDirectoryContents(fileInArchive);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtUtil::FileInfo FileUtils::GetFileInfoForFileInArchive(const osgDB::ArchiveExtended& a, const std::string& strFile) const
   {
      dtUtil::FileInfo info;

      // chop trailing slashes off
      std::string choppedStr = strFile;
      if (strFile.size() > 0 && (strFile[strFile.size() - 1] == '\\' ||
         strFile[strFile.size() - 1] == '/'))
      {
         choppedStr = strFile.substr(0, strFile.length() - 1);
      }

      info.fileType = GetFileTypeFromOSGDBFileType(a.getFileType(strFile));

      if(info.fileType != FILE_NOT_FOUND)
      {

         info.extensionlessFileName = osgDB::getStrippedName(choppedStr);
         info.baseName = osgDB::getSimpleFileName(choppedStr);
         info.fileName = choppedStr;
         info.path = osgDB::getFilePath(choppedStr);
         info.extension = osgDB::getFileExtension(choppedStr);
         info.isInArchive = true;

         //todo- how to get these?
         info.size = 0;
         info.lastModified = 0;
      }

      return info;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osgDB::ArchiveExtended* FileUtils::FindArchive(const std::string& filename) const
   {
      dtCore::RefPtr<osgDB::Archive> archiveResult = NULL;
      FileInfo archiveInfo = GetFileInfo(filename);

      if(archiveInfo.fileType != FILE_NOT_FOUND)
      {
         osgDB::Registry* reg = osgDB::Registry::instance();

         osgDB::ReaderWriter::ReadResult result = osgDB::ReaderWriter::ReadResult::FILE_NOT_HANDLED;

         std::string archiveFilename;
         std::string strippedFilename;
         std::string absoluteFilename = filename;

         if(!IsAbsolutePath(filename))
         {
            absoluteFilename = ArchiveRelativeToAbsolute(filename);
         }

         SplitArchiveFilename(absoluteFilename, archiveFilename, strippedFilename);

         archiveResult = reg->getFromArchiveCache(archiveFilename);
         if(archiveResult == NULL)
         {
            //attempt to load
            dtCore::RefPtr<osgDB::ReaderWriter::Options> options = reg->getOptions() ?
               static_cast<osgDB::ReaderWriter::Options*>(reg->getOptions()->clone(osg::CopyOp::SHALLOW_COPY)) :
            new osgDB::ReaderWriter::Options;

            osgDB::ReaderWriter::ReadResult readResult = reg->openArchive(archiveFilename, osgDB::ReaderWriter::READ, 4096, options.get());
            if(readResult.success())
            {
               archiveResult = readResult.getArchive();
            }

         }
      }

      if(archiveResult.valid())
      {
         return dynamic_cast<osgDB::ArchiveExtended*>(archiveResult.get());
      }
      else
      {
         return NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtUtil::FileType FileUtils::GetFileTypeFromOSGDBFileType( osgDB::FileType ft ) const
   {
      if(ft == osgDB::REGULAR_FILE)
      {
         return dtUtil::REGULAR_FILE;
      }
      else if (ft == osgDB::DIRECTORY)
      {
         return dtUtil::DIRECTORY;
      }
      else
      {
         return dtUtil::FILE_NOT_FOUND;
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   bool FileUtils::ContainsArchiveExtension(const std::string& path) const
   {

//osgDB::Registry::getArchiveExtensions() was submitted and should be released in OSG 3.0
#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 3
      osgDB::Registry* reg = osgDB::Registry::instance();

      const osgDB::Registry::ArchiveExtensionList& extensions = reg->getArchiveExtensions();

      osgDB::Registry::ArchiveExtensionList::const_iterator iter = extensions.begin();
      osgDB::Registry::ArchiveExtensionList::const_iterator iterEnd = extensions.end();
      for(;iter != iterEnd; ++iter)
      {
         const std::string& archiveExtension = *iter;
#else
         std::string archiveExtension = ".zip";
         {
#endif

         std::string::size_type positionArchive = path.find(archiveExtension);
         if(positionArchive != std::string::npos)
         {
            return true;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Object* FileUtils::ReadObject(const std::string& filename, osgDB::ReaderWriter::Options* options)
   {
      FileInfo info = GetFileInfo(filename);
      osgDB::Registry* reg = osgDB::Registry::instance();

      osg::Object* result = NULL;

      if(info.fileType == ARCHIVE || info.isInArchive)
      {
         std::string archiveFilename;
         std::string strippedFilename;
         std::string absoluteFilename = filename;

         if(!IsAbsolutePath(filename))
         {
            absoluteFilename = ArchiveRelativeToAbsolute(filename);
         }

         SplitArchiveFilename(absoluteFilename, archiveFilename, strippedFilename);

         osg::Object* obj = reg->getFromObjectCache(strippedFilename);
         if(obj != NULL)
         {
            result = obj;
         }
         else
         {
            osgDB::Archive* arch = FindArchive(archiveFilename);
            if(arch != NULL)
            {
               osgDB::ReaderWriter::ReadResult readResult = arch->readObject(strippedFilename, options);
               if(readResult.validObject())
               {
                  result = readResult.takeObject();
                  if (result != NULL && options != NULL && (options->getObjectCacheHint() & osgDB::ReaderWriter::Options::CACHE_OBJECTS))
                  {
                     reg->addEntryToObjectCache(strippedFilename, result);
                  }
               }
            }
         }
      }
      else if(info.fileType == REGULAR_FILE)
      {
         result = osgDB::readObjectFile(filename, options);
      }

      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Node* FileUtils::ReadNode(const std::string& filename, osgDB::ReaderWriter::Options* options)
   {
      FileInfo info = GetFileInfo(filename);
      osgDB::Registry* reg = osgDB::Registry::instance();

      osg::Node* result = NULL;

      if(info.fileType == ARCHIVE || info.isInArchive)
      {
         std::string archiveFilename;
         std::string strippedFilename;
         std::string absoluteFilename = filename;

         if(!IsAbsolutePath(filename))
         {
            absoluteFilename = ArchiveRelativeToAbsolute(filename);
         }

         SplitArchiveFilename(absoluteFilename, archiveFilename, strippedFilename);

         osg::Object* obj = reg->getFromObjectCache(strippedFilename);
         if(obj != NULL)
         {
            result = dynamic_cast<osg::Node*>(obj);
         }
         else
         {
            osgDB::Archive* arch = FindArchive(archiveFilename);
            if(arch != NULL)
            {
               osgDB::ReaderWriter::ReadResult readResult = arch->readNode(strippedFilename, options);
               if(readResult.validNode())
               {
                  result = readResult.takeNode();

                  if (result != NULL && options != NULL && (options->getObjectCacheHint() & osgDB::ReaderWriter::Options::CACHE_NODES))
                  {
                     reg->addEntryToObjectCache(strippedFilename, result);
                  }
               }
            }
         }
      }
      else // resume with usual loading
      {
         result = osgDB::readNodeFile(filename, options);
      }

      return result;
   }

   /*void FileUtils::AbsoluteToRelative(const std::string& pcAbsPath, std::string& relPath)
   {
      char pcRelPath[MAX_PATH];
      char acTmpCurrDir[MAX_PATH];
      char acTmpAbsPath[MAX_PATH];
      int count = 0;
      std::string curDir = CurrentDirectory();

      for (size_t i = 0; i < curDir.size(); ++i)
      {
         if (curDir[i] == '\\')
         {
            curDir[i] = '/';
         }
      }

      strcpy(acTmpCurrDir, curDir.c_str());
      strcpy(acTmpAbsPath, pcAbsPath.c_str());

      std::stack<char*> tmpStackAbsPath;
      std::stack<char*> tmpStackCurrPath;
      std::stack<char*> tmpStackOutput;
      std::queue<char*> tmpMatchQueue;

      const char* pathSep = "/";

      char* sTmp = strtok(acTmpAbsPath, pathSep);
      while (sTmp)
      {
         tmpStackAbsPath.push(sTmp);
         sTmp = strtok(0, pathSep);
      }

      sTmp = strtok(acTmpCurrDir, pathSep);
      while (sTmp)
      {
         tmpStackCurrPath.push(sTmp);
         sTmp = strtok(0, pathSep);
      }

      sTmp = pcRelPath;
      while (tmpStackCurrPath.size() > tmpStackAbsPath.size())
      {
         *sTmp++ = '.';
         *sTmp++ = '.';
         *sTmp++ = '/';
         tmpStackCurrPath.pop();
      }

      while (tmpStackAbsPath.size() > tmpStackCurrPath.size())
      {
         char* pcTmp = tmpStackAbsPath.top();
         tmpStackOutput.push(pcTmp);
         tmpStackAbsPath.pop();
      }

      while (!tmpStackAbsPath.empty())
      {
         if (strcmp(tmpStackAbsPath.top(),tmpStackCurrPath.top()) == 0)
         {
            tmpMatchQueue.push(tmpStackAbsPath.top());
         }
         else
         {
            while (!tmpMatchQueue.empty())
            {
               tmpStackOutput.push(tmpMatchQueue.front());
               tmpMatchQueue.pop();
            }
            tmpStackOutput.push(tmpStackAbsPath.top());
            sTmp[count++] = '.';
            sTmp[count++] = '.';
            sTmp[count++] = '/';
         }
         tmpStackAbsPath.pop();
         tmpStackCurrPath.pop();
      }
      while (!tmpStackOutput.empty())
      {
         char* pcTmp= tmpStackOutput.top();
         while (*pcTmp != '\0')
         {
            sTmp[count++] = *pcTmp++;
         }
         tmpStackOutput.pop();
         sTmp[count++] = '/';
      }
      sTmp[--count] = '\0';

      relPath = sTmp;
   }*/

   /////////////////////////////////////////////////////////////////////////////
   FileUtilIOException::FileUtilIOException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   FileNotFoundException::FileNotFoundException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   DirectoryPush::DirectoryPush(const std::string& dir)
   : mSucceeded(false)
   {
      try
      {
         FileUtils::GetInstance().PushDirectory(dir);
         mSucceeded = true;
      }
      catch(FileNotFoundException& ex)
      {
         // Eat it because we have a bool to say if it passed.
         mError = ex.ToString();
      }
   }
   /////////////////////////////////////////////////////////////////////////////
   DirectoryPush::~DirectoryPush()
   {
      if (mSucceeded)
      {
         FileUtils::GetInstance().PopDirectory();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DirectoryPush::GetSucceeded()
   {
      return mSucceeded;
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& DirectoryPush::GetError()
   {
      return mError;
   }


} // namespace dtUtil
