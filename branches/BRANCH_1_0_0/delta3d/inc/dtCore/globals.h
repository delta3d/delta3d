#ifndef DELTA_GLOBALS
#define DELTA_GLOBALS


#include <string>

#include "dtCore/export.h"

namespace dtCore
{
   ///Set the list of data file paths
   DT_EXPORT void SetDataFilePathList( std::string pathList );

   ///Get the Delta Data path list
   DT_EXPORT std::string GetDeltaDataPathList(void);
};


#endif // DELTA_GLOBALS
