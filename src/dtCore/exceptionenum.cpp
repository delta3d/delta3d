#include <prefix/dtcoreprefix.h>
#include <dtCore/exceptionenum.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////////
   BaseException::BaseException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectException::ProjectException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectInvalidContextException::ProjectInvalidContextException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectReadOnlyException::ProjectReadOnlyException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectFileNotFoundException::ProjectFileNotFoundException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectResourceErrorException::ProjectResourceErrorException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   MapParsingException::MapParsingException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   MapSaveException::MapSaveException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   MapException::MapException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectConfigSaveException::ProjectConfigSaveException(const std::string& message, const std::string& filename, unsigned int linenum)
   : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   XMLLoadParsingException::XMLLoadParsingException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ObjectFactoryUnknownTypeException::ObjectFactoryUnknownTypeException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   InvalidActorException::InvalidActorException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   InvalidParameterException::InvalidParameterException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   InvalidContextException::InvalidContextException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }
}
