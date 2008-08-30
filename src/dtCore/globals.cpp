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
#include <prefix/dtcoreprefix-src.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>
#include <dtUtil/macros.h>

#include <dtCore/globals.h>

#include <osgDB/FileUtils>

namespace dtCore
{

   /**
    * Set the list of paths that dtCore should use to search for files to load.  Paths
    * are separated with a single ";" on Win32 and a single ":" on Linux. Remember to
    * double-up your backslashes, lest they be escaped.
    *
    * @param pathList : The list of all paths to be used to find data files
    */
   void SetDataFilePathList( const std::string& pathList )
   {
      std::string modpath = pathList;
      for( std::string::size_type i = 0; i < pathList.size(); i++ )
      {
         #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
         try
         {
            if( modpath.at(i) == ':' && modpath.at(i+1) != '\\' )
            {
               modpath.at(i) = ';';
            }
         }
         catch( std::out_of_range myexcept )
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
   
   /**
    * Get the list of paths that dtCore should use to search for files to load.  Paths
    * are separated with a single ";" on Win32 and a single ":" on Linux.
    * 
    * @see SetDataFilePathList()
    */
   std::string GetDataFilePathList()
   {
      osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   
      std::string pathString = "";
   
      typedef std::deque<std::string> StringDeque;
      for(StringDeque::iterator itr = pathList.begin(); itr != pathList.end(); itr++)
      {
         pathString += *itr;
   
         StringDeque::iterator next = itr + 1;
         if( next != pathList.end() )
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
   
   std::string FindFileInPathList(const std::string &fileName)
   {
      return osgDB::findDataFile(fileName);
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
   
   
   /** 
    * Simple method to return the system environment variable.  If the env var
    * is not set, the local path will be returned.
    *
    * @param env The system environment variable to be queried
    * @return The value of the environment variable
    */
   std::string GetEnvironment( const std::string& env )
   {
      if( char* ptr = getenv( env.c_str() ) )
      {
         return std::string(ptr);
      }
      else
      {
         return std::string("./");
      }
   }
  
   //////////////////////////////////////////////////////////////////////
   void SetEnvironment(const std::string& name, const std::string& value)
   {
#ifdef DELTA_WIN32
      std::ostringstream oss;
      oss << name << "=" << value;  
      putenv(oss.str().c_str());
#else
      setenv(name.c_str(), value.c_str(), true);
#endif
   }
   
   /**
    * Get the Delta Data file path.  This comes directly from the environment 
    * variable "DELTA_DATA".  If the environment variable is not set, the local
    * directory will be returned.
    * @todo need to decide how paths will be handled.  We need to decide if DELTA_DATA is a list or a single item.
    */
   std::string GetDeltaDataPathList()
   {
      return GetEnvironment("DELTA_DATA");
   }
   
   /** 
    * If the DELTA_ROOT environment is not set, the local directory will be
    * returned.
    */
   std::string GetDeltaRootPath()
   {
      return GetEnvironment("DELTA_ROOT");
   }
}
