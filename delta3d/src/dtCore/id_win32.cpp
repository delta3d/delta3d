#include "dtCore/id.h"
#include "dtCore/notify.h"

#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace dtCore;
using namespace std;
   
Id::Id()
{
   assert( UuidCreate( &mId ) == RPC_S_OK );
}

Id::Id( const Id& id )
{
   //Data1 = ...

}

bool Id::operator== ( Id id )
{
   RPC_STATUS status;
   int result = UuidEqual( &mId, &id.mId, &status );
   assert( status == RPC_S_OK );

   return result == TRUE;
}

void Id::Set( const std::string& stringId )
{
   if( UuidFromString( reinterpret_cast<unsigned char*>(const_cast<char*>(stringId.c_str())), &mId ) != RPC_S_OK )
      Notify( WARN, "Could not convert std::string to dtCore::Id." );
}

void Id::Get( std::string& stringId ) const
{
   unsigned char* guidChar;
   if( UuidToString( const_cast<UUID*>(&mId), &guidChar ) != RPC_S_OK )
      Notify( WARN, "Could not convert dtCore::Id to std::string");

   stringId = std::string( reinterpret_cast<const char*>(guidChar) );
}
