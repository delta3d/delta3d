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
#include <prefix/dtgameprefix.h>
#include <dtGame/binarylogstream.h>
#include <dtGame/messagetype.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datastream.h>
#include <dtCore/uniqueid.h>

#include <iostream>
#include <set>

#include <osgDB/FileNameUtils>

#include <cstring>

using dtUtil::DataStream;

namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   const std::string BinaryLogStream::LOGGER_MSGDB_MAGIC_NUMBER("GMLOGMSGDB");
   const std::string BinaryLogStream::LOGGER_INDEX_MAGIC_NUMBER("GMLOGINDEXTAB");
   const unsigned char BinaryLogStream::LOGGER_MAJOR_VERSION = 1;
   const unsigned char BinaryLogStream::LOGGER_MINOR_VERSION = 1;

   const std::string BinaryLogStream::MESSAGE_DB_EXT(".dlm");
   const std::string BinaryLogStream::INDEX_EXT(".dli");

   const unsigned char BinaryLogStream::MESSAGE_DEID = 0;
   const unsigned char BinaryLogStream::TAG_DEID = 1;
   const unsigned char BinaryLogStream::KEYFRAME_DEID = 2;
   const unsigned char BinaryLogStream::END_SECTION_DEID = 255;

   //////////////////////////////////////////////////////////////////////////
   BinaryLogStream::BinaryLogStream(MessageFactory& msgFactory)
      : LogStream(msgFactory)
      , mMessagesFile(NULL)
      , mIndexTablesFile(NULL)
      , mCurrentMinorVersion(0)
      , mFilesAreOpenForWriting(false)
   {
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
      {
         Flush();
      }

      // Flush uses this, so don't set this until after FLUSH.
      mFilesAreOpenForWriting = false;

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
   void BinaryLogStream::Create(const std::string& logsPath, const std::string& logResourceName)
   {
      // Make sure the stream is not already open.
      Close();

      // Make sure we remove any trailing slashes from the cache path.
      std::string newPath = logsPath;
      if (newPath[newPath.length()-1] == '/' || newPath[newPath.length()-1] == '\\')
      {
         newPath = newPath.substr(0,newPath.length()-1);
      }

      // Create the messages file...
      mMessagesFileName = newPath + "/" + logResourceName;
      mMessagesFileName += BinaryLogStream::MESSAGE_DB_EXT;
      mMessagesFile = fopen(mMessagesFileName.c_str(), "wb");
      if (mMessagesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Could not create the messages"
            " database file: " + mMessagesFileName, __FILE__, __LINE__);
      }

      // Create the index tables file...
      mIndexTablesFileName = newPath + "/" + logResourceName;
      mIndexTablesFileName += BinaryLogStream::INDEX_EXT;
      mIndexTablesFile = fopen(mIndexTablesFileName.c_str(), "wb");
      if (mIndexTablesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Could not create the logger"
            " index file: " + mIndexTablesFileName, __FILE__, __LINE__);
      }

      // Write out the headers for both files.
      MessageDataBaseHeader msgHeader;
      msgHeader.magicNumber = BinaryLogStream::LOGGER_MSGDB_MAGIC_NUMBER;
      msgHeader.majorVersion = BinaryLogStream::LOGGER_MAJOR_VERSION;
      msgHeader.minorVersion = BinaryLogStream::LOGGER_MINOR_VERSION;
      msgHeader.recordLength = 0.0;
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

      mFilesAreOpenForWriting = true; // Write mode - Probably in a Record mode.

      mEndOfStream = false;
   }

   //////////////////////////////////////////////////////////////////////////
   static inline void WriteToLog(const char* data, size_t dataSize, size_t count, FILE* file)
   {
      size_t written = fwrite(data,dataSize, count, file);
      // This check is done in case the file is not in write mode as Mac OS X won't set
      // ferror if it couldn't write the data.
      if (written < count)
      {
         throw dtGame::LogStreamIOException( "Error writing to IO stream.  Data not written. "
            "Check to see if the log file is in write mode.", __FILE__, __LINE__);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::Open(const std::string& logsPath, const std::string& logResourceName)
   {
      // Make sure the stream is not already open.
      Close();

      // Make sure we remove any trailing slashes from the cache path.
      std::string newPath = logsPath;
      if (newPath[newPath.length()-1] == '/' || newPath[newPath.length()-1] == '\\')
      {
         newPath = newPath.substr(0,newPath.length()-1);
      }

      // Open the messages file.
      mMessagesFileName = newPath + "/" + logResourceName;
      mMessagesFileName += BinaryLogStream::MESSAGE_DB_EXT;
      mMessagesFile = fopen(mMessagesFileName.c_str(), "rb");
      if (mMessagesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Could not open the messages"
            " database file: " + mMessagesFileName, __FILE__, __LINE__);
      }

      // Open the index tables file.  Note, we open this file in
      // read and write mode so we can read the existing index and
      // append to it if needed.
      mIndexTablesFileName = newPath + "/" + logResourceName;
      mIndexTablesFileName += BinaryLogStream::INDEX_EXT;
      mIndexTablesFile = fopen(mIndexTablesFileName.c_str(), "rb");
      if (mIndexTablesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Could not open the logger"
            " index file: " + mIndexTablesFileName, __FILE__, __LINE__);
      }

      // Write out the headers for both files.
      MessageDataBaseHeader msgHeader;
      ReadMessageDataBaseHeader(msgHeader);
      SetRecordDuration(msgHeader.recordLength);

      // For the index file, we need to read the header and the keyframe/log entries
      // contained in it.
      IndexTableHeader indexHeader;
      ReadIndexTableHeader(indexHeader);
      mCurrentMinorVersion = indexHeader.minorVersion;
      ReadIndexTables();

      // Close the index file stream.. When we flush the newly created tags and keyframes,
      // we reopen the index file in write only mode.
      fclose(mIndexTablesFile);
      mIndexTablesFile = NULL;

      mFilesAreOpenForWriting = false; // Read only - We're probably in PLAYBACK mode
      mEndOfStream = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::Delete(const std::string& logsPath,
      const std::string& logResourceName)
   {
      Close();

      // Make sure we remove any trailing slashes from the cache path.
      std::string newPath = logsPath;
      if (newPath[newPath.length()-1] == '/' || newPath[newPath.length()-1] == '\\')
      {
         newPath = newPath.substr(0,newPath.length()-1);
      }

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      if (!fileUtils.DirExists(logsPath))
      {
         LOG_WARNING("Could not locate the directory: " + logsPath);
         return;
      }

      std::string msgPath = newPath + "/" + logResourceName + ".dlm";
      std::string indexPath = newPath + "/" + logResourceName + ".dli";

      if (!fileUtils.FileExists(msgPath) || !fileUtils.FileExists(indexPath))
      {
         LOG_WARNING("Could not delete the file: " + msgPath);
         return;
      }

      fileUtils.FileDelete(msgPath);
      fileUtils.FileDelete(indexPath);
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::GetAvailableLogs(const std::string& logsPath,
            std::vector<std::string>& logs)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      if (!fileUtils.DirExists(logsPath))
      {
         throw dtGame::LogStreamIOException( "Could not get available log"
            " files.  Log Directory: " + logsPath + " does not exist.", __FILE__, __LINE__);
      }

      logs.clear();

      // First sort all the files in the specified directory into bins
      // containing index files and messages files.
      dtUtil::DirectoryContents fileList = fileUtils.DirGetFiles(logsPath);
      dtUtil::DirectoryContents::iterator itor = fileList.begin();
      std::set<std::string> messagesFiles, indexFiles;
      for (itor = fileList.begin(); itor != fileList.end(); ++itor)
      {
         if (itor->length() < 4)
         {
            continue;
         }

         if (itor->substr(itor->length() - 4,4) == ".dlm")
         {
            messagesFiles.insert(itor->substr(0, itor->length() - 4));
         }
         else if ((itor->substr(itor->length() - 4, 4) == ".dli"))
         {
            indexFiles.insert(itor->substr(0, itor->length() - 4));
         }
      }

      // Now using the messages file set, match it with the index file set.  If
      // we found a match, then add it to the list of available logs.
      std::set<std::string>::iterator msgItor;
      for (msgItor = messagesFiles.begin(); msgItor != messagesFiles.end(); ++msgItor)
      {
         std::set<std::string>::iterator indexItor;
         indexItor = indexFiles.find(*msgItor);
         if (indexItor != indexFiles.end())
         {
            logs.push_back(*msgItor);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::ReadMessageDataBaseHeader(MessageDataBaseHeader& header)
   {
      if (mMessagesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Failed to read messages "
            "database header.  File is not valid.", __FILE__, __LINE__);
      }

      // Move to the beginning of the file.
      fseek(mMessagesFile, 0L, SEEK_SET);

      char magicNumber[11];
      size_t numRead = fread(&magicNumber[0], 1, 10, mMessagesFile);
      CheckFileStatus(mMessagesFile);
      magicNumber[10] = '\0';
      header.magicNumber = magicNumber;

      numRead = fread((char*)&header.majorVersion, 1, 1, mMessagesFile);
      CheckFileStatus(mMessagesFile);
      numRead = fread((char*)&header.minorVersion, 1, 1, mMessagesFile);
      CheckFileStatus(mMessagesFile);
      numRead = fread((char*)&header.recordLength, sizeof(double), 1, mMessagesFile);
      CheckFileStatus(mMessagesFile);
      numRead = fread((char*)&header.indexTableFileNameLength, sizeof(unsigned short), 1, mMessagesFile);
      CheckFileStatus(mMessagesFile);

      char* indexFile = new char[header.indexTableFileNameLength + 1];
      numRead = fread(&indexFile[0], 1, header.indexTableFileNameLength, mMessagesFile);
      CheckFileStatus(mMessagesFile);
      indexFile[header.indexTableFileNameLength] = '\0';
      header.indexTableFileName = indexFile;
      delete [] indexFile;

      std::string errorString;
      if (!header.validate(errorString))
      {
         throw dtGame::LogStreamIOException( "Malformed message database"
            " header. Reason: " + errorString, __FILE__, __LINE__);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::WriteMessageDataBaseHeader(MessageDataBaseHeader& header)
   {
      if (mMessagesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Failed to write messages "
            "database header.  File is not valid.", __FILE__, __LINE__);
      }

      std::string errorString;
      if (!header.validate(errorString))
      {
         throw dtGame::LogStreamIOException( "Malformed message database"
            " header. Reason: " + errorString, __FILE__, __LINE__);
      }

      WriteToLog(header.magicNumber.c_str(), 1, header.magicNumber.length(), mMessagesFile);
      WriteToLog((char*)&header.majorVersion, 1, 1, mMessagesFile);
      WriteToLog((char*)&header.minorVersion, 1, 1, mMessagesFile);
      WriteToLog((char*)&header.recordLength, sizeof(double), 1, mMessagesFile);

      std::string simpleName = osgDB::getSimpleFileName(header.indexTableFileName);

      size_t length = simpleName.length();

      WriteToLog((char*)&length, 1, sizeof(unsigned short), mMessagesFile);
      WriteToLog(simpleName.c_str(), 1, simpleName.length(), mMessagesFile);
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::ReadIndexTableHeader(IndexTableHeader& header)
   {
      if (mIndexTablesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Failed to read index "
            "tables header.  File is not valid.", __FILE__, __LINE__);
      }

      // Move to the beginning of the file.
      fseek(mIndexTablesFile, 0L, SEEK_SET);

      char magicNumber[14];
      size_t numRead = fread(&magicNumber[0], 1, 13, mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);

      magicNumber[13] = '\0';
      header.magicNumber = magicNumber;

      numRead = fread((char*)&header.majorVersion, 1, 1, mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);
      numRead = fread((char*)&header.minorVersion, 1, 1, mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);
      numRead = fread((char*)&header.msgDBFileNameLength, sizeof(unsigned short), 1, mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);

      char* msgDBFile = new char[header.msgDBFileNameLength + 1];
      numRead = fread(&msgDBFile[0], 1, header.msgDBFileNameLength, mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);
      msgDBFile[header.msgDBFileNameLength] = '\0';
      header.msgDBFileName = msgDBFile;
      delete [] msgDBFile;

      std::string errorString;
      if (!header.validate(errorString))
      {
         throw dtGame::LogStreamIOException( "Malformed index database"
            " header. Reason: " + errorString, __FILE__, __LINE__);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::WriteIndexTableHeader(IndexTableHeader& header)
   {
      if (mIndexTablesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Failed to write index "
            "tables header.  File is not valid.", __FILE__, __LINE__);
      }

      std::string errorString;
      if (!header.validate(errorString))
      {
         throw dtGame::LogStreamIOException( "Malformed message database"
            " header. Reason: " + errorString, __FILE__, __LINE__);
      }

      WriteToLog(header.magicNumber.c_str(), 1, header.magicNumber.length(), mIndexTablesFile);
      WriteToLog((char*)&header.majorVersion, 1, 1, mIndexTablesFile);
      WriteToLog((char*)&header.minorVersion, 1, 1, mIndexTablesFile);

      std::string simpleName = osgDB::getSimpleFileName(header.msgDBFileName);
      size_t length = simpleName.length();

      WriteToLog((char*)&length, 1, sizeof(unsigned short), mIndexTablesFile);
      WriteToLog(simpleName.c_str(), 1, simpleName.length(), mIndexTablesFile);
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::WriteMessage(const Message& msg, double timeStamp)
   {
      // Make sure we have a valid file.
      if (mMessagesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Failed to write message. "
            "Message database file is not valid.", __FILE__, __LINE__);
      }

      unsigned short msgID = msg.GetMessageType().GetId();
      WriteToLog((char*)&BinaryLogStream::MESSAGE_DEID, 1, 1, mMessagesFile);
      WriteToLog((char*)&msgID, sizeof(unsigned short), 1, mMessagesFile);
      WriteToLog((char*)&timeStamp, sizeof(double), 1, mMessagesFile);

      // Make sure we wrote the data properly.
      CheckFileStatus(mMessagesFile);

      // Get the size of the message and write that along with the message data stream.
      unsigned int bufferSize;
      dtUtil::DataStream dataStream;

      dataStream << msg.GetAboutActorId() << msg.GetSendingActorId();
      msg.ToDataStream(dataStream);
      bufferSize = dataStream.GetBufferSize();
      WriteToLog((char*)&bufferSize, sizeof(unsigned int), 1, mMessagesFile);
      if (bufferSize != 0)
      {
         WriteToLog((char*)dataStream.GetBuffer(), 1, bufferSize, mMessagesFile);
      }

      CheckFileStatus(mMessagesFile);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Message> BinaryLogStream::ReadMessage(double& timeStamp)
   {
      // Make sure we have a valid file.
      if (mMessagesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Failed to read message. "
            "Message database file is not valid.", __FILE__, __LINE__);
      }

      if (feof(mMessagesFile))
      {
         mEndOfStream = true;
         return NULL;
      }

      unsigned char dEID;
      size_t numRead = fread((char*)&dEID, 1, 1, mMessagesFile);
      CheckFileStatus(mMessagesFile);
      if (feof(mMessagesFile))
      {
         return NULL;
      }

      CheckFileStatus(mMessagesFile);
      if (dEID != BinaryLogStream::MESSAGE_DEID)
      {
         throw dtGame::LogStreamIOException( "Failed to read message. "
            "Invalid message element identifier found.", __FILE__, __LINE__);
      }

      // Get the type of message and create the message object.
      unsigned short msgID;
      numRead = fread((char*)&msgID, sizeof(unsigned short), 1, mMessagesFile);
      CheckFileStatus(mMessagesFile);
      numRead = fread((char*)&timeStamp, sizeof(double), 1, mMessagesFile);
      CheckFileStatus(mMessagesFile);
      const MessageType& msgType = GetMessageFactory().GetMessageTypeById(msgID);

      dtCore::RefPtr<Message> msg = NULL;
      msg = GetMessageFactory().CreateMessage(msgType);

      // Create a temporary buffer, and read the message from it.
      unsigned int bufferSize;
      numRead = fread((char*)&bufferSize, sizeof(unsigned int), 1, mMessagesFile);
      CheckFileStatus(mMessagesFile);
      if (bufferSize != 0)
      {
         char* tempBuffer = new char[bufferSize];
         numRead = fread(&tempBuffer[0], 1, bufferSize, mMessagesFile);
         CheckFileStatus(mMessagesFile);

         dtUtil::DataStream stream(tempBuffer, bufferSize);

         dtCore::UniqueId sendingActorId, aboutActorId;
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
      // Make sure we have a valid file.
      if (mIndexTablesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Failed to read index table. "
            "Index table file is not valid.", __FILE__, __LINE__);
      }

      unsigned char deID;
      size_t numRead = fread((char*)&deID, 1, 1, mIndexTablesFile);
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
            throw dtGame::LogStreamIOException( "Malformed index tables file. "
               "Encountered an invalid data element id.", __FILE__, __LINE__);
            break;
         }

         numRead = fread((char*)&deID, 1, 1, mIndexTablesFile);
         CheckFileStatus(mIndexTablesFile);
      }

      CheckFileStatus(mIndexTablesFile);
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::CheckFileStatus(FILE* fp)
   {
      int error = ferror(fp);
      if (error != 0)
      {
         throw dtGame::LogStreamIOException( std::string(strerror(error)), __FILE__, __LINE__);
      }
      //else if (feof(fp))
      //   throw dtGame::LogStreamIOException( "End of file unexpectedly reached.", __FILE__, __LINE__);
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::GetTagIndex(std::vector<LogTag>& tags)
   {
      // The tag index is a collection of the existing tags plus
      // the new ones that were created during playback or record.
      std::vector<LogTag>::iterator itor;
      tags.clear();

      for (itor = mExistingTags.begin(); itor != mExistingTags.end(); ++itor)
      {
         tags.push_back(*itor);
      }

      for (itor = mNewTags.begin(); itor != mNewTags.end(); ++itor)
      {
         tags.push_back(*itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::GetKeyFrameIndex(std::vector<LogKeyframe>& keyFrames)
   {
      // The keyframe index is a collection of the existing keyframes
      // plus any new keyframes that were generated during record or
      // playback.
      std::vector<LogKeyframe>::iterator itor;
      keyFrames.clear();

      for (itor = mExistingKeyFrames.begin(); itor != mExistingKeyFrames.end(); ++itor)
      {
         keyFrames.push_back(*itor);
      }

      for (itor = mNewKeyFrames.begin(); itor != mNewKeyFrames.end(); ++itor)
      {
         keyFrames.push_back(*itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::InsertTag(LogTag& newTag)
   {
      mNewTags.push_back(newTag);
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::InsertKeyFrame(LogKeyframe& newKeyFrame)
   {
      if (mMessagesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Could not insert a new keyframe. "
            "The messages database file is invalid.", __FILE__, __LINE__);
      }

      newKeyFrame.SetLogFileOffset(ftell(mMessagesFile));
      mNewKeyFrames.push_back(newKeyFrame);
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::JumpToKeyFrame(const LogKeyframe& keyFrame)
   {
      if (mMessagesFile == NULL)
      {
         throw dtGame::LogStreamIOException( "Could not jump to the keyframe. "
            "The messages database file is invalid.", __FILE__, __LINE__);
      }

      // First determine if this keyframe exists within the keyframe index.
      bool validKeyFrame = false;
      std::vector<LogKeyframe>::iterator itor;

      for (itor = mExistingKeyFrames.begin(); itor != mExistingKeyFrames.end(); ++itor)
      {
         if (itor->GetUniqueId() == keyFrame.GetUniqueId())
         {
            validKeyFrame = true;
            break;
         }
      }

      if (!validKeyFrame)
      {
         for (itor = mNewKeyFrames.begin(); itor != mNewKeyFrames.end(); ++itor)
         {
            if (itor->GetUniqueId() == keyFrame.GetUniqueId())
            {
               validKeyFrame = true;
               break;
            }
         }
      }

      if (!validKeyFrame)
      {
         throw dtGame::LogStreamIOException( "Cannot jump to keyframe:" +
            keyFrame.GetName() + " .  The Keyframe has not been added.", __FILE__, __LINE__);
      }

      // Now we can proceed with the jump.
      fseek(mMessagesFile, keyFrame.GetLogFileOffset(), SEEK_SET);
      CheckFileStatus(mMessagesFile);
      mEndOfStream = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::Flush()
   {
      std::vector<LogTag>::iterator tagItor;
      std::vector<LogKeyframe>::iterator keyFrameItor;

      if (mFilesAreOpenForWriting)
      {
         if (mIndexTablesFile == NULL)
         {
            // If the index file in NULL then we must have opened the log stream for
            // reading.  So to flush, we need to open the index file in write mode.
            mIndexTablesFile = fopen(mIndexTablesFileName.c_str(), "ab");
            if (mIndexTablesFile == NULL)
            {
               throw dtGame::LogStreamIOException( "Cannot flush the stream. "
                  "Index tables file is invalid.", __FILE__, __LINE__);
            }
         }

         // Add the new tags...
         for (tagItor = mNewTags.begin(); tagItor != mNewTags.end(); ++tagItor)
         {
            WriteTag(*tagItor);
            mExistingTags.push_back(*tagItor);
         }

         for (keyFrameItor = mNewKeyFrames.begin(); keyFrameItor != mNewKeyFrames.end();
            ++keyFrameItor)
         {
            WriteKeyFrame(*keyFrameItor);
            mExistingKeyFrames.push_back(*keyFrameItor);
         }

         fflush(mIndexTablesFile);
      } // if (mFilesAreOpenForWriting)

      mNewTags.clear();
      mNewKeyFrames.clear();

      if (mIndexTablesFile != NULL)
      {
         fclose(mIndexTablesFile);
      }
      mIndexTablesFile = NULL;

      // Update the header which can possible contain an updated record duration since
      // this is typically last set just before closing a log.  This requires that we reopen
      // the messages file.
      if (mMessagesFile == NULL)
      {
         return;
      }

      if (mFilesAreOpenForWriting)
      {
         fflush(mMessagesFile);
      }
      fclose(mMessagesFile);

      if (mFilesAreOpenForWriting)
      {
         mMessagesFile = fopen(mMessagesFileName.c_str(), "rb+");
         if (mMessagesFile == NULL)
         {
            return;
         }

         MessageDataBaseHeader msgHeader;
         msgHeader.magicNumber = BinaryLogStream::LOGGER_MSGDB_MAGIC_NUMBER;
         msgHeader.majorVersion = BinaryLogStream::LOGGER_MAJOR_VERSION;
         msgHeader.minorVersion = BinaryLogStream::LOGGER_MINOR_VERSION;
         msgHeader.recordLength = GetRecordDuration();
         msgHeader.indexTableFileNameLength = mIndexTablesFileName.length();
         msgHeader.indexTableFileName = osgDB::getSimpleFileName(mIndexTablesFileName);
         WriteMessageDataBaseHeader(msgHeader);

         // Close the messages file.
         fflush(mMessagesFile);
         fclose(mMessagesFile);
      }
      mMessagesFile = NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::WriteTag(const LogTag& tag)
   {
      DataStream stream;
      unsigned int bufferSize;

      stream << tag.GetName() << tag.GetDescription() << tag.GetSimTimeStamp() <<
         tag.GetUniqueId() << tag.GetKeyframeUniqueId() << tag.GetCaptureKeyframe();
      bufferSize = stream.GetBufferSize();

      WriteToLog((char*)&BinaryLogStream::TAG_DEID,1,1,mIndexTablesFile);
      WriteToLog((char*)&bufferSize,sizeof(unsigned int),1,mIndexTablesFile);
      WriteToLog((char*)stream.GetBuffer(),bufferSize,1,mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);
   }

   //////////////////////////////////////////////////////////////////////////
   LogTag BinaryLogStream::ReadTag()
   {
      LogTag tag;

      unsigned int bufferSize;
      size_t numRead = fread((char*)&bufferSize, sizeof(unsigned int), 1, mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);

      char* tempBuffer = new char[bufferSize];
      numRead = fread(&tempBuffer[0], bufferSize, 1, mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);

      DataStream stream(tempBuffer, bufferSize);
      std::string name, desc;
      dtCore::UniqueId uuid, kfuuid;
      double simTime;
      bool captureKeyframe;
      stream >> name >> desc >> simTime >> uuid >> kfuuid >> captureKeyframe;

      tag.SetName(name);
      tag.SetDescription(desc);
      tag.SetSimTimeStamp(simTime);
      tag.SetUniqueId(uuid);
      tag.SetKeyframeUniqueId(kfuuid);
      tag.SetCaptureKeyframe(captureKeyframe);
      return tag;
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryLogStream::WriteKeyFrame(const LogKeyframe& keyFrame)
   {
      DataStream stream;
      unsigned int bufferSize;

      stream << keyFrame.GetName() << keyFrame.GetDescription() << keyFrame.GetSimTimeStamp() <<
         keyFrame.GetUniqueId() << keyFrame.GetTagUniqueId();

      const LogKeyframe::NameVector& mapNames = keyFrame.GetActiveMaps();

      unsigned short mapCount = mapNames.size();
      stream << mapCount;

      LogKeyframe::NameVector::const_iterator i = mapNames.begin();
      LogKeyframe::NameVector::const_iterator end = mapNames.end();

      for (; i != end; ++i)
      {
         stream << *i;
      }

      stream << keyFrame.GetLogFileOffset();
      bufferSize = stream.GetBufferSize();

      WriteToLog((char*)&BinaryLogStream::KEYFRAME_DEID, 1, 1, mIndexTablesFile);
      WriteToLog((char*)&bufferSize, sizeof(unsigned int), 1, mIndexTablesFile);
      WriteToLog((char*)stream.GetBuffer(), bufferSize, 1, mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);
   }

   //////////////////////////////////////////////////////////////////////////
   LogKeyframe BinaryLogStream::ReadKeyFrame()
   {
      LogKeyframe keyFrame;

      unsigned int bufferSize;
      size_t numRead = fread((char*)&bufferSize, sizeof(unsigned int), 1, mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);

      char* tempBuffer = new char[bufferSize];
      numRead = fread(&tempBuffer[0], bufferSize, 1, mIndexTablesFile);
      CheckFileStatus(mIndexTablesFile);

      DataStream stream(tempBuffer, bufferSize);
      std::string name, desc;
      LogKeyframe::NameVector activeMaps;
      double simTime;
      long offset;
      dtCore::UniqueId uuid, taguuid;
      stream >> name >> desc >> simTime >> uuid >> taguuid;

      // version 1.0 has one map
      if (mCurrentMinorVersion == 0)
      {
         std::string map;
         stream >> map;
         activeMaps.push_back(map);
      }
      else
      {
         // version 1.1 and greater have many maps.
         unsigned short mapCount;
         stream >> mapCount;
         activeMaps.reserve(mapCount);
         for (unsigned i = 0; i < mapCount; ++i)
         {
            std::string map;
            stream >> map;
            activeMaps.push_back(map);
         }
      }

      stream >> offset;

      keyFrame.SetName(name);
      keyFrame.SetDescription(desc);
      keyFrame.SetSimTimeStamp(simTime);
      keyFrame.SetUniqueId(uuid);
      keyFrame.SetTagUniqueId(taguuid);
      keyFrame.SetActiveMaps(activeMaps);
      keyFrame.SetLogFileOffset(offset);
      return keyFrame;
   }

} // namespace dtGame
