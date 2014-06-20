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
 * John K. Grant
 */

#ifndef DELTA_XERCES_UTILS_INC
#define DELTA_XERCES_UTILS_INC

#include <dtUtil/warningdisable.h>
DT_DISABLE_WARNING_START_MSVC(4276)

#include <dtUtil/export.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <string.h>
#ifdef __cplusplus
}
#endif

#include <string>
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
        const char* c_str() const
        {
            if (mLocalForm == NULL)
            {
               return "";
            }

            return mLocalForm;
        }

        const std::string ToString() const { return std::string(c_str()); }

    private :
        char* mLocalForm;
        XMLStringConverter(const XMLStringConverter&) {}
        XMLStringConverter& operator=(const XMLStringConverter&) { return *this;}
    };

   /**
    * Utility methods for using strings, often for XML purposes.
    * This is a simple class that lets us do easy (though not terribly efficient)
    * trancoding of string data to XMLCh.
    */
   class StringToXMLConverter
   {
      public:
         StringToXMLConverter(const std::string& charData): mXmlForm(NULL)
         {
            mXmlForm = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(charData.c_str());
         }

         ~StringToXMLConverter()
         {
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&mXmlForm);
         }

         /**
          * @return the XMLCh string as a char*
          */
         const XMLCh* ToXmlString()
         {
            return mXmlForm;
         }
      private:
         XMLCh* mXmlForm;
         StringToXMLConverter(const StringToXMLConverter&) {}
         StringToXMLConverter& operator=(const StringToXMLConverter&) { return *this;}
   };


   /** A utility that finds the string value for a specifically named attribute when a DOM Node is available.
     * Needed for DOM Document traversal.
     * @param attributeName the name of the attribute of interest.
     * @param attrs the NamedNodeMap (attributes) to be searched.
     */
   std::string DT_UTIL_EXPORT FindAttributeValueFor(const char* attributeName,
                                               XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap* attrs);

   /** \brief Searches a Xerces XML Attribute list for names of interest.
     * A helper used when searching with a known string.  e.g. "mystring", "Type", "Name", or "Value".
     * For example:
     * \code
     *  void myHandler::startElement(... Attributes& attrs)
     *  {
     *    AttributeSearch attSearch;
     *    AttributeSearch::ResultMap results = attrSearch(attrs)
     *    AttributeSearch::ResultMap::iterator itr = results.find("Name");
     *    if (itr != results.end())
     *    {
     *       std::cout << "name is: " << itr->second << std::endl;
     *    }
     *  }
     *  \endcode
     */
   class DT_UTIL_EXPORT AttributeSearch
   {
   public:
      typedef std::map<std::string,std::string> ResultMap;

      AttributeSearch();

      ~AttributeSearch();

      ResultMap& GetAttributes();
      const ResultMap& GetAttributes() const;

      void SetValue(const std::string& attrName, const std::string& value);
      std::string GetValue(const std::string& attrName) const;

      bool HasAttribute(const std::string& attrName) const;

      std::string ToString() const;

      /** Process the Attributes and make an easier structure to use.
        * @return produces an associative container indexed on the attribute names.
        */
      ResultMap& operator ()(const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);

   private:
      ResultMap mAttrs;
   };

}

DT_DISABLE_WARNING_END

#endif // DELTA_XERCES_UTILS_INC
