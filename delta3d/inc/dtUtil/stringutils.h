/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

/** \file dtUtil/stringutils.h
  * Utility methods for using strings, often for XML purposes.
  * \author John K. Grant
  */

#ifndef DELTA_STRING_UTILS_INC
#define DELTA_STRING_UTILS_INC

#include <dtCore/export.h>

#include <string>                       // for parameters
#include <sstream>                      // for std::ostringstream
#include <xercesc/util/XercesDefs.hpp>  // for xerces namespace macros
#include <xercesc/sax2/Attributes.hpp>  // for parameter

#include <vector>  // for member
#include <map>     // for return argument

#ifdef _MSC_VER
   #pragma warning( disable:4251 )  
#endif

XERCES_CPP_NAMESPACE_BEGIN
   class DOMNamedNodeMap;
XERCES_CPP_NAMESPACE_END

namespace dtUtil
{
   /** a utility function to convert a basic type into a string.
     * @param T the type being passed.
     * @param t the instance of the type to converted.
     */
   template<typename T>
   std::string ToString(const T& t)
   {
      std::ostringstream ss;
      ss << t;
      return ss.str();
   }

   /** Converts a string to a float.*/
   float DT_EXPORT ToFloat(const std::string& d);

   /** A utility that finds the string value for a specifically named attribute when a DOM Node is available.
     * @param name the name of the attribute of interest.
     * @param attrs the NamedNodeMap (attributes) to be searched.
     */
   std::string DT_EXPORT FindAttributeValueFor(const char* name,
                                               XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap* attrs);

   /** \brief Searches a Xerces XML Attribute list for names of interest.
     * Append the search keys with attribute names to search for named attributes.
     */
   class DT_EXPORT AttributeSearch
   {
   public:
      typedef std::vector<std::string> SearchKeyVector;
      typedef std::map<std::string,std::string> ResultMap;

      AttributeSearch();

      /// sets the search key vector
      AttributeSearch(const SearchKeyVector& k);

      ~AttributeSearch();

      void SetSearchKeys(const SearchKeyVector& k) { mKeys = k; }
      SearchKeyVector& GetSearchKeys() { return mKeys; }
      const SearchKeyVector& GetSearchKeys() const { return mKeys; }

      /** searches for all keys on each attribute.
        * @return ResultMap a map containing keys that were found, with attribute values as the second of the pair.
        */
      ResultMap operator ()(const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);

   private:
      SearchKeyVector mKeys;
   };

};

#endif // DELTA_STRING_UTILS_INC
