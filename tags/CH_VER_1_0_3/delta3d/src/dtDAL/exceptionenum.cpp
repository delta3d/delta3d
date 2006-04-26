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

   ExceptionEnum ExceptionEnum::ObjectFactoryUnknownType("Unknown Object Type");
   ExceptionEnum ExceptionEnum::InvalidActorException("Invalid Actor Type");

}//namespace dtDAL

