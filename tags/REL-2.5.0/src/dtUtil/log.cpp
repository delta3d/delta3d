/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005-2010, BMH Associates, Inc.
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
 * Matthew W. Campbell, Curtiss Murphy, Erik Johnson
 */
#include <prefix/dtutilprefix.h>
#include <dtUtil/log.h>
#include <dtUtil/bits.h>
#include <dtUtil/logobserver.h>
#include <dtUtil/logobserverconsole.h>
#include <dtUtil/logobserverfile.h>
#include <dtCore/refptr.h>

#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>

#include <algorithm>
#include <cstdarg>
//#include <cstdio>
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

   //forward declaration
   class LogManager;

   static dtCore::RefPtr<LogManager> LOG_MANAGER(NULL);
   static Log::LogMessageType DEFAULT_LOG_LEVEL(Log::LOG_WARNING);


   //////////////////////////////////////////////////////////////////////////
   class LogManager: public osg::Referenced
   {
   public:
      dtCore::RefPtr<LogObserver> mLogObserverConsole; ///writes to console
      dtCore::RefPtr<LogObserverFile> mLogObserverFile; ///writes to file

      ////////////////////////////////////////////////////////////////
      LogManager()
      : mLogObserverConsole(new LogObserverConsole())
      , mLogObserverFile(new LogObserverFile())
      {
      }

      ////////////////////////////////////////////////////////////////
      ~LogManager()
      {
         mInstances.clear();
         mLogObserverConsole = NULL;
         mLogObserverFile = NULL;
      }

      ////////////////////////////////////////////////////////////////
      bool AddInstance(const std::string& name, Log* log)
      {
         return mInstances.insert(std::make_pair(name, dtCore::RefPtr<Log>(log))).second;
      }

      ////////////////////////////////////////////////////////////////
      Log* GetInstance(const std::string& name)
      {
         std::map<std::string, dtCore::RefPtr<Log> >::iterator i = mInstances.find(name);
         if (i == mInstances.end())
         {
            return NULL;
         }
         return i->second.get();
      }

      ////////////////////////////////////////////////////////////////
      void SetAllLogLevels(const Log::LogMessageType &newLevel)
      {
         std::map<std::string, dtCore::RefPtr<Log> >::iterator i, iend;

         i = mInstances.begin();
         iend = mInstances.end();

         for (;i != iend; i++)
         {
            Log* log = i->second.get();
            log->SetLogLevel(newLevel);
         }
      }

      OpenThreads::Mutex mMutex;
   private:
      std::map<std::string, dtCore::RefPtr<Log> > mInstances;
   };

   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////

   /** This will close the existing file (if opened) and create a new file with
    *  the supplied filename.
    * @param name : The name of the new file (will be written using HTML)
    */
   void LogFile::SetFileName(const std::string& name)
   {
      //std::cout << "LogFile try to change files to " << name << std::endl;

      sLogFileName = name.c_str();
      if (LOG_MANAGER == NULL)
      {
         LOG_MANAGER = new LogManager;
      }
      else
      {
         // reset open failed if the file name changes.
         LOG_MANAGER->mLogObserverFile->mOpenFailed = false;
         LOG_MANAGER->mLogObserverFile->OpenFile();
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
      , mLevel(DEFAULT_LOG_LEVEL)
      , mObservers()
      {
      }

      static const std::string mDefaultName;

      unsigned int mOutputStreamBit; ///<the current output stream option
      std::string mName;
      Log::LogMessageType mLevel;
      Log::LogObserverContainer mObservers;
   };

   const std::string LogImpl::mDefaultName("__+default+__");
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   Log::Log(const std::string& name)
      : 
        mImpl(new LogImpl(name))      
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
      {
         return;
      }

      if (msgType < mImpl->mLevel)
      {
         return;
      }

      struct tm *t;
      time_t cTime;

      time(&cTime);
      t = localtime(&cTime);


      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(LOG_MANAGER->mMutex);

      if (dtUtil::Bits::Has(mImpl->mOutputStreamBit, Log::TO_FILE))
      {
         LOG_MANAGER->mLogObserverFile->LogMessage(msgType,
                                                   t->tm_hour, t->tm_min, t->tm_sec,
                                                   source, line, msg);
      }

      if (dtUtil::Bits::Has(mImpl->mOutputStreamBit, Log::TO_CONSOLE))
      {
         LOG_MANAGER->mLogObserverConsole->LogMessage(msgType,
                                                      t->tm_hour, t->tm_min, t->tm_sec,
                                                      source, line, msg);
      }

      if (dtUtil::Bits::Has(mImpl->mOutputStreamBit, Log::TO_OBSERVER) && !mImpl->mObservers.empty())
      {
         Log::LogObserverContainer::iterator itr = mImpl->mObservers.begin();
         while (itr != mImpl->mObservers.end())
         {
            (*itr)->LogMessage(msgType, t->tm_hour, t->tm_min, t->tm_sec, source, line, msg);
            ++itr;
         }
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
      if (mImpl->mOutputStreamBit == Log::NO_OUTPUT)
      {
         return;
      }

      if (dtUtil::Bits::Has(mImpl->mOutputStreamBit, Log::TO_FILE))
      {
         LOG_MANAGER->mLogObserverFile->LogHorizRule();
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
      if (LOG_MANAGER == NULL)
      {
         LOG_MANAGER = new LogManager;
      }

      Log* l = LOG_MANAGER->GetInstance(name);
      if (l == NULL)
      {
         l = new Log(name);
         LOG_MANAGER->AddInstance(name, l);
      }

      return *l;
   }

   //////////////////////////////////////////////////////////////////////////
   void Log::SetDefaultLogLevel(LogMessageType newLevel)
   {
      DEFAULT_LOG_LEVEL = newLevel;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string Log::GetLogLevelString(Log::LogMessageType msgType) //static
   {
      std::string lev;

      switch (msgType)
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
   Log::LogMessageType Log::GetLogLevelForString(const std::string& levelString)  //static
   {
      if (levelString == "Always" || levelString == "ALWAYS")
      {
         return LOG_ALWAYS;
      }
      else if (levelString == "Error" || levelString == "ERROR")
      {
         return LOG_ERROR;
      }
      else if (levelString == "Warn" || levelString == "WARN" || levelString == "Warning" || levelString == "WARNING")
      {
         return LOG_WARNING;
      }
      else if (levelString == "Info" || levelString == "INFO")
      {
         return LOG_INFO;
      }
      else if (levelString == "Debug" || levelString == "DEBUG")
      {
         return LOG_DEBUG;
      }
      else
      {
         return LOG_WARNING;
      }
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

   ////////////////////////////////////////////////////////////////////////////////
   bool Log::IsLevelEnabled(LogMessageType msgType) const
   {
      return msgType >= mImpl->mLevel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Log::SetLogLevel(LogMessageType msgType)
   {
      mImpl->mLevel = msgType;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtUtil::Log::LogMessageType Log::GetLogLevel() const
   {
      return mImpl->mLevel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Log::SetAllLogLevels(LogMessageType newLevel) //static
   {
      if (LOG_MANAGER.valid())
      {
         LOG_MANAGER->SetAllLogLevels(newLevel);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Log::AddObserver(LogObserver& observer)
   {
      mImpl->mObservers.push_back(&observer);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Log::LogObserverContainer& Log::GetObservers() const
   {
      return mImpl->mObservers;
   }

   //////////////////////////////////////////////////////////////////////////
   Log::LogObserverContainer& Log::GetObservers()
   {
      return mImpl->mObservers;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Log::RemoveObserver(LogObserver& observer)
   {
      LogObserverContainer::iterator found = std::find(mImpl->mObservers.begin(),
                                                       mImpl->mObservers.end(), &observer);
      if (found != mImpl->mObservers.end())
      {
         mImpl->mObservers.erase(found);
      }
   }

} // namespace dtUtil
