
#include "globals.h"
#include "osgDB/FileUtils"


/*!
 * Set the list of paths that dtCore should use to search for files to load.  Paths
 * are separated with a single ";".
 *
 * @param pathList : The list of all paths to be used to find data files
 */
void dtCore::SetDataFilePathList(std::string pathList )
{
   osgDB::setDataFilePathList(pathList);
}