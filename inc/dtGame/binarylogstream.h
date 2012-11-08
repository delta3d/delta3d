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

#ifndef DELTA_BINARYLOGSTREAM
#define DELTA_BINARYLOGSTREAM

#include <cstdio>
#include <dtGame/logstream.h>
#include <dtGame/export.h>

namespace dtGame
{
   /**
    * This is a log stream class which supports a binary log file format.
    * The stream actually manages two separate files.  The first file contains
    * a dump of game messages in addition to key-frame captures.  The second
    * file contains two index tables, one for the tags, and one for the key-frames.
    */
   class DT_GAME_EXPORT BinaryLogStream : public LogStream
   {
   public:
      ///Used to identify logger message database files.  The file header must begin
      ///with this string identifier. Equals "GMLOGMSGDB"
      static const std::string LOGGER_MSGDB_MAGIC_NUMBER;

      ///Used to identify logger index table files.  The file header must begin with
      ///this string identifier. Equals "GMLOGINDEXTAB"
      static const std::string LOGGER_INDEX_MAGIC_NUMBER;

      ///Logger major version number.  Equals 1
      static const unsigned char LOGGER_MAJOR_VERSION;

      ///Logger minor version number.  Equals 1
      static const unsigned char LOGGER_MINOR_VERSION;

      /**
       * Constructs a new log stream.
       */
      BinaryLogStream(MessageFactory& msgFactory);

      /**
       * Flushes any write buffers, closes an open file handles, and
       * frees any read/write buffers currently in use.
       */
      virtual void Close();

      /**
       * Creates a new log.  As already noted, the stream is comprised of
       * two physical files.  First, a messages file is created.  This has a
       * an extension of ".dlm" attached to the log resource name.  Second, an
       * index file is created.  This has an extension of ".dli" attached to the
       * log resource name.
       * @param logResourceName The base name of the new log file to create.  This can
       *    be a relative or absolute path; however, this should not include any sort
       *    of file extensions.
       * @note This method implicitly opens the log stream for writing.
       *    Any attempts to read from the stream after this method is called
       *    results in an ILLEGAL_OPERATION exception.
       * @note If any error occurs while creating the stream, a LogStream::LOGGER_IO_EXCEPTION
       *    is thrown.
       */
      virtual void Create(const std::string& logsPath, const std::string& logResourceName);

      /**
       * Opens an existing log.  Again, this implies that two physical files are opened
       * for reading.  These files follow the same naming convention as described in
       * BinaryLogStream::Create().
       * @note This method implicitly opens the log stream for reading.
       *    Any attempts to write to the stream after this method is called results
       *    in an ILLEGAL_OPERATION exception.
       * @note If any error occurs while creating the stream, a LogStream::LOGGER_IO_EXCEPTION
       *    is thrown.
       */
      virtual void Open(const std::string& logsPath, const std::string& logResourceName);

      /**
       * Deletes the specified log resource.
       * @param logsPath The absolute path containing the logs.
       * @param logResourceName The base name of the log resource to destroy.
       *    Implementation should use this in some form in their naming conventions
       *    for log files.
       */
      virtual void Delete(const std::string& logsPAth,
         const std::string& logResourceName);

      /**
       * Gets a list of the current logs available to the log stream by
       * searching the specified directory.
       * @param logsPath An absolute path to the directory containing the
       *    available logs.
       * @param logs Filled with the available logs.
       * @note The log format for the binary log stream actually stores the
       *    log in two files.  Therefore, each entry in the resulting
       *    logs list contains the base name of each log.  This name is used
       *    by the other methods in the BinaryLogStream to identity logs
       *    for reading and writing.
       */
      virtual void GetAvailableLogs(const std::string& logsPath,
         std::vector<std::string>& logs);

      /**
       * Writes a game message to the message database file.
       * @param msg The message to write.
       * @param timeStamp The time stamp matched to this message.
       */
      virtual void WriteMessage(const Message& msg, double timeStamp);

      /**
       * Reads a game message from the messages database file.
       * @param This parameter will contain the time stamp of the read
       *    message.
       * @return The next game message in file.
       */
      virtual dtCore::RefPtr<Message> ReadMessage(double& timeStamp);

      /**
       * Creates and inserts a new tag into the current log stream.
       * @param newTag The new tag to insert.
       */
      virtual void InsertTag(LogTag& newTag);

      /**
       * Creates and inserts a new keyframe into the current log stream.
       * @param newKeyFrame The new keyframe to insert.
       */
      virtual void InsertKeyFrame(LogKeyframe& newKeyFrame);

      /**
       * This method causes the current read position in the messages
       * database file to be positioned at the location indicated by the
       * keyframe.
       * @param keyFrame The keyframe to go to.
       */
      virtual void JumpToKeyFrame(const LogKeyframe& keyFrame);

      /**
       * Gets the list of tags in this log stream.  The tags are
       * located in the index table.
       * @param tags This vector is filled with the tags currently in
       *    the log stream.
       */
      virtual void GetTagIndex(std::vector<LogTag>& tags);

      /**
       * Gets the list of keyframes contained within the log stream.  The keyframes
       * are located in the index table.
       * @param keyFrames This vector is filled with the tags currently
       *    in the log stream.
       */
      virtual void GetKeyFrameIndex(std::vector<LogKeyframe>& keyFrames);

      /**
       * Ensures the all outstanding messages, tags, and keyframes are
       * written to the appropriate files.
       */
      virtual void Flush();

   protected:
      ///The postfix string attached to the base log file name corresponding to
      ///the file containing the database of messages.
      static const std::string MESSAGE_DB_EXT;

      ///File extension for all log files.
      static const std::string INDEX_EXT;

