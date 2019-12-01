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
#include <dtUtil/logtimeprovider.h>
#include <dtUtil/stringutils.h>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osgDB/FileNameUtils>



#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>

#include <algorithm>
#include <cstdarg>
//#include <cstdio>
#include <dtUtil/hashmap.h>

namespace dtUtil
{
   static std::string sLogFileName = "delta3d_log.html";

#ifdef _DEBUG
   static std::string sTitle("delta3d Engine Log File (Debug Libs)");
#else
   static std::string sTitle("delta3d Engine Log File");
#endif

   //forward declaration
   class LogManager;

   static osg::ref_ptr<LogManager> LOG_MANAGER(NULL);
   static Log::LogMessageType DEFAULT_LOG_LEVEL(Log::LOG_WARNING);


   //////////////////////////////////////////////////////////////////////////
   class LogManager: public osg::Referenced
   {
   public:
      osg::ref_ptr<LogObserver> mLogObserverConsole; ///writes to console
      osg::ref_ptr<LogObserverFile> mLogObserverFile; ///writes to file
      osg::observer_ptr<osg::Referenced> mLogTimeProviderAsRef;
      LogTimeProvider* mLogTimeProvider;

      ////////////////////////////////////////////////////////////////
      LogManager()
      : mLogObserverConsole(new LogObserverConsole())
      , mLogObserverFile(new LogObserverFile())
      , mLogTimeProvider(NULL)
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
         return mInstances.insert(std::make_pair(name, osg::ref_ptr<Log>(log))).second;
      }

      ////////////////////////////////////////////////////////////////
      Log* GetInstance(const std::string& name)
      {
         dtUtil::HashMap<std::string, osg::ref_ptr<Log> >::iterator i = mInstances.find(name);
         if (i == mInstances.end())
         {
            return NULL;
         }
         return i->second.get();
      }

      ////////////////////////////////////////////////////////////////
      void SetAllLogLevels(const Log::LogMessageType& newLevel)
      {
         std::for_each(mInstances.begin(), mInstances.end(), [this, &newLevel](dtUtil::HashMap<std::string, osg::ref_ptr<Log> >::value_type& value)
         {
            Log* log = value.second.get();
            log->SetLogLevel(newLevel);
         });
      }

      ////////////////////////////////////////////////////////////////
      void SetAllOutputStreamBits(unsigned int option)
      {
         std::for_each(mInstances.begin(), mInstances.end(), [this, option](dtUtil::HashMap<std::string, osg::ref_ptr<Log> >::value_type& value)
         {
            Log* log = value.second.get();
            log->SetOutputStreamBit(option);
         });
      }


      bool IsLogTimeProviderValid() const
      {
    	  return mLogTimeProviderAsRef.valid() && mLogTimeProvider != NULL;
      }

