#include <prefix/dtcoreprefix-src.h>
#include <dtCore/exceptionenum.h>

namespace dtCore
{
   IMPLEMENT_ENUM(ExceptionEnum)

   ExceptionEnum ExceptionEnum::INVALID_PARAMETER("Invalid method parameter");   
   ExceptionEnum ExceptionEnum::INVALID_CONTEXT("Invalid graphics context");   
}
