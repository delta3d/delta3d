/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Matthew W. Campbell
 */
#ifndef DELTA_EXCEPTION_ENUM
#define DELTA_EXCEPTION_ENUM

#include <dtUtil/enumeration.h>
#include <dtUtil/exception.h>
#include <dtDAL/export.h>


namespace dtDAL
{
   ///DEPRECATE 2/16/10 Use concrete dtUtil::Exceptions instead
   class DT_DAL_EXPORT ExceptionEnum : public dtUtil::Enumeration
   {
         DECLARE_ENUM(ExceptionEnum);
      public:
         static ExceptionEnum BaseException;

         static ExceptionEnum ProjectInvalidContext;
         static ExceptionEnum ProjectReadOnly;
         static ExceptionEnum ProjectIOException;
         static ExceptionEnum ProjectFileNotFound;
         static ExceptionEnum ProjectResourceError;
         static ExceptionEnum ProjectException;

         static ExceptionEnum MapLoadParsingError;
         static ExceptionEnum MapSaveError;
         static ExceptionEnum MapException;

         static ExceptionEnum XMLLoadParsingError;
         static ExceptionEnum XMLSaveError;
         static ExceptionEnum XMLException;

         static ExceptionEnum ObjectFactoryUnknownType;

         static ExceptionEnum InvalidActorException;
         static ExceptionEnum InvalidParameter;

      protected:
         ExceptionEnum(const std::string &name) : Enumeration(name)
         {
            AddInstance(this);
         }
   };

   class DT_DAL_EXPORT BaseException : public dtUtil::Exception
   {
   public:
   	BaseException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~BaseException() {};
   };

   class DT_DAL_EXPORT ProjectException : public dtUtil::Exception
   {
   public:
      ProjectException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectException() {};
   };

   class DT_DAL_EXPORT ProjectInvalidContextException : public ProjectException
   {
   public:
      ProjectInvalidContextException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectInvalidContextException() {};
   };

   class DT_DAL_EXPORT ProjectReadOnlyException : public ProjectException
   {
   public:
      ProjectReadOnlyException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectReadOnlyException() {};
   };

   class DT_DAL_EXPORT ProjectFileNotFoundException : public ProjectException
   {
   public:
      ProjectFileNotFoundException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectFileNotFoundException() {};
   };

   class DT_DAL_EXPORT ProjectResourceErrorException : public ProjectException
   {
   public:
      ProjectResourceErrorException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectResourceErrorException() {};
   };

   class DT_DAL_EXPORT MapParsingException : public dtUtil::Exception
   {
   public:
      MapParsingException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~MapParsingException() {};
   };

   class DT_DAL_EXPORT MapSaveException : public dtUtil::Exception
   {
   public:
      MapSaveException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~MapSaveException() {};
   };

   class DT_DAL_EXPORT MapException : public dtUtil::Exception
   {
   public:
      MapException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~MapException() {};
   };

   class DT_DAL_EXPORT XMLLoadParsingException : public dtUtil::Exception
   {
   public:
      XMLLoadParsingException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~XMLLoadParsingException() {};
   };

   class DT_DAL_EXPORT ObjectFactoryUnknownTypeException : public dtUtil::Exception
   {
   public:
      ObjectFactoryUnknownTypeException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ObjectFactoryUnknownTypeException() {};
   };

   class DT_DAL_EXPORT InvalidActorException : public dtUtil::Exception
   {
   public:
      InvalidActorException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~InvalidActorException() {};
   };

   class DT_DAL_EXPORT InvalidParameterException : public dtUtil::Exception
   {
   public:
      InvalidParameterException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~InvalidParameterException() {};
   };

}//namespace dtDAL

#endif //DELTA_EXCEPTION_ENUM
