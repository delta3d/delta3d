#include <prefix/dtutilprefix.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/mswinmacros.h>

#include <cstdio>  // for sscanf, atoi
#include <sstream> // for sscanf, atoi
#include <iomanip> // for sscanf, atoi
#include <cmath>

namespace dtUtil
{

#ifdef DELTA_WIN32
   const std::string IsSpace::DEFAULT_LOCALE_NAME("english");
#elif defined (__APPLE__)
   const std::string IsSpace::DEFAULT_LOCALE_NAME("C");
#else
   const std::string IsSpace::DEFAULT_LOCALE_NAME("en_US.UTF-8");
#endif

#ifndef DELTA_WIN32
#ifndef stricmp
#define stricmp strcasecmp
#include <strings.h>
#endif
#endif

   /////////////////////////////////////////////////////////////////////////////
   const std::string& Trim(std::string& toTrim)
   {
      while (!toTrim.empty() && isspace(static_cast<unsigned char>(toTrim[toTrim.length()-1])))
      {
         toTrim.erase(toTrim.length()-1);
      }

      while (!toTrim.empty() && isspace(static_cast<unsigned char>(toTrim[0])))
      {
          toTrim.erase(0,1);
      }

      return toTrim;
   }

   /////////////////////////////////////////////////////////////////////////////
   int StrCompare(const std::string& one, const std::string& two, bool caseSensitive)
   {
      int result = 0;
      if (caseSensitive)
      {
         result = one.compare(two.c_str());
      }
      else
      {
         result = stricmp(one.c_str(), two.c_str());
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ToLowerCase(std::string& str)
   {
      for (unsigned i = 0 ; i < str.size(); ++i)
      {
         str[i] = tolower(str[i]);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ToUpperCase(std::string& str)
   {
      for (unsigned i = 0 ; i < str.size(); ++i)
      {
         str[i] = toupper(str[i]);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   static bool Scan(const char*& wildCards, const char*& str)
   {
      // scan '?' and '*'

      // remove the '?' and '*'
      for (wildCards++; *str != '\0' && (*wildCards == '?' || *wildCards == '*'); ++wildCards)
      {
         if (*wildCards == '?')
         {
            ++str;
         }
      }

      while (*wildCards == '*')
      {
         wildCards++;
      }

      // if str is empty and Wildcards has more characters or,
      // Wildcards is empty, return
      if (*str == '\0' && *wildCards != '\0')
      {
         return false;
      }
      else if (*str == '\0' && *wildCards == '\0')
      {
         return true;
         // else search substring
      }
      else
      {
         const char* wdsCopy = wildCards;
         const char* strCopy = str;
         bool  Yes     = true;
         do
         {
            if (!Match(wildCards, str)) { ++strCopy; }
            wildCards = wdsCopy;
            str       = strCopy;
            while ((*wildCards != *str) && (*str != '\0')) { ++str; }
            wdsCopy = wildCards;
            strCopy = str;
         } while ((*str != '\0') ? !Match(wildCards, str) : (Yes = false) != false);

         if (*str == '\0' && *wildCards == '\0')
         {
            return true;
         }

         return Yes;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   static bool WildMatch(const char* wildCards, const char* str)
   {
      bool Yes = true;

      // iterate and delete '?' and '*' one by one
      while (*wildCards != '\0' && Yes && *str != '\0')
      {
         if (*wildCards == '?')
         {
            ++str;
         }
         else if (*wildCards == '*')
         {
            Yes = Scan(wildCards, str);
            --wildCards;
         }
         else
         {
            Yes = (*wildCards == *str);
            ++str;
         }
         ++wildCards;
      }
      while (*wildCards == '*' && Yes) { ++wildCards; }

      return Yes && *str == '\0' && *wildCards == '\0';
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Match(const char* Wildcards, const char* str)
   {
      return WildMatch(Wildcards, str);
   }

  /////////////////////////////////////////////////////////////////////////////
   float ToFloat(const std::string& str)
   {
      return ToType<float>(str);
   }

   /////////////////////////////////////////////////////////////////////////////
   double ToDouble(const std::string& str)
   {
      return ToType<double>(str);
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int ToUnsignedInt(const std::string& str)
   {
      return ToType<unsigned int>(str);
   }

   /////////////////////////////////////////////////////////////////////////////
   template<>
   bool ToType<bool>(const std::string& u)
   {
      return u == "1" || u == "true" || u == "True" || u == "TRUE";
   }

   /////////////////////////////////////////////////////////////////////////////
   void MakeIndexString(unsigned index, std::string& toFill, unsigned paddedLength)
   {
      std::ostringstream ss;
      ss << std::setfill('0') << std::setw(paddedLength);
      ss << index;
      ss.str().swap(toFill);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool TakeToken(std::string& data, std::string& outToken, char openChar, char closeChar)
   {
      bool result = true;
      outToken.clear();

      if (data.empty())
      {
         result = false;
      }
      // If the first character in the data string is not the opening character,
      //  we will just assume the entire data string is the token.
      else if (data.c_str()[0] != openChar)
      {
         outToken = data;
         data.clear();
      }
      else
      {
         int depth = 0;
         int dataIndex = 0;
         while (!data.empty())
         {
            bool appendChar = true;

            // Opening characters increase the depth counter.
            if (data[dataIndex] == openChar)
            {
               depth++;

               if (depth == 1)
               {
                  appendChar = false;
               }
            }
            // Closing characters decrease the depth counter.
            else if (data[dataIndex] == closeChar)
            {
               depth--;

               if (depth == 0)
               {
                  appendChar = false;
               }
            }

            // All other characters are added to the return buffer.
            if (appendChar)
            {
               outToken.append(data.c_str(), 1);
            }

            // Remove the left most character from the data string.
            // Some versions of the stl can be picky so don't allow the subscript to go out of range.
            if (data.length() > 1)
            {
               data = &data[1];
            }
            else
            {
               // Don't leave leftovers like brackets
               data.clear();
            }

            // We are done once our depth returns to 0.
            if (depth <= 0)
            {
               break;
            }
         }
         //  we ran out of string, so we stopped reading, which means the data is invalid.
         if (depth > 0)
         {
            result = false;
         }
      }
      return result;
   }

  /////////////////////////////////////////////////////////////////////////////
   void FindAndReplace(std::string& modifiedString, const std::string& findWhat, const std::string& replaceWith)
   {
      if (!findWhat.empty())
      {
         size_t pos = 0;
         while((pos = modifiedString.find(findWhat, pos)) != std::string::npos)
         {
            modifiedString.replace(pos, findWhat.length(), replaceWith);
            pos += replaceWith.length();
         }
      }
      else
      {
         LOG_ERROR("Trying to find an empty string within a string.");
      }
   }
} // namespace dtUtil
