#include "dtCore/id.h"

#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>

IMPLEMENT_MANAGEMENT_LAYER(Id)

using namespace dtCore;
using namespace std;
   
Id::Id()
{
   assert( CoCreateGuid( &mId ) == S_OK );
}

Id::Id( const Id& id )
{
   
}

virtual Id::~Id()
{
}

bool Id::operator== ( Id id )
{
   if( mId.Data1 == id.Data1 &&
       mId.Data2 == id.Data2 &&
       mId.Data3 == id.Data3 &&
       mId.Data4[0] == id.Data4[0] &&
       mId.Data4[1] == id.Data4[1] &&
       mId.Data4[2] == id.Data4[2] &&
       mId.Data4[3] == id.Data4[3] &&
       mId.Data4[4] == id.Data4[4] &&
       mId.Data4[5] == id.Data4[5] &&
       mId.Data4[6] == id.Data4[6] &&
       mId.Data4[7] == id.Data4[7] )
      return true;
   else
      return false;
}
      
std::string Id::ToString()
{
   //add in dashes, << "-" ???
   
   ostringstream id;
   id << setw( 8 ) << setfill( '0' ) << hex << mId.Data1 << setw( 4 ) << setfill( '0' ) << hex << mId.Data2 << setw( 4 ) << setfill( '0' ) << hex << mId.Data3;

   for ( size_t i = 0; i < 8; ++i )
      id << setw( 2 ) << setfill( '0' ) << hex << (unsigned short) mId.Data4[ i ];

   id << ends;

   return id.str();
}
