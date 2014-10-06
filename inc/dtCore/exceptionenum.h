/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute
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
 * Erik Johnson 05/16/2006
 */

#ifndef __DELTA_CORE_EXCEPTION_ENUM_H__
#define __DELTA_CORE_EXCEPTION_ENUM_H__

#include <dtUtil/enumeration.h>
#include <dtCore/export.h>
#include <dtUtil/exception.h>

namespace dtCore
{
   class DT_CORE_EXPORT InvalidParameterException : public dtUtil::Exception
   {
   public:
   	InvalidParameterException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~InvalidParameterException() {};
   };
   
   class DT_CORE_EXPORT InvalidContextException : public dtUtil::Exception
   {
   public:
   	InvalidContextException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~InvalidContextException() {};
   };
   
   class DT_CORE_EXPORT BaseException : public dtUtil::Exception
   {
   public:
      BaseException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~BaseException() {};
   };

   class DT_CORE_EXPORT ProjectException : public dtUtil::Exception
   {
   public:
      ProjectException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectException() {};
   };

   class DT_CORE_EXPORT ProjectInvalidContextException : public ProjectException
   {
   public:
      ProjectInvalidContextException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectInvalidContextException() {};
   };

   class DT_CORE_EXPORT ProjectReadOnlyException : public ProjectException
   {
   public:
      ProjectReadOnlyException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectReadOnlyException() {};
   };

   class DT_CORE_EXPORT ProjectFileNotFoundException : public ProjectException
   {
   public:
      ProjectFileNotFoundException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectFileNotFoundException() {};
   };

   class DT_CORE_EXPORT ProjectResourceErrorException : public ProjectException
   {
   public:
      ProjectResourceErrorException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectResourceErrorException() {};
   };

   class DT_CORE_EXPORT MapParsingException : public dtUtil::Exception
   {
   public:
      MapParsingException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~MapParsingException() {};
   };

   class DT_CORE_EXPORT MapSaveException : public dtUtil::Exception
   {
   public:
      MapSaveException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~MapSaveException() {};
   };

   class DT_CORE_EXPORT MapException : public dtUtil::Exception
   {
   public:
      MapException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~MapException() {};
   };

   class DT_CORE_EXPORT ProjectConfigSaveException : public dtUtil::Exception
   {
   public:
      ProjectConfigSaveException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ProjectConfigSaveException() {};
   };

   class DT_CORE_EXPORT XMLLoadParsingException : public dtUtil::Exception
   {
   public:
      XMLLoadParsingException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~XMLLoadParsingException() {};
   };

   class DT_CORE_EXPORT ObjectFactoryUnknownTypeException : public dtUtil::Exception
   {
   public:
      ObjectFactoryUnknownTypeException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ObjectFactoryUnknownTypeException() {};
   };

   class DT_CORE_EXPORT InvalidActorException : public dtUtil::Exception
   {
   public:
      InvalidActorException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~InvalidActorException() {};
   };

}//namespace dtCore

#endif // __DELTA_CORE_EXCEPTION_ENUM_H__
