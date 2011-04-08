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
#include <iomanip>
#include <iostream>
#include <stdarg.h>
#include <time.h>

namespace dtUtil 
{
    const std::string Log::mDefaultName("__+default+__");

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
                //First attempt to create the log file.
                logFile.open("delta3d_log.html");
                if (!logFile.is_open()) 
                {
                    std::cout << "could not open file \"delta3d_log.html\"" << std::endl;
                    return;
                } 
                else 
                {
                   //std::cout << "Using file \"delta3d_log.html\" for logging" << std::endl;
                }
                //Write a decent header to the html file.
                logFile << "<html><title>Delta 3D Engine</title><body>" << std::endl;
                logFile << "<h1 align=\"center\">Delta 3D Engine Log File</h1><hr>" << std::endl;
                logFile << "<pre><h3 align=\"center\""
                            "<font color=#808080><b>  Debug     </b></font>"
                            "<font color=#008080><b>  Information     </b></font>"
                            "<font color=#808000><b>  Warning  </b></font>"
                            "<font color=#FF0000><b>  Error   </b></font></h3></pre><hr>"
                        << std::endl;

                logFile.flush();
            }
            //std::cout.flush();
        }

        ~LogManager() 
        {
            //std::cout << "BEING DESTROYED - LogManager" << std::endl;
            //std::cout.flush();
            if (logFile.is_open())
            {
                //std::cout << "Closing log file" << std::endl;
                //std::cout.flush();
                logFile << "</body></html>" << std::endl;
                logFile.flush();
                //Log::logFile.close();
            }
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

    //////////////////////////////////////////////////////////////////////////
    Log::Log()
        :mLevel(LOG_WARNING)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    Log::~Log()
    {
    }



    //////////////////////////////////////////////////////////////////////////
    void Log::LogMessage(const std::string &source, int line, const std::string &msg,
                LogMessageType msgType)
    {
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

        manager->logFile << color << GetLogLevelString(msgType) << ": "
                          << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
                          << std::setw(2) << std::setfill('0') << t->tm_min << ":"
                          << std::setw(2) << std::setfill('0') << t->tm_sec << ": &lt;"
                          << source;
        if (line > 0)
            manager->logFile << ":" << line;

        manager->logFile << "&gt; " << msg << "</font></b><br>" << std::endl;
        
        std::cout << GetLogLevelString(msgType) << ": "
           << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
           << std::setw(2) << std::setfill('0') << t->tm_min << ":"
           << std::setw(2) << std::setfill('0') << t->tm_sec << ":<"
           << source << ":" << line << ">" << msg << std::endl;

        manager->logFile.flush(); //Make sure everything is written, in case of a crash.
    }

    //////////////////////////////////////////////////////////////////////////
    void Log::LogMessage(LogMessageType msgType, const std::string &source,
                            const char *msg, ...) 
    {
        static char buffer[2049];
        va_list list;
        struct tm *t;
        time_t cTime;
        std::string color;

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

        manager->logFile << color << GetLogLevelString(msgType) << ": "
                          << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
                          << std::setw(2) << std::setfill('0') << t->tm_min << ":"
                          << std::setw(2) << std::setfill('0') << t->tm_sec << ": &lt;"
                          << source << "&gt; " << buffer << "</font></b><br>" << std::endl;

        std::cout << GetLogLevelString(msgType) << ": "
           << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
           << std::setw(2) << std::setfill('0') << t->tm_min << ":"
           << std::setw(2) << std::setfill('0') << t->tm_sec << ":<"
           << source << ">" << buffer << std::endl;

        manager->logFile.flush();
    }

    //////////////////////////////////////////////////////////////////////////
    void Log::LogMessage(LogMessageType msgType, const std::string &source, int line,
                            const char *msg, ...)
    {
        static char buffer[2049];
        va_list list;
        struct tm *t;
        time_t cTime;
        std::string color;

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

        manager->logFile << color << GetLogLevelString(msgType) << ": "
                          << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
                          << std::setw(2) << std::setfill('0') << t->tm_min << ":"
                          << std::setw(2) << std::setfill('0') << t->tm_sec << ": &lt;"
                          << source << ":" << line << "&gt; " << buffer << "</font></b><br>" << std::endl;


        std::cout << GetLogLevelString(msgType) << ": "
           << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
           << std::setw(2) << std::setfill('0') << t->tm_min << ":"
           << std::setw(2) << std::setfill('0') << t->tm_sec << ":<"
           << source << ":" << line << ">" << buffer << std::endl;

        manager->logFile.flush();
    }

    //////////////////////////////////////////////////////////////////////////
    void Log::LogHorizRule()
    {
        if (!manager->logFile.is_open())
            return;

        manager->logFile << "<hr>" << std::endl;
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
    std::string Log::GetLogLevelString( LogMessageType msgType)
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

} //end namespace