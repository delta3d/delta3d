/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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
*/

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
