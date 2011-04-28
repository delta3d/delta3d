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
#include <prefix/dtcoreprefix.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/mswinmacros.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/globals.h>


namespace dtCore
{

   /**
    * Set the list of paths that dtCore should use to search for files to load.  Paths
    * are separated with a single ";" on Win32 and a single ":" on Linux. Remember to
    * double-up your backslashes, lest they be escaped.
    *
    * @param pathList : The list of all paths to be used to find data files
    */
   DEPRECATE_FUNC void SetDataFilePathList( const std::string& pathList )
   {
      DEPRECATE("dtCore::SetDataFilePathList", "dtUtil::SetDataFilePathList");

      dtUtil::SetDataFilePathList(pathList);
   }

   /**
    * Get the list of paths that dtCore should use to search for files to load.  Paths
    * are separated with a single ";" on Win32 and a single ":" on Linux.
    *
    * @see SetDataFilePathList()
    */
   DEPRECATE_FUNC std::string GetDataFilePathList()
   {
      DEPRECATE("dtCore::GetDataFilePathList", "dtUtil::GetDataFilePathList");

      return dtUtil::GetDataFilePathList();
   }

   DEPRECATE_FUNC std::string FindFileInPathList(const std::string& fileName)
   {
      DEPRECATE("dtCore::FindFileInPathList", "dtUtil::FindFileInPathList");

      return dtUtil::FindFileInPathList(fileName);
   }


   /**
    * Simple method to return the system environment variable.  If the env var
    * is not set, the local path will be returned.
    *
    * @param env The system environment variable to be queried
    * @return The value of the environment variable
    */
   DEPRECATE_FUNC std::string GetEnvironment(const std::string& env)
   {
      DEPRECATE("dtCore::GetEnvironment", "dtUtil::GetEnvironment");

      if (char* ptr = getenv(env.c_str()))
      {
         return std::string(ptr);
      }
      else
      {
         return std::string("./");
      }
   }

   //////////////////////////////////////////////////////////////////////
   DEPRECATE_FUNC void SetEnvironment(const std::string& name, const std::string& value)
   {
      DEPRECATE("dtCore::SetEnvironment", "dtUtil::SetEnvironment");

#ifdef DELTA_WIN32
      std::ostringstream oss;
      oss << name << "=" << value;
      _putenv(oss.str().c_str());
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
   DEPRECATE_FUNC std::string GetDeltaDataPathList()
   {
      DEPRECATE("dtCore::GetDeltaDataPathList", "dtUtil::GetDeltaDataPathList");

      return dtUtil::GetEnvironment("DELTA_DATA");
   }

   /**
    * If the DELTA_ROOT environment is not set, the local directory will be
    * returned.
    */
   DEPRECATE_FUNC std::string GetDeltaRootPath()
   {
      DEPRECATE("dtCore::GetDeltaRootPath", "dtUtil::GetDeltaRootPath");

      return dtUtil::GetEnvironment("DELTA_ROOT");
   }

   ////////////////////////////////////////////////////////////////////////////////
   DEPRECATE_FUNC DT_CORE_EXPORT bool IsEnvironment(const std::string& env)
   {
      DEPRECATE("dtCore::IsEnvironment", "dtUtil::IsEnvironment");

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
