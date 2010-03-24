#include <prefix/dtcoreprefix.h>
#include <dtCore/exceptionenum.h>

namespace dtCore
{
   IMPLEMENT_ENUM(ExceptionEnum)

   ExceptionEnum ExceptionEnum::INVALID_PARAMETER("Invalid method parameter");   
   ExceptionEnum ExceptionEnum::INVALID_CONTEXT("Invalid graphics context");   

   ////////////////////////////////////////////////////////////////////////////////
   InvalidParameterException::InvalidParameterException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtCore::ExceptionEnum::INVALID_PARAMETER;
   }

   ////////////////////////////////////////////////////////////////////////////////
   InvalidContextException::InvalidContextException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtCore::ExceptionEnum::INVALID_CONTEXT;
   }
}
