/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#ifndef DELTA_GLOBALS
#define DELTA_GLOBALS


#include <string>

#include "dtCore/export.h"

namespace dtCore
{
   ///Set the list of data file paths
   DT_EXPORT void SetDataFilePathList(const std::string& pathList );

   ///Get the list of data file paths
   DT_EXPORT std::string GetDataFilePathList();

   ///Get the Delta Data path list (equivalent to the DELTA_DATA environment)
   DT_EXPORT std::string GetDeltaDataPathList(void);

   ///Get the root path to Delta3D (equivalent to the DELTA_ROOT environment)
   DT_EXPORT std::string GetDeltaRootPath(void);

   ///Convienence method to get the supplied environment
   DT_EXPORT std::string GetEnvironment( std::string env);


};


#endif // DELTA_GLOBALS
