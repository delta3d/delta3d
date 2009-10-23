#include <dtCore/uniqueid.h>
#include <dtUtil/datastream.h>
#include <iostream>

namespace dtCore
{
   ////////////////////////////////////////////////
   const std::string& UniqueId::ToString() const
   {
      return mId;
   }

   ////////////////////////////////////////////////
   UniqueId& UniqueId::operator=(const UniqueId& rhs)
   {
      if (this == &rhs)
      {
         return *this;
      }

      mId = rhs.mId;
      return *this;
   }

   ////////////////////////////////////////////////
   UniqueId& UniqueId::operator=(const std::string& rhs)
   {
      mId = rhs;
      return *this;
   }

   ////////////////////////////////////////////////
   std::ostream& operator << (std::ostream& o, const UniqueId& id)
   {
      o << id.ToString();
      return o;
   }

   ////////////////////////////////////////////////
   std::istream& operator >> (std::istream& i, UniqueId& id)
   {
      std::string value;
      i >> value;
      id = value;
      return i;
   }

   ////////////////////////////////////////////////
   dtUtil::DataStream& operator << (dtUtil::DataStream& ds, const UniqueId& id)
   {
      ds << id.ToString();
      return ds;
   }

   ////////////////////////////////////////////////
   dtUtil::DataStream& operator >> (dtUtil::DataStream& ds, UniqueId& id)
   {
      std::string value;
      ds >> value;
      id = value;
      return ds;
   }
}
