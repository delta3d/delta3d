#include <iostream>

#include <CoreFoundation/CoreFoundation.h>

#include "dtCore/uniqueid.h"

using namespace dtCore;

std::string CFStringRefToStdString(CFStringRef stringRef)
{
   CFIndex  size =
   CFStringGetMaximumSizeForEncoding(CFStringGetLength(stringRef),
                                     kCFStringEncodingASCII);
   char* buf = new char[size + 1];
   std::string result;

   if(CFStringGetCString(stringRef, buf, size + 1, kCFStringEncodingASCII) != TRUE) {
      //Need to log, but really this should never happen.
      result = "Memory Allocation Error";
   } else {
      result = buf;
   }

   delete[] buf;
   return result;
}

UniqueId::UniqueId(bool createNewId)
{
   if (createNewId)
   {
      CFUUIDRef uuid;
      CFStringRef string;
      uuid = CFUUIDCreate( NULL );
      string = CFUUIDCreateString(NULL, uuid);

      mId = CFStringRefToStdString(string);

      if (string) CFRelease(string);
      CFRelease(uuid);
   }
}

//bool UniqueId::operator< ( const UniqueId& rhs ) const
//{
//   return mId < rhs.mId ;
//}
//
//bool UniqueId::operator> ( const UniqueId& rhs ) const
//{
//   return mId > rhs.mId ;
//}
