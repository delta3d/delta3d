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
#include <prefix/dtgameprefix.h>
#include <dtGame/loggermessages.h>
#include <dtGame/logtag.h>
#include <dtGame/logkeyframe.h>
#include <dtGame/logstatus.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/messagetype.h>
#include <dtUtil/datastream.h>

namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogStatusMessage::LogStatusMessage()
   {
      AddParameter(new EnumMessageParameter("StateEnum"));
      AddParameter(new DoubleMessageParameter("CurrentSimTime"));
      AddParameter(new StringMessageParameter("LogFile"));
      AddParameter(new DoubleMessageParameter("AutoRecordKeyframeInterval"));
      AddParameter(new DoubleMessageParameter("CurrentRecordDuration"));
      AddParameter(new UnsignedIntMessageParameter("NumRecordedMessages"));
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

      std::vector<std::string> maps;
      GetMapNames(maps);
      result.SetActiveMaps(maps);

      const StringMessageParameter *logfile =
         static_cast<const StringMessageParameter*>(GetParameter("LogFile"));
      result.SetLogFile(logfile->GetValue());

      const DoubleMessageParameter *keyframeInterval =
         static_cast<const DoubleMessageParameter*>(GetParameter("AutoRecordKeyframeInterval"));
      result.SetAutoRecordKeyframeInterval(keyframeInterval->GetValue());

      const DoubleMessageParameter *recordDur =
         static_cast<const DoubleMessageParameter*>(GetParameter("CurrentRecordDuration"));
      result.SetCurrentRecordDuration(recordDur->GetValue());

      const UnsignedIntMessageParameter *numRecorded =
         static_cast<const UnsignedIntMessageParameter*>(GetParameter("NumRecordedMessages"));
      result.SetNumMessages(numRecorded->GetValue());

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

      SetMapNames(status.GetActiveMaps());

      StringMessageParameter *logFile =
         static_cast< StringMessageParameter*>(GetParameter("LogFile"));
      logFile->SetValue(status.GetLogFile());

      DoubleMessageParameter *keyframeInterval =
         static_cast< DoubleMessageParameter*>(GetParameter("AutoRecordKeyframeInterval"));
      keyframeInterval->SetValue(status.GetAutoRecordKeyframeInterval());

      DoubleMessageParameter *recordDuration =
         static_cast< DoubleMessageParameter*>(GetParameter("CurrentRecordDuration"));
      recordDuration->SetValue(status.GetCurrentRecordDuration());

      UnsignedIntMessageParameter *numRecorded =
         static_cast< UnsignedIntMessageParameter*>(GetParameter("NumRecordedMessages"));
      numRecorded->SetValue(status.GetNumMessages());

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
   LogAvailableLogsMessage::LogAvailableLogsMessage()
   {
      AddParameter(new StringMessageParameter("LogList","",true));
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogInsertTagMessage::LogInsertTagMessage()
   {
      AddParameter(new StringMessageParameter("Name"));
      AddParameter(new StringMessageParameter("Description"));
      AddParameter(new ActorMessageParameter("UniqueId"));
      AddParameter(new DoubleMessageParameter("SimTime"));
      AddParameter(new BooleanMessageParameter("KeyframeCapture"));
      AddParameter(new ActorMessageParameter("KeyframeUniqueId"));
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

      const ActorMessageParameter *kfUniqueId =
         static_cast<const ActorMessageParameter*>(GetParameter("KeyframeUniqueId"));
      result.SetKeyframeUniqueId(kfUniqueId->GetValue());

      const BooleanMessageParameter *captureKf =
         static_cast<const BooleanMessageParameter*>(GetParameter("KeyframeCapture"));
      result.SetCaptureKeyframe(captureKf->GetValue());

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

      ActorMessageParameter *kfUniqueId =
         static_cast<ActorMessageParameter*>(GetParameter("KeyframeUniqueId"));
      kfUniqueId->SetValue(tag.GetKeyframeUniqueId());

      BooleanMessageParameter *captureKf =
         static_cast<BooleanMessageParameter*>(GetParameter("KeyframeCapture"));
      captureKf->SetValue(tag.GetCaptureKeyframe());
   }


   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogCaptureKeyframeMessage::LogCaptureKeyframeMessage()
   {
      AddParameter(new StringMessageParameter("Name"));
      AddParameter(new StringMessageParameter("Description"));
      AddParameter(new ActorMessageParameter("UniqueId"));
      AddParameter(new DoubleMessageParameter("SimTime"));
      AddParameter(new ActorMessageParameter("TagUniqueId"));
      AddParameter(new LongIntMessageParameter("LogFileOffset"));
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

      std::vector<std::string> maps;
      GetMapNames(maps);
      result.SetActiveMaps(maps);
 
      const ActorMessageParameter *tagUniqueId =
         static_cast<const ActorMessageParameter*>(GetParameter("TagUniqueId"));
      result.SetTagUniqueId(tagUniqueId->GetValue());

      const LongIntMessageParameter *logFileOffset =
         static_cast<const LongIntMessageParameter*>(GetParameter("LogFileOffset"));
      result.SetLogFileOffset(logFileOffset->GetValue());

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

      SetMapNames(keyframe.GetActiveMaps());

      ActorMessageParameter *tagUniqueId =
         static_cast<ActorMessageParameter*>(GetParameter("TagUniqueId"));
      tagUniqueId->SetValue(keyframe.GetTagUniqueId());

      LongIntMessageParameter *logFileOffset =
         static_cast<LongIntMessageParameter*>(GetParameter("LogFileOffset"));
      logFileOffset->SetValue(keyframe.GetLogFileOffset());
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogJumpToKeyframeMessage::LogJumpToKeyframeMessage()
   {
   }
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogGetKeyframeListMessage::LogGetKeyframeListMessage()
   {
      AddParameter(new StringMessageParameter("Name","",true));
      AddParameter(new StringMessageParameter("Description","",true));
      AddParameter(new ActorMessageParameter("UniqueId",dtCore::UniqueId(),true));
      AddParameter(new ActorMessageParameter("TagUniqueId",dtCore::UniqueId(),true));
      AddParameter(new DoubleMessageParameter("SimTime",0.0,true));
      AddParameter(new StringMessageParameter("ActiveMap","",true));
      AddParameter(new LongIntMessageParameter("LogFileOffset", 0, true));
   }

   //////////////////////////////////////////////////////////////////////////
   LogGetKeyframeListMessage::~LogGetKeyframeListMessage()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void LogGetKeyframeListMessage::SetKeyframeList(const std::vector<LogKeyframe> &kfList)
   {
      //The keyframe list is actually broken up into parallel arrays with are
      //stored as message parameters.
      StringMessageParameter* nameParam =
         static_cast<StringMessageParameter*>(GetParameter("Name"));
      StringMessageParameter* descParam =
         static_cast<StringMessageParameter*>(GetParameter("Description"));
      ActorMessageParameter* idParam =
         static_cast<ActorMessageParameter*>(GetParameter("UniqueId"));
      ActorMessageParameter* tagIdParam =
         static_cast<ActorMessageParameter*>(GetParameter("TagUniqueId"));
      StringMessageParameter* mapParam =
         static_cast<StringMessageParameter*>(GetParameter("ActiveMap"));
      DoubleMessageParameter* simTimeParam =
         static_cast<DoubleMessageParameter*>(GetParameter("SimTime"));
      LongIntMessageParameter* logFileOffsetParam =
         static_cast<LongIntMessageParameter*>(GetParameter("LogFileOffset"));

      std::vector<StringMessageParameter::value_type>& nameList = nameParam->GetValueList();
      std::vector<StringMessageParameter::value_type>& descList = descParam->GetValueList();
      std::vector<StringMessageParameter::value_type>& mapList = mapParam->GetValueList();
      std::vector<DoubleMessageParameter::value_type>& simTimeList = simTimeParam->GetValueList();
      std::vector<ActorMessageParameter::value_type>& idList = idParam->GetValueList();
      std::vector<ActorMessageParameter::value_type>& tagIdList = tagIdParam->GetValueList();
      std::vector<LongIntMessageParameter::value_type>& logFileOffsetList = logFileOffsetParam->GetValueList();

      nameList.clear();
      descList.clear();
      mapList.clear();
      simTimeList.clear();
      idList.clear();
      tagIdList.clear();
      mKeyframeList.clear();
      logFileOffsetList.clear();

      std::vector<LogKeyframe>::const_iterator itor;
      for (itor=kfList.begin(); itor!=kfList.end(); ++itor)
      {
         //First build the parallel arrays.
         nameList.push_back(itor->GetName());
         descList.push_back(itor->GetDescription());
         // TODO fix this to send the whole list.
         if (itor->GetActiveMaps().empty())
            mapList.push_back("");
         else
            mapList.push_back(itor->GetActiveMaps()[0]);
         
         simTimeList.push_back(itor->GetSimTimeStamp());
         idList.push_back(itor->GetUniqueId());
         tagIdList.push_back(itor->GetTagUniqueId());
         logFileOffsetList.push_back(itor->GetLogFileOffset());

         //Second, store the keyframe in the message's list of keyframes to
         //essentially caching the keyframes so they do have to be recreated
         //from the parallel arrays anytime a user requests them.
         mKeyframeList.push_back(*itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   const std::vector<LogKeyframe>& LogGetKeyframeListMessage::GetKeyframeList() const 
   {
      return mKeyframeList;
   }

   //////////////////////////////////////////////////////////////////////////
   bool LogGetKeyframeListMessage::FromString(const std::string& source)
   {
      bool okay = true;
      okay = okay && Message::FromString(source);
      UpdateInternalKeyframeList();
      return okay;
   }
   
   //////////////////////////////////////////////////////////////////////////
   void LogGetKeyframeListMessage::UpdateInternalKeyframeList()
   {
      //The keyframe list is actually broken up into parallel arrays with are
      //stored as message parameters.
      StringMessageParameter* nameParam =
         static_cast<StringMessageParameter*>(GetParameter("Name"));
      StringMessageParameter* descParam =
         static_cast<StringMessageParameter*>(GetParameter("Description"));
      ActorMessageParameter* idParam =
         static_cast<ActorMessageParameter*>(GetParameter("UniqueId"));
      ActorMessageParameter* tagIdParam =
         static_cast<ActorMessageParameter*>(GetParameter("TagUniqueId"));
      StringMessageParameter* mapParam =
         static_cast<StringMessageParameter*>(GetParameter("ActiveMap"));
      DoubleMessageParameter* simTimeParam =
         static_cast<DoubleMessageParameter*>(GetParameter("SimTime"));
      LongIntMessageParameter* logFileOffsetParam =
         static_cast<LongIntMessageParameter*>(GetParameter("LogFileOffset"));

      std::vector<StringMessageParameter::value_type>& nameList = nameParam->GetValueList();
      std::vector<StringMessageParameter::value_type>& descList = descParam->GetValueList();
      std::vector<StringMessageParameter::value_type>& mapList = mapParam->GetValueList();
      std::vector<DoubleMessageParameter::value_type>& simTimeList = simTimeParam->GetValueList();
      std::vector<ActorMessageParameter::value_type>& idList = idParam->GetValueList();
      std::vector<ActorMessageParameter::value_type>& tagIdList = tagIdParam->GetValueList();
      std::vector<LongIntMessageParameter::value_type>& logFileOffsetList = logFileOffsetParam->GetValueList();



      if (nameList.size() != descList.size() ||
          nameList.size() != mapList.size() ||
          nameList.size() != simTimeList.size() ||
          nameList.size() != idList.size() ||
          nameList.size() != tagIdList.size() ||
          nameList.size() != logFileOffsetList.size())
      {
         throw dtGame::InvalidParameterException(
            "Parameter list sizes were not equal.", __FILE__, __LINE__);
      }

      mKeyframeList.clear();
      for (unsigned int i=0; i<nameList.size(); i++)
      {
         LogKeyframe kf;
         kf.SetName(nameList[i]);
         kf.SetDescription(descList[i]);
         kf.SetUniqueId(idList[i]);
         
         LogKeyframe::NameVector mapNames;
         mapNames.push_back(mapList[i]);
         kf.SetActiveMaps(mapNames);
         
         kf.SetSimTimeStamp(simTimeList[i]);
         kf.SetTagUniqueId(tagIdList[i]);
         kf.SetLogFileOffset(logFileOffsetList[i]);
         mKeyframeList.push_back(kf);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   LogGetTagListMessage::LogGetTagListMessage()
   {
      AddParameter(new StringMessageParameter("Name","",true));
      AddParameter(new StringMessageParameter("Description","",true));
      AddParameter(new ActorMessageParameter("UniqueId",dtCore::UniqueId(""),true));
      AddParameter(new ActorMessageParameter("KeyframeUniqueId",dtCore::UniqueId(""),true));
      AddParameter(new DoubleMessageParameter("SimTime",0.0,true));
      AddParameter(new BooleanMessageParameter("CaptureKeyframe",false,true));
   }

   //////////////////////////////////////////////////////////////////////////
   LogGetTagListMessage::~LogGetTagListMessage()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   const std::vector<LogTag>& LogGetTagListMessage::GetTagList() const
   {
      return mTagList;
   }
   
   //////////////////////////////////////////////////////////////////////////
   bool LogGetTagListMessage::FromString(const std::string &source)
   {
      bool okay = true;
      okay = okay && Message::FromString(source);
      UpdateInternalTagList();
      return okay;
   }

   //////////////////////////////////////////////////////////////////////////
   void LogGetTagListMessage::UpdateInternalTagList()
   {
      //The tag list is actually broken up into parallel arrays with are
      //stored as message parameters.
      StringMessageParameter *nameParam =
         static_cast<StringMessageParameter*>(GetParameter("Name"));
      StringMessageParameter *descParam =
         static_cast<StringMessageParameter*>(GetParameter("Description"));
      ActorMessageParameter *idParam =
         static_cast<ActorMessageParameter*>(GetParameter("UniqueId"));
      ActorMessageParameter *kfIdParam =
         static_cast<ActorMessageParameter*>(GetParameter("KeyframeUniqueId"));
      DoubleMessageParameter *simTimeParam =
         static_cast<DoubleMessageParameter*>(GetParameter("SimTime"));
      BooleanMessageParameter *captureKfParam =
         static_cast<BooleanMessageParameter*>(GetParameter("CaptureKeyframe"));

      std::vector<std::string> &nameList = nameParam->GetValueList();
      std::vector<std::string> &descList = descParam->GetValueList();
      std::vector<double> &simTimeList = simTimeParam->GetValueList();
      std::vector<dtCore::UniqueId> &idList = idParam->GetValueList();
      std::vector<dtCore::UniqueId> &kfIdList = kfIdParam->GetValueList();
      std::vector<bool> &captureKfList = captureKfParam->GetValueList();

      if (nameList.size() != descList.size() ||
          nameList.size() != simTimeList.size() ||
          nameList.size() != idList.size() ||
          nameList.size() != kfIdList.size() ||
          nameList.size() != captureKfList.size())
      {
         throw dtGame::InvalidParameterException("Parameter list sizes were not "
            "equal.", __FILE__, __LINE__);
      }

      mTagList.clear();
      for (unsigned int i=0; i<nameList.size(); i++)
      {
         LogTag tag;
         tag.SetName(nameList[i]);
         tag.SetDescription(descList[i]);
         tag.SetUniqueId(idList[i]);
         tag.SetKeyframeUniqueId(kfIdList[i]);
         tag.SetSimTimeStamp(simTimeList[i]);
         tag.SetCaptureKeyframe(captureKfList[i]);

         mTagList.push_back(tag);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void LogGetTagListMessage::SetTagList(const std::vector<LogTag> &tagList)
   {
      //The tag list is actually broken up into parallel arrays with are
      //stored as message parameters.
      StringMessageParameter *nameParam =
         static_cast<StringMessageParameter*>(GetParameter("Name"));
      StringMessageParameter *descParam =
         static_cast<StringMessageParameter*>(GetParameter("Description"));
      ActorMessageParameter *idParam =
         static_cast<ActorMessageParameter*>(GetParameter("UniqueId"));
      ActorMessageParameter *kfIdParam =
         static_cast<ActorMessageParameter*>(GetParameter("KeyframeUniqueId"));
      DoubleMessageParameter *simTimeParam =
         static_cast<DoubleMessageParameter*>(GetParameter("SimTime"));
      BooleanMessageParameter *captureKfParam =
         static_cast<BooleanMessageParameter*>(GetParameter("CaptureKeyframe"));

      std::vector<std::string> &nameList = nameParam->GetValueList();
      std::vector<std::string> &descList = descParam->GetValueList();
      std::vector<double> &simTimeList = simTimeParam->GetValueList();
      std::vector<dtCore::UniqueId> &idList = idParam->GetValueList();
      std::vector<dtCore::UniqueId> &kfIdList = kfIdParam->GetValueList();
      std::vector<bool> &captureKfList = captureKfParam->GetValueList();

      nameList.clear();
      descList.clear();
      simTimeList.clear();
      idList.clear();
      kfIdList.clear();
      captureKfList.clear();
      mTagList.clear();

      std::vector<LogTag>::const_iterator itor;
      for (itor=tagList.begin(); itor!=tagList.end(); ++itor)
      {
         //First build the parallel arrays.
         nameList.push_back(itor->GetName());
         descList.push_back(itor->GetDescription());
         simTimeList.push_back(itor->GetSimTimeStamp());
         idList.push_back(itor->GetUniqueId());
         kfIdList.push_back(itor->GetKeyframeUniqueId());
         captureKfList.push_back(itor->GetCaptureKeyframe());

         //Second, store the keyframe in the message's list of keyframes to
         //essentially caching the keyframes so they do have to be recreated
         //from the parallel arrays anytime a user requests them.
         mTagList.push_back(*itor);
      }
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

   bool LogGetKeyframeListMessage::FromDataStream(dtUtil::DataStream &stream)
   {
      bool okay = true;
      okay = okay && Message::FromDataStream(stream);
      UpdateInternalKeyframeList();
      return okay;
   }

   bool LogGetTagListMessage::FromDataStream(dtUtil::DataStream &stream)
   {
      bool okay = true;
      okay = okay && Message::FromDataStream(stream);
      UpdateInternalTagList();
      return okay;
   }

   ////////////////////////////////////////////////////////////////////////////////
   LogIgnoreMessageTypeMessage::LogIgnoreMessageTypeMessage()
   {
      AddParameter(new UnsignedShortIntMessageParameter("IgnoredMessageTypeID"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogIgnoreMessageTypeMessage::SetIgnoredMessageType(const dtGame::MessageType& ignoredMsgType)
   {
      SetIgnoredMessageType(ignoredMsgType.GetId());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogIgnoreMessageTypeMessage::SetIgnoredMessageType(unsigned short ignoredMsgTypeID)
   {
      UnsignedShortIntMessageParameter *p = 
         static_cast<UnsignedShortIntMessageParameter*>(GetParameter("IgnoredMessageTypeID"));

      p->SetValue(ignoredMsgTypeID);   
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned short LogIgnoreMessageTypeMessage::GetIgnoredMessageType() const
   {
      const UnsignedShortIntMessageParameter *p = 
         static_cast<const UnsignedShortIntMessageParameter*>(GetParameter("IgnoredMessageTypeID"));

      return p->GetValue();
   }
}
