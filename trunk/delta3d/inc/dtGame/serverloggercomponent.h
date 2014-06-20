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
 * Matthew W. Campbell
 */
#ifndef DELTA_SERVERLOGGERCOMPONENT
#define DELTA_SERVERLOGGERCOMPONENT

#include <set>
#include <dtGame/gmcomponent.h>
#include <dtGame/logstatus.h>

namespace dtGame
{
   class LogStream;
   class Message;
   class MachineInfo;
   class TickMessage;
   class LogKeyframe;
   class LogCaptureKeyframeMessage;
   class LogInsertTagMessage;
   class LogJumpToKeyframeMessage;

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
      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
      static const std::string DEFAULT_NAME;
      static const std::string AUTO_KEYFRAME_TIMER_NAME;

      /**
       * Constructs the logger component.
       * @param logStream The stream with which to serialize game and other state data.
       */
      ServerLoggerComponent(LogStream& logStream, dtCore::SystemComponentType& type = *TYPE);

      /*
       * Constructor that only takes a type.  This constructor allows setup of the stream later.
       * This is a TODO so this component can be created in a map.
       */
      //ServerLoggerComponent(dtCore::SystemComponentType& type = *TYPE);

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
      const LogStream& GetLogStream() const { return *mLogStream.get(); }

      /**
       * Sets the current directory where the server logger component stores
       * its log files.  If the directory does not exist it will be created.
       * @param dir A directory to store the logs.  Can be either a relative
       *    or absolute directory.
       * @note By default, the current directory is the log directory.
       */
      bool SetLogDirectory(const std::string& dir);

      /**
       * Gets the current log directory.
       * @return A string containing the current log directory.
       */
      const std::string& GetLogDirectory() const { return mLogDirectory; }

      /**
       * Gets the number of actors ignored from the recording state.
       */
      int GetIgnoredActorCount() const;

      /**
       * Gets the number of actors currently created by the playback state.
       */
      int GetPlaybackActorCount() const;

      /**
       * Determines if an actor ID is being ignored from the recording state.
       * @param id the ID of the actor in question
       */
      bool IsIgnoredActorId(const dtCore::UniqueId& id) const;

      /**
       * Determines if an actor, of ID, has been created during the playback state.
       * @param id the ID of the actor in question
       */
      bool IsPlaybackActorId(const dtCore::UniqueId& id) const;

      /**
       * Determines if an actor ID is being ignored from the recording state.
       * @param id the ID of the actor in question
       */
      bool IsIgnoredActorId(dtCore::UniqueId id);

      /**
       * Determines if an actor, of ID, has been created during the playback state.
       * @param id the ID of the actor in question
       */
      bool IsPlaybackActorId(dtCore::UniqueId id);

      /**
       * Access the set of ignored actor IDs.
       * @return Set of ignore actor IDs of actors that should not be recorded.
       */
      void GetIgnoredActorIds(std::vector<dtCore::UniqueId>& toFill) const
      {
         toFill.clear();
         for (std::set<dtCore::UniqueId>::const_iterator i = mRecordIgnoreList.begin();
            i != mRecordIgnoreList.end(); ++i)
         {
            toFill.push_back(*i);
         }
      }

      /**
       * Access the set of playback actor IDs.
       * @return Set of actor IDs of actors that are created by playback mode.
       */
      void GetPlaybackActorIds(std::vector<dtCore::UniqueId>& toFill) const
      {
         toFill.clear();
         for (std::set<dtCore::UniqueId>::const_iterator i = mPlaybackList.begin();
            i != mPlaybackList.end(); ++i)
         {
            toFill.push_back(*i);
         }
      }

   protected:
      ///Cleans up the logger component.
      virtual ~ServerLoggerComponent();

      /**
       * Handles the log status message request - sends a status message with the
       * current state of the logger.
       * @param message The message that came in from ProcessMessage()
       */
      void HandleStatusMessageRequest(const Message& message);

      /**
       * Called when a tick message is received by this component.
       * @param The incoming tick message.
       */
      void ProcessTickMessage(const TickMessage& message);