      static const unsigned char MESSAGE_DEID;
      static const unsigned char TAG_DEID;
      static const unsigned char KEYFRAME_DEID;
      static const unsigned char END_SECTION_DEID;

      /**
       * This structure corresponds to the header located at the beginning
       * of the messages database file.
       */
      struct MessageDataBaseHeader
      {
         /**
          * Ensure the data contained in the header is correct.
          * @param error If not valid, this contains the reason why.
          * @return True if valid, false otherwise.
          */
         bool validate(std::string& error)
         {
            if (magicNumber != BinaryLogStream::LOGGER_MSGDB_MAGIC_NUMBER)
            {
               error = "Bad magic number.";
               return false;
            }

            if (majorVersion != BinaryLogStream::LOGGER_MAJOR_VERSION)
            {
               error = "Major version mismatch.";
               return false;
            }

            if (minorVersion != BinaryLogStream::LOGGER_MINOR_VERSION)
            {
               error = "Minor version mismatch.";
               return false;
            }

            error = "";
            return true;
         }

         std::string magicNumber;
         unsigned char majorVersion;
         unsigned char minorVersion;
         double recordLength;
         unsigned short indexTableFileNameLength;
         std::string indexTableFileName;
      };

      /**
       * This structure corresponds to the header located at the beginning
       * of the logs index table file.
       */
      struct IndexTableHeader
      {
         bool validate(std::string& error)
         {
            if (magicNumber != BinaryLogStream::LOGGER_INDEX_MAGIC_NUMBER)
            {
               error = "Bad magic number.";
               return false;
            }

            if (majorVersion != BinaryLogStream::LOGGER_MAJOR_VERSION)
            {
               error = "Major version mismatch.";
               return false;
            }

            if (minorVersion != BinaryLogStream::LOGGER_MINOR_VERSION)
            {
               error = "Minor version mismatch.";
               return false;
            }

            error = "";
            return true;
         }

         std::string magicNumber;
         unsigned char majorVersion;
         unsigned char minorVersion;
         unsigned short msgDBFileNameLength;
         std::string msgDBFileName;
      };

      /**
       * Reads the file header located in the message database file.
       * @param header This structure is filled with the header
       *    information.
       * @note If a valid message data base file does not exist or
       *    the header is malformed, a LOGGER_IO_EXCEPTION is thrown.
       */
      void ReadMessageDataBaseHeader(MessageDataBaseHeader& header);

      /**
       * Writes the file header to the message database file.
       * @param header This structure contains the header
       *    information.
       * @note If a valid message data base file does not exist or
       *    the header is malformed, a LOGGER_IO_EXCEPTION is thrown.
       * @note This method automatically seeks to the beginning of the file
       *    before writing the header data.
       */
      void WriteMessageDataBaseHeader(MessageDataBaseHeader& header);

      /**
       * Reads the file header located in the index table file.
       * @param header This structure is filled with the header
       *    information.
       * @note If a valid index table file does not exist or
       *    the header is malformed, a LOGGER_IO_EXCEPTION is thrown.
       */
      void ReadIndexTableHeader(IndexTableHeader& header);

      /**
       * Writes the file header to the index table file.
       * @param header This structure contains the header
       *    information.
       * @note If a valid index table file does not exist or
       *    the header is malformed, a LOGGER_IO_EXCEPTION is thrown.
       * @note This method automatically seeks to the beginning of the file
       *    before writing the header data.
       */
      void WriteIndexTableHeader(IndexTableHeader& header);

      /**
       * Destructor.  Calls the Close() method.
       */
      virtual ~BinaryLogStream();

      /**
       * This method loads all the tags and keyframe entries found in the
       * current log index tables file.
       * @note If an error occurs, a LOGGER_IO_EXCEPTION is thrown.
       */
      void ReadIndexTables();

      /**
       * Checks the stream for any errors, throwing an exception if an error occurs.
       * @param fp The file stream to check.
       */
      void CheckFileStatus(FILE* fp);

      /**
       * Writes a keyframe to the index tables file.  This method
       * assumes a valid index tables file exists.
       * @param keyFrame The keyframe to add to the file.
       */
      void WriteKeyFrame(const LogKeyframe& keyFrame);

      /**
       * Reads a keyframe from the current read position in the index tables
       * file.
       * @return The keyframe that was read from the file.
       */
      LogKeyframe ReadKeyFrame();

      /**
       * Writes a tag to the index tables file. This method assumes
       * a valid index tables file exists.
       * @param tag The tag to add to the index tables file.
       */
      void WriteTag(const LogTag& tag);

      /**
       * Reads a tag from the current read position in the index tables
       * file.
       * @return The tag that was read from the file.
       */
      LogTag ReadTag();

   private:
      FILE* mMessagesFile;
      std::string mMessagesFileName;

      FILE* mIndexTablesFile;
      std::string mIndexTablesFileName;

      ///List of existing tags in the index file.
      std::vector<LogTag> mExistingTags;

      ///List of tags there were added during recording or playback.
      ///These are inserted into the file if it is flushed or closed.
      std::vector<LogTag> mNewTags;

      ///List of existing keyframes in the index file.
      std::vector<LogKeyframe> mExistingKeyFrames;

      ///List of keyframes that were added during recording.
      ///These are inserted into the file if it flushed or closed.
      std::vector<LogKeyframe> mNewKeyFrames;

      int mCurrentMinorVersion;

      // Tracks whether we have opened the files for write mode (typically RECORD only)
      // or for read mode (typically playback).
      bool mFilesAreOpenForWriting;
   };

} // namespace dtGame

#endif // DELTA_BINARYLOGSTREAM
