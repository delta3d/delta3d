
#include "globals.h"
#include "osgDB/FileUtils"



/*!
 * Set the list of paths that dtCore should use to search for files to load.  Paths
 * are separated with a single ";" on Win32 and a single ":" on Linux.
 *
 * @param pathList : The list of all paths to be used to find data files
 */
void dtCore::SetDataFilePathList(std::string pathList )
{
   for(std::string::iterator itr = pathList.begin(); itr != pathList.end(); itr++)
   {
      #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
      if( *itr == ':' )
         *itr = ';';
      #else
      if( *itr == ';' )
         *itr = ':'; 
      #endif
   }
   
   osgDB::setDataFilePathList(pathList);
}

