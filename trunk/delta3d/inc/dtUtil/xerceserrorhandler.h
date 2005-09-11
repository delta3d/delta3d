#ifndef DTUTIL_XERCES_ERROR_HANDLER_INC
#define DTUTIL_XERCES_ERROR_HANDLER_INC

#include <dtCore/export.h>

#include <xercesc/sax/ErrorHandler.hpp>       // for base class
#include <xercesc/sax/SAXParseException.hpp>  // for base class

namespace dtUtil
{
   class DT_EXPORT XercesErrorHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ErrorHandler
   {
   public:
      XercesErrorHandler();
      ~XercesErrorHandler();

      virtual void warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e);
      virtual void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e);
      virtual void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e);
      virtual void resetErrors();
   };
};

#endif // DTUTIL_XERCES_ERROR_HANDLER_INC
