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
* Curtiss Murphy
*/

#ifndef DELTA_LOGGERMESSAGES
#define DELTA_LOGGERMESSAGES

#include <dtGame/message.h>
#include <dtGame/messageparameter.h>
#include <dtGame/basemessages.h>

namespace dtUtil
{
   class DataStream;
}

namespace dtGame
{
   class LogKeyframe;
   class LogTag;
   class LogStatus;

   /**
    * This message class is sent by the ServerLoggerComponent - it has logger status info;
    * it provides the complete status of the server logger component. Can be sent at anytime. 
    * Can ONLY be sent by the server.  All information is from the server logger's perspective.
    * The same message is used for status for all states, so not all parameters will always be relevant.
    * Note that this message wraps the status params with a LogStatus object.
    * @par
    *    Related Message Types: LOG_INFO_STATUS\n
    *    Parameter: CurrentSimTime: The server's current sim time (always set, any state, double).\n
    *    Parameter: ActiveMaps: The current loaded set (may be empty, any state, string).\n
    *    Parameter: LogFile: The current log file (may be empty, any state, string).\n
    *    Parameter: AutoRecordKeyframeInterval: Interval between automatic keyframes. Zero means no auto keyframes. (record only, double).\n
    *    Parameter: EstPlaybackTimeRemaining: Estimated time remaining for playback of cur file (playback only, double).\n
    *    Parameter: CurrentRecordDuration: Current length of the active recoring (record only, double).\n
    *    Parameter: NumRecordedMessages: Number of messages logged in current recording (record only, unsigned long).\n
    */
   class DT_GAME_EXPORT LogStatusMessage : public MapMessage
   {      
      public:          
         /*
          * Construct - Adds this messages parameters to its parameter list.
          */            
         LogStatusMessage();
         
         /**
          * Creates a LogStatus and populates it with the Status values from the message
          * @return A Status object with the message's parameter values.
          */
         LogStatus GetStatus() const;
         
         /**
          * Sets the Status parameters from the passed in LogStatus (server only)
          * @param status The Status object - the status attributes will be set on the message
          */
         void SetStatus(const LogStatus &status);

      protected:      
         virtual ~LogStatusMessage() { }
   };

   /**
    * This message class is sent to the ServerLoggerComponent to change the 
    * current log file.  This can only be sent during IDLE state (can't change the log
    * file while you are currently replaying or recording).
    * @par
    *    Related Message Types: LOG_REQ_SET_LOGFILE\n
    *    Parameter: LogFileName : Logical name of the log.  This could be concatenated with
    *       other custom naming identifiers (including a file extension) depending on a 
    *       particular LogStream implementation.\n
    */
   class DT_GAME_EXPORT LogSetLogfileMessage : public Message
   {      
      public:
            
         /*
          * Construct - Adds this messages parameters to its parameter list.
          */            
         LogSetLogfileMessage();
         
         /**
          * Gets the new log file name.
          * @return A string containing the file name.
          */
         const std::string &GetLogFileName() const 
         {
            const StringMessageParameter *p = 
               static_cast<const dtCore::NamedStringParameter*>(GetParameter("LogFileName"));
            return p->GetValue(); 
         }
         
         /**
          * Sets the log name contained within this message.
          * @param logFileName The name for the log.
          */
         void SetLogFileName(const std::string &logFileName)
         {
            StringMessageParameter *p = 
               static_cast<dtCore::NamedStringParameter*>(GetParameter("LogFileName"));
            return p->SetValue(logFileName); 
         } 
                  
      protected:      
         virtual ~LogSetLogfileMessage() { }
   };
   
   /**
    * This message class is sent to the ServerLoggerComponent to delete a log file. 
    * This can only be sent during IDLE state (can't be deleting logs 
    * while you are currently replaying or recording).  
    * @par
    *    Related Message Types: LOG_REQ_DELETE_LOG\n
    *    Parameter: LogFileName : Logical name of the log.  This could be concatenated with
    *       other custom naming identifiers (including a file extension) depending on a 
    *       particular LogStream implementation.\n
    */
   class DT_GAME_EXPORT LogDeleteLogfileMessage : public Message
   {      
      public:
            
         /*
          * Construct - Adds this messages parameters to its parameter list.
          */            
         LogDeleteLogfileMessage();
         
         /**
          * Gets the new log file name.
          * @return A string containing the file name.
          */
         const std::string &GetLogFileName() const 
         {
            const StringMessageParameter *p = 
               static_cast<const dtCore::NamedStringParameter*>(GetParameter("LogFileName"));
            return p->GetValue(); 
         }
         
