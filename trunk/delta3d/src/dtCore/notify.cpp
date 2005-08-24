#include "dtCore/notify.h"
#include <string>
#include <iostream>
#include <fstream>
#include <osg/Notify>
#include <stdarg.h>
#include <stdio.h>


 
///Deprecated 8/23/05
void dtCore::SetNotifyLevel(dtCore::NotifySeverity severity, NotifySeverity osgSeverity)
{
   DEPRECATE("void SetNotifyLevel( NotifySeverity severity, NotifySeverity osgSeverity ) ",
      "dtUtil::Log::SetLogLevel(LogMessageType msgType)" )

   dtUtil::Log::LogMessageType logType;

   switch(severity) {
      case ALWAYS: logType = dtUtil::Log::LOG_ALWAYS;  	break;
      case FATAL: logType = dtUtil::Log::LOG_ERROR;  	break;
      case WARN: logType = dtUtil::Log::LOG_WARNING;  	break;
      case NOTICE: logType = dtUtil::Log::LOG_INFO;  	break;
      case INFO: logType = dtUtil::Log::LOG_INFO;  	break;
      case DEBUG_INFO: logType = dtUtil::Log::LOG_DEBUG;  	break;
      default: logType = dtUtil::Log::LOG_INFO;
   }

   dtUtil::Log::GetInstance().SetLogLevel(logType);
}





/*!
 * Output the string for the given severity level.
 * @param level The severity level to output the string of
 */
inline static void PrintSeverity(const dtCore::NotifySeverity level)
{
   switch(level)
   {
   case dtCore::ALWAYS: std::cout << "Always";  break;
   case dtCore::FATAL:  std::cout << "Fatal";   break;
   case dtCore::WARN:   std::cout << "Warn";    break;
   case dtCore::NOTICE: std::cout << "Notice";  break;
   case dtCore::INFO:   std::cout << "Info";    break;
   case dtCore::DEBUG_INFO:  std::cout << "Debug";   break;
   default:
      break;
   }
}

///Deprecated 8/23/05
void dtCore::Notify(dtCore::NotifySeverity level, const char *fmt, ...)
{
   DEPRECATE("void dtCore::Notify(NotifySeverity level, const char *fmt, ...)",
      "void dtUtil::Log::LogMessage(LogMessageType msgType, const std::string &source, const char *msg, ...)");


   dtUtil::Log::LogMessageType logType;

   switch(level) {
      case ALWAYS: logType = dtUtil::Log::LOG_ALWAYS;  	break;
      case FATAL: logType = dtUtil::Log::LOG_ERROR;  	break;
      case WARN: logType = dtUtil::Log::LOG_WARNING;  	break;
      case NOTICE: logType = dtUtil::Log::LOG_INFO;  	break;
      case INFO: logType = dtUtil::Log::LOG_INFO;  	break;
      case DEBUG_INFO: logType = dtUtil::Log::LOG_DEBUG;  	break;
      default: logType = dtUtil::Log::LOG_INFO;
   }

   static char buffer[1024];
   va_list list;
   va_start(list, fmt);
   vsprintf(buffer, fmt, list);
   va_end(list);

   dtUtil::Log::GetInstance().LogMessage(logType, "---", buffer);
}


///Deprecated 8/23/05
std::ostream& dtCore::Notify(const dtCore::NotifySeverity severity)
{
   DEPRECATE("std::ostream& Notify(const NotifySeverity severity)",
      "void dtUtil::Log::LogMessage(LogMessageType msgType, const std::string &source, const char *msg, ...)")

   return std::cout;
}

