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
#include <dtUtil/log.h>
#include <dtUtil/bits.h>

#include <dtCore/refptr.h>

#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <map>

namespace dtUtil
{
   static const char* sLogFileName = "delta3d_log.html";

#ifdef _DEBUG
   static std::string sTitle("Delta 3D Engine Log File (Debug Libs)");
#else
   static std::string sTitle("Delta 3D Engine Log File");
#endif

   //////////////////////////////////////////////////////////////////////////

   class LogManager: public osg::Referenced
   {
   public:
      std::ofstream logFile;

      LogManager()
      {
         //std::cout << "Creating logger" << std::endl;

         //if (!logFile.is_open())
         //{
         //   OpenFile();
         //}
         //std::cout.flush();
      }

      ~LogManager()
      {
         mInstances.clear();
         //std::cout << "BEING DESTROYED - LogManager" << std::endl;
         //std::cout.flush();
         if (logFile.is_open())
         {
            //std::cout << "Closing log file" << std::endl;
            //std::cout.flush();
            EndFile();
            //Log::logFile.close();
         }
      }

      void EndFile()
      {
         logFile << "</body></html>" << std::endl;
         logFile.flush();
      }

      void OpenFile()
      {
         //std::cout << "LogManager try to open file to " << sLogFileName << std::endl;
         if (logFile.is_open())
         {
            logFile << "<p>Change to log file: "<< sLogFileName<< std::endl;
            TimeTag("At ");
            EndFile();
            logFile.close();
         }

         //First attempt to create the log file.
         logFile.open(sLogFileName);
         if (!logFile.is_open())
         {
            std::cout << "could not open file \""<<sLogFileName<<"\"" << std::endl;
            return;
         }
         else
         {
            //std::cout << "Using file \"delta3d_log.html\" for logging" << std::endl;
         }
         //Write a decent header to the html file.
         logFile << "<html><title>" << sTitle <<"</title><body>" << std::endl;
         logFile << "<h1 align=\"center\">" << sTitle << "</h1><hr>" << std::endl;
         logFile << "<pre><h3 align=\"center\""
            "<font color=#808080><b>  Debug     </b></font>"
            "<font color=#008080><b>  Information     </b></font>"
            "<font color=#808000><b>  Warning  </b></font>"
            "<font color=#FF0000><b>  Error   </b></font></h3></pre><hr>"
            << std::endl;

         TimeTag("Started at ");

         logFile.flush();
         //std::cout.flush();
      }

      void TimeTag(std::string prefix){

         struct tm *t;
         time_t cTime;
         std::string color;

         time(&cTime);
         t = localtime(&cTime);
         logFile << prefix
            << std::setw(2) << std::setfill('0') << (1900+t->tm_year) << "/"
            << std::setw(2) << std::setfill('0') << t->tm_mon << "/"
            << std::setw(2) << std::setfill('0') << t->tm_mday << " "
            << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << t->tm_min << ":"
            << std::setw(2) << std::setfill('0') << t->tm_sec << "<br>"
            << std::endl;
         logFile.flush();
      }

      bool AddInstance(const std::string& name, Log* log)
      {
         return mInstances.insert(std::make_pair(name, dtCore::RefPtr<Log>(log))).second;
      }

      Log* GetInstance(const std::string& name)
      {
         std::map<std::string, dtCore::RefPtr<Log> >::iterator i = mInstances.find(name);
         if (i == mInstances.end())
         {
            return NULL;
         }
         return i->second.get();
      }

      OpenThreads::Mutex mMutex;
   private:
      std::map<std::string, dtCore::RefPtr<Log> > mInstances;
   };

   static dtCore::RefPtr<LogManager> manager(NULL);

   /** This will close the existing file (if opened) and create a new file with
    *  the supplied filename.
    * @param name : The name of the new file (will be written using HTML)
    */
   void LogFile::SetFileName(const std::string& name)
   {
      //std::cout << "LogFile try to change files to " << name << std::endl;

      sLogFileName = name.c_str();
      if (manager == NULL) {
         manager = new LogManager;
      } else {
         manager->OpenFile();
      }
   }

   const std::string LogFile::GetFileName()
   {
      return std::string(sLogFileName);
   }

   void LogFile::SetTitle(const std::string& title)
   {
      sTitle = title;
   }

