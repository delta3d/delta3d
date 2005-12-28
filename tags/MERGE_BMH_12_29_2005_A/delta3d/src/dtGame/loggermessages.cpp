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
#include "dtGame/loggermessages.h"
#include "dtGame/logtag.h"
#include "dtGame/logkeyframe.h"
#include "dtGame/logstatus.h"

namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogStatusMessage::LogStatusMessage()
   {
      AddParameter(new EnumMessageParameter("StateEnum"));
      AddParameter(new DoubleMessageParameter("CurrentSimTime"));       
      AddParameter(new StringMessageParameter("ActiveMap"));   
      AddParameter(new StringMessageParameter("LogFile"));   
      AddParameter(new DoubleMessageParameter("AutoRecordKeyframeInterval"));       
      AddParameter(new DoubleMessageParameter("EstPlaybackTimeRemaining"));       
      AddParameter(new DoubleMessageParameter("CurrentRecordDuration"));       
      AddParameter(new UnsignedLongIntMessageParameter("NumRecordedMessages"));       
   }
   
   //////////////////////////////////////////////////////////////////////////
   LogStatus LogStatusMessage::GetStatus() const
   {
      LogStatus result;

      const EnumMessageParameter *stateEnumParam = 
         static_cast<const EnumMessageParameter*>(GetParameter("StateEnum"));
      LogStateEnumeration *stateEnum = (LogStateEnumeration *) LogStateEnumeration::GetValueForName(stateEnumParam->GetValue());
      result.SetStateEnum(*stateEnum); 

      const DoubleMessageParameter *simTime = 
         static_cast<const DoubleMessageParameter*>(GetParameter("CurrentSimTime"));
      result.SetCurrentSimTime(simTime->GetValue()); 

      const StringMessageParameter *map = 
         static_cast<const StringMessageParameter*>(GetParameter("ActiveMap"));
      result.SetActiveMap(map->GetValue()); 

      const StringMessageParameter *logfile = 
         static_cast<const StringMessageParameter*>(GetParameter("LogFile"));
      result.SetLogFile(logfile->GetValue()); 

      const DoubleMessageParameter *keyframeInterval = 
         static_cast<const DoubleMessageParameter*>(GetParameter("AutoRecordKeyframeInterval"));
      result.SetAutoRecordKeyframeInterval(keyframeInterval->GetValue()); 

      const DoubleMessageParameter *playbackRemaining = 
         static_cast<const DoubleMessageParameter*>(GetParameter("EstPlaybackTimeRemaining"));
      result.SetEstPlaybackTimeRemaining(playbackRemaining->GetValue()); 

      const DoubleMessageParameter *recordDur = 
         static_cast<const DoubleMessageParameter*>(GetParameter("CurrentRecordDuration"));
      result.SetCurrentRecordDuration(recordDur->GetValue()); 

      const UnsignedLongIntMessageParameter *numRecorded = 
         static_cast<const UnsignedLongIntMessageParameter*>(GetParameter("NumRecordedMessages"));
      result.SetNumRecordedMessages(numRecorded->GetValue()); 

      return result;
   }

   //////////////////////////////////////////////////////////////////////////
   void LogStatusMessage::SetStatus(const LogStatus &status)
   {
      EnumMessageParameter *stateEnumParam = 
         static_cast< EnumMessageParameter*>(GetParameter("StateEnum"));
      const LogStateEnumeration &stateEnum = status.GetStateEnum();
      stateEnumParam->SetValue(stateEnum.GetName()); 

      DoubleMessageParameter *simTime = 
         static_cast< DoubleMessageParameter*>(GetParameter("CurrentSimTime"));
      simTime->SetValue(status.GetCurrentSimTime()); 

      StringMessageParameter *map = 
         static_cast< StringMessageParameter*>(GetParameter("ActiveMap"));
      map->SetValue(status.GetActiveMap());

      StringMessageParameter *logFile = 
         static_cast< StringMessageParameter*>(GetParameter("LogFile"));
      logFile->SetValue(status.GetLogFile());

      DoubleMessageParameter *keyframeInterval = 
         static_cast< DoubleMessageParameter*>(GetParameter("AutoRecordKeyframeInterval"));
      keyframeInterval->SetValue(status.GetAutoRecordKeyframeInterval()); 

      DoubleMessageParameter *playbackRemaining = 
         static_cast< DoubleMessageParameter*>(GetParameter("EstPlaybackTimeRemaining"));
      playbackRemaining->SetValue(status.GetEstPlaybackTimeRemaining()); 

      DoubleMessageParameter *recordDuration = 
         static_cast< DoubleMessageParameter*>(GetParameter("CurrentRecordDuration"));
      recordDuration->SetValue(status.GetCurrentRecordDuration()); 

      UnsignedLongIntMessageParameter *numRecorded = 
         static_cast< UnsignedLongIntMessageParameter*>(GetParameter("NumRecordedMessages"));
      numRecorded->SetValue(status.GetNumRecordedMessages());

   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogSetLogfileMessage::LogSetLogfileMessage()
   {
      AddParameter(new StringMessageParameter("LogFileName"));   
   }


   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogDeleteLogfileMessage::LogDeleteLogfileMessage()
   {
      AddParameter(new StringMessageParameter("LogFileName"));   
   }


   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogInsertTagMessage::LogInsertTagMessage()
   {
      AddParameter(new StringMessageParameter("Name"));   
      AddParameter(new StringMessageParameter("Description"));   
      AddParameter(new ActorMessageParameter("UniqueId"));
      AddParameter(new DoubleMessageParameter("SimTime"));       
   }
   
   //////////////////////////////////////////////////////////////////////////
   LogTag LogInsertTagMessage::GetTag() const
   {
      LogTag result;

      const StringMessageParameter *name = 
         static_cast<const StringMessageParameter*>(GetParameter("Name"));
      result.SetName(name->GetValue()); 

      const StringMessageParameter *description = 
         static_cast<const StringMessageParameter*>(GetParameter("Description"));
      result.SetDescription(description->GetValue()); 

      const ActorMessageParameter *uniqueId = 
         static_cast<const ActorMessageParameter*>(GetParameter("UniqueId"));
      result.SetUniqueId(uniqueId->GetValue()); 

      const DoubleMessageParameter *simTime = 
         static_cast<const DoubleMessageParameter*>(GetParameter("SimTime"));
      result.SetSimTimeStamp(simTime->GetValue()); 

      return result;
   }

   //////////////////////////////////////////////////////////////////////////
   void LogInsertTagMessage::SetTag(const LogTag &tag)
   {
      StringMessageParameter *name = 
         static_cast< StringMessageParameter*>(GetParameter("Name"));
      name->SetValue(tag.GetName());

      StringMessageParameter *description = 
         static_cast< StringMessageParameter*>(GetParameter("Description"));
      description->SetValue(tag.GetDescription());

      ActorMessageParameter *uniqueId = 
         static_cast< ActorMessageParameter*>(GetParameter("UniqueId"));
      uniqueId->SetValue(tag.GetUniqueId());

      DoubleMessageParameter *simTime = 
         static_cast< DoubleMessageParameter*>(GetParameter("SimTime"));
      simTime->SetValue(tag.GetSimTimeStamp()); 
   }


   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogCaptureKeyframeMessage::LogCaptureKeyframeMessage()
   {
      AddParameter(new StringMessageParameter("Name"));   
      AddParameter(new StringMessageParameter("Description"));   
      AddParameter(new ActorMessageParameter("UniqueId"));
      AddParameter(new DoubleMessageParameter("SimTime"));       
      AddParameter(new StringMessageParameter("ActiveMap"));   
   }
   
   //////////////////////////////////////////////////////////////////////////
   LogKeyframe LogCaptureKeyframeMessage::GetKeyframe() const
   {
      LogKeyframe result;

      const StringMessageParameter *name = 
         static_cast<const StringMessageParameter*>(GetParameter("Name"));
      result.SetName(name->GetValue()); 

      const StringMessageParameter *description = 
         static_cast<const StringMessageParameter*>(GetParameter("Description"));
      result.SetDescription(description->GetValue()); 

      const ActorMessageParameter *uniqueId = 
         static_cast<const ActorMessageParameter*>(GetParameter("UniqueId"));
      result.SetUniqueId(uniqueId->GetValue()); 

      const DoubleMessageParameter *simTime = 
         static_cast<const DoubleMessageParameter*>(GetParameter("SimTime"));
      result.SetSimTimeStamp(simTime->GetValue()); 

      const StringMessageParameter *activeMap = 
         static_cast<const StringMessageParameter*>(GetParameter("ActiveMap"));
      result.SetActiveMap(activeMap->GetValue()); 

      return result;
   }

   //////////////////////////////////////////////////////////////////////////
   void LogCaptureKeyframeMessage::SetKeyframe(const LogKeyframe &keyframe)
   {
      StringMessageParameter *name = 
         static_cast< StringMessageParameter*>(GetParameter("Name"));
      name->SetValue(keyframe.GetName());

      StringMessageParameter *description = 
         static_cast< StringMessageParameter*>(GetParameter("Description"));
      description->SetValue(keyframe.GetDescription());

      ActorMessageParameter *uniqueId = 
         static_cast< ActorMessageParameter*>(GetParameter("UniqueId"));
      uniqueId->SetValue(keyframe.GetUniqueId());

      DoubleMessageParameter *simTime = 
         static_cast< DoubleMessageParameter*>(GetParameter("SimTime"));
      simTime->SetValue(keyframe.GetSimTimeStamp()); 

      StringMessageParameter *activeMap = 
         static_cast< StringMessageParameter*>(GetParameter("ActiveMap"));
      activeMap->SetValue(keyframe.GetActiveMap());

   }

  
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogSetAutoKeyframeIntervalMessage::LogSetAutoKeyframeIntervalMessage()
   {
      AddParameter(new DoubleMessageParameter("AutoKeyframeInterval"));       
   }
   
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogEndLoadKeyframeMessage::LogEndLoadKeyframeMessage()
   {
      AddParameter(new BooleanMessageParameter("SuccessFlag"));   
      AddParameter(new StringMessageParameter("FailureReason"));   
   }
   
}
