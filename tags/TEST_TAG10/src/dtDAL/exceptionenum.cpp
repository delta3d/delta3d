#include <prefix/dtdalprefix.h>
#include <dtDAL/exceptionenum.h>


namespace dtDAL
{

   IMPLEMENT_ENUM(ExceptionEnum);

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
   dtDAL::BaseException::BaseException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::BaseException;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ProjectException::ProjectException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::ProjectException;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ProjectInvalidContextException::ProjectInvalidContextException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::ProjectInvalidContext;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ProjectReadOnlyException::ProjectReadOnlyException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::ProjectReadOnly;
   }
    
   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ProjectFileNotFoundException::ProjectFileNotFoundException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::ProjectFileNotFound;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ProjectResourceErrorException::ProjectResourceErrorException(const std::string& message, const std::string& filename, unsigned int linenum)
      : ProjectException(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::ProjectResourceError;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::MapParsingException::MapParsingException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::MapLoadParsingError;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::MapSaveException::MapSaveException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::MapSaveError;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::MapException::MapException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::MapException;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ProjectConfigSaveException::ProjectConfigSaveException(const std::string& message, const std::string& filename, unsigned int linenum)
   : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::XMLLoadParsingException::XMLLoadParsingException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::XMLLoadParsingError;
   } 

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ObjectFactoryUnknownTypeException::ObjectFactoryUnknownTypeException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::ObjectFactoryUnknownType;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::InvalidActorException::InvalidActorException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::InvalidActorException;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::InvalidParameterException::InvalidParameterException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
      mType = &dtDAL::ExceptionEnum::InvalidParameter;
   }
}//namespace dtDAL

