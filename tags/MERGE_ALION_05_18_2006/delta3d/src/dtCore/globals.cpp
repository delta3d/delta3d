#include <dtCore/globals.h>
#include <dtUtil/log.h>
#include <stdexcept>

#include <osgDB/FileUtils>

/**
 * Set the list of paths that dtCore should use to search for files to load.  Paths
 * are separated with a single ";" on Win32 and a single ":" on Linux. Remember to
 * double-up your backslashes, lest they be escaped.
 *
 * @param pathList : The list of all paths to be used to find data files
 */
void dtCore::SetDataFilePathList( const std::string& pathList )
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
std::string dtCore::GetDataFilePathList()
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
         #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
         pathString += ';';
         #else
         pathString += ':';
         #endif
      }
   }

   return pathString;
}

/** 
 * Simple method to return the system environment variable.  If the env var
 * is not set, the local path will be returned.
 *
 * @param env The system environment variable to be queried
 * @return The value of the environment variable
 */
DT_CORE_EXPORT std::string dtCore::GetEnvironment( const std::string& env )
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

/**
 * Get the Delta Data file path.  This comes directly from the environment 
 * variable "DELTA_DATA".  If the environment variable is not set, the local
 * directory will be returned.
 */
DT_CORE_EXPORT std::string dtCore::GetDeltaDataPathList()
{
   return GetEnvironment("DELTA_DATA");
}

/** 
 * If the DELTA_ROOT environment is not set, the local directory will be
 * returned.
 */
DT_CORE_EXPORT std::string dtCore::GetDeltaRootPath()
{
   return GetEnvironment("DELTA_ROOT");
}
