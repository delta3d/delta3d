#include "dtUtil/xerceserrorhandler.h"
#include "dtUtil/stringutils.h"
#include "dtUtil/log.h"

#include <string>
#include <xercesc/util/XMLString.hpp>

using namespace dtUtil;
XERCES_CPP_NAMESPACE_USE

XercesErrorHandler::XercesErrorHandler()
{
}

XercesErrorHandler::~XercesErrorHandler()
{
}

void XercesErrorHandler::warning(const SAXParseException& e)
{
   char* msg = XMLString::transcode(e.getMessage());
   std::string line = ToString<XMLSSize_t>( e.getLineNumber() );
   LOG_WARNING("Xerces parsing warning occurred at line " + line + ", with message: " + std::string(msg) )
   XMLString::release( &msg );
}

void XercesErrorHandler::error(const SAXParseException& e)
{
   char* msg = XMLString::transcode(e.getMessage());
   std::string line = ToString<XMLSSize_t>(e.getLineNumber() );
   LOG_ERROR("Xerces parsing error occurred at line " + line + ", with message: " + std::string(msg) )
   XMLString::release( &msg );
}

void XercesErrorHandler::fatalError(const SAXParseException& e)
{
   char* msg = XMLString::transcode(e.getMessage());
   LOG_ERROR("Xerces parsing fatal error occurred with message: " + std::string(msg) )
   XMLString::release( &msg );
}

void XercesErrorHandler::resetErrors()
{
}
