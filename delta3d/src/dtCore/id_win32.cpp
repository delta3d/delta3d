#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <Rpc.h>
#include <Rpcdce.h>

#include "dtCore/id.h"
#include "dtCore/notify.h"

using namespace dtCore;
using namespace std;
   
Id::Id()
{
   GUID guid;

   assert( UuidCreate( &guid ) == RPC_S_OK );

   unsigned char* guidChar;
   assert( UuidToString( const_cast<UUID*>(&guid), &guidChar ) == RPC_S_OK );

   mId = std::string( reinterpret_cast<const char*>(guidChar) );
}

bool Id::operator== ( const Id& rhs ) const
{
   GUID lhsGuid;
   GUID rhsuid;

   assert( UuidFromString( reinterpret_cast<unsigned char*>( const_cast<char*>( mId.c_str() ) ), &lhsGuid ) == RPC_S_OK );
   assert( UuidFromString( reinterpret_cast<unsigned char*>( const_cast<char*>( rhs.mId.c_str() ) ), &rhsuid ) == RPC_S_OK );

   RPC_STATUS status;
   int result = UuidCompare( &lhsGuid, &rhsuid, &status );
   assert( status == RPC_S_OK );

   return result == 0;
}

bool Id::operator< ( const Id& rhs ) const
{
   GUID lhsGuid;
   GUID rhsuid;

   assert( UuidFromString( reinterpret_cast<unsigned char*>( const_cast<char*>( mId.c_str() ) ), &lhsGuid ) == RPC_S_OK );
   assert( UuidFromString( reinterpret_cast<unsigned char*>( const_cast<char*>( rhs.mId.c_str() ) ), &rhsuid ) == RPC_S_OK );

   RPC_STATUS status;
   int result = UuidCompare( &lhsGuid, &rhsuid, &status );
   assert( status == RPC_S_OK );

   return result == -1;
}

bool Id::operator> ( const Id& rhs ) const
{
   GUID lhsGuid;
   GUID rhsuid;

   assert( UuidFromString( reinterpret_cast<unsigned char*>( const_cast<char*>( mId.c_str() ) ), &lhsGuid ) == RPC_S_OK );
   assert( UuidFromString( reinterpret_cast<unsigned char*>( const_cast<char*>( rhs.mId.c_str() ) ), &rhsuid ) == RPC_S_OK );

   RPC_STATUS status;
   int result = UuidCompare( &lhsGuid, &rhsuid, &status );
   assert( status == RPC_S_OK );

   return result == 1;
}
