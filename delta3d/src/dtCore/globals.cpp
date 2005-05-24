#include <dtCore/globals.h>

#include <osgDB/FileUtils>

/*!
 * Set the list of paths that dtCore should use to search for files to load.  Paths
 * are separated with a single ";" on Win32 and a single ":" on Linux. Remember to
 * double-up your backslashes, lest they be escaped.
 *
 * @param pathList : The list of all paths to be used to find data files
 */
void dtCore::SetDataFilePathList( std::string pathList )
{
   for( std::string::size_type i = 0; i < pathList.size(); i++ )
   {
      #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
      try
      {
         if( pathList.at(i) == ':' && pathList.at(i+1) != '\\' )
         {
            pathList.at(i) = ';';
         }
      }
      catch( std::out_of_range )
      {}
      #else
      if( pathList[i] == ';' )
      {
         pathList[i] = ':'; 
      }
      #endif
   }
   
   osgDB::setDataFilePathList(pathList);
}

/*!
* Get the list of paths that dtCore should use to search for files to load.  Paths
* are separated with a single ";" on Win32 and a single ":" on Linux.
*/
std::string dtCore::GetDataFilePathList()
{
   osgDB::FilePathList pathList = osgDB::getDataFilePathList();

   std::string pathString = "";
   for(std::deque<std::string>::iterator itr = pathList.begin(); itr != pathList.end(); itr++)
   {
      pathString += *itr;

      std::deque<std::string>::iterator next = itr + 1;
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

/*!
 * Get the Delta Data file path.  This comes directly from the environment 
 * variable "DELTA_DATA".  If the environment variable is not set, the local
 * directory will be returned.
 */
DT_EXPORT std::string dtCore::GetDeltaDataPathList(void)
{
   char *ptr;
   if( (ptr = getenv( "DELTA_DATA" )) )
   {
      return (std::string(ptr));
   }
   else return (std::string("./"));
}
