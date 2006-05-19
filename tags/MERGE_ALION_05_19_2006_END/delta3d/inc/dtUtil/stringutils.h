/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
*/

/** @file
  * Utility methods for using strings.
  * @author David Guthrie
  * @author John K. Grant
  * @author William E. Johnson II
  * @author Chris Osborn
  */

#ifndef DELTA_STRING_UTILS_INC
#define DELTA_STRING_UTILS_INC

#include <dtUtil/export.h>

#include <sstream>         // for std::ostringstream
#include <algorithm>
#include <locale>          // for std::locale, std::isspace
#include <string>
#include <vector>
#include <functional>
#include <osg/io_utils>

namespace dtUtil
{
   class IsSpace;

   template <class Pred=IsSpace>
   class StringTokenizer
   {
   public:
      //The predicate should evaluate to true when applied to a separator.
      static void tokenize(std::vector<std::string>& roResult, std::string const& rostr,
                           Pred const& roPred=Pred());
   };

   //The predicate should evaluate to true when applied to a separator.
   template <class Pred>
   inline void StringTokenizer<Pred>::tokenize(std::vector<std::string>& roResult,
                                             std::string const& rostr, Pred const& roPred)
   {
      //First clear the results std::vector
      roResult.clear();
      std::string::const_iterator it = rostr.begin();
      std::string::const_iterator itTokenEnd = rostr.begin();
      while(it != rostr.end())
      {
         //Eat seperators
         if(roPred(*it))
         {
            it++;
         }
         else
         {
            //Find next token
            itTokenEnd = std::find_if(it, rostr.end(), roPred);
            //Append token to result
            if(it < itTokenEnd)
            {
               roResult.push_back(std::string(it, itTokenEnd));
            }
            it = itTokenEnd;
         }
      }
   }

   /** \brief A functor which tests if a character is whitespace.
     * This "predicate" needed to have 'state', the locale member.
     */
   class IsSpace : public std::unary_function<char, bool>
   {
      public:
         #if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
            IsSpace(const std::locale& loc=std::locale("english") ) : mLocale(loc) {}
         #elif defined (__APPLE__)
            IsSpace(const std::locale& loc=std::locale("C") ) : mLocale(loc) {}
         #else
            IsSpace(const std::locale& loc=std::locale("en_US.UTF-8") ) : mLocale(loc) {}
         #endif

         const std::locale& GetLocale() const { return mLocale; }
         bool operator()(char c) const { return std::isspace(c,mLocale); }

   private:
      std::locale mLocale;
   };

   /**
    * Determines if the current character is a forward slash.
    */
   class IsSlash : public std::unary_function<char, bool>
   {
       public:
           bool operator()(char c) const { return c == '/'; }
   };

   /**
    * Generic string delimeter check function class.  Based on the character
    * passed to the constructor, this class will check for that character.
    */
   class IsDelimeter : public std::unary_function<char,bool>
   {
      public:
         IsDelimeter(char delim) : mDelimeter(delim) { }
         bool operator()(char c) const { return c == mDelimeter; }
      private:
         char mDelimeter;
   };

   /**
   * Trims whitespace off the front and end of a string
   * @param toTrim the string to trim.
   */
   inline void trim(std::string& toTrim)
   {
      if(toTrim.empty())
         return;

      for (std::string::iterator i = toTrim.begin(); i != toTrim.end();)
      {
         if (isspace(*i))
               i = toTrim.erase(i);
         else
               break;
      }

      for (unsigned int i = (toTrim.size() - 1); i >= 0; --i)
      {
         if (isspace(toTrim[i]))
               //we can just erase from the end because
               //it will shorted the part of the string already covered by the loop.
               toTrim.erase(i);
         else
               break;
      }
   }

   /**
    * A templated function for taking any of the osg vector types and reading the data from a string.
    * If the string is empty or "NULL" it will set the vector to all 0s. It expects the data to be the proper number
    * floating point values.  The function will fail if there are not enough values.
    *
    * @param value the string data.
    * @param vec the vector to fill.
    * @param size the length of the vector since the osg types have no way to query that.
    * @param numberPrecision This value indicates how much precision the numbers will
    *    contain when read from the string.  (setprecision on std::istream)
    * @return true if reading the data was successful or false if not.
    */
   template<class VecType>
   bool ParseVec(const std::string& value, VecType& vec, unsigned size,
      unsigned numberPrecision=16)
   {
      bool result = true;
      unsigned int i;

      if (value.empty() || value == "NULL")
      {
         for (i = 0; i < size; ++i)
         {
            vec[i] = 0.0;
         }
      }
      else
      {
         std::istringstream iss(value);
         iss.precision(numberPrecision);
         for (i = 0; i < size && !iss.eof(); ++i)
         {
            iss >> vec[i];
         }

         //did we run out of data?
         if (i < size)
            result = false;
      }

      return result;
   }

   /**
    * A utility function to convert a basic type into a string. Use
    * template argument T for the type you'd like to convert.
    *
    * @param t the instance of the type to converted.
    */
   template<typename T>
   std::string ToString(const T& t)
   {
      std::ostringstream ss;
      ss << t;
      return ss.str();
   }

   // Gets the current timezone
   long DT_UTIL_EXPORT GetTimeZone(tm &timeParts);

   /**
    * @param time the time in seconds since the epoch to convert.
    * @return the given time as a UTC formatted string.
    */
   const std::string DT_UTIL_EXPORT TimeAsUTC(time_t time);

   /** Converts a string to a float.*/
   float DT_UTIL_EXPORT ToFloat(const std::string& d);

   double DT_UTIL_EXPORT ToDouble(const std::string& str);

   unsigned int DT_UTIL_EXPORT ToUnsignedInt(const std::string& u);

   bool DT_UTIL_EXPORT Match(char* wildCards, char* str);
};

#endif // DELTA_STRING_UTILS_INC
