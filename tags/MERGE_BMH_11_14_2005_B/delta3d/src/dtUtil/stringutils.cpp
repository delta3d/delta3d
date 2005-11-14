#include "dtUtil/stringutils.h"

#include <cstdio>                         // for sscanf

// scan '?' and '*'
static bool Scan(char*& wildCards, char*& str) 
{
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
         if (!dtUtil::Match(wildCards, str)) strCopy ++;
         wildCards = wdsCopy;
         str       = strCopy;
         while ((*wildCards != *str) && (*str != '\0')) str ++;
         wdsCopy = wildCards;
         strCopy = str;
      } while ((*str != '\0') ? !dtUtil::Match(wildCards, str) : (Yes = false) != false);

      if (*str == '\0' && *wildCards == '\0')
         return true;

      return Yes;
   }
}

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

bool dtUtil::Match(char* Wildcards, char* str)
{
   return WildMatch(Wildcards, str);    
}

float dtUtil::ToFloat(const std::string& str)
{
   float tmp(0.0f);
   sscanf(str.c_str(), " %f", &tmp);
   return tmp;
}