      OpenThreads::Mutex mMutex;
   private:
      dtUtil::HashMap<std::string, osg::ref_ptr<Log> > mInstances;
   };

   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////
   const std::string LogFile::LOG_DEFAULT_NAME("");

   /** This will close the existing file (if opened) and create a new file with
    *  the supplied filename.
    * @param name : The name of the new file (will be written using HTML)
    */
   void LogFile::SetFileName(const std::string& name)
   {
      //std::cout << "LogFile try to change files to " << name << std::endl;
      bool sameName = name == sLogFileName;

      sLogFileName = name;
      if (LOG_MANAGER == NULL)
      {
         LOG_MANAGER = new LogManager;
      }
      else if (!sameName)
      {
         // reset open failed if the file name changes.
         LOG_MANAGER->mLogObserverFile->mOpenFailed = false;
         LOG_MANAGER->mLogObserverFile->OpenFile();
      }
   }

   const std::string LogFile::GetFileName()
   {
      return sLogFileName;
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
   class LogImpl //: std::stringbuf
   {
   public:
      LogImpl(const std::string& name)
      : mOutputStreamBit(Log::STANDARD)
      , mName(name)
      , mLevel(DEFAULT_LOG_LEVEL)
      , mObservers()
      {
      }

      unsigned int mOutputStreamBit; ///<the current output stream option
      std::string mName;
      Log::LogMessageType mLevel;
      Log::LogObserverContainer mObservers;
   };
//   /** Stream buffer calling notify handler when buffer is synchronized (usually on std::endl).
//    * Stream stores last notification severity to pass it to handler call.
//    */
//   struct NotifyStreamBuffer : public std::stringbuf
//   {
//       NotifyStreamBuffer() : _severity(osg::NOTICE)
//       {
//       }
//
//       void setNotifyHandler(osg::NotifyHandler *handler) { _handler = handler; }
//       osg::NotifyHandler *getNotifyHandler() const { return _handler.get(); }
//
//       /** Sets severity for next call of notify handler */
//       void setCurrentSeverity(osg::NotifySeverity severity)
//       {
//           if (_severity != severity)
//           {
//               sync();
//               _severity = severity;
//           }
//       }
//
//       osg::NotifySeverity getCurrentSeverity() const { return _severity; }
//
//   private:
//
//       int sync()
//       {
//           sputc(0); // string termination
//           if (_handler.valid())
//               _handler->notify(_severity, pbase());
//           pubseekpos(0, std::ios_base::out); // or str(std::string())
//           return 0;
//       }
//
//       osg::ref_ptr<osg::NotifyHandler> _handler;
//       osg::NotifySeverity _severity;
//   };


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
   void Log::LogMessage(const std::string& file, const std::string& method,
                        int line, const std::string& msg, LogMessageType msgType) const
   {
      if (mImpl->mOutputStreamBit == Log::NO_OUTPUT)
      {
         return;
      }

      if (msgType < mImpl->mLevel)
      {
         return;
      }


      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(LOG_MANAGER->mMutex);
      bool hasLogTimeProvider = LOG_MANAGER->IsLogTimeProviderValid();

      LogObserver::LogData logData;
      if (hasLogTimeProvider)
      {
    	  logData.frameNumber = LOG_MANAGER->mLogTimeProvider->GetFrameNumber();
    	  logData.time = LOG_MANAGER->mLogTimeProvider->GetDateTime();
      }
      else
      {
    	  logData.time.SetToLocalTime();
      }

      logData.type = msgType;
      logData.logName = mImpl->mName;
      logData.file = osgDB::getSimpleFileName(file);
      logData.method = method;
      logData.line = line;
      logData.msg = msg;


      if (dtUtil::Bits::Has(mImpl->mOutputStreamBit, Log::TO_FILE))
      {
         LOG_MANAGER->mLogObserverFile->LogMessage(logData);
      }

      if (dtUtil::Bits::Has(mImpl->mOutputStreamBit, Log::TO_CONSOLE))
      {
         LOG_MANAGER->mLogObserverConsole->LogMessage(logData);
      }

      if (dtUtil::Bits::Has(mImpl->mOutputStreamBit, Log::TO_OBSERVER) && !mImpl->mObservers.empty())
      {
         Log::LogObserverContainer::iterator itr = mImpl->mObservers.begin();
         while (itr != mImpl->mObservers.end())
         {
            (*itr)->LogMessage(logData);
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

      LogMessage("",source, line, buffer, msgType);
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
      LogMessage("", source, line, msg, msgType);
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
      if (dtUtil::StrCompare(levelString, "always", false) == 0)
      {
         return LOG_ALWAYS;
      }
      else if (dtUtil::StrCompare(levelString, "error", false) == 0)
      {
         return LOG_ERROR;
      }
      else if (dtUtil::StrCompare(levelString, "warn", false) == 0 || dtUtil::StrCompare(levelString, "warning", false) == 0)
      {
         return LOG_WARNING;
      }
      else if (dtUtil::StrCompare(levelString, "info", false) == 0 || dtUtil::StrCompare(levelString, "information", false) == 0)
      {
         return LOG_INFO;
      }
      else if (dtUtil::StrCompare(levelString, "debug", false) == 0)
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
   void Log::SetAllOutputStreamBits(unsigned int option)
   {
      if (LOG_MANAGER.valid())
      {
         LOG_MANAGER->SetAllOutputStreamBits(option);
      }
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
   void Log::SetLogTimeProvider(LogTimeProvider* ltp)
   {
	  if (LOG_MANAGER.valid())
	  {
		 LOG_MANAGER->mLogTimeProvider = ltp;
		 if (ltp != NULL)
		 {
			 LOG_MANAGER->mLogTimeProviderAsRef = ltp->AsReferenced();
		 }
		 else
		 {
			 LOG_MANAGER->mLogTimeProviderAsRef = NULL;
		 }

		 if (ltp != NULL && !LOG_MANAGER->IsLogTimeProviderValid())
		 {
			 LOG_ERROR("Unable to assign log time provider because it did not provide a referenced object as a key for deletion");
			 LOG_MANAGER->mLogTimeProvider = NULL;
		 }
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

   ////////////////////////////////////////////////////////////////////////////////
   LoggingOff::LoggingOff(const std::string& name)
   : mLog(dtUtil::Log::GetInstance(name))
   {
      mOldLevel = mLog.GetLogLevel();
      // Only ALWAYS log levels are on.
      mLog.SetLogLevel(dtUtil::Log::LOG_ALWAYS);
   }
   ////////////////////////////////////////////////////////////////////////////////
   LoggingOff::~LoggingOff()
   {
      mLog.SetLogLevel(mOldLevel);
   }


} // namespace dtUtil