   const std::string& LogFile::GetTitle()
   {
      return sTitle;
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   struct LogImpl
   {
      LogImpl(const std::string& name)
      : mOutputStreamBit(Log::STANDARD)
      , mName(name)
      {
      }

      static const std::string mDefaultName;

      unsigned int mOutputStreamBit; ///<the current output stream option
      std::string mName;
   };

   const std::string LogImpl::mDefaultName("__+default+__");
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   Log::Log(const std::string& name)
   : mLevel(Log::LOG_WARNING)
   , mImpl(new LogImpl(name))
   {
   }

   //////////////////////////////////////////////////////////////////////////
   Log::~Log()
   {
      delete mImpl;
      mImpl = NULL;
   }


   //////////////////////////////////////////////////////////////////////////
   void Log::LogMessage(const std::string& source, int line, const std::string& msg,
                LogMessageType msgType) const
   {
      if (mImpl->mOutputStreamBit == Log::NO_OUTPUT)
         return;

      if (msgType < mLevel)
         return;

      struct tm *t;
      time_t cTime;
      std::string color;

      time(&cTime);
      t = localtime(&cTime);

      switch (msgType)
      {
      case LOG_DEBUG:
         color = "<b><font color=#808080>";
         break;

      case LOG_INFO:
         color = "<b><font color=#008080>";
         break;

      case LOG_ERROR:
         color = "<b><font color=#FF0000>";
         break;

      case LOG_WARNING:
         color = "<b><font color=#808000>";
         break;

      case LOG_ALWAYS:
         color = "<b><font color=#000000>";
         break;

      }

      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(manager->mMutex);

      if (dtUtil::Bits::Has(mImpl->mOutputStreamBit, Log::TO_FILE))
      {
         if (!manager->logFile.is_open())
         {
            manager->OpenFile();

            if (!manager->logFile.is_open())
            {
               return;
            }
         }

         static const std::string htmlNewline ( "<br>\n" );
         std::string htmlMsg ( msg );
         for ( size_t lineEnd = htmlMsg.find( '\n' );
              lineEnd != std::string::npos;
              lineEnd = htmlMsg.find( '\n', lineEnd ) )
         {
            htmlMsg.replace ( lineEnd, 1, htmlNewline );
            lineEnd += htmlNewline.size() + 1;
         }
         manager->logFile << color << GetLogLevelString(msgType) << ": "
            << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << t->tm_min << ":"
            << std::setw(2) << std::setfill('0') << t->tm_sec << ": &lt;"
            << source;
         if (line > 0)
            manager->logFile << ":" << line;

         manager->logFile << "&gt; " << htmlMsg << "</font></b><br>" << std::endl;

         manager->logFile.flush(); //Make sure everything is written, in case of a crash.
      }

      if (dtUtil::Bits::Has(mImpl->mOutputStreamBit, Log::TO_CONSOLE))
      {
         std::cout << GetLogLevelString(msgType) << ": "
            << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << t->tm_min << ":"
            << std::setw(2) << std::setfill('0') << t->tm_sec << ":<"
            << source;
         if (line > 0)
         {
            std:: cout << ":" << line;
         }
         std::cout << ">" << msg << std::endl;
      }

   }

   //////////////////////////////////////////////////////////////////////////
   void Log::LogMessage(LogMessageType msgType, const std::string& source, int line,
                            const char* msg, va_list list) const
   {
      char buffer[2049];

      vsnprintf(buffer, 2049, msg, list);

      LogMessage(source, line, buffer, msgType);
   }

   //////////////////////////////////////////////////////////////////////////
   void Log::LogMessage(LogMessageType msgType, const std::string& source,
                     const char* msg, ...) const
   {
      va_list list;

      va_start(list,msg);
      LogMessage(msgType, source, -1, msg, list);
      va_end(list);

   }

   //////////////////////////////////////////////////////////////////////////
   void Log::LogMessage(LogMessageType msgType, const std::string& source, int line,
                            const char* msg, ...) const
   {
      va_list list;

      va_start(list,msg);
      LogMessage(msgType, source, line, msg, list);
      va_end(list);
   }

   //////////////////////////////////////////////////////////////////////////
   void Log::LogMessage(LogMessageType msgType,
                        const std::string& source,
                        int line,
                        const std::string& msg) const
   {
      LogMessage(source, line, msg, msgType);
   }

   //////////////////////////////////////////////////////////////////////////
   void Log::LogHorizRule()
   {
      if (!manager->logFile.is_open())
         return;

      if (mImpl->mOutputStreamBit == Log::NO_OUTPUT)
         return;

      if (dtUtil::Bits::Has(mImpl->mOutputStreamBit, Log::TO_FILE))
      {
         manager->logFile << "<hr>" << std::endl;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   Log& Log::GetInstance()
   {
      return GetInstance(LogImpl::mDefaultName);
   }

   //////////////////////////////////////////////////////////////////////////
   Log& Log::GetInstance(const std::string& name)
   {
      if (manager == NULL)
         manager = new LogManager;

      Log* l = manager->GetInstance(name);
      if (l == NULL)
      {
         l = new Log(name);
         manager->AddInstance(name, l);
      }

      return *l;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string Log::GetLogLevelString( Log::LogMessageType msgType) const
   {
      std::string lev;

      switch(msgType)
      {
      case Log::LOG_ALWAYS:  lev = "Always";  break;
      case Log::LOG_ERROR:   lev = "Error";   break;
      case Log::LOG_WARNING: lev = "Warn";    break;
      case Log::LOG_INFO:    lev = "Info";    break;
      case Log::LOG_DEBUG:   lev = "Debug";   break;
      default:
         break;
      }

      return lev;
   }

   //////////////////////////////////////////////////////////////////////////
   Log::LogMessageType Log::GetLogLevelForString( const std::string& levelString) const
   {
      if (levelString == "Always" || levelString == "ALWAYS")
         return LOG_ALWAYS;
      else if (levelString == "Error" || levelString == "ERROR")
         return LOG_ERROR;
      else if (levelString == "Warn" || levelString == "WARN" || levelString == "Warning" || levelString == "WARNING")
         return LOG_WARNING;
      else if (levelString == "Info" || levelString == "INFO")
         return LOG_INFO;
      else if (levelString == "Debug" || levelString == "DEBUG")
         return LOG_DEBUG;

      else return LOG_WARNING;
   }

   ///////////////////////////////////////////////////////////////////////////
   void Log::SetOutputStreamBit(unsigned int option)
   {
      mImpl->mOutputStreamBit = option;
   }

   ///////////////////////////////////////////////////////////////////////////
   unsigned int Log::GetOutputStreamBit() const
   {
      return mImpl->mOutputStreamBit;
   }

   ///////////////////////////////////////////////////////////////////////////
   const std::string& Log::GetName() const
   {
      return mImpl->mName;
   }

} //end namespace
