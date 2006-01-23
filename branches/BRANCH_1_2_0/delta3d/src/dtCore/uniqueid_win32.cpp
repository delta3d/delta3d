#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <Rpc.h>
#include <Rpcdce.h>

#include <dtCore/uniqueid.h>
#include <dtUtil/log.h>

using namespace dtCore;
   
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
