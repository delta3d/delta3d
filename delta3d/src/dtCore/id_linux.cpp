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
   uuid_unparse( mId, buffer );

   mId = std::string( buffer );
}

/*
Id::Id( const Id& id )
{
   uuid_clear( mId );
   uuid_copy( mId, id.mId );

   char str[37];
   uuid_unparse( mId, str );

   mId = std::string( str );
}
*/

bool Id::operator== ( const Id& rhs ) const
{
   uuid_t lhsUuid;
   uuid_t rhsUid;

   assert( uuid_parse( mId.c_str(), lhsUuid ) != 0 );
   assert( uuid_parse( ths.mId.c_str(), rhsUid ) != 0 );

   return uuid_compare( lhsUuid, rhsUid ) == 0;
}

bool Id::operator< ( const Id& rhs ) const
{
   uuid_t lhsUuid;
   uuid_t rhsUid;

   assert( uuid_parse( mId.c_str(), lhsUuid ) != 0 );
   assert( uuid_parse( ths.mId.c_str(), rhsUid ) != 0 );

   return uuid_compare( lhsUuid, rhsUid ) == -1;
}

bool Id::operator> ( const Id& rhs ) const
{
   uuid_t lhsUuid;
   uuid_t rhsUid;

   assert( uuid_parse( mId.c_str(), lhsUuid ) != 0 );
   assert( uuid_parse( ths.mId.c_str(), rhsUid ) != 0 );

   return uuid_compare( lhsUuid, rhsUid ) == 1;
}