         /**
          * Sets the log name contained within this message.
          * @param logFileName The name for the log.
          */
         void SetLogFileName(const std::string &logFileName)
         {
            StringMessageParameter *p = 
               static_cast<dtCore::NamedStringParameter*>(GetParameter("LogFileName"));
            return p->SetValue(logFileName); 
         }
                  
      protected:      
         virtual ~LogDeleteLogfileMessage() { }
   };
   
   /**
    * This message class is sent from the ServerLoggerComponent to the log controller
    * specifying the list of keyframes in the current log.
    * @par
    *    Related Message Types: LOG_REQ_GET_KEYFRAMES</br>
    *    Parameter: KeyframeList: </br>
    */
   class DT_GAME_EXPORT LogGetKeyframeListMessage : public Message
   {
      public:
      
         /**
          * Construct - Adds the message parameters for this message to the
          * parameter list.
          */
         LogGetKeyframeListMessage();
         
         /**
          * Gets the list of keyframes this message contains.
          * @return A list of keyframe objects.
          */
         const std::vector<LogKeyframe>& GetKeyframeList() const;
         
         /**
          * Overloaded to update the internal cached keyframe list.
          * @return true if it was able to assign the value based on the string or false if not.
          * @param source The string to deserialized into a message.
          */
         virtual bool FromString(const std::string &source);
         
         /**
          * Overloaded to update the internal cached keyframe list.
          * @return true if it was able to assign the value based on the stream or false if not.
          * @param stream The stream to deserialized into a message.
          */
         virtual bool FromDataStream(dtUtil::DataStream &stream);
         
         /**
          * Sets the list of keyframes this message contains.
          * @param kfList The list of keyframes to package into a message.
          */
         void SetKeyframeList(const std::vector<LogKeyframe> &kfList);    
                  
      protected:      
         virtual ~LogGetKeyframeListMessage();
         void UpdateInternalKeyframeList();
         
      private:
         std::vector<LogKeyframe> mKeyframeList;
         
   };
   
   /**
    * This message class is sent from the ServerLoggerComponent to the log controller
    * specifying the list of tags in the current log.
    * @par
    *    Related Message Types: LOG_REQ_GET_TAGS</br>
    *    Parameter: TagList: </br>
    */
   class DT_GAME_EXPORT LogGetTagListMessage : public Message
   {
      public:
      
         /**
          * Construct - Adds the message parameters for this message to the
          * parameter list.
          */
         LogGetTagListMessage();
         
         /**
          * Gets the list of tags this message contains.
          * @return A list of strings corresponding to keyframes.
          */
         const std::vector<LogTag> &GetTagList() const;
         
         /**
          * Overloaded to update the internal cached tags list.
          * @return true if it was able to assign the value based on the string or false if not.
          * @param source The string to deserialize into a message.
          */
         virtual bool FromString(const std::string &source);
         
         /**
          * Overloaded to update the internal cached tags list.
          * @return true if it was able to assign the value based on the stream or false if not.
          * @param stream The stream to deserialized into a message.
          */
         virtual bool FromDataStream(dtUtil::DataStream &stream);
         
         /**
          * Sets the list of logs this message contains.
          * @param logList The list of logs.
          */
         void SetTagList(const std::vector<LogTag> &tagList);    
                  
      protected:      
         virtual ~LogGetTagListMessage();
         void UpdateInternalTagList();
         
      private:
         std::vector<LogTag> mTagList;
   };
   
   /**
    * This message class is sent from the ServerLoggerComponent to the log controller
    * specifying the current logs available.
    * @par
    *    Related Message Types: LOG_REQ_GET_LOGFILES</br>
    *    Parameter: LogList: List of strings corresponding to the list of 
    *       available logs.<br>
    */
   class DT_GAME_EXPORT LogAvailableLogsMessage : public Message
   {      
      public:
            
         /*
          * Construct - Adds the message parameters for this message to the parameter
          * list.
          */            
         LogAvailableLogsMessage();
         
         /**
          * Gets the list of logs this message contains.
          * @return A list of strings corresponding to logs.
          */
         const std::vector<std::string> &GetLogList() const
         {
            const StringMessageParameter *p =
               static_cast<const dtCore::NamedStringParameter*>(GetParameter("LogList"));
            return p->GetValueList();
         }
         
         /**
          * Sets the list of logs this message contains.
          * @param logList The list of logs.
          */
         void SetLogList(const std::vector<std::string> &logList)
         {
            StringMessageParameter *p =
               static_cast<dtCore::NamedStringParameter*>(GetParameter("LogList"));
            p->SetValueList(logList);
         }    
                  
