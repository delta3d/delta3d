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
         /// @return the number of shared strings.
         static size_t GetSharedStringCount();
         
         RefString(const std::string& value = "");
         RefString(const char* value);
         RefString(const RefString& toCopy);
         ~RefString();

         operator const std::string&() const { return *mString; }
         dtUtil::RefString& operator=(const std::string& value);
         dtUtil::RefString& operator=(const dtUtil::RefString& value);

         RefString operator+(const std::string& string) const;
         RefString operator+(const RefString& refString) const;
         const std::string* operator->() const { return mString; }
         std::string::value_type operator[](int index) const { return (*mString)[index]; }

         bool operator<(const dtUtil::RefString& toCompare) const 
         { return this->Get() < toCompare.Get(); }

         bool operator==(const dtUtil::RefString& toCompare) const
         { return this->Get() == toCompare.Get(); }

         bool operator!=(const dtUtil::RefString& toCompare) const
         { return !(*this == toCompare); }

         bool operator==(const std::string& toCompare) const
         { return this->Get() == toCompare; }

         bool operator!=(const std::string& toCompare) const
         { return !(*this == toCompare); }

         const std::string& Get() const { return *mString; }
      private:
         const std::string* mString;

         void Intern(const std::string& value);
   };

   inline bool operator==(const std::string& s1, const RefString& s2)
   {
      return s2 == s1;
   }

   inline bool operator!=(const std::string& s1, const RefString& s2)
   {
      return s2 != s1;
   }
}

#endif /*REFSTRING_H_*/
