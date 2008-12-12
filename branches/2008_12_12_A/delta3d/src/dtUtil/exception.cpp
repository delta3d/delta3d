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
 * Matthew W. Campbell
 */

#include <prefix/dtutilprefix-src.h>
#include <sstream>
#include <dtUtil/exception.h>
#include <dtUtil/log.h>
#include <iostream>

namespace dtUtil
{
   IMPLEMENT_ENUM(BaseExceptionType);
   BaseExceptionType BaseExceptionType::GENERAL_EXCEPTION("GENERAL_EXCEPTION");

   //////////////////////////////////////////////////////////////////////////
   Exception::Exception(Enumeration& type, const std::string& message, const std::string& filename,
         unsigned int lineNum)
      : mType(type)
      , mMessage(message)
      , mFileName(filename)
      , mLineNum(lineNum)
   {
      LogException(dtUtil::Log::LOG_DEBUG, dtUtil::Log::GetInstance());
   }

   //////////////////////////////////////////////////////////////////////////
   Exception::Exception(const std::string& message, const std::string& filename,
         unsigned int lineNum)
      : mType(BaseExceptionType::GENERAL_EXCEPTION)
      , mMessage(message)
      , mFileName(filename)
      , mLineNum(lineNum)
   {
      LogException(dtUtil::Log::LOG_DEBUG, dtUtil::Log::GetInstance());
   }

   //////////////////////////////////////////////////////////////////////////
   void Exception::Print() const
   {
      std::cerr << "Exception: " << mMessage << std::endl <<
      "\tFile: " << mFileName << std::endl <<
      "\tLine: " << mLineNum << std::endl;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string Exception::ToString() const
   {
      std::ostringstream ss;
      ss << "Reason: " << mMessage << " | File: " << mFileName << " | Line: " << mLineNum;
      return ss.str();
   }

   //////////////////////////////////////////////////////////////////////////
   void Exception::LogException(dtUtil::Log::LogMessageType level) const
   {
      LogException(level,dtUtil::Log::GetInstance());
   }

   //////////////////////////////////////////////////////////////////////////
   void Exception::LogException(dtUtil::Log::LogMessageType level, const std::string& loggerName) const
   {
      LogException(level,dtUtil::Log::GetInstance(loggerName));
   }

   //////////////////////////////////////////////////////////////////////////
   void Exception::LogException(dtUtil::Log::LogMessageType level, dtUtil::Log& logger) const
   {
      if (logger.IsLevelEnabled(level))
      {
         logger.LogMessage(level,__FUNCTION__, __LINE__,
               "Exception Thrown: %s File: %s  Line: %d  Type: %s",
               mMessage.c_str(), mFileName.c_str(), mLineNum,
               mType.GetName().c_str());
      }
   }
} // namespace dtUtil