      protected:      
         virtual ~LogAvailableLogsMessage() { }
   };   
     
   /**
    * This message class is sent to the ServerLoggerComponent to insert a tag. 
    * Tags are user friendly information that can be used to mark interesting moments
    * in a simulation.  This can only be sent during Playback or Record.  Typically, 
    * this is during record, but some systems may support capturing new keyframes during 
    * Playback depending on the server implementation or the current logstream implementation. 
    * Note that this wraps the tag params with a LogTag object.
    * @par
    *    Related Message Types: LOG_REQ_INSERT_TAG\n
    *    Parameter: TagName: Simple name of the tag (may be blank).\n
    *    Parameter: TagDescription: Longer Description of the tag (may be blank).\n
    *    Parameter: UniqueId: Unique ID for the Tag object (required).\n
    *    Parameter: SimTime: The simulation time that the tag was created (double).\n
    */
   class DT_GAME_EXPORT LogInsertTagMessage : public Message
   {      
      public:          
         /*
          * Construct - Adds this messages parameters to its parameter list.
          */            
         LogInsertTagMessage();
         
         /**
          * Creates a LogTag and populates it with the Tag values from the message
          * @return A Tag object with the message's tag parameter values.
          */
         LogTag GetTag() const;
         
         /**
          * Sets the tag parameters from the passed in log tag
          * @param tag The tag object - the tag attributes will be set on the message
          */
         void SetTag(const LogTag &tag);

      protected:      
         virtual ~LogInsertTagMessage() { }
   };

   /**
    * This message class is sent to the ServerLoggerComponent to capture a keyframe. 
    * Keyframes are complete snapshots in time.  They capture every game actor currently
    * in the system and log it.  Keyframes are required to jump back and forth in time.  You
    * also have a keyframe at the start of every log file.  Keyframes should be completely sufficient
    * to define the state of a simulation.  Typically, this is during record, but some systems
    * may support capturing new keyframes during Playback depending on the server implementation or
    * the current logstream implementation.  
    * Note that this message wraps the keyframe params with a LogKeyframe object.
    * @par
    *    Related Message Types: LOG_REQ_CAPTURE_KEYFRAME\n
    *    Parameter: KeyframeName: Simple name of the keyframe (may be blank).\n
    *    Parameter: KeyframeDescription: Longer Description of the keyframe (may be blank).\n
    *    Parameter: UniqueId: Unique ID for the Keyframe object (required).\n
    *    Parameter: SimTime: The simulation time that the Keyframe was created (required - double).\n
    *    Parameter: MapNames: The map name set that was current when the keyframe was snapshotted (required).\n
    */
   class DT_GAME_EXPORT LogCaptureKeyframeMessage : public MapMessage
   {      
      public:          
         /*
          * Construct - Adds this messages parameters to its parameter list.
          */            
         LogCaptureKeyframeMessage();
         
         /**
          * Creates a LogKeyframe and populates it with the keyframe values from the message
          * @return A LogKeyframe object with the message's keyframe parameter values.
          */
         LogKeyframe GetKeyframe() const;
         
         /**
          * Sets the messages Keyframe parameters from the passed in log keyframe
          * @param keyframe The keyframe object - the attributes will be set on the message
          */
         void SetKeyframe(const LogKeyframe &keyframe);

      protected:      
         virtual ~LogCaptureKeyframeMessage() { }
   };

   /**
    * This message class is sent to the ServerLoggerComponent to jump to a specific keyframe. 
    * Keyframes are complete snapshots in time.  They capture every game actor currently
    * in the system and log it.  Keyframes are required to jump back and forth in time.  You
    * also have a keyframe at the start of every log file.  Keyframes should be completely sufficient
    * to define the state of a simulation.  You can only jump to a keyframe during playback.  
    * Note that this message wraps the keyframe params with a LogKeyframe object.
    * @par
    *    Related Message Types: LOG_REQ_JUMP_TO_KEYFRAME\n
    *    Parameter: KeyframeName: Simple name of the keyframe (may be blank).\n
    *    Parameter: KeyframeDescription: Longer Description of the keyframe (may be blank).\n
    *    Parameter: UniqueId: Unique ID for the Keyframe object (required).\n
    *    Parameter: SimTime: The simulation time that the Keyframe was created (required - double).\n
    *    Parameter: ActiveMap: The map name that was current when the keyframe was snapshotted (required).\n
    */
   class DT_GAME_EXPORT LogJumpToKeyframeMessage : public LogCaptureKeyframeMessage
   {      
      public:          
         /*
          * Construct - Adds this messages parameters to its parameter list.
          */            
         LogJumpToKeyframeMessage();

