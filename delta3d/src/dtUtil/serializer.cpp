#include "dtUtil/serializer.h"
#include "dtUtil/stringutils.h"

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace dtUtil;

DOMElement* Serializer::ToFloat(float value, char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   // make the element
   XMLCh* NAME = XMLString::transcode( name );
   DOMElement* element = doc->createElement( NAME );
   XMLString::release( &NAME );

   // make the attribute
   XMLCh* FLOAT_STRING = XMLString::transcode( "float" );
   XMLCh* VALUE = XMLString::transcode( dtUtil::ToString<float>(value).c_str() );
   element->setAttribute( FLOAT_STRING, VALUE );
   XMLString::release( &FLOAT_STRING );
   XMLString::release( &VALUE );

   return element;
}

DOMElement* Serializer::ToBool(bool state, char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   XMLCh* NAME = XMLString::transcode( name );
   DOMElement* element = doc->createElement( NAME );
   XMLString::release( &NAME );

   XMLCh* BOOL_STRING = XMLString::transcode( "bool" );
   XMLCh* VALUE = XMLString::transcode( dtUtil::ToString<bool>(state).c_str() );
   element->setAttribute( BOOL_STRING, VALUE );
   XMLString::release( &BOOL_STRING );
   XMLString::release( &VALUE );
   return element;
}
