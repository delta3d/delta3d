#include <prefix/dtutilprefix.h>
#include <dtUtil/serializer.h>
#include <dtUtil/stringutils.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace dtUtil;

DOMElement* Serializer::ToInt(const int value, const char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   // make the element
   XMLCh* NAME = XMLString::transcode(name);
   DOMElement* element = doc->createElement(NAME);
   XMLString::release(&NAME);

   // make the attribute
   XMLCh* TYPE_STRING = XMLString::transcode("int");
   XMLCh* VALUE = XMLString::transcode(dtUtil::ToString<int>(value).c_str());
   element->setAttribute(TYPE_STRING, VALUE);
   XMLString::release(&VALUE);
   XMLString::release(&TYPE_STRING);

   return element;
}

DOMElement* Serializer::ToFloat(const float value, const char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   // make the element
   XMLCh* NAME = XMLString::transcode(name);
   DOMElement* element = doc->createElement(NAME);
   XMLString::release(&NAME);

   // make the attribute
   XMLCh* TYPE_STRING = XMLString::transcode("float");
   XMLCh* VALUE = XMLString::transcode(dtUtil::ToString<float>(value).c_str());
   element->setAttribute(TYPE_STRING, VALUE);
   XMLString::release(&VALUE);
   XMLString::release(&TYPE_STRING);

   return element;
}

DOMElement* Serializer::ToDouble(const double value, const char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   // make the element
   XMLCh* NAME = XMLString::transcode(name);
   DOMElement* element = doc->createElement(NAME);
   XMLString::release(&NAME);

   // make the attribute
   XMLCh* TYPE_STRING = XMLString::transcode("double");
   XMLCh* VALUE = XMLString::transcode(dtUtil::ToString<double>(value).c_str());
   element->setAttribute(TYPE_STRING, VALUE);
   XMLString::release(&VALUE);
   XMLString::release(&TYPE_STRING);

   return element;
}

DOMElement* Serializer::ToBool(const bool value, const char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   // make the element
   XMLCh* NAME = XMLString::transcode(name);
   DOMElement* element = doc->createElement(NAME);
   XMLString::release(&NAME);

   // make the attribute
   XMLCh* TYPE_STRING = XMLString::transcode("bool");
   XMLCh* VALUE = XMLString::transcode(dtUtil::ToString<bool>(value).c_str());
   element->setAttribute(TYPE_STRING, VALUE);
   XMLString::release(&VALUE);
   XMLString::release(&TYPE_STRING);

   return element;
}

DOMElement* Serializer::ToString(const std::string& value, const char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   // make the element
   XMLCh* NAME = XMLString::transcode(name);
   DOMElement* element = doc->createElement(NAME);
   XMLString::release(&NAME);

   // make the attribute
   XMLCh* TYPE_STRING = XMLString::transcode("string");
   XMLCh* VALUE = XMLString::transcode(dtUtil::ToString<std::string>(value).c_str());
   element->setAttribute(TYPE_STRING, VALUE);
   XMLString::release(&VALUE);
   XMLString::release(&TYPE_STRING);

   return element;
}
