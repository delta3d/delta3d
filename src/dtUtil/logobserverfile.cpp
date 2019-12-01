/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2010, MOVES Institute
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
 * Erik Johnson
 */
#include <prefix/dtutilprefix.h>
#include <dtUtil/logobserverfile.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <iostream>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////////
dtUtil::LogObserverFile::LogObserverFile() 
: mOpenFailed(false)
{
}

////////////////////////////////////////////////////////////////////////////////
dtUtil::LogObserverFile::~LogObserverFile()
{
   if (logFile.is_open())
   {
      EndFile();
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtUtil::LogObserverFile::EndFile()
{
   logFile << "</body></html>" << std::endl;
   logFile.flush();
}

////////////////////////////////////////////////////////////////////////////////
void dtUtil::LogObserverFile::OpenFile()
{
   if (mOpenFailed)
   {
      return;
   }

   if (logFile.is_open())
   {
      logFile << "<p>Change to log file: "<< LogFile::GetFileName()<< std::endl;
      TimeTag("At ");
      EndFile();
      logFile.close();
   }

   std::string fullLogPath = LogFile::GetFileName();
   if (LogFile::GetFileName().find(dtUtil::FileUtils::PATH_SEPARATOR) == std::string::npos)
   {
      //Store logs in a system dependent place.
      std::string homePath = dtUtil::GetHomeDirectory();
      if (!homePath.empty())
      {
#ifdef __APPLE__
         std::string logDir = homePath + "/Library/Logs/delta3d/";
#elif defined(DELTA_WIN32)
         std::string logDir = homePath + "\\delta3d\\logs\\";
#else
         std::string logDir = homePath + "/.delta3d/logs/";
#endif
         try
         {
            dtUtil::FileUtils::GetInstance().MakeDirectoryEX(logDir);
         }
         catch (const dtUtil::Exception&)
         {
            std::cerr << "Unable to create the log directory : \""<<logDir<<"\".  The log file will be written to the current working directory if possible." << std::endl;
            logDir.clear();
         }

         fullLogPath = logDir + LogFile::GetFileName();

      }
      //First attempt to create the log file.
   }
   logFile.open(fullLogPath.c_str());

   if (!logFile.is_open())
   {
      std::cerr << "Could not open the Log file \""<<fullLogPath<<"\"" << std::endl;
      mOpenFailed = true;
      return;
   }
//   else
//   {
//      std::cout << "Using file \""<< fullLogPath << "\" for logging" << std::endl;
//   }
   //Write a decent header to the html file.
   logFile << "<html><title>" << LogFile::GetTitle() <<"</title><body>" << std::endl;
   logFile << "<h1 align=\"center\">" << LogFile::GetTitle() << "</h1><hr>" << std::endl;
   logFile << "<pre><h3 align=\"center\""
      "<font color=#A000A0><b>  Debug     </b></font>"
      "<font color=#00A000><b>  Information     </b></font>"
      "<font color=#CF6F00><b>  Warning  </b></font>"
      "<font color=#C00000><b>  Error   </b></font></h3></pre><hr>"
      << std::endl;

   TimeTag("Started at ");

   logFile.flush();
}

////////////////////////////////////////////////////////////////////////////////
void dtUtil::LogObserverFile::TimeTag(std::string prefix)
{
   dtUtil::DateTime dt;
   logFile << prefix << dt.ToString(dtUtil::DateTime::TimeFormat::CLOCK_TIME_24_HOUR_FORMAT) << std::endl;
   logFile.flush();
}

//////////////////////////////////////////////////////////////////////////
void dtUtil::LogObserverFile::LogMessage(const LogData& logData)
{
   if (!logFile.is_open())
   {
      OpenFile();

      if (!logFile.is_open())
      {
         return;
      }
   }

   std::string color;
   switch (logData.type)
   {
   case Log::LOG_DEBUG:
	   color = "<b><font color=#A000A0>";
	   break;

   case Log::LOG_INFO:
	   color = "<b><font color=#00A000>";
	   break;

   case Log::LOG_ERROR:
	   color = "<b><font color=#C00000>";
	   break;

   case Log::LOG_WARNING:
	   color = "<b><font color=#CF6F00>";
	   break;

   case Log::LOG_ALWAYS:
	   color = "<b><font color=#000000>";
	   break;
   }

   static const std::string htmlNewline ("<br>\n");
   std::string htmlMsg (logData.msg);

   for (size_t lineEnd = htmlMsg.find('\n');
        lineEnd != std::string::npos;
        lineEnd = htmlMsg.find('\n', lineEnd))
   {
      htmlMsg.replace (lineEnd, 1, htmlNewline);
      lineEnd += htmlNewline.size() + 1;
   }
   
   logFile << color << "[" << logData.time.ToString(dtUtil::DateTime::TimeFormat::CLOCK_TIME_24_HOUR_FORMAT);

   if (logData.frameNumber > 0)
   {
	   logFile << " Frm&#35; " << logData.frameNumber;
   }

   logFile << " " << Log::GetLogLevelString(logData.type) << "] ";

   logFile << htmlMsg << " [";

   if (!logData.logName.empty())
   {
      logFile << "'" << logData.logName << "' ";
   }
   
   if (!logData.method.empty())
   {
      logFile << logData.method << "()";
   }

   if (!logData.file.empty())
   {
      logFile << " " << logData.file;

      if (logData.line > 0)
      {
         logFile << "(" << logData.line << ")";
      }
   }

   logFile << "]" << "</font></b><br>" << std::endl;

   logFile.flush(); //Make sure everything is written, in case of a crash.
}

////////////////////////////////////////////////////////////////////////////////
void dtUtil::LogObserverFile::LogHorizRule()
{
   if (!logFile.is_open())
   {
      return;
   }

   logFile << "<hr>" << std::endl;
}
