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
   mId.Data1 = id.mId.Data1;
   mId.Data2 = id.mId.Data2;
   mId.Data3 = id.mId.Data3;
   mId.Data4[0] = id.mId.Data4[0];
   mId.Data4[1] = id.mId.Data4[1];
   mId.Data4[2] = id.mId.Data4[2];
   mId.Data4[3] = id.mId.Data4[3];
   mId.Data4[4] = id.mId.Data4[4];
   mId.Data4[5] = id.mId.Data4[5];
   mId.Data4[6] = id.mId.Data4[6];
   mId.Data4[7] = id.mId.Data4[7];
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
