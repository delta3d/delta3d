/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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

#ifndef DELTA_UNIQUEID
#define DELTA_UNIQUEID

// uniqueid.h: Cross-platform implementation of Universally Unique IDs
//
//////////////////////////////////////////////////////////////////////

#include <string>

#include <iosfwd>

#include <dtCore/export.h>

namespace dtUtil
{
   class DataStream;
}

namespace dtCore
{
   ///Conforms to OSF DCE 1.1
   class DT_CORE_EXPORT UniqueId
   {
      public:
         UniqueId();
         UniqueId(const UniqueId& toCopy) { mId = toCopy.mId; }

         explicit UniqueId(const std::string& stringId) : mId(stringId) {}
         virtual ~UniqueId() {}

         bool operator== ( const UniqueId& rhs ) const { return mId == rhs.mId; }
         bool operator!= ( const UniqueId& rhs ) const { return mId != rhs.mId; }
         bool operator< ( const UniqueId& rhs ) const  { return mId <  rhs.mId; }
         bool operator> ( const UniqueId& rhs ) const  { return mId >  rhs.mId; }

         const std::string& ToString() const;

         /**
          * The assignment operator is public so that unique id's can be changed if they are
          * member variables.  Use const to control when they are changed.
          */
         UniqueId& operator=(const UniqueId& rhs);

         /**
          * The assignment operator is public so that unique id's can be changed if they are
          * member variables.  Use const to control when they are changed.
          */
         UniqueId& operator=(const std::string& rhs);

    protected:
         std::string mId;
   };

   ////////////////////////////////////////////////////
   /////////////  Stream Operators        /////////////
   ////////////////////////////////////////////////////
   DT_CORE_EXPORT std::ostream& operator << (std::ostream& o, const UniqueId& id);

   DT_CORE_EXPORT std::istream& operator >> (std::istream& i, UniqueId& id);

   DT_CORE_EXPORT dtUtil::DataStream& operator << (dtUtil::DataStream& ds, const UniqueId& id);

   DT_CORE_EXPORT dtUtil::DataStream& operator >> (dtUtil::DataStream& ds, UniqueId& id);
}

#endif // DELTA_UNIQUEID
