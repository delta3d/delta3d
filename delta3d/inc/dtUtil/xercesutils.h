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
 * @author John K. Grant
 */

#ifndef DELTA_XERCES_UTILS_INC
#define DELTA_XERCES_UTILS_INC

#ifdef _MSC_VER
   #pragma warning( disable:4251 )  
#endif

#include <dtUtil/export.h>

#include <string>
#include <vector>                       // for member
#include <map>                          // for return argument
#include <xercesc/util/XercesDefs.hpp>  // for xerces namespace macros
#include <xercesc/sax2/Attributes.hpp>  // for parameter
#include <xercesc/dom/DOM.hpp>          // for DOM support
#include <xercesc/util/XMLString.hpp>   // for XML String

namespace dtUtil
{
    /**
     * Utility methods for using strings, often for XML purposes.
     * This is a simple class that lets us do easy (though not terribly efficient)
     * trancoding of XMLCh data to local code page for display.  This code was take from
     * the xerces-c 2.6 samples \n It's main reason for existing is to allow short and quick 
     * translations for printing out debugging info.
     */
    class XMLStringConverter
    {
    public :
        XMLStringConverter(const XMLCh* const charData): mLocalForm(NULL)
        {
            if (charData != NULL)
            {
                mLocalForm = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(charData);
            }
        }

        ~XMLStringConverter()
        {
            if (mLocalForm != NULL)
                XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&mLocalForm);
        }

        /**
         * @return the XMLCh string as a char*
         */
        const char* c_str()
        {
            if (mLocalForm == NULL)
               return "";

            return mLocalForm;
        }

        const std::string ToString() { return std::string(c_str()); }
    private :
        char* mLocalForm;
        XMLStringConverter(const XMLStringConverter&) {}
        XMLStringConverter& operator=(const XMLStringConverter&) { return *this;}
    };


   /** A utility that finds the string value for a specifically named attribute when a DOM Node is available.
     * Needed for DOM Document traversal.
     * @param attributeName the name of the attribute of interest.
     * @param attrs the NamedNodeMap (attributes) to be searched.
     */
   std::string DT_UTIL_EXPORT FindAttributeValueFor(const char* attributeName,
                                               XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap* attrs);

   /** \brief Searches a Xerces XML Attribute list for names of interest.
     * A helper used when searching with a known string.  e.g. "mystring", "Type", "Name", or "Value"
     */
   class DT_UTIL_EXPORT AttributeSearch
   {
   public:
      ///\deprecated not needed anymore since this class now searches all Attributes by design.
      typedef std::vector<std::string> SearchKeyVector;

      typedef std::map<std::string,std::string> ResultMap;

      AttributeSearch();

      /// sets the search key vector
      AttributeSearch(const SearchKeyVector& k);

      ~AttributeSearch();

      /// \deprecated functor searches all Attributes now.
      void SetSearchKeys(const SearchKeyVector& k) { mKeys = k; }

      /// \deprecated functor searches all Attributes now.
      SearchKeyVector& GetSearchKeys() { return mKeys; }

      /// \deprecated functor searches all Attributes now.
      const SearchKeyVector& GetSearchKeys() const { return mKeys; }

      /** searches for all keys on each attribute.
        * @return ResultMap a map containing keys that were found, with attribute values as the second of the pair.
        */
      ResultMap operator ()(const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);

   private:
      SearchKeyVector mKeys;
   };
}

#endif // DELTA_XERCES_UTILS_INC
