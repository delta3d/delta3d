#if !defined(GLOBALS_H_INCLUDED)
#define GLOBALS_H_INCLUDED

#include <string>

#include "export.h"

namespace dtCore
{
   ///Set the list of data file paths
   DT_EXPORT void SetDataFilePathList( std::string pathList );
};

#endif  //GLOBALS_H_INCLUDED