#include "dtCore/id.h"

//#include <uuid/uuid.h>

using namespace dtCore;
using namespace std;
   
Id::Id()
{
   uuid_generate( mId );
}

Id::Id( const Id& id )
{
   uuid_copy( mId, id.mId );
}

Id::~Id()
{
}

bool Id::operator== ( Id id )
{
   if( uuid_compare( mId, id.mId ) == 0 )
      return true;
   else
      return false;
}

std::string Id::ToString()
{
   char str[37];
   uuid_unparse( mId, str );

   return string( str );
}
