#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <Rpc.h>
#include <Rpcdce.h>

#include "dtCore/uniqueid.h"
#include <dtUtil/log.h>

using namespace dtCore;
using namespace std;
   
UniqueId::UniqueId()
{
   GUID guid;
   
   if( UuidCreate( &guid ) == RPC_S_OK )
   {
      unsigned char* guidChar;

      if( UuidToString( const_cast<UUID*>(&guid), &guidChar ) == RPC_S_OK )
      {
         mId = std::string( reinterpret_cast<const char*>(guidChar) );
      }
      else
      {
         LOG_WARNING("Could not convert UniqueId to std::string." );
      }
   }
   else
   {
      LOG_WARNING("Could not generate UniqueId." );
   }
}

//bool UniqueId::operator< ( const UniqueId& rhs ) const
//{
//   GUID lhsGuid;
//   GUID rhsuid;
//
//   if(   UuidFromString( reinterpret_cast<unsigned char*>( const_cast<char*>( mId.c_str() ) ), &lhsGuid ) == RPC_S_OK &&
//         UuidFromString( reinterpret_cast<unsigned char*>( const_cast<char*>( rhs.mId.c_str() ) ), &rhsuid ) == RPC_S_OK )
//   {
//      RPC_STATUS status;
//      return UuidCompare( &lhsGuid, &rhsuid, &status ) == -1;
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
//   GUID lhsGuid;
//   GUID rhsuid;
//
//   if(   UuidFromString( reinterpret_cast<unsigned char*>( const_cast<char*>( mId.c_str() ) ), &lhsGuid ) == RPC_S_OK &&
//         UuidFromString( reinterpret_cast<unsigned char*>( const_cast<char*>( rhs.mId.c_str() ) ), &rhsuid ) == RPC_S_OK )
//   {
//      RPC_STATUS status;
//      return UuidCompare( &lhsGuid, &rhsuid, &status ) == 1;
//   }
//   else
//   {
//      Notify( WARN, "Could not convert std::string to UniqueId." );
//      return mId > rhs.mId;
//   }
//}
