#include <prefix/dtcoreprefix.h>
#include <dtCore/exceptionenum.h>

namespace dtCore
{
   IMPLEMENT_ENUM(ExceptionEnum)

   ExceptionEnum ExceptionEnum::INVALID_PARAMETER("Invalid method parameter");   
   ExceptionEnum ExceptionEnum::INVALID_CONTEXT("Invalid graphics context");   
   ExceptionEnum ExceptionEnum::BaseException("Base Exception");

   ExceptionEnum ExceptionEnum::ProjectInvalidContext("Invalid or Not-Set Context Directory.");
   ExceptionEnum ExceptionEnum::ProjectReadOnly("The Current Project is Read-only.");
   ExceptionEnum ExceptionEnum::ProjectIOException("File IO Exception");
   ExceptionEnum ExceptionEnum::ProjectFileNotFound("File Not Found");
   ExceptionEnum ExceptionEnum::ProjectResourceError("Resource Error");
   ExceptionEnum ExceptionEnum::ProjectException("General Project Exception");


   ExceptionEnum ExceptionEnum::MapLoadParsingError("Error parsing map file.");
   ExceptionEnum ExceptionEnum::MapSaveError("Error saving map file.");
   ExceptionEnum ExceptionEnum::MapException("General Map Exception");

   ExceptionEnum ExceptionEnum::XMLLoadParsingError("Error parsing XML file.");
   ExceptionEnum ExceptionEnum::XMLSaveError("Error saving XML file.");
   ExceptionEnum ExceptionEnum::XMLException("General XML Exception");

   ExceptionEnum ExceptionEnum::ObjectFactoryUnknownType("Unknown Object Type");
   ExceptionEnum ExceptionEnum::InvalidActorException("Invalid Actor Type");
   ExceptionEnum ExceptionEnum::InvalidParameter("Invalid Parameter");


   ////////////////////////////////////////////////////////////////////////////////
   BaseException::BaseException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &ExceptionEnum::BaseException;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectException::ProjectException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &ExceptionEnum::ProjectException;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectInvalidContextException::ProjectInvalidContextException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
      mType = &ExceptionEnum::ProjectInvalidContext;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectReadOnlyException::ProjectReadOnlyException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
      mType = &ExceptionEnum::ProjectReadOnly;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectFileNotFoundException::ProjectFileNotFoundException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
      mType = &ExceptionEnum::ProjectFileNotFound;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ProjectResourceErrorException::ProjectResourceErrorException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
      mType = &ExceptionEnum::ProjectResourceError;
   }

   ////////////////////////////////////////////////////////////////////////////////
   MapParsingException::MapParsingException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &ExceptionEnum::MapLoadParsingError;
   }

   ////////////////////////////////////////////////////////////////////////////////
   MapSaveException::MapSaveException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &ExceptionEnum::MapSaveError;
   }

   ////////////////////////////////////////////////////////////////////////////////
   MapException::MapException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &ExceptionEnum::MapException;
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
      mType = &ExceptionEnum::XMLLoadParsingError;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ObjectFactoryUnknownTypeException::ObjectFactoryUnknownTypeException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &ExceptionEnum::ObjectFactoryUnknownType;
   }

   ////////////////////////////////////////////////////////////////////////////////
   InvalidActorException::InvalidActorException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &ExceptionEnum::InvalidActorException;
   }

   ////////////////////////////////////////////////////////////////////////////////
   InvalidParameterException::InvalidParameterException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &ExceptionEnum::InvalidParameter;
   }

   ////////////////////////////////////////////////////////////////////////////////
   InvalidContextException::InvalidContextException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &ExceptionEnum::INVALID_CONTEXT;
   }
}