      /**
       * Handles the change state to idle message.
       * Transitions to idle state from playback state will clear all playback
       * actors from the game manager.
       * @param message The message that came in from ProcessMessage()
       */
      void HandleChangeStateIdle(const Message& message);

      /**
       * Performs some basic error checking and instructs the logger to capture a
       * keyframe.
       * @param msg The message containing the keyframe specifications.
       * @see DumpKeyFrame
       */
      void HandleCaptureKeyFrame(const LogCaptureKeyframeMessage& msg);

      /**
       * Performs some basic error checking and then jumps to the keyframe. Only in Playback.
       * @param msg The message containing the keyframe specifications.
       * @see JumpToKeyFrame
       */
      void HandleJumpToKeyFrame(const LogJumpToKeyframeMessage& message);

      /**
       * Initiates a keyframe capture.  This is a heavy operation that queries the
       * Game Manager for a list of the game actors, captures their state, and dumps
       * then to the log stream.
       * @note The keyframe is prefixed by a BEGIN_KEYFRAME_TRANSACTION message and
       *    postfixed by an END_KEYFRAME_TRANSACTION message.
       */
      void DumpKeyFrame(LogKeyframe& kf);

      /**
       * Jumps to a keyframe.  This is a heavy operation that requires many things to occur.
       * Notify the world we are starting a keyframe jump.  Change the map if the map in the
       * keyframe is different than the current map.  Progress though each actor update
       * message, sending out update and create messages.  Destroy an actors that are not
       * present in the keyframe.  Finally, change the current sim time and notify the world
       * that we have completed the jump.
       * @param kf The keyframe to jump to.
       */
      void JumpToKeyFrame(LogKeyframe& kf);

      /**
       * Handles the change state to record message.
       * @param message The message that came in from ProcessMessage()
       */
      void HandleChangeStateRecord(const Message& message);

      /**
       * Handles the change state to playback message.  This will open the current
       * log file, and jump to the keyframe marking the beginning of the log.
       * @param message The playback message.
       * @note When the end of the Log is reached, the System will go into Pause mode.
       */
      void HandleChangeStatePlayback(const Message& message);

      /**
       * Handles the request to set the log file message.
       * @param message The message that came in from ProcessMessage()
       */
      void HandleRequestSetLogFile(const Message& message);

      /**
       * Handles the request to remove a log from the server's list of logs.
       * @param message The Message that came in from ProcessMessage()
       */
      void HandleRequestDeleteLogFile(const Message& message);

      /**
       * Handles the request to get the server's available logs
       * @param message The message that came in from ProcessMessage()
       */
      void HandleRequestGetLogs(const Message& message);

      /**
       * Handles the request to get the list of tags currently present in
       * the current log.
       * @param message The message that came in from ProcessMessage()
       */
      void HandleRequestGetTags(const Message& message);

      /**
       * Handles the request to tag the log stream at the specified
       * location.
       * @param message The message that came from ProcessMessage()
       */
      void HandleRequestInsertTag(const LogInsertTagMessage& message);

      /**
       * Handles the request to get the list of keyframes currently present in
       * the current log.
       * @param message The incoming request message.
       */
      void HandleRequestGetKeyFrames(const Message& message);

      /**
       * Handles the request to set the current auto keyframe capture
       * interval.
       * @param message The incoming request message.
       */
      void HandleRequestSetAutoKeyframeInterval(const Message& message);

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
      void HandleMapLoadedMessage(const Message& message);

      /**
       * Handles the request to add an actor to the playback join list.
       * @param message Contains the ID of the actor to be joining playback.
       */
      void HandleAddPlaybackActorMessage(const Message& message);

      /**
       * Handles the request to remove an actor from the playback join list.
       * @param message Contains the ID of the actor exiting playback.
       */
      void HandleRemovePlaybackActorMessage(const Message& message);

      /**
       * Handles the request to clear the joining playback actor list.
       */
      void HandleClearPlaybackListMessage();

