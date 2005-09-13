#include "dtUtil/stringutils.h"

#include <algorithm>
#include <cstdio>                         // for sscanf
#include <xercesc/dom/DOMDocument.hpp>    // for xerces DOMDocument, DOMNode
#include <xercesc/dom/DOMNodeFilter.hpp>  // for xerces parameters
#include <xercesc/dom/DOMTreeWalker.hpp>  // for interface
#include <xercesc/dom/DOMAttr.hpp>        // for interface
#include <xercesc/util/XMLString.hpp>     // for interface
#include <xercesc/dom/DOMNamedNodeMap.hpp>

#include "dtUtil/log.h"
XERCES_CPP_NAMESPACE_USE

float dtUtil::ToFloat(const std::string& str)
{
   float tmp(0.0f);
   sscanf(str.c_str(), " %f", &tmp);
   return tmp;
}

/** \brief A Xerces utility that finds the string value for a specifically named attribute.
  * @param name the name of the attribute of interest.
  * @param node the node with attributes to be searched.
  * @param doc the current document that node exists within.
  */
std::string dtUtil::FindAttributeValueFor(const char* attributeName,
                                          DOMNamedNodeMap* namednodes)
{
   if( !namednodes )
   {
      LOG_INFO("Searching for attributes in an invalid DOMNamedNodeMap.")
      return "";
   }

   std::string value("");
   unsigned int n = namednodes->getLength();
   for(unsigned int i=0; i<n; i++)
   {
      DOMNode* namednode = namednodes->item(i);
      if( DOMAttr* attr = static_cast<DOMAttr*>( namednode ) )
      {
         char* attrname = XMLString::transcode( attr->getName() );

         if( std::string(attrname) == attributeName )
         {
            char* attribvalue = XMLString::transcode( attr->getValue() );
            value = attribvalue;
            XMLString::release( &attribvalue );
            break;
         }

         XMLString::release( &attrname );
      }
   }

   return value;
}


using namespace dtUtil;

AttributeSearch::AttributeSearch() : mKeys()
{
}

AttributeSearch::AttributeSearch(const SearchKeyVector& k) : mKeys(k)
{
}

AttributeSearch::~AttributeSearch()
{
}

AttributeSearch::ResultMap AttributeSearch::operator ()(const Attributes& attrs)
{
   ResultMap rmap;

   unsigned int n = attrs.getLength();
   for(unsigned int i=0; i<n; i++)
   {
      char* aname = XMLString::transcode( attrs.getLocalName(i) );

      // search all keys for lname
      SearchKeyVector::iterator iter = std::find( mKeys.begin(), mKeys.end(), aname );

      if( iter != mKeys.end() )
      {
         char* aval = XMLString::transcode( attrs.getValue(i) );
         rmap.insert( ResultMap::value_type( aname , aval ) );
         XMLString::release( &aval );
      }

      XMLString::release( &aname );
   }

   return rmap;
}

