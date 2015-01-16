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
 * Chris DuBuc
 */

#ifndef DELTA_LMS_EXCEPTION_ENUM
#define DELTA_LMS_EXCEPTION_ENUM

#include <dtUtil/enumeration.h>
#include <dtUtil/exception.h>
#include <dtLMS/export.h>

namespace dtLMS
{
   ///Exception enumeration used for general game manager exceptions.
   class LMSConnectionException : public dtUtil::Exception
   {
   public:
   	LMSConnectionException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~LMSConnectionException() {};
   };
   
   ///Exception enumeration used for method invalid parameters.
   class LMSInvalidMessageException : public dtUtil::Exception
   {
   public:
   	LMSInvalidMessageException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~LMSInvalidMessageException() {};
   };
   
} // namespace dtLMS

#endif // DELTA_LMS_EXCEPTION_ENUM
