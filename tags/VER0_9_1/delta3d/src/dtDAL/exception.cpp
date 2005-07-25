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
#include "dtDAL/exception.h"
#include "dtDAL/log.h"
#include <iostream>

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

    //////////////////////////////////////////////////////////////////////////
    Exception::Exception(ExceptionEnum &type, const std::string &message, const std::string &filename,
        unsigned int lineNum) : mType(type),mMessage(message),mFileName(filename),mLineNum(lineNum) 
    {
        LogException(Log::LOG_DEBUG, Log::GetInstance());
    }

    //////////////////////////////////////////////////////////////////////////
    void Exception::Print()
    {
        std::cerr << "Exception: " << mMessage << std::endl <<
            "\tFile: " << mFileName << std::endl <<
            "\tLine: " << mLineNum << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////
    void Exception::LogException(Log::LogMessageType level) 
    {
        LogException(level,Log::GetInstance());
    }

    //////////////////////////////////////////////////////////////////////////
    void Exception::LogException(Log::LogMessageType level, const std::string& loggerName) 
    {
        LogException(level,Log::GetInstance(loggerName));
    }

    //////////////////////////////////////////////////////////////////////////
    void Exception::LogException(Log::LogMessageType level, Log& logger) 
    {
        if (logger.IsLevelEnabled(level)) 
        {
            logger.LogMessage(level,__FUNCTION__, __LINE__,
                    "Exception Thrown: %s File: %s  Line: %d  Type: %s",
                    mMessage.c_str(),mFileName.c_str(),mLineNum,
                    mType.GetName().c_str());
        }

    }

}
