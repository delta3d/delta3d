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

#ifndef DELTA_ID
#define DELTA_ID

// id.h: Cross-platform implementation of Universally Unique IDs
//
//////////////////////////////////////////////////////////////////////


#include <string>

#include "dtCore/export.h"

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__)
#   include <uuid/uuid.h>
#else
#   include <Rpc.h>
#   include <Rpcdce.h>
#endif

namespace dtCore
{
   ///Conforms to OSF DCE 1.1
   class DT_EXPORT Id
   {
      public:

         Id();
         Id( const dtCore::Id& id );
         Id( const std::string& stringId ) { Set( stringId ); }
         virtual ~Id() {}

         bool operator== ( Id id );
      
         void Set( const std::string& stringId );
         void Get( std::string& stringId ) const;
      
      private:
      
         #if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__)
         uuid_t mId;
         #else
         GUID mId;
         #endif

   };
};


#endif // DELTA_ID
