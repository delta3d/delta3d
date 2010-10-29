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
  * David Guthrie
  * John K. Grant
  * William E. Johnson II
  * Chris Osborn
  */

#ifndef DELTA_STRING_UTILS_INC
#define DELTA_STRING_UTILS_INC

#include <dtUtil/export.h>

#include <sstream>         // for std::ostringstream, std::istringstream
#include <algorithm>
#include <locale>          // for std::locale, std::isspace
#include <string>
#include <vector>
#include <functional>
#include <osg/io_utils>
#include <dtUtil/deprecationmgr.h>

namespace dtUtil
{
   class IsSpace;

   template <class Pred=IsSpace>
   class StringTokenizer
   {
   public:
      //The predicate should evaluate to true when applied to a separator.
      static void tokenize(std::vector<std::string> &tokens,
                           const std::string &stringToParse,
                           const Pred &predFxn = Pred());
   };

   //The predicate should evaluate to true when applied to a separator.
   template <class Pred>
   inline void StringTokenizer<Pred>::tokenize(std::vector<std::string> &tokens,
                                               const std::string &stringToParse,
                                               const Pred &predFxn)
   {
      //First clear the results std::vector
      tokens.clear();
      std::string::const_iterator it = stringToParse.begin();
      std::string::const_iterator itTokenEnd = stringToParse.begin();
      while (it != stringToParse.end())
      {
         //Eat separators
         if (predFxn(*it))
         {
            it++;
         }
         else
         {
            //Find next token
            itTokenEnd = std::find_if(it, stringToParse.end(), predFxn);
            //Append token to result
            if (it < itTokenEnd)
            {
               tokens.push_back(std::string(it, itTokenEnd));
            }
            it = itTokenEnd;
         }
      }
   }

   /** \brief A functor which tests if a character is whitespace.
     * This "predicate" needed to have 'state', the locale member.
     */
   class DT_UTIL_EXPORT IsSpace : public std::unary_function<char, bool>
   {
   public:
      static const std::string DEFAULT_LOCALE_NAME;
      IsSpace(const std::locale& loc=std::locale(DEFAULT_LOCALE_NAME.c_str()) ) : mLocale(loc) {}

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
   DT_UTIL_EXPORT const std::string& Trim(std::string& toTrim);

   /**
    * Compares strings like strcmp or stricmp or strcasecmp.
    * @param one the first string to compare
    * @param two the string to compare one to.
    * @param caseSenstive optional parameter to specify case sensitivity, which defaults to on.
    * @return 0 for equal, negative for one less that two, and positive for one > two.
    */
   DT_UTIL_EXPORT int StrCompare(const std::string& one, const std::string& two, bool caseSensitive = true);

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
   std::string ToString(const T& t, int precision = -1)
   {
      std::ostringstream ss;
      if (precision > 0)
      {
         ss.precision(precision);
      }
      ss << t;
      return ss.str();
   }

   /// Converts a string to a 'float'
   ///\deprecated Favor ToType<float> instead
   float DT_UTIL_EXPORT ToFloat(const std::string& d);

   /// converts a std::string to a 'double'
   ///\deprecated Favor ToType<double> instead
   double DT_UTIL_EXPORT ToDouble(const std::string& str);

   /// converts a std::string to an 'unsigned int'
   ///\deprecated Favor ToType<unsigned int> instead
   unsigned int DT_UTIL_EXPORT ToUnsignedInt(const std::string& u);


   /// Converts a string to a specified type.
   /// @param the string to be converted to the specified template argument type.
   /// @return the type that you specify as the template argument.
   ///
   /// Typical use:
   /// @code
   /// std::string mystring("0");
   /// bool mybool = dtUtil::ToType<bool>( mystring );
   /// @endcode
   ///
   template<typename T>
   T ToType(const std::string& u)
   {
      T result;
      std::istringstream is;
      is.str(u);
      is >> result;
      return result;
   }

   /// Special exception for bool where things like "True", "TRUE", and "true" should be accepted.
   template<>
   bool DT_UTIL_EXPORT ToType<bool>(const std::string& u);

   bool DT_UTIL_EXPORT Match(const char* wildCards, const char* str);

   /// @return a string with text as an int value padded to the size specified.
   void DT_UTIL_EXPORT MakeIndexString(unsigned index, std::string& toFill, unsigned paddedLength = 4);

   /**
    * Reads the next token form the given string data.
    * This will also remove the token from the data string
    * and return you the token (with the open and close characters removed).
    * The beginning of the data string must always begin with
    * an opening character or this will cause problems.
    *
    * @param[in]  data  The string data.
    * @param[out] outToken  The first token from the string data.
    * @param[in]  openChar  The character that starts the token
    * @param[in]  closeChar  The character that ends the token
    * @return true if a token was found.
    */
   bool DT_UTIL_EXPORT TakeToken(std::string& data, std::string& outToken, char openChar, char closeChar);

}

#endif // DELTA_STRING_UTILS_INC
