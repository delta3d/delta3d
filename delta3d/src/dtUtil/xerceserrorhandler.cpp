#include <prefix/dtutilprefix.h>
#include <dtUtil/xerceserrorhandler.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>

#include <string>
#include <xercesc/util/XMLString.hpp>

using namespace dtUtil;
XERCES_CPP_NAMESPACE_USE

////////////////////////////////////////////////////////////////////////////////
XercesErrorHandler::XercesErrorHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
XercesErrorHandler::~XercesErrorHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
void XercesErrorHandler::warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e)
{
   char* msg = XMLString::transcode(e.getMessage());
   char* file = XMLString::transcode(e.getSystemId());

   std::string line = dtUtil::ToString(e.getLineNumber());
   std::string filename = dtUtil::ToString(file);
   LOG_WARNING("XML parsing warning in '" + filename + "'(" + line + "): " + std::string(msg));
   XMLString::release(&msg);
   XMLString::release(&file);
}

////////////////////////////////////////////////////////////////////////////////
void XercesErrorHandler::error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e)
{
   char* msg = XMLString::transcode(e.getMessage());
   char* file = XMLString::transcode(e.getSystemId());

   std::string line = dtUtil::ToString(e.getLineNumber());
   std::string filename = dtUtil::ToString(file);
   LOG_ERROR("XML parsing error in '" + filename + "'(" + line + "): " + std::string(msg));
   XMLString::release(&msg);
   XMLString::release(&file);
   throw(e);
}

////////////////////////////////////////////////////////////////////////////////
void XercesErrorHandler::fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e)
{
   char* msg = XMLString::transcode(e.getMessage());
   char* file = XMLString::transcode(e.getSystemId());

   std::string line = dtUtil::ToString(e.getLineNumber());
   std::string filename = dtUtil::ToString(file);
   LOG_ERROR("XML fatal parsing error in '" + filename + "'(" + line + "): " + std::string(msg));
   XMLString::release(&msg);
   XMLString::release(&file);
   throw(e);
}

////////////////////////////////////////////////////////////////////////////////
void XercesErrorHandler::resetErrors()
{
}
