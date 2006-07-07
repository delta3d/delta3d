#include "dtUtil/stringutils.h"

#include <cstdio>        // for sscanf, atoi

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #define snprintf _snprintf
#endif

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

double dtUtil::ToDouble(const std::string& str)
{
   double tmp(0.0);
   sscanf(str.c_str(), " %lf", &tmp);
   return tmp;
}

unsigned int dtUtil::ToUnsignedInt(const std::string& str)
{
   unsigned int tmp(0);
   sscanf(str.c_str(), " %u", &tmp);
   return tmp;
}

long dtUtil::GetTimeZone(tm &timeParts)
{
   #ifdef __APPLE__
      return timeParts.tm_gmtoff;
   #else
      tzset();
      return timezone * -1;
   #endif
}

const std::string dtUtil::TimeAsUTC(time_t time)
{
   char data[28];
   std::string result;
   struct tm timeParts;
   struct tm* tp = localtime(&time);
   timeParts = *tp;

   long tz = GetTimeZone(timeParts);

   int tzHour = (int)floor(fabs((double)tz / 3600));
   int tzMin = (int)floor(fabs((double)tz / 60) - (60 * tzHour));

   //since the function of getting hour does fabs,
   //this needs to check the sign of tz.
   if (tz < 0)
      tzHour *= -1;

   snprintf(data, 28, "%04d-%02d-%02dT%02d:%02d:%02d.0%+03d:%02d",
      timeParts.tm_year + 1900, timeParts.tm_mon + 1, timeParts.tm_mday,
      timeParts.tm_hour, timeParts.tm_min, timeParts.tm_sec,
      tzHour, tzMin);

   result.assign(data);
   return result;
}
