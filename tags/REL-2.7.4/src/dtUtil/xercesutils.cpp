#include <prefix/dtutilprefix.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

#include <algorithm>
#include <sstream>

#include <xercesc/dom/DOMDocument.hpp>   // for xerces DOMDocument, DOMNode
#include <xercesc/dom/DOMNodeFilter.hpp> // for xerces parameters
#include <xercesc/dom/DOMTreeWalker.hpp> // for interface
#include <xercesc/dom/DOMAttr.hpp>       // for interface
#include <xercesc/util/XMLString.hpp>    // for interface

XERCES_CPP_NAMESPACE_USE

/**
 * \brief A Xerces utility that finds the string value for a specifically named attribute.
 * @param attributeName the name of the attribute of interest.
 * @param attrs the node with attributes to be searched.
 */
std::string dtUtil::FindAttributeValueFor(const char* attributeName,
                                          XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap* attrs)
{
   if (!attrs)
   {
      LOG_INFO("Searching for attributes in an invalid DOMNamedNodeMap.")
      return "";
   }

   std::string value("");
   unsigned int n = attrs->getLength();
   for (unsigned int i = 0; i < n; ++i)
   {
      DOMNode* namednode = attrs->item(i);
      if (DOMAttr* attr = static_cast<DOMAttr*>(namednode))
      {
         char* attrname = XMLString::transcode(attr->getName());

         if (std::string(attrname) == attributeName)
         {
            char* attribvalue = XMLString::transcode(attr->getValue());
            value = attribvalue;
            XMLString::release(&attribvalue);
            XMLString::release(&attrname);
            break;
         }

         XMLString::release(&attrname);
      }
   }

   return value;
}

using namespace dtUtil;

AttributeSearch::AttributeSearch()
{
}

AttributeSearch::~AttributeSearch()
{
}

AttributeSearch::ResultMap& AttributeSearch::GetAttributes()
{
   return mAttrs;
}

const AttributeSearch::ResultMap& AttributeSearch::GetAttributes() const
{
   return mAttrs;
}

void AttributeSearch::SetValue(const std::string& attrName, const std::string& value)
{
   mAttrs[attrName] = value;
}

std::string AttributeSearch::GetValue(const std::string& attrName) const
{
   ResultMap::const_iterator foundIter = mAttrs.find(attrName);
   return foundIter == mAttrs.end() ? "" : foundIter->second;
}

bool AttributeSearch::HasAttribute(const std::string& attrName) const
{
   ResultMap::const_iterator foundIter = mAttrs.find(attrName);
   return foundIter != mAttrs.end();
}

std::string AttributeSearch::ToString() const
{
   std::ostringstream oss;
   ResultMap::const_iterator curIter = mAttrs.begin();
   ResultMap::const_iterator endIter = mAttrs.end();
   for (; curIter != endIter;)
   {
      // Add the attribute name and value such as name="value"
      oss << curIter->first << "=\"" << curIter->second << "\"";

      // Check if there is a next attribute to write.
      ++curIter;
      if(curIter != endIter)
      {
         // ...then add a space before the next one.
         oss << " ";
      }
   }

   return oss.str();
}

AttributeSearch::ResultMap& AttributeSearch::operator ()(const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
{
   mAttrs.clear();

   unsigned int n = attrs.getLength();
   for (unsigned int i = 0; i < n; ++i)
   {
      char* aname = XMLString::transcode(attrs.getLocalName(i));
      char* aval = XMLString::transcode(attrs.getValue(i));

      mAttrs.insert( ResultMap::value_type(aname, aval));

      XMLString::release(&aval);
      XMLString::release(&aname);
   }

   return mAttrs;
}
