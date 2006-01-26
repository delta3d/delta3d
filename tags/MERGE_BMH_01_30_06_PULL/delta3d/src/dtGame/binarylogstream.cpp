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

#include "dtGame/binarylogstream.h"
#include "dtGame/messagetype.h"
#include "dtUtil/exception.h"

#include <iostream>

namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   const std::string BinaryLogStream::LOGGER_MSGDB_MAGIC_NUMBER("GMLOGMSGDB");
   const std::string BinaryLogStream::LOGGER_INDEX_MAGIC_NUMBER("GMLOGINDEXTAB");
   const unsigned char BinaryLogStream::LOGGER_MAJOR_VERSION = 1;
   const unsigned char BinaryLogStream::LOGGER_MINOR_VERSION = 0;
   
   const std::string BinaryLogStream::MESSAGE_DB_EXT(".dlm");
   const std::string BinaryLogStream::INDEX_EXT(".dli");
   
   const unsigned char BinaryLogStream::MESSAGE_DEID = 0;
   const unsigned char BinaryLogStream::TAG_DEID = 1;
   const unsigned char BinaryLogStream::KEYFRAME_DEID = 2;
   const unsigned char BinaryLogStream::END_SECTION_DEID = 255;   
   
   //////////////////////////////////////////////////////////////////////////
   BinaryLogStream::BinaryLogStream(MessageFactory &msgFactory) :
      LogStream(msgFactory)
   {
      mMessagesFile = mIndexTablesFile = NULL;
   }
   
   //////////////////////////////////////////////////////////////////////////
   BinaryLogStream::~BinaryLogStream()
   {
      Close();
   }
   
   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::Close()
   {
      if (mIndexTablesFile != NULL || mMessagesFile != NULL)
         Flush();
      
      if (mMessagesFile != NULL)
      {
         LOG_DEBUG("Closing logger messages database file: " + mMessagesFileName);
         fclose(mMessagesFile);
      }
      
      if (mIndexTablesFile != NULL)
      {
         LOG_DEBUG("Closing logger index file: " + mIndexTablesFileName);
         fclose(mIndexTablesFile);
      }
         
      mMessagesFile = mIndexTablesFile = NULL;
      mMessagesFileName = mIndexTablesFileName = "";
      mExistingTags.clear();
      mNewTags.clear();
      mExistingKeyFrames.clear();
      mNewKeyFrames.clear();
   }
   
   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::Create(const std::string &logResourceName)
   {
      //Make sure the stream is not already open.
      Close();
      
      //Create the messages file...
      mMessagesFileName = logResourceName;
      mMessagesFileName += BinaryLogStream::MESSAGE_DB_EXT;
      mMessagesFile = fopen(mMessagesFileName.c_str(),"wb");
      if (mMessagesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Could not create the messages"
            " database file: " + mMessagesFileName);    
                  
      //Create the index tables file...      
      mIndexTablesFileName = logResourceName;
      mIndexTablesFileName += BinaryLogStream::INDEX_EXT;
      mIndexTablesFile = fopen(mIndexTablesFileName.c_str(),"wb");
      if (mIndexTablesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Could not create the logger"
            " index file: " + mIndexTablesFileName);
            
            
      //Write out the headers for both files.
      MessageDataBaseHeader msgHeader;
      msgHeader.magicNumber = BinaryLogStream::LOGGER_MSGDB_MAGIC_NUMBER;
      msgHeader.majorVersion = BinaryLogStream::LOGGER_MAJOR_VERSION;
      msgHeader.minorVersion = BinaryLogStream::LOGGER_MINOR_VERSION;
      msgHeader.indexTableFileNameLength = mIndexTablesFileName.length();
      msgHeader.indexTableFileName = mIndexTablesFileName;
      WriteMessageDataBaseHeader(msgHeader);      
      
      IndexTableHeader indexHeader;
      indexHeader.magicNumber = BinaryLogStream::LOGGER_INDEX_MAGIC_NUMBER;
      indexHeader.majorVersion = BinaryLogStream::LOGGER_MAJOR_VERSION;
      indexHeader.minorVersion = BinaryLogStream::LOGGER_MINOR_VERSION;
      indexHeader.msgDBFileNameLength = mMessagesFileName.length();
      indexHeader.msgDBFileName = mMessagesFileName;
      WriteIndexTableHeader(indexHeader);
      
      mEndOfStream = false;
   }

   //////////////////////////////////////////////////////////////////////////   
   static inline void WriteToLog(const char* data, size_t dataSize, size_t count, FILE* file) throw (dtUtil::Exception)
   {
      
      size_t written = fwrite(data,dataSize, count, file);
      //This check is done in case the file is not in write mode as Mac OS X won't set
      //ferror if it couldn't write the data.
      if (written < count)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Error writing to IO stream.  Data not written.  Check to see if the log file is in write mode.");         
      
   }

   //////////////////////////////////////////////////////////////////////////   
   void BinaryLogStream::Open(const std::string &logResourceName)
   {
      //Make sure the stream is not already open.
      Close();
      
      //Open the messages file.
      mMessagesFileName = logResourceName;
      mMessagesFileName += BinaryLogStream::MESSAGE_DB_EXT;
      mMessagesFile = fopen(mMessagesFileName.c_str(),"rb");
      if (mMessagesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Could not open the messages"
            " database file: " + mMessagesFileName);    
                  
      //Open the index tables file.  Note, we open this file in
      //read and write mode so we can read the existing index and 
      //append to it if needed.      
      mIndexTablesFileName = logResourceName;
      mIndexTablesFileName += BinaryLogStream::INDEX_EXT;
      mIndexTablesFile = fopen(mIndexTablesFileName.c_str(),"rb");
      if (mIndexTablesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Could not open the logger"
            " index file: " + mIndexTablesFileName);
            
      //Write out the headers for both files.
      MessageDataBaseHeader msgHeader;
      ReadMessageDataBaseHeader(msgHeader);      
      
      //For the index file, we need to read the header and the keyframe/log entries
      //contained in it.
      IndexTableHeader indexHeader;
      ReadIndexTableHeader(indexHeader);
      ReadIndexTables();
      
      //Close the index file stream.. When we flush the newly created tags and keyframes,
      //we reopen the index file in write only mode.
      fclose(mIndexTablesFile);
      mIndexTablesFile = NULL;
      
      mEndOfStream = false;
   }
   
   //////////////////////////////////////////////////////////////////////////   
   void BinaryLogStream::ReadMessageDataBaseHeader(MessageDataBaseHeader &header)
   {
      if (mMessagesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Failed to read messages "
            "database header.  File is not valid."); 
            
      //Move to the beginning of the file.
      fseek(mMessagesFile,0L,SEEK_SET);
      
      char magicNumber[11];
      fread(&magicNumber[0],1,10,mMessagesFile);
      magicNumber[10] = '\0';
      header.magicNumber = magicNumber;
      
      fread((char *)&header.majorVersion,1,1,mMessagesFile);
      fread((char *)&header.minorVersion,1,1,mMessagesFile);      
      fread((char *)&header.indexTableFileNameLength,sizeof(unsigned short),1,mMessagesFile);
      
      char *indexFile = new char[header.indexTableFileNameLength+1];
      fread(&indexFile[0],1,header.indexTableFileNameLength,mMessagesFile);
      indexFile[header.indexTableFileNameLength] = '\0';
      header.indexTableFileName = indexFile;
      delete [] indexFile;
         
      std::string errorString;
      if (!header.validate(errorString))
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Malformed message database"
            " header. Reason: " + errorString); 
   }
   
   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::WriteMessageDataBaseHeader(MessageDataBaseHeader &header)
   {
      if (mMessagesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Failed to write messages "
            "database header.  File is not valid.");         
            
      std::string errorString;
      if (!header.validate(errorString))
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Malformed message database"
            " header. Reason: " + errorString); 
            
      WriteToLog(header.magicNumber.c_str(),1,header.magicNumber.length(),mMessagesFile);
      WriteToLog((char *)&header.majorVersion,1,1,mMessagesFile);
      WriteToLog((char *)&header.minorVersion,1,1,mMessagesFile);
      WriteToLog((char *)&header.indexTableFileNameLength,1,sizeof(unsigned short),mMessagesFile);
      WriteToLog(header.indexTableFileName.c_str(),1,
         header.indexTableFileName.length(),mMessagesFile);
   }
   
   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::ReadIndexTableHeader(IndexTableHeader &header)
   {
      if (mIndexTablesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Failed to read index "
            "tables header.  File is not valid."); 
            
      //Move to the beginning of the file.
      fseek(mIndexTablesFile,0L,SEEK_SET);
      
      char magicNumber[14];
      fread(&magicNumber[0],1,13,mIndexTablesFile);
      magicNumber[13] = '\0';
      header.magicNumber = magicNumber;
      
      fread((char *)&header.majorVersion,1,1,mIndexTablesFile);
      fread((char *)&header.minorVersion,1,1,mIndexTablesFile);      
      fread((char *)&header.msgDBFileNameLength,sizeof(unsigned short),1,mIndexTablesFile);
      
      char *msgDBFile = new char[header.msgDBFileNameLength+1];
      fread(&msgDBFile[0],1,header.msgDBFileNameLength,mIndexTablesFile);
      msgDBFile[header.msgDBFileNameLength] = '\0';
      header.msgDBFileName = msgDBFile;
      delete [] msgDBFile;
         
      std::string errorString;
      if (!header.validate(errorString))
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Malformed index database"
            " header. Reason: " + errorString); 
   }
   
   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::WriteIndexTableHeader(IndexTableHeader &header)
   {
      if (mIndexTablesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Failed to write index "
            "tables header.  File is not valid.");         
            
      std::string errorString;
      if (!header.validate(errorString))
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Malformed message database"
            " header. Reason: " + errorString); 
            
      WriteToLog(header.magicNumber.c_str(),1,header.magicNumber.length(),mIndexTablesFile);
      WriteToLog((char *)&header.majorVersion,1,1,mIndexTablesFile);
      WriteToLog((char *)&header.minorVersion,1,1,mIndexTablesFile);
      WriteToLog((char *)&header.msgDBFileNameLength,1,sizeof(unsigned short),mIndexTablesFile);
      WriteToLog(header.msgDBFileName.c_str(),1,
         header.msgDBFileNameLength,mIndexTablesFile);
   }

   //////////////////////////////////////////////////////////////////////////   
   void BinaryLogStream::WriteMessage(const Message &msg, double timeStamp)
   {
      //Make sure we have a valid file.
      if (mMessagesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Failed to write message. "
            "Message database file is not valid.");      
      
      unsigned short msgID = msg.GetMessageType().GetId();
      WriteToLog((char *)&BinaryLogStream::MESSAGE_DEID,1,1,mMessagesFile);      
      WriteToLog((char *)&msgID,sizeof(unsigned short),1,mMessagesFile);      
      WriteToLog((char *)&timeStamp,sizeof(double),1,mMessagesFile);      
      
      //Make sure we wrote the data properly.
      CheckFileStatus(mMessagesFile);
      
      //Get the size of the message and write that along with the message data stream.
      unsigned int bufferSize;
      DataStream dataStream;
      
      dataStream << msg.GetAboutActorId() << msg.GetSendingActorId();
      msg.ToDataStream(dataStream);
      bufferSize = dataStream.GetBufferSize();
      WriteToLog((char *)&bufferSize,sizeof(unsigned int),1,mMessagesFile);
      if (bufferSize != 0)
         WriteToLog((char *)dataStream.GetBuffer(),1,bufferSize,mMessagesFile);
         
      CheckFileStatus(mMessagesFile);
   }
   
   //////////////////////////////////////////////////////////////////////////      
   dtCore::RefPtr<Message> BinaryLogStream::ReadMessage(double &timeStamp)
   {
      //Make sure we have a valid file.
      if (mMessagesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Failed to read message. "
            "Message database file is not valid.");      
            
      if (feof(mMessagesFile)) {
         mEndOfStream = true;
         return NULL;      
      }
      
      unsigned char dEID;
      fread((char *)&dEID,1,1,mMessagesFile);        
      if (feof(mMessagesFile))
         return NULL;
      
      CheckFileStatus(mMessagesFile);
      if (dEID != BinaryLogStream::MESSAGE_DEID)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Failed to read message. "
            "Invalid message element identifier found.");
      
      //Get the type of message and create the message object.
      unsigned short msgID;
      fread((char *)&msgID,sizeof(unsigned short),1,mMessagesFile);
      fread((char *)&timeStamp,sizeof(double),1,mMessagesFile);
      const MessageType &msgType = GetMessageFactory().GetMessageTypeById(msgID);
      
      dtCore::RefPtr<Message> msg = NULL;
      msg = GetMessageFactory().CreateMessage(msgType);
      
      //Create a temporary buffer, and read the message from it.      
      unsigned int bufferSize;
      fread((char *)&bufferSize,sizeof(unsigned int),1,mMessagesFile);
      if (bufferSize != 0)
      {
         char *tempBuffer = new char[bufferSize];
         fread(&tempBuffer[0],1,bufferSize,mMessagesFile);
         
         DataStream stream(tempBuffer,bufferSize);
         
         dtCore::UniqueId sendingActorId,aboutActorId;
         stream >> aboutActorId >> sendingActorId;
         msg->SetAboutActorId(aboutActorId);
         msg->SetSendingActorId(sendingActorId);
         msg->FromDataStream(stream);
      }     
      
      CheckFileStatus(mMessagesFile);
      return msg;
   }
  
   //////////////////////////////////////////////////////////////////////////       
   void BinaryLogStream::ReadIndexTables()
   {
      //Make sure we have a valid file.
      if (mIndexTablesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Failed to read index table. "
            "Index table file is not valid.");
            
      unsigned char deID;
      fread((char *)&deID,1,1,mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);
      while (!feof(mIndexTablesFile))
      {
         switch (deID)
         {
            case BinaryLogStream::TAG_DEID:
               mExistingTags.push_back(ReadTag());
               break;
               
            case BinaryLogStream::KEYFRAME_DEID:
               mExistingKeyFrames.push_back(ReadKeyFrame());
               break;
               
            default:
               EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Malformed index tables file. "
                  "Encountered an invalid data element id.");
               break;
         }         
         
         fread((char *)&deID,1,1,mIndexTablesFile);
      } 
      
      CheckFileStatus(mIndexTablesFile);     
   }
   
   //////////////////////////////////////////////////////////////////////////      
   void BinaryLogStream::CheckFileStatus(FILE *fp)
   {
      int error = ferror(fp);      
      if (error != 0)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,std::string(strerror(error)));         
   }

   //////////////////////////////////////////////////////////////////////////         
   void BinaryLogStream::GetTagIndex(std::vector<LogTag> &tags)
   {
      //The tag index is a collection of the existing tags plus
      //the new ones that were created during playback or record.
      std::vector<LogTag>::iterator itor;
      tags.clear();
      
      for (itor=mExistingTags.begin(); itor!=mExistingTags.end(); ++itor)
         tags.push_back(*itor);
         
      for (itor=mNewTags.begin(); itor!=mNewTags.end(); ++itor)
         tags.push_back(*itor);      
   }
   
   //////////////////////////////////////////////////////////////////////////      
   void BinaryLogStream::GetKeyFrameIndex(std::vector<LogKeyframe> &keyFrames)
   {
      //The keyframe index is a collection of the existing keyframes
      //plus any new keyframes that were generated during record or
      //playback.
      std::vector<LogKeyframe>::iterator itor;
      keyFrames.clear();
      
      for (itor=mExistingKeyFrames.begin(); itor!=mExistingKeyFrames.end(); ++itor)
         keyFrames.push_back(*itor);
         
      for (itor=mNewKeyFrames.begin(); itor!=mNewKeyFrames.end(); ++itor)
         keyFrames.push_back(*itor);      
   }
   
   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::InsertTag(LogTag &newTag)
   {
      mNewTags.push_back(newTag);
   }
   
   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::InsertKeyFrame(LogKeyframe &newKeyFrame)
   {
      if (mMessagesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Could not insert a new keyframe. "
            "The messages database file is invalid.");
            
      newKeyFrame.SetLogFileOffset(ftell(mMessagesFile));
      mNewKeyFrames.push_back(newKeyFrame);
   }

   //////////////////////////////////////////////////////////////////////////      
   void BinaryLogStream::JumpToKeyFrame(const LogKeyframe &keyFrame)
   {
      if (mMessagesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Could not jump to the keyframe. "
            "The messages database file is invalid.");
            
      //First determine if this keyframe exists within the keyframe index.
      bool validKeyFrame = false;
      std::vector<LogKeyframe>::iterator itor;
      
      for (itor=mExistingKeyFrames.begin(); itor!=mExistingKeyFrames.end(); ++itor)
      {
         if (itor->GetUniqueId() == keyFrame.GetUniqueId())
         {
            validKeyFrame = true;
            break;
         }
      }
      
      if (!validKeyFrame)
      {
         for (itor=mNewKeyFrames.begin(); itor!=mNewKeyFrames.end(); ++itor)
         {
            if (itor->GetUniqueId() == keyFrame.GetUniqueId())
            {
               validKeyFrame = true;
               break;
            }
         }
      }
      
      if (!validKeyFrame)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Cannot jump to keyframe:" +
            keyFrame.GetName() + " .  The Keyframe has not been added.");
            
      //Now we can proceed with the jump.
      fseek(mMessagesFile,keyFrame.GetLogFileOffset(),SEEK_SET);
      CheckFileStatus(mMessagesFile);
   }
   
   //////////////////////////////////////////////////////////////////////////   
   void BinaryLogStream::Flush()
   {
      std::vector<LogTag>::iterator tagItor;
      std::vector<LogKeyframe>::iterator keyFrameItor;
      
      if (mMessagesFile == NULL)
         EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Cannot flush the stream. "
            "Messages database file is invalid.");
            
      if (mIndexTablesFile == NULL)
      {         
         //If the index file in NULL then we must have opened the log stream for
         //reading.  So to flush, we need to open the index file in write mode.
         mIndexTablesFile = fopen(mIndexTablesFileName.c_str(),"ab");
         if (mIndexTablesFile == NULL)
            EXCEPT(LogStreamException::LOGGER_IO_EXCEPTION,"Cannot flush the stream. "
               "Index tables file is invalid.");
      }
            
      //Add the new tags...
      for (tagItor=mNewTags.begin(); tagItor!=mNewTags.end(); ++tagItor)
      {
         WriteTag(*tagItor);
         mExistingTags.push_back(*tagItor);
      }
       
      for (keyFrameItor = mNewKeyFrames.begin(); keyFrameItor!=mNewKeyFrames.end();
         ++keyFrameItor)
      {
         WriteKeyFrame(*keyFrameItor);
         mExistingKeyFrames.push_back(*keyFrameItor);
      }
      
      mNewTags.clear();
      mNewKeyFrames.clear();
      
      fflush(mMessagesFile);
      
      //Close the index file.
      fflush(mIndexTablesFile);
      fclose(mIndexTablesFile);
      mIndexTablesFile = NULL;
   }
   
   //////////////////////////////////////////////////////////////////////////   
   void BinaryLogStream::WriteTag(const LogTag &tag)
   {
      DataStream stream;
      unsigned int bufferSize;
      
      stream << tag.GetName() << tag.GetDescription() << tag.GetSimTimeStamp() <<
         tag.GetUniqueId();
      bufferSize = stream.GetBufferSize();
      
      WriteToLog((char *)&BinaryLogStream::TAG_DEID,1,1,mIndexTablesFile);
      WriteToLog((char *)&bufferSize,sizeof(unsigned int),1,mIndexTablesFile);
      WriteToLog((char *)stream.GetBuffer(),bufferSize,1,mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);
   }
   
   //////////////////////////////////////////////////////////////////////////
   LogTag BinaryLogStream::ReadTag()
   {
      LogTag tag;
            
      unsigned int bufferSize;
      fread((char *)&bufferSize,sizeof(unsigned int),1,mIndexTablesFile);
     
      char *tempBuffer = new char[bufferSize];
      fread(&tempBuffer[0],bufferSize,1,mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);
      
      DataStream stream(tempBuffer,bufferSize);
      std::string name,desc;
      dtCore::UniqueId uuid;
      double simTime;      
      stream >> name >> desc >> simTime >> uuid;
      
      tag.SetName(name);
      tag.SetDescription(desc);
      tag.SetSimTimeStamp(simTime);      
      tag.SetUniqueId(uuid);            
      return tag;
   }
    
   //////////////////////////////////////////////////////////////////////////   
   void BinaryLogStream::WriteKeyFrame(const LogKeyframe &keyFrame)
   {
      DataStream stream;
      unsigned int bufferSize;
      
      stream << keyFrame.GetName() << keyFrame.GetDescription() << keyFrame.GetSimTimeStamp() <<
         keyFrame.GetUniqueId() << keyFrame.GetActiveMap() << keyFrame.GetLogFileOffset();
      bufferSize = stream.GetBufferSize();
      
      WriteToLog((char *)&BinaryLogStream::KEYFRAME_DEID,1,1,mIndexTablesFile);
      WriteToLog((char *)&bufferSize,sizeof(unsigned int),1,mIndexTablesFile);
      WriteToLog((char *)stream.GetBuffer(),bufferSize,1,mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);
   }
   
   //////////////////////////////////////////////////////////////////////////
   LogKeyframe BinaryLogStream::ReadKeyFrame()
   {
      LogKeyframe keyFrame;
      
      unsigned int bufferSize;
      fread((char *)&bufferSize,sizeof(unsigned int),1,mIndexTablesFile);

      char *tempBuffer = new char[bufferSize];
      fread(&tempBuffer[0],bufferSize,1,mIndexTablesFile);      
      CheckFileStatus(mIndexTablesFile);
      
      DataStream stream(tempBuffer,bufferSize);   
      std::string name,desc,activeMap;
      double simTime;
      long offset;
      dtCore::UniqueId uuid;               
      stream >> name >> desc >> simTime >> uuid >> activeMap >> offset;
      
      keyFrame.SetName(name);
      keyFrame.SetDescription(desc);
      keyFrame.SetSimTimeStamp(simTime);
      keyFrame.SetUniqueId(uuid);
      keyFrame.SetActiveMap(activeMap);
      keyFrame.SetLogFileOffset(offset);      
      return keyFrame;
   }
   
}
