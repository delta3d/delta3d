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
#ifndef DELTA_SERVERLOGGERCOMPONENT
#define DELTA_SERVERLOGGERCOMPONENT

#include <dtCore/refptr.h>
#include "dtGame/gmcomponent.h"
#include "dtGame/logstream.h"
#include "dtGame/loggermessages.h"
#include "dtGame/basemessages.h"
#include "dtGame/logstatus.h"

namespace dtGame 
{
   
   /**
    * This is a GameManager component that servers as the primary component 
    * for trapping and logging all game messages that progress through the 
    * system.  The component itself is message driven such that a LogContoller
    * component must exist in the system instructing this component what to do.
    * Under the covers it contains a LogStream which serializes the actual game 
    * messages to disk or other storage medium.
    * @see LogController
    * @see LogStream
    */
   class DT_GAME_EXPORT ServerLoggerComponent : public GMComponent
   {      
      public:
         
         /**
          * Constructs the logger component.
          * @param logStream The stream with which to serialize game and other state data.
          */          
         ServerLoggerComponent(LogStream &logStream);
         
         /**
          * Called when a message arrives at the Game Manager.  In most cases the message is 
          * forwarded to the LogStream which serializes the message.
          * @param message The message to process.
          * @note Some of these messages may be logger command messages.  These are not logged
          *    however, are used to drive this component.
          */
         virtual void ProcessMessage(const Message& message);
         
         /**
          * Gets the LogStream in use by this logger component.
          * @return A constant reference to the log stream interface.
          */
         const LogStream &GetLogStream() const { return *mLogStream.get(); }
         
      protected:
      
         ///Cleans up the logger component.
         virtual ~ServerLoggerComponent();
         
         /**
          * Handles the log status message request - sends a status message with the 
          * current state of the logger.
          * @param message The message that came in from ProcessMessage()
          */
         void HandleStatusMessageRequest(const Message &message);
         
         /**
          * Called when a tick message is received by this component.
          * @param The incoming tick message.
          */
         void ProcessTickMessage(const TickMessage &message);

         /**
          * Handles the change state to idle message.  
          * @param message The message that came in from ProcessMessage()
          */
         void HandleChangeStateIdle(const Message &message);
         
         /**
          * Performs some basic error checking and instructs the logger to capture a
          * keyframe.
          * @param msg The message containing the keyframe specifications.  
          * @see DumpKeyFrame
          */
         void HandleCaptureKeyFrame(const LogCaptureKeyframeMessage &msg);
         
         /**
          * Initiates a keyframe capture.  This is a heavy operation that queries the
          * Game Manager for a list of the game actors, captures their state, and dumps
          * then to the log stream.  
          * @note The keyframe is prefixed by a BEGIN_KEYFRAME_TRANSACTION message and
          *    postfixed by an END_KEYFRAME_TRANSACTION message.
          */ 
         void DumpKeyFrame(LogKeyframe &kf);
         
         /**
          * Jumps to a keyframe.  This is a heavy operation that requires many things to occur.
          * Notify the world we are starting a keyframe jump.  Change the map if the map in the
          * keyframe is different than the current map.  Progress though each actor update
          * message, sending out update and create messages.  Destroy an actors that are not
          * present in the keyframe.  Finally, change the current sim time and notify the world
          * that we have completed the jump.
          * @param kf The keyframe to jump to.
          */
         void JumpToKeyFrame(LogKeyframe &kf);
         
         /**
          * Handles the change state to record message.  
          * @param message The message that came in from ProcessMessage()
          */
         void HandleChangeStateRecord(const Message &message);

         /**
          * Handles the change state to playback message.  This will open the current
          * log file, and jump to the keyframe marking the beginning of the log.
          * @param message The playback message.
          */
         void HandleChangeStatePlayback(const Message &message);
         
         /**
          * Handles the request to set the log file message.
          * @param message the message that came in from ProcessMessage()
          */
         void HandleRequestSetLogFile(const Message &message);

         /**
          * Lots of methods send out a status, so make one method to do it
          * @param destination Some messages send status to one client, most send to all
          */
         void DoSendStatusMessage(const MachineInfo *destination);

         /**
          * When a map has been loaded, each client and the server, will likely send 
          * an info map loaded message.  We want to listen for the GM's instance of 
          * the info map loaded message and then update our status with the active map.
          */
         void HandleMapLoadedMessage(const Message &message);

         /**
          * Called when a message we received in ProcessMessage() is a candidate for writing.
          * It determines what state we're in and write the message.  A failure here does 
          * not cause any wierd behavior or change any state, but it should do a log.info message.
          */
         void DoRecordMessage(const Message &message);
         
         /**
          * Internal utility method for common behavior to set the state to idle
          */
         void SetToIdleState();
         
      private:
         LogStatus mLogStatus;
         dtCore::RefPtr<LogStream> mLogStream;
         dtCore::RefPtr<Message> mNextMessage;
         double mNextMessageSimTime;

         // in order to indicate that messages came from the server logger, we use a 
         // bogus machine info.  Over time, this will probably be more sophisticated. 
         // The GM and network components will probably have to know about it.
         dtCore::RefPtr<MachineInfo> mLogComponentMachineInfo; 
   };

}

#endif 
