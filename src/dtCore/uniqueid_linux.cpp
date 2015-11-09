#include <iostream>

#include <uuid/uuid.h>

#include "dtCore/uniqueid.h"

using namespace dtCore;

UniqueId::UniqueId(bool createNewId)
{
   if (createNewId)
   {
      uuid_t uuid;
      uuid_generate( uuid );

      char buffer[37];
      uuid_unparse( uuid, buffer );

      mId = std::string( buffer );
   }
}

//bool UniqueId::operator< ( const UniqueId& rhs ) const
//{
//   uuid_t lhsUuid;
//   uuid_t rhsUuid;
//
//   if( uuid_parse( mId.c_str(), lhsUuid ) == 0 &&
//       uuid_parse( rhs.mId.c_str(), rhsUuid ) == 0 )
//   {
//      return uuid_compare( lhsUuid, rhsUuid ) < 0;
//   }
//   else
//   {
//      Notify( WARN, "Could not convert std::string to UniqueId." );
//      return mId < rhs.mId;
//   }
//}
//
//bool UniqueId::operator> ( const UniqueId& rhs ) const
//{
//   uuid_t lhsUuid;
//   uuid_t rhsUuid;
//
//   if( uuid_parse( mId.c_str(), lhsUuid ) == 0 &&
//       uuid_parse( rhs.mId.c_str(), rhsUuid ) == 0 )
//   {
//      return uuid_compare( lhsUuid, rhsUuid ) > 0;
//   }
//   else
//   {
//      Notify( WARN, "Could not convert std::string to UniqueId." );
//      return mId > rhs.mId;
//   }
//}
