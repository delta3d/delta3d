#ifndef NOTIFY_H_INCLUDED
#define NOTIFY_H_INCLUDED

#include <ostream>

namespace dtCore
{
   
 /** Range of notify levels from DEBUG_INFO through to FATAL, ALWAYS
   * is reserved for forcing the absorption of all messages.  
   */
   enum NotifySeverity {
      ALWAYS=0,
         FATAL=1,
         WARN=2,
         NOTICE=3,
         INFO=4,
         DEBUG_INFO=5
   };
   

   ///Set the notification level
   extern void SetNotifyLevel( NotifySeverity severity, 
                               NotifySeverity osgSeverity = FATAL );


   ///Display a notification to the console using printf format
   extern void Notify(NotifySeverity level, const char *fmt, ...);

   ///Display a notification to the console using cout format
   extern std::ostream& Notify(const NotifySeverity severity);

};


#undef _AUTOLIBNAME

#if defined(_DEBUG)
#define _AUTOLIBNAME "osgD.lib"
#else
#define _AUTOLIBNAME "osg.lib"
#endif

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
#pragma message( "Will automatically link with " _AUTOLIBNAME )
#endif

#pragma comment(lib, _AUTOLIBNAME)


#endif
