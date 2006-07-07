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
#include <dtUtil/log.h>
#include <dtUtil/bits.h>
#include <iomanip>
#include <iostream>
#include <stdarg.h>
#include <time.h>

namespace dtUtil 
{
   const std::string Log::mDefaultName("__+default+__");
   static const char *sLogFileName = "delta3d_log.html";

#ifdef _DEBUG
static const char *sTitle = "Delta 3D Engine Log File (Debug Libs)";
#else
static const char *sTitle = "Delta 3D Engine Log File";
#endif

   //////////////////////////////////////////////////////////////////////////

   class LogManager: public osg::Referenced 
   {
   public:
      std::ofstream logFile;

      LogManager() 
      {
         //std::cout << "Creating logger" << std::endl;

         if (!logFile.is_open()) 
         {
            OpenFile();
         }
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
         return mInstances.insert(std::make_pair(name, osg::ref_ptr<Log>(log))).second;
      }

      Log* GetInstance(const std::string& name) 
      {
         std::map<std::string, osg::ref_ptr<Log> >::iterator i = mInstances.find(name);
         if (i == mInstances.end()) 
         {
            return NULL;
         }
         return i->second.get();
      }

   private:
      std::map<std::string, osg::ref_ptr<Log> > mInstances;
   };

   static osg::ref_ptr<LogManager> manager(NULL);

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
      sTitle = title.c_str();
   }

   const std::string LogFile::GetTitle()
   {
      return std::string(sTitle);
   }

   //////////////////////////////////////////////////////////////////////////
   Log::Log()
      :mLevel(LOG_WARNING),
      mOutputStreamBit(Log::STANDARD)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   Log::~Log()
   {
   }



   //////////////////////////////////////////////////////////////////////////
   void Log::LogMessage(const std::string &source, int line, const std::string &msg,
                LogMessageType msgType) const
   {
      if (mOutputStreamBit == Log::NO_OUTPUT) 
         return;

      if (msgType < mLevel)
         return;

      if (!manager->logFile.is_open())
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

      if (dtUtil::Bits::Has(mOutputStreamBit, Log::TO_FILE))
      {
         manager->logFile << color << GetLogLevelString(msgType) << ": "
            << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << t->tm_min << ":"
            << std::setw(2) << std::setfill('0') << t->tm_sec << ": &lt;"
            << source;
         if (line > 0)
            manager->logFile << ":" << line;

         manager->logFile << "&gt; " << msg << "</font></b><br>" << std::endl;
        
         manager->logFile.flush(); //Make sure everything is written, in case of a crash.
      }

      if (dtUtil::Bits::Has(mOutputStreamBit, Log::TO_CONSOLE))
      {
         std::cout << GetLogLevelString(msgType) << ": "
            << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << t->tm_min << ":"
            << std::setw(2) << std::setfill('0') << t->tm_sec << ":<"
            << source << ":" << line << ">" << msg << std::endl;
      }

   }

   //////////////////////////////////////////////////////////////////////////
   void Log::LogMessage(LogMessageType msgType, const std::string &source,
                            const char *msg, ...) const
   {
      static char buffer[2049];
      va_list list;
      struct tm *t;
      time_t cTime;
      std::string color;

      if (mOutputStreamBit == Log::NO_OUTPUT) 
         return;

      if (msgType < mLevel)
         return;

      if (!manager->logFile.is_open())
         return;

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

      va_start(list,msg);
      vsprintf(buffer,msg,list);
      va_end(list);

      if (dtUtil::Bits::Has(mOutputStreamBit, Log::TO_FILE))
      {
         manager->logFile << color << GetLogLevelString(msgType) << ": "
            << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << t->tm_min << ":"
            << std::setw(2) << std::setfill('0') << t->tm_sec << ": &lt;"
            << source << "&gt; " << buffer << "</font></b><br>" << std::endl;
 
         manager->logFile.flush();
      }

      if (dtUtil::Bits::Has(mOutputStreamBit, Log::TO_CONSOLE))
      {
         std::cout << GetLogLevelString(msgType) << ": "
            << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << t->tm_min << ":"
            << std::setw(2) << std::setfill('0') << t->tm_sec << ":<"
            << source << ">" << buffer << std::endl;
      }

   }

   //////////////////////////////////////////////////////////////////////////
   void Log::LogMessage(LogMessageType msgType, const std::string &source, int line,
                            const char *msg, ...) const
   {
      static char buffer[2049];
      va_list list;
      struct tm *t;
      time_t cTime;
      std::string color;

      if (mOutputStreamBit == Log::NO_OUTPUT) 
         return;

      if (msgType < mLevel)
         return;

      if (!manager->logFile.is_open())
         return;

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

      va_start(list,msg);
      vsprintf(buffer,msg,list);
      va_end(list);

      if (dtUtil::Bits::Has(mOutputStreamBit, Log::TO_FILE))
      {
         manager->logFile << color << GetLogLevelString(msgType) << ": "
            << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << t->tm_min << ":"
            << std::setw(2) << std::setfill('0') << t->tm_sec << ": &lt;"
            << source << ":" << line << "&gt; " << buffer << "</font></b><br>" << std::endl;
 
         manager->logFile.flush();
      }


      if (dtUtil::Bits::Has(mOutputStreamBit, Log::TO_CONSOLE))
      {
         std::cout << GetLogLevelString(msgType) << ": "
            << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << t->tm_min << ":"
            << std::setw(2) << std::setfill('0') << t->tm_sec << ":<"
            << source << ":" << line << ">" << buffer << std::endl;
      }

   }

   //////////////////////////////////////////////////////////////////////////
   void Log::LogHorizRule()
   {
      if (!manager->logFile.is_open())
         return;

      if (mOutputStreamBit == Log::NO_OUTPUT) 
         return;

      if (dtUtil::Bits::Has(mOutputStreamBit, Log::TO_FILE))
      {
         manager->logFile << "<hr>" << std::endl;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   Log &Log::GetInstance()
   {
      return GetInstance(mDefaultName);
   }

   //////////////////////////////////////////////////////////////////////////
   Log &Log::GetInstance(const std::string& name)
   {
      if (manager == NULL)
         manager = new LogManager;
      Log* l = manager->GetInstance(name);
      if (l == NULL) 
      {
         l = new Log;
         manager->AddInstance(name, l);
      }

      return *l;
   }

   //////////////////////////////////////////////////////////////////////////
    const std::string Log::GetLogLevelString( LogMessageType msgType) const
   {
      std::string lev;

      switch(msgType)
      {
      case Log::LOG_ALWAYS:    lev = "Always";  break;
      case Log::LOG_ERROR:   lev = "Error";   break;
      case Log::LOG_WARNING: lev = "Warn";    break;
      case Log::LOG_INFO:    lev = "Info";    break;
      case Log::LOG_DEBUG:   lev = "Debug";   break;
      default:
         break;
      }

      return lev;
   }

   /** Tell the Log where to send output messages.  The supplied parameter is a
    *  bitwise combination of OutputStreamOptions.  The default is STANDARD, which 
    *  directs messages to both the console and the output file.
    *  For example, to tell the Log to output to the file and console:
    *  \code 
    *   dtUtil::Log::GetInstance().SetOutputStreamBit(dtUtil::Log::TO_FILE | dtUtil::Log::TO_CONSOLE);
    *  \endcode
    *  \param option A bitwise combination of options.
    */
   void Log::SetOutputStreamBit(unsigned int option)
   {
      mOutputStreamBit = option;
   }

   unsigned int Log::GetOutputStreamBit() const
   {
      return mOutputStreamBit;
   }
} //end namespace
