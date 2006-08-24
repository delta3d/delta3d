#include <prefix/dtutilprefix-src.h>
#include <dtUtil/xerceserrorhandler.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>

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

void XercesErrorHandler::warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e)
{
   char* msg = XMLString::transcode(e.getMessage());
   std::string line = ToString( e.getLineNumber() );
   LOG_WARNING("Xerces parsing warning occurred at line " + line + ", with message: " + std::string(msg) )
   XMLString::release( &msg );
}

void XercesErrorHandler::error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e)
{
   char* msg = XMLString::transcode(e.getMessage());
   std::string line = dtUtil::ToString( e.getLineNumber() );
   LOG_ERROR("Xerces error occurred while parsing at line, " + line + ", with message: " + std::string(msg) )
   XMLString::release( &msg );
}

void XercesErrorHandler::fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e)
{
   char* msg = XMLString::transcode(e.getMessage());
   std::string line = dtUtil::ToString( e.getLineNumber() );
   LOG_ERROR("Xerces fatal error occurred while parsing at line, " + line + ", with message: " + std::string(msg) )
   XMLString::release( &msg );
}

void XercesErrorHandler::resetErrors()
{
}
