#include "dtCore/notify.h"
#include <string>
#include <iostream>
#include <fstream>
#include <osg/Notify>
#include <stdarg.h>
#include <stdio.h>

static dtCore::NotifySeverity NotifyLevel = dtCore::NOTICE;

/// Used internally
static bool InitNotifyLevel()
{
   static bool s_NotifyInit = false;
   
   if (s_NotifyInit) return true;
   
   s_NotifyInit = true;
   
   
   NotifyLevel = dtCore::NOTICE; // Default value  
      
   return true;  
}

 
/*!
 * Set the notify level for outgoing messages.  Optionally set the notify
 * level for OpenSceneGraph messages as well, otherwise only the FATAL will
 * be outputted.
 *
 * @param severity : The level of dtCore messages that should be displayed.
 * @param osgSeverity : The level of OSG messages that should be displayed.
 */
void dtCore::SetNotifyLevel(dtCore::NotifySeverity severity, NotifySeverity osgSeverity)
{
   InitNotifyLevel();
   
   NotifyLevel = severity;

   //also set the OSG notify level to match
   osg::NotifySeverity level;
   switch (osgSeverity)
   {
   case dtCore::ALWAYS: level = osg::ALWAYS;       break;
   case dtCore::FATAL:  level = osg::FATAL;        break;
   case dtCore::WARN:   level = osg::WARN;         break;
   case dtCore::NOTICE: level = osg::NOTICE;       break;
   case dtCore::INFO:   level = osg::INFO;         break;
   case dtCore::DEBUG_INFO:  level = osg::DEBUG_INFO;   break;
   default: level = osg::NOTICE;
   }
   
   osg::setNotifyLevel(level);
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

 

/*!
 * Notify mechanism can be used to output text to the console.  A global
 * severity level can be set to control what level of output should be
 * displayed.  \n
 * Usage is just like a printf, for example:\n
 * \code  Notify(INFO, "hello world %d", 5); \endcode
 *
 * @param level : Level of severity of the message
 * @param fmt : The format of the message (printf-like)
 */
void dtCore::Notify(dtCore::NotifySeverity level, const char *fmt, ...)
{
   char t[255] = {0}; 
   
   static bool initialized = InitNotifyLevel();
   
   if (level <= NotifyLevel)
   {
      va_list argptr;
      va_start( argptr, fmt );

      #if defined(_WIN32) || defined(WIN32)
      _vsnprintf(t, sizeof(t), fmt, argptr );
      #else
      vsnprintf(t, sizeof(t), fmt, argptr );
      #endif
      
      va_end( argptr );
      
      std::cout << "dtCore-";
      PrintSeverity(level);
      std::cout << ":" << t << std::endl;
   }
   
}


#if defined(WIN32) && !(defined(__CYGWIN__) || defined(__MINGW32__))
static const char* NullStreamName = "nul";
#else
static const char* NullStreamName = "/dev/null";
#endif

 
/*!
 * Notify mechanism can be used to output text to the console.  A global
 * severity level can be set to control what level of output should be
 * displayed.  \n
 * Usage is just like a cout, for example:\n
 * \code  Notify(INFO) << "hello world " << 5 << std::endl; \endcode
 *
 * @param severity : Level of severity of the message
 */
std::ostream& dtCore::Notify(const dtCore::NotifySeverity severity)
{
   // set up global notify null stream for inline notify
   static std::ofstream s_NotifyNulStream(NullStreamName);
   
   static bool initialized = false;
   if (!initialized) 
   {
      std::cerr<<""; // dummy op to force construction of cerr, before a reference is passed back to calling code.
      std::cout<<""; // dummy op to force construction of cout, before a reference is passed back to calling code.
      initialized = InitNotifyLevel();
   }
   
   if (severity<=NotifyLevel)
   {
      if (severity<=dtCore::WARN) return std::cerr;
      else 
      {
         std::cout << "dtCore-";
         PrintSeverity(severity);
         std::cout << ":";
         return std::cout;
      }
   }
   return s_NotifyNulStream;
}
