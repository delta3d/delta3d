#include <dtUtil/refstring.h>
#include <set>

namespace dtUtil
{
   static std::set<std::string> StringSet;

   RefString::RefString(const std::string& value)
   {
      Intern(value);
   }

   RefString::~RefString()
   {
      
   }

   void RefString::operator=(const std::string& value)
   {
      Intern(value);
   }

   void RefString::Intern(const std::string& value)
   {
      //One can only insert a string once, but it will still return the iterator.
      mString = &(*StringSet.insert(value).first);
   }

}
