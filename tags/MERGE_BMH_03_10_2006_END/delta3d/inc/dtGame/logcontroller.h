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
* @author Curtiss Murphy
*/
#ifndef DELTA_LOGCONTROLLER
#define DELTA_LOGCONTROLLER

#include <string>
#include "dtGame/gmcomponent.h"
#include "dtGame/logstatus.h"
#include "dtGame/logkeyframe.h"
#include "dtCore/sigslot.h"

namespace dtGame
{
   class LogTag;
   
   /**
    * The Log Controller is a GM Component 
    * @see class dtGame::GameManager
    * @see class dtGame::GMComponent
    */
   class DT_GAME_EXPORT LogController : public GMComponent
   {
      public:
      
         LogController();
         
         virtual void ProcessMessage(const Message& message);

         /**
          * Sends a request to the server logger component to tell it to 
          * change state to PLAYBACK.  Only valid in Idle state (sends request anyway).  
          */
         virtual void RequestChangeStateToPlayback();
         
         /**
          * Sends a request to the server logger component to tell it to 
          * change state to RECORD.  Only valid in Idle state (sends request anyway).
          */
         virtual void RequestChangeStateToRecord();
         
         /**
          * Sends a request to the server logger component to tell it to change state to 
          * IDLE.  Only valid in Record or Playback states (sends request anyway).  This 
          * message will terminate any currently active record or playback and cause 
          * all open logger files to be closed and released.  
          */
         virtual void RequestChangeStateToIdle();
        
         /**
          * Sends a request to the server logger component to tell it to 
          * insert a new keyframe.  Primarily used in Record.  May work in 
          * playback mode depending on the server's implementation and the logstream.
          * @param keyframe The keyframe information to send to the server.
          */
         virtual void RequestCaptureKeyframe(const LogKeyframe &keyframe);

         /**
          * Sends a request to the server logger component to ask it to 
          * send out the current list of keyframes.  Valid in Playback or Record.
          */
         virtual void RequestServerGetKeyframes();

         /**
          * Sends a request to the server logger component to ask it to 
          * send the current logfiles.  Valid in Playback or Record.
          */
         virtual void RequestServerGetLogfiles();

         /**
          * Sends a request to the server logger component to ask it to 
          * send the current tags.  Valid in Playback or Record.
          */
         virtual void RequestServerGetTags();

         /**
          * Sends a request to the server logger component to ask it to 
          * send a current logger status message.  Valid anytime.
          */
         virtual void RequestServerGetStatus();

         /**
          * Sends a request to the server logger component to tell it to 
          * insert a new tag.  Primarily used in Record.  May work in 
          * playback mode depending on the server's implementation and the logstream.
          * @param logTag The tag information to send to the server.
          */
         virtual void RequestInsertTag(const LogTag &tag);

         /**
          * Sends a request to the server logger component to tell it to delete the specified 
          * log file by name.  Only valid in Idle state (sends request anyway).
          * @param logFile The log file to tell the server to delete.
          */
         virtual void RequestDeleteLogFile(const std::string &logFile);

         /**
          * Sends a request to the server logger component to tell it to change the log 
          * file name.  Only valid in Idle state (sends request anyway).  A successful set
          * should cause a status message, error (like not idle state) sends a reject.
          * @param logFile The log file to send to the server.
          */
         virtual void RequestSetLogFile(const std::string &logFile);

         /**
          * Sends a request to the server logger component to change the auto keyframe 
          * intervale (in seconds).  If this interval is not 0 on the server, it will 
          * automatically log a keyframe at this interval.  It is STRONGLY recommended 
          * that you keep this interval far apart (like 5-10 minutes).  A keyframe is a 
          * fairly expensive operation.  If you set this interval small, your performance 
          * will most likely greatly suffer.
          * @param interval The interval to send to the server.
          */
         virtual void RequestSetAutoKeyframeInterval(double interval);

         /**
          * Returns the last received LogStatus.  No Set, since only this object should set it.
          */
         const LogStatus &GetLastKnownStatus() { return mLastKnownStatus; }
         
         /**
          * Gets the list of logs cached since the last query from the server logger component.
          * @return A list of logs stored as strings.
          */
         const std::vector<std::string> &GetLastKnownLogList() const 
         { 
            return mLastKnownLogList; 
         }
         
         /**
          * Gets the list of keyframes cached since the last query to the server logger
          * component.
          * @return A list of keyframes.
          */
         const std::vector<LogKeyframe> &GetLastKnownKeyframeList() const
         {
            return mLastKnownKeyframeList;
         }
         
         /**
          * Gets the list of tags cached since the last query to the server logger
          * component.
          * @return A list of tags.
          */
         const std::vector<LogTag> &GetLastKnownTagList() const
         {
            return mLastKnownTagList;
         }

         /**
          * Delta3D signal/slot - sent when a logger status message is received by 
          * the logger controller component.  Bind to this with something like this:
          *    myController->SignalReceivedStatus().connect_slot(this, &MyClass::MySlotMethod)
          * @return the signal
          */
         sigslot::signal1<const LogStatus &> &SignalReceivedStatus() { return _receivedStatus; }

         /**
          * Delta3D signal/slot - sent when a logger rejection message is received by 
          * the logger controller component.  Bind to this with something like this:
          *    myController->SignalReceivedRejection().connect_slot(this, &MyClass::MySlotMethod)
          * @return the signal
          */
         sigslot::signal1<const Message &> &SignalReceivedRejection() { return _receivedRejection; }

      protected:
      
         virtual ~LogController();

      private:
         LogStatus mLastKnownStatus;
         
         //Holds the list of keyframes retreived from the last request.
         std::vector<LogKeyframe> mLastKnownKeyframeList;
         
         //Holds the list of tags retreived from the last request.
         std::vector<LogTag> mLastKnownTagList;
         
         //This list stores the list of logs available since the last query to the server logger component.
         std::vector<std::string> mLastKnownLogList;

         // The signal that gets triggered when processMessage receives a LOG_INFO_STATUS message
         sigslot::signal1<const LogStatus &> _receivedStatus;
         // The signal that gets triggered when processMessage receives a rejection message
         sigslot::signal1<const Message &> _receivedRejection;

   };
   
}

#endif 
