#ifndef DELTA_NOTIFY
#define DELTA_NOTIFY


#include <ostream>

#include "dtCore/export.h"

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
   extern DT_EXPORT void SetNotifyLevel( NotifySeverity severity, 
                                         NotifySeverity osgSeverity = FATAL );


   ///Display a notification to the console using printf format
   extern DT_EXPORT void Notify(NotifySeverity level, const char *fmt, ...);

   ///Display a notification to the console using cout format
   extern DT_EXPORT std::ostream& Notify(const NotifySeverity severity);

};

#endif // DELTA_NOTIFY
