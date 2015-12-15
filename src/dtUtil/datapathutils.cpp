/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2006 MOVES Institute 
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
 */

#include <prefix/dtutilprefix.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/log.h>
#include <dtUtil/mswinmacros.h>
#include <dtUtil/mswin.h>

#include <dtUtil/fileutils.h>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <sstream>

#ifdef __APPLE__
#include <AvailabilityMacros.h>
#include <Foundation/Foundation.h>
#endif
#ifdef DELTA_WIN32
#include <Shlobj.h>
#else
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace dtUtil
{
   static OpenThreads::Mutex gDatapathMutex;

   std::string GetHomeDirectory()
   {
      std::string homedir;
#ifdef DELTA_WIN32
      char path[MAX_PATH];
      if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, path))) {
         homedir = path;
      }
#else
      int maxSize = sysconf(_SC_GETPW_R_SIZE_MAX);
      char* buffer = new char[maxSize];
      struct passwd pw, *resultOut = NULL;
      int rc = getpwuid_r(getuid(), &pw, buffer, maxSize, &resultOut);
      if (rc == 0 && resultOut != NULL)
      {
         homedir = pw.pw_dir;
      }
      delete[] buffer;
      buffer = NULL;
#endif
      return homedir;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetDataFilePathList(const std::string& pathList)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(gDatapathMutex);

      std::string modpath = pathList;
      std::string::size_type pathLength = pathList.size();
      for (std::string::size_type i = 0; i < pathLength; i++)
      {
#ifdef DELTA_WIN32
         try
         {
            // TODO: Determine why this silly thing is even being done.
            // Did anyone consider that this would destroy a path with
            // drive letter such as C:/... ? How about we do not concatenate
            // paths with colons? CR
            if (modpath.at(i) == ':' && (i + 1 >= pathLength || modpath.at(i + 1) != '\\'))
            {
               modpath.at(i) = ';';
            }
         }
         catch(std::out_of_range myexcept)
         {
            LOG_WARNING(myexcept.what());
         }
#else
         if( modpath[i] == ';' )
         {
            modpath[i] = ':'; 
         }
#endif
      }
      osgDB::setDataFilePathList(modpath);
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string GetDataFilePathList()
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(gDatapathMutex);

      osgDB::FilePathList pathList = osgDB::getDataFilePathList();

      std::string pathString = "";

      typedef std::deque<std::string> StringDeque;
      for(StringDeque::iterator itr = pathList.begin(); itr != pathList.end(); itr++)
      {
         pathString += *itr;

         StringDeque::iterator next = itr + 1;         if(next != pathList.end())
         {
#ifdef DELTA_WIN32
            pathString += ';';
#else
            pathString += ':';
#endif
         }
      }

      return pathString;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string FindFileInPathList(const std::string& fileName)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(gDatapathMutex);

      std::string filePath = osgDB::findDataFile(fileName, osgDB::CASE_INSENSITIVE);

      // In some cases, filePath will contain a url that is
      // relative to the current working directory so for
      // consistency, be sure to return the full path every time
      if (!filePath.empty())
      {
         filePath = osgDB::getRealPath(filePath);
      }

      return filePath;

      /**
      std::vector<std::string> pathList;
      std::vector<std::string>::const_iterator itor;

   #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
      dtUtil::IsDelimeter delimCheck(';');
   #else
      dtUtil::IsDelimeter delimCheck(':');
   #endif

      dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(pathList,
                                                             GetDataFilePathList(),delimCheck);

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      std::string path;
      for (itor=pathList.begin(); itor!=pathList.end(); ++itor)
      {
         path = *itor;
         //Make sure we remove any trailing slashes from the cache path.
         if (path[path.length()-1] == '/' || path[path.length()-1] == '\\')
            path = path.substr(0,path.length()-1);

         if (fileUtils.FileExists(path + dtUtil::FileUtils::PATH_SEPARATOR + fileName))
            return path + dtUtil::FileUtils::PATH_SEPARATOR + fileName;
      }     

      return std::string();
       */
   }   

   /////////////////////////////////////////////////////////////////////////////
   std::string FindFileInPathList(const std::string& fileName, std::vector<std::string> pathList, bool caseInsensitive)
   {
      std::vector<std::string>::const_iterator itor;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      std::string path;
      for (itor=pathList.begin(); itor!=pathList.end(); ++itor)
      {
         path = *itor;
         //Make sure we remove any trailing slashes from the cache path.
         if (path[path.length()-1] == '/' || path[path.length()-1] == '\\')
            path = path.substr(0,path.length()-1);

         FileInfo fi = fileUtils.GetFileInfo(path + FileUtils::PATH_SEPARATOR + fileName, caseInsensitive);
         if (fi.fileType != FILE_NOT_FOUND)
         {
            return fi.fileName;
         }
      }

      return std::string();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string GetEnvironment(const std::string& env)
   {
#ifdef DELTA_WIN32
      const size_t bufferSize = 32767;
      TCHAR  buffer[bufferSize], buffer2[bufferSize];
      size_t sizeOut1 = GetEnvironmentVariable(env.c_str(), buffer, bufferSize);
      size_t sizeOut2 = ExpandEnvironmentStrings(buffer, buffer2, bufferSize);
      if (sizeOut1 <= bufferSize && sizeOut2 <= bufferSize && sizeOut1 > 0 && sizeOut2 > 0)
      {
         return buffer2;
      }
#endif
      char* ptr = getenv(env.c_str());
      if(ptr != NULL)
      {
         return std::string(ptr);
      }
      else
      {
         return std::string();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetEnvironment(const std::string& name, const std::string& value)
   {
#ifdef DELTA_WIN32
      //technically we have to do both because windows has two different environments
      SetEnvironmentVariable(name.c_str(), value.c_str());
      _putenv_s(name.c_str(), value.c_str());
#else
      setenv(name.c_str(), value.c_str(), true);
#endif
   }

#ifdef __APPLE__
   /**
    * Internal function for finding the OS X app bundle resource path.
    */
   std::string GetBundleResourcesPath()
   {
      // Since this is currently the only Objective-C code in the
      // library, we need an autoreleasepool for obj-c memory management.
      // If more Obj-C is added, we might move this pool to another
      // location so it can be shared. Pools seem to be stackable,
      // so I don't think there will be a problem if multiple pools
      // exist at a time.
      NSAutoreleasePool* mypool = [[NSAutoreleasePool alloc] init];

      // This is the path to the resources inside the app bundle.
      NSString* resourcePathNS = [[NSBundle mainBundle] resourcePath];

      // Make a c string from the cocoa one tack data on the end.
      std::string result = std::string([resourcePathNS UTF8String]);

      // Clean up the autorelease pool
      [mypool release];
      return result;
   }

   std::string GetBundlePlugInsPath()
   {
      // Since this is currently the only Objective-C code in the
      // library, we need an autoreleasepool for obj-c memory management.
      // If more Obj-C is added, we might move this pool to another
      // location so it can be shared. Pools seem to be stackable,
      // so I don't think there will be a problem if multiple pools
      // exist at a time.
      NSAutoreleasePool* mypool = [[NSAutoreleasePool alloc] init];

      // This is the path to the resources inside the app bundle.
      NSString* resourcePathNS = [[NSBundle mainBundle] builtInPlugInsPath];

      // Make a c string from the cocoa one tack data on the end.
      std::string result = std::string([resourcePathNS UTF8String]);

      // Clean up the autorelease pool
      [mypool release];
      return result;
   }

   std::string GetBundlePath()
   {
      // Since this is currently the only Objective-C code in the
      // library, we need an autoreleasepool for obj-c memory management.
      // If more Obj-C is added, we might move this pool to another
      // location so it can be shared. Pools seem to be stackable,
      // so I don't think there will be a problem if multiple pools
      // exist at a time.
      NSAutoreleasePool* mypool = [[NSAutoreleasePool alloc] init];

      // This is the path to the resources inside the app bundle.
      NSString* resourcePathNS = [[NSBundle mainBundle] bundlePath];

      // Make a c string from the cocoa one tack data on the end.
      std::string result = std::string([resourcePathNS UTF8String]);

      // Clean up the autorelease pool
      [mypool release];
      return result;
   }

   void RemovePSNCommandLineOption(int& argc, char**& argv)
   {
      static std::string osxPSN("-psn_");
      for (int i = 0; i < argc; ++i)
      {
         if (std::string(argv[i]).compare(0, osxPSN.length(), osxPSN) == 0)
         {
            for (int j = i; j < argc; ++j)
            {
               if (j == argc-1)
               {
                  argv[j] = 0;
               }
               else
               {
                  argv[j] = argv[j+1];
               }
            }
            --argc;
         }
      }
   }
#endif

   /**
    * Get the Delta Data file path.  This comes directly from the environment 
    * variable "DELTA_DATA".  If the environment variable is not set, the local
    * directory will be returned.
    * @todo need to decide how paths will be handled.  We need to decide if DELTA_DATA is a list or a single item.
    */
   std::string GetDeltaDataPathList()
   {
      std::string result = GetEnvironment("DELTA_DATA");
#ifdef __APPLE__
      if (result.empty())
      {
         result = GetBundleResourcesPath() + "/deltaData";
      }
#endif
      return result;
   }

   /** 
    * If the DELTA_ROOT environment is not set, the local directory will be
    * returned.
    */
   std::string GetDeltaRootPath()
   {
      std::string result = GetEnvironment("DELTA_ROOT");
#ifdef __APPLE__
      if (result.empty())
      {
         result = GetBundleResourcesPath();
      }
#endif
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool IsEnvironment(const std::string& env)
   {
      if (getenv(env.c_str()))
      {
         return true;
      }
      else
      {
         return false;
      }
   }
}
