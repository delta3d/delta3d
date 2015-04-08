/// Modifier : Pjotr from Virthualis 

#include <prefix/dtcoreprefix.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <Rpc.h>
#include <Rpcdce.h>

#include <dtCore/uniqueid.h>
#include <dtUtil/log.h>

using namespace dtCore;
   
UniqueId::UniqueId(bool createNewId)
{
   if (createNewId)
   {
      GUID guid;

      if( UuidCreate( &guid ) == RPC_S_OK )
      {
         unsigned char* guidChar;

         if( UuidToString( const_cast<UUID*>(&guid), &guidChar ) == RPC_S_OK )
         {
            mId = std::string( reinterpret_cast<const char*>(guidChar) );
            if(RpcStringFree(&guidChar) != RPC_S_OK)
            {
               LOG_ERROR("Could not free memory.");
            }
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
}
