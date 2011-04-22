#include <prefix/dtutilprefix-src.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/macros.h>

#include <cstdio>        // for sscanf, atoi
#include <sstream>        // for sscanf, atoi
#include <iomanip>        // for sscanf, atoi
#include <cmath>

namespace dtUtil
{
   
   ////////////////////////////////////////////////////////////////////
   static bool Scan(char*& wildCards, char*& str) 
   {
      // scan '?' and '*'

      // remove the '?' and '*'
      for(wildCards++; *str != '\0' && (*wildCards == '?' || *wildCards == '*'); wildCards++)
         if (*wildCards == '?')
            str++;
   
      while ( *wildCards == '*')
         wildCards++;
   
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
         char* wdsCopy = wildCards;
         char* strCopy = str;
         bool  Yes     = true;
         do 
         {
            if (!Match(wildCards, str)) strCopy ++;
            wildCards = wdsCopy;
            str       = strCopy;
            while ((*wildCards != *str) && (*str != '\0')) str ++;
            wdsCopy = wildCards;
            strCopy = str;
         } while ((*str != '\0') ? !Match(wildCards, str) : (Yes = false) != false);
   
         if (*str == '\0' && *wildCards == '\0')
            return true;
   
         return Yes;
      }
   }
   
   ////////////////////////////////////////////////////////////////////
   static bool WildMatch(char *wildCards, char *str)
   {
      bool Yes = true;
   
      //iterate and delete '?' and '*' one by one
      while(*wildCards != '\0' && Yes && *str != '\0') 
      {
         if (*wildCards == '?')
         {
            str ++;
         } 
         else if (*wildCards == '*')
         {
            Yes = Scan(wildCards, str);
            wildCards --;
         } 
         else 
         {
            Yes = (*wildCards == *str);
            str ++;
         }
         wildCards ++;
      }
      while (*wildCards == '*' && Yes)  wildCards ++;
   
      return Yes && *str == '\0' && *wildCards == '\0';
   }
   
   ////////////////////////////////////////////////////////////////////
   bool Match(char* Wildcards, char* str)
   {
      return WildMatch(Wildcards, str);    
   }
   
   ////////////////////////////////////////////////////////////////////
   float ToFloat(const std::string& str)
   {
      return ToType<float>(str);
   }
   
   ////////////////////////////////////////////////////////////////////
   double ToDouble(const std::string& str)
   {
      return ToType<double>(str);
   }
   
   ////////////////////////////////////////////////////////////////////
   unsigned int ToUnsignedInt(const std::string& str)
   {
      return ToType<unsigned int>(str);
   }
   
   ////////////////////////////////////////////////////////////////////
   template<>
   bool ToType<bool>(const std::string& u)
   {
      return (u == "1" || u == "true" || u == "True" || u == "TRUE");
   }

   ////////////////////////////////////////////////////////////////////
   void MakeIndexString(unsigned index, std::string& toFill, unsigned paddedLength)
   {
      std::ostringstream ss;
      ss << std::setfill('0') << std::setw(paddedLength);
      ss << index;
      ss.str().swap(toFill);
   }
}