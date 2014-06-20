/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* Matthew W. Campbell
*/
#ifndef DELTA_TERRAINDATATYPE
#define DELTA_TERRAINDATATYPE

#include "dtUtil/enumeration.h"
#include "dtTerrain/terrain_export.h"

namespace dtTerrain 
{
 
   /**
    * This class defines the various types of terrain data supported by default
    * in Delta3D.  In order to support your own formats, you must extend this
    * enumeration and return the appropriate type in your data readers and renderers.
    */
   class DT_TERRAIN_EXPORT TerrainDataType : public dtUtil::Enumeration 
   {
      DECLARE_ENUM(TerrainDataType);
        
      public:
   
         ///DTED levels 0,1,2
         static const TerrainDataType DTED;
        
      protected:
      
         ///Simple constructor for enumerations.
         TerrainDataType(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
        
    };   
    
}

#endif