      protected:      
         virtual ~LogJumpToKeyframeMessage() { }
   };

   /**
    * This message class is sent to the ServerLoggerComponent to change the current automatic 
    * keyframe interval (in seconds).  This interval causes the server to automatically
    * generate a periodic keyframe. If you use this at all, it is strongly recommended that 
    * you set this interval to be VERY large (like 5 or 10 minutes). Only relevant for record.
    * @par
    *    Related Message Types: LOG_REQ_SET_AUTOKEYFRAMEINTERVAL\n
    *    Parameter: AutoKeyframeInterval: Interval in seconds (record state only, double)\n
    */
   class DT_GAME_EXPORT LogSetAutoKeyframeIntervalMessage : public Message
   {      
      public:
            
         /*
          * Construct - Adds this messages parameters to its parameter list.
          */            
         LogSetAutoKeyframeIntervalMessage();
         
         /**
          * Gets the auto keyframe interval
          * @return The interval
          */
         double GetAutoKeyframeInterval() const
         {
            const DoubleMessageParameter *p = 
               static_cast<const dtCore::NamedDoubleParameter*>(GetParameter("AutoKeyframeInterval"));
            return p->GetValue(); 
         }
         
         /**
          * Sets the auto keyframe interval
          * @param interval the auto keyframe interval.
          */
         void SetAutoKeyframeInterval(double interval)
         {
            DoubleMessageParameter *p = 
               static_cast<dtCore::NamedDoubleParameter*>(GetParameter("AutoKeyframeInterval"));
            return p->SetValue(interval); 
         } 
                  
      protected:      
         virtual ~LogSetAutoKeyframeIntervalMessage() { }
   };
   
   /**
    * This message class is sent from the ServerLoggerComponent to let clients know
    * that a load keyframe is done.  It has the success/failure status.
    * This can only be sent during a PLAYBACK state.
    * @par
    *    Related Message Types: LOG_COMMAND_BEGIN_LOADKEYFRAME_TRANS\n
    *    Parameter: SuccessFlag : True if success, false if failure. \n
    *    Parameter: FailureReason : May have a reason if successflag is false. \n
    */
   class DT_GAME_EXPORT LogEndLoadKeyframeMessage : public Message
   {      
      public:
            
         /*
          * Construct - Adds this messages parameters to its parameter list.
          */            
         LogEndLoadKeyframeMessage();
         
         /**
          * Gets the failure reason. May be empty, even if it failed.
          * @return A string with the failure reason.
          */
         bool GetSuccessFlag() const 
         {
            const BooleanMessageParameter *p = 
               static_cast<const dtCore::NamedBooleanParameter*>(GetParameter("SuccessFlag"));
            return p->GetValue(); 
         }
         
         /**
          * Sets the failure reason. May be empty, even if it failed. Should only be called by server.
          * @param failureReason A string with the failure reason.
          */
         void SetSuccessFlag(bool successFlag)
         {
            BooleanMessageParameter *p = 
               static_cast<dtCore::NamedBooleanParameter*>(GetParameter("SuccessFlag"));
            return p->SetValue(successFlag); 
         } 
                  
         /**
          * Gets the failure reason. May be empty, even if it failed.
          * @return A string with the failure reason.
          */
         const std::string &GetFailureReason() const 
         {
            const StringMessageParameter *p = 
               static_cast<const dtCore::NamedStringParameter*>(GetParameter("FailureReason"));
            return p->GetValue(); 
         }
         
         /**
          * Sets the failure reason. May be empty, even if it failed. Should only be called by server.
          * @param failureReason A string with the failure reason.
          */
         void SetFailureReason(const std::string &logFileName)
         {
            StringMessageParameter *p = 
               static_cast<dtCore::NamedStringParameter*>(GetParameter("FailureReason"));
            return p->SetValue(logFileName); 
         } 
                  
      protected:      
         virtual ~LogEndLoadKeyframeMessage() { }
   };


   /** 
    * A Message that contains a MessageType the ServerLoggerController should ignore
    * or stop ignoring.
    */
   class DT_GAME_EXPORT LogIgnoreMessageTypeMessage : public Message
   {
   public:
      LogIgnoreMessageTypeMessage();
      void SetIgnoredMessageType(const dtGame::MessageType& ignoredMsgType);
      void SetIgnoredMessageType(unsigned short ignoredMsgTypeID);
      unsigned short GetIgnoredMessageType() const; 

   protected:
      virtual ~LogIgnoreMessageTypeMessage() {};
   };
   
}

#endif 
