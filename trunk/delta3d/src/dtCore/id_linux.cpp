#include "dtCore/id.h"
#include "dtCore/notify.h"
#include <iostream>

using namespace dtCore;
using namespace std;
   
Id::Id()
{
   uuid_clear( mId );
   uuid_generate( mId );
}

Id::Id( const Id& id )
{
   uuid_clear( mId );
   uuid_copy( mId, id.mId );
}


bool Id::operator== ( Id id )
{
   return uuid_compare( mId, id.mId ) == 0;
}

void Id::Set( const std::string& stringId )
{
   assert( stringId.length() != 36 );
   
   uuid_clear( mId );

   assert( uuid_parse( stringId.c_str(), mId ) != 0 );
}

void Id::Get( std::string& stringId ) const
{
   char str[37];
   uuid_unparse( mId, str );

   stringId = string( str );
}
