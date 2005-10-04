#ifndef __EXCEPTION_ENUM_H__
#define __EXCEPTION_ENUM_H__


#include <dtUtil/enumeration.h>
#include <dtUtil/exception.h>

/*
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
* @author Matthew W. Campbell
*/

namespace dtDAL
{




class DT_EXPORT ExceptionEnum : public dtUtil::Enumeration
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

   static ExceptionEnum ObjectFactoryUnknownType;

   static ExceptionEnum InvalidActorException;

protected:
   ExceptionEnum(const std::string &name) : Enumeration(name)
   {
      AddInstance(this);
   }
};



}//namespace dtDAL

#endif //__EXCEPTION_ENUM_H__
