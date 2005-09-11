#ifndef DELTA_STRING_UTILS_INC
#define DELTA_STRING_UTILS_INC

#include "dtCore/export.h"

#include <string>                       // for parameters
#include <sstream>                      // for std::ostringstream
#include <xercesc/util/XercesDefs.hpp>  // for xerces namespace macros

XERCES_CPP_NAMESPACE_BEGIN
   class DOMNode;
XERCES_CPP_NAMESPACE_END

namespace dtUtil
{
   /** a utility function to convert a basic type into a string.
     * @param T the type being passed.
     * @param t the instance of the type to converted.
     */
   template<typename T>
   std::string ToString(const T& t)
   {
      std::ostringstream ss;
      ss << t;
      return ss.str();
   }

   /** Converts a string to a float.*/
   float DT_EXPORT ToFloat(const std::string& d);

   /** A utility that finds the string value for a specifically named attribute.
     * @param doc the document needed for walking the attribute list
     * @param node the node with attributes to be searched.
     * @param name the name of the attribute of interest.
     */
   std::string DT_EXPORT GetAttributeValueFor(const char* name,
                                              XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* node);

};

#endif // DELTA_STRING_UTILS_INC
