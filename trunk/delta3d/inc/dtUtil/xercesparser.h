#ifndef DELTA_XERCES_PARSER_INC
#define DELTA_XERCES_PARSER_INC

#include <string>
#include <xercesc/sax2/ContentHandler.hpp>
#include <dtCore/export.h>

namespace dtUtil
{
   class DT_EXPORT XercesParser
   {
   public:
      XercesParser();
      ~XercesParser();

      bool Parse(const std::string& data,
                 XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler& handler,
                 const std::string& schema="");
   };
};

#endif  // DELTA_XERCES_PARSER_INC
