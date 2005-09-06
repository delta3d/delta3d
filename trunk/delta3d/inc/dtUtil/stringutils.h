#ifndef DELTA_STRING_UTILS_INC
#define DELTA_STRING_UTILS_INC

#include <string>
#include <sstream> // for std::ostringstream

namespace dtUtil
{
   /** a utility function to convert a basic type into a string.
     * @param T the type being passed.
     * @param t the instance of the type to converted.
     */
   template<typename T>
   std::string ToString(T& t)
   {
      std::ostringstream ss;
      ss << t;
      return ss.str();
   }
};

#endif // DELTA_STRING_UTILS_INC
