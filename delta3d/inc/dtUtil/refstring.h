#ifndef DELTA_REFSTRING
#define DELTA_REFSTRING

#include <string>

namespace dtUtil
{

   /**
    * A string wrapper that will "intern" all of the strings so that strings with the same
    * value will point to the same memory.  The strings are always only accessible as const, but
    * a new string may be assigned to the refstring  
    */
   class RefString
   {
      public:
         RefString(const std::string& value = "");
         ~RefString();

         operator const std::string&() const { return *mString; }
         void operator=(const std::string& value);
         const std::string* operator->() const { return mString; }
         std::string::value_type operator[](int index) const { return (*mString)[index]; }

         bool operator<(const dtUtil::RefString& toCompare) const 
         { return this->Get() < toCompare.Get(); }

         bool operator==(const dtUtil::RefString& toCompare)
         { return this->Get() == toCompare.Get(); }

         bool operator!=(const dtUtil::RefString& toCompare)
         { return !(*this == toCompare); }

         bool operator==(const std::string& toCompare)
         { return this->Get() == toCompare; }

         bool operator!=(const std::string& toCompare)
         { return !(*this == toCompare); }

         const std::string& Get() const { return *mString; }
      private:
         const std::string* mString;

         void Intern(const std::string& value);
   };

}

#endif /*REFSTRING_H_*/
