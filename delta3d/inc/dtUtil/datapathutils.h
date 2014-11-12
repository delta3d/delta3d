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

#ifndef DELTA_DATA_FILE_PATH_LIST
#define DELTA_DATA_FILE_PATH_LIST

#include <dtUtil/export.h>
#include <string>
#include <vector>

namespace dtUtil
{
   DT_UTIL_EXPORT std::string GetHomeDirectory();

   /// Set the list of data file paths
   DT_UTIL_EXPORT void SetDataFilePathList(const std::string& pathList);

   /// Get the list of data file paths
   DT_UTIL_EXPORT std::string GetDataFilePathList();

   /// Get the Delta Data path list (equivalent to the DELTA_DATA environment)
   DT_UTIL_EXPORT std::string GetDeltaDataPathList();

   /// Get the root path to Delta3D (equivalent to the DELTA_ROOT environment)
   DT_UTIL_EXPORT std::string GetDeltaRootPath();

   /// Convienence method to get the supplied environment variable
   DT_UTIL_EXPORT std::string GetEnvironment(const std::string& env);

   /// Is the supplied environment variable defined?
   DT_UTIL_EXPORT bool IsEnvironment(const std::string& env);

   /**
    * Sets an environment variable.
    * @param name the name of the variable to set.
    * @param value the value of the environment variable to set.
    */
   DT_UTIL_EXPORT void SetEnvironment(const std::string& name, const std::string& value);

   /**
    * Searches for a file in the list of paths found by calling dtCore::GetDataFilePathList() 
    * @param fileName Can be a single filename or a path and file name relative
    *  to the current Delta3D data path list.
    * @return The full path to the file requested or empty string if it's not found. 
    */
   DT_UTIL_EXPORT std::string FindFileInPathList(const std::string& fileName);

   /**
    * Searches a given path list for a file name.
    * @param fileName Can be a single filename or a path and file name relative
    *  to the current Delta3D data path list.
    * @param paths the list of file paths to search.
    * @param caseInsensitive  If the search should be case insensitive.  This only works around case-sensitive file systems,
    *                         it cannot not make a search fail if the case is wrong on a case insensitive one and one sets it to false.
    *
    */
   DT_UTIL_EXPORT std::string FindFileInPathList(const std::string& fileName, std::vector<std::string> paths, bool caseInsensitive = true);

#ifdef __APPLE__
   /**
    * @return the resource path inside the current application bundle.
    * This is used internally, and this api is subject to change.
    */
   std::string GetBundleResourcesPath();

   /**
    * @return the plugins path inside the current application bundle.
    * This is used internally, and this api is subject to change.
    */
   std::string GetBundlePlugInsPath();

   /**
    * @return the application bundle path.
    * This is used internally, and this api is subject to change.
    */
   std::string GetBundlePath();


   void RemovePSNCommandLineOption(int& argc, char**& argv);
#else
   inline void RemovePSNCommandLineOption(int& argc, char**& argv) {}
#endif

}

#endif // DELTA_DATA_FILE_PATH_LIST