      /**
       * Handles the request add an actor to the ignore list.
       * @param message Contains the ID of the actor to be ignored.
       */
      void HandleAddIgnoredActorMessage(const Message& message);

      /**
       * Handles the request remove an actor from the ignore list.
       * @param message Contains the ID of the actor be removed from
       * the recording ignore list.
       */
      void HandleRemoveIgnoredActorMessage(const Message& message);

      /**
       * Handles the request to clear the ignored actor list.
       */
      void HandleClearIgnoreListMessage();

      /**
       *  Add a MessageType to the list of Messages to not record.
       *  @param message a LogIgnoreMessageTypeMessage containing the MessageType to ignore
       */
      void HandleAddIgnoredMessageTypeMessage(const Message& message);

      /**
       *  Remove a MessageType to the list of Messages to not record.
       *  @param message a LogIgnoreMessageTypeMessage containing the MessageType to stop ignoring
       */
      void HandleRemoveIgnoredMessageTypeMessage(const Message& message);

      /**
       *  Stop ignoring all previously ignored MessageTypes
       */
      void HandleClearIgnoredMessageTypeMessage();

      /**
       * Called when a message we received in ProcessMessage() is a candidate for writing.
       * It determines what state we're in and write the message.  A failure here does
       * not cause any wierd behavior or change any state, but it should do a log.info message.
       */
      void DoRecordMessage(const Message& message);

      /**
       * Internal utility method for common behavior to set the state to idle.
       * Transitions to idle state from playback state will clear all playback
       * actors from the game manager.
       */
      void SetToIdleState();

      /**
       * Tells the GameManager to delete all actors created by playback mode.
       * All actors, whose IDs are contained in the playback actor list, will
       * be deleted on the next tick of the GameManager.
       * @return Total number of actors requested to be deleted.
       */
      int RequestDeletePlaybackActors();

      /**
       * Check if the supplied Message is okay to record.
       * @return true if message should be written, false otherwise
       */
      bool OkToRecord(const Message& message) const;

   private:
      /**
       * Determines if the specified actor ID exists within the specified list.
       * @param actorID The ID of the actor in question
       * @param list The list of IDs that may contain the queried actorID
       * @return bool True if the ID has been found in the list
       */
      bool IsActorIdInList(const dtCore::UniqueId& actorID, const std::set<dtCore::UniqueId>& checkedSet) const;

      /**
       * Determines if the specified actor ID exists within the specified list.
       * @param actorID The ID of the actor in question
       * @param list The list of IDs that may contain the queried actorID
       * @return bool True if the ID has been found in the list
       */
      bool IsActorIdInList(dtCore::UniqueId actorID, std::set<dtCore::UniqueId>& checkedSet);

      LogStatus mLogStatus;
      dtCore::RefPtr<LogStream> mLogStream;
      dtCore::RefPtr<Message> mNextMessage;
      double mNextMessageSimTime;

      // in order to indicate that messages came from the server logger, we use a
      // bogus machine info.  Over time, this will probably be more sophisticated.
      // The GM and network components will probably have to know about it.
      dtCore::RefPtr<MachineInfo> mLogComponentMachineInfo;

      //The relative directory containing all the log files available to the
      //server logger component.
      std::string mLogDirectory;

      //Cached list of available log files.
      std::set<std::string> mLogCache;

      //The ignore list for actors to be ignored during the record state.
      std::set<dtCore::UniqueId> mRecordIgnoreList;

      ///The list of MessageTypes that should be ignored during the record state.
      std::set<const dtGame::MessageType*> mIgnoredMessageTypeList;

      //The list of actor ids of actors who join the simulation during
      //the playback state.
      std::set<dtCore::UniqueId> mPlaybackList;

      // Previous log status before map load sets it to idle
      const LogStateEnumeration* mPreviousLogState;

      // Previous record duration before map load resets it
      double mPreviousRecordDuration;

      // Previous number of messages before map load reset it
      unsigned long mPreviousNumberOfMessages;
   };

} // namespace dtGame

#endif // DELTA_SERVERLOGGERCOMPONENT
