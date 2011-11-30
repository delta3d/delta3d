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
#include <iostream>
#include <ctime>
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

   //First attempt to create the log file.
   logFile.open(LogFile::GetFileName().c_str());
   if (!logFile.is_open())
   {
      std::cout << "could not open file \""<<LogFile::GetFileName()<<"\"" << std::endl;
      mOpenFailed = true;
      return;
   }
   else
   {
      //std::cout << "Using file \"delta3d_log.html\" for logging" << std::endl;
   }
   //Write a decent header to the html file.
   logFile << "<html><title>" << LogFile::GetTitle() <<"</title><body>" << std::endl;
   logFile << "<h1 align=\"center\">" << LogFile::GetTitle() << "</h1><hr>" << std::endl;
   logFile << "<pre><h3 align=\"center\""
      "<font color=#808080><b>  Debug     </b></font>"
      "<font color=#008080><b>  Information     </b></font>"
      "<font color=#808000><b>  Warning  </b></font>"
      "<font color=#FF0000><b>  Error   </b></font></h3></pre><hr>"
      << std::endl;

   TimeTag("Started at ");

   logFile.flush();
}

////////////////////////////////////////////////////////////////////////////////
void dtUtil::LogObserverFile::TimeTag(std::string prefix)
{
   struct tm *t;
   time_t cTime;

   time(&cTime);
   t = localtime(&cTime);
   logFile << prefix
      << std::setw(2) << std::setfill('0') << (1900+t->tm_year) << "/"
      << std::setw(2) << std::setfill('0') << t->tm_mon+1 << "/"
      << std::setw(2) << std::setfill('0') << t->tm_mday << " "
      << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
      << std::setw(2) << std::setfill('0') << t->tm_min << ":"
      << std::setw(2) << std::setfill('0') << t->tm_sec << "<br>"
      << std::endl;
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
      color = "<b><font color=#808080>";
      break;

   case Log::LOG_INFO:
      color = "<b><font color=#008080>";
      break;

   case Log::LOG_ERROR:
      color = "<b><font color=#FF0000>";
      break;

   case Log::LOG_WARNING:
      color = "<b><font color=#CCCC00>";
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
   
   logFile << color << "[" << std::setw(2) << std::setfill('0') << logData.time.tm_hour << ":"
      << std::setw(2) << std::setfill('0') << logData.time.tm_min << ":"
      << std::setw(2) << std::setfill('0') << logData.time.tm_sec 
      << " " << Log::GetLogLevelString(logData.type) << "] ";

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
