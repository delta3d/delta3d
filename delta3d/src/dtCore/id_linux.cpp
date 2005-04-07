#include <iostream>

#include <uuid/uuid.h>

#include "dtCore/id.h"
#include "dtCore/notify.h"

using namespace dtCore;
   
Id::Id()
{
   uuid_t uuid;
   uuid_generate( uuid );

   char buffer[37];
   uuid_unparse( uuid, buffer );

   mId = std::string( buffer );
}

bool Id::operator== ( const Id& rhs ) const
{
   uuid_t lhsUuid;
   uuid_t rhsUuid;
   
   if( uuid_parse( mId.c_str(), lhsUuid ) != 0 ||
       uuid_parse( rhs.mId.c_str(), rhsUuid ) != 0 )
      Notify( ALWAYS, "Could not convert std::string to uuid_t." );

   return uuid_compare( lhsUuid, rhsUuid ) == 0;
}

bool Id::operator< ( const Id& rhs ) const
{
   uuid_t lhsUuid;
   uuid_t rhsUuid;

   if( uuid_parse( mId.c_str(), lhsUuid ) != 0 ||
       uuid_parse( rhs.mId.c_str(), rhsUuid ) != 0 )
      Notify( ALWAYS, "Could not convert std::string to uuid_t." );

   return uuid_compare( lhsUuid, rhsUuid ) < 0;
}

bool Id::operator> ( const Id& rhs ) const
{
   uuid_t lhsUuid;
   uuid_t rhsUuid;

   if( uuid_parse( mId.c_str(), lhsUuid ) != 0 ||
       uuid_parse( rhs.mId.c_str(), rhsUuid ) != 0 )
      Notify( ALWAYS, "Could not convert std::string to uuid_t." );

   return uuid_compare( lhsUuid, rhsUuid ) > 0;
}
