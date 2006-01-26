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
#ifndef DELTA_LOGSTREAM
#define DELTA_LOGSTREAM

#include <string>
#include <osg/Referenced>
#include "dtGame/export.h"
#include "dtGame/message.h"
#include "dtGame/messagefactory.h"
#include "dtGame/logtag.h"
#include "dtGame/logkeyframe.h"
#include "dtUtil/enumeration.h"

namespace dtGame
{
   /**
    * This exception enumeration defines the possible exceptions that may
    * be thrown by LogStream operations.
    */
   class DT_GAME_EXPORT LogStreamException : public dtUtil::Enumeration
   {
      DECLARE_ENUM(LogStreamException);
      public:
            
         ///Throw when an error occurs while performing IO operations on the stream.
         ///This includes creating or opening a stream.
         static LogStreamException LOGGER_IO_EXCEPTION;
         
      private:
         LogStreamException(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };
   
   /**
    * This is an interface class to a stream used by the ServerLoggerComponent
    * to stream data to and from a log file.  It can be extended to support any
    * log file format desired.  
    * @note There are no return values for errors.  
    *    Implementations of this interface should throw exceptions when an error
    *    occurs.
    * @note There may not be a one to one mapping between a LogStream instance and a 
    *    physical file.
    * @see BinaryLogStream
    */
   class LogStream : public osg::Referenced
   {      
      public:         
      
         /**
          * Constructs the logging stream.
          * @param msgFactory This is the message factory assigned to
          *    the log stream.  This factory is used to re-create 
          *    that have been read from the log stream.
          */
         LogStream(MessageFactory &msgFactory) : 
            mMessageFactory(&msgFactory) { }
      
         /**
          * Implementations should create the necessary resources to support
          * log streaming.
          * @param logResourceName The base name of the new log resource.  Implementations
          *    should use this in some form in their naming conventions for log files.
          */
         virtual void Create(const std::string &logResourceName) = 0;
         
         /**
          * Implementations should cleanup any used resources and close the 
          * log stream.
          */
         virtual void Close() = 0;
         
         /**
          * Opens a log resource for reading.
          * @param logResourceName The base name of the new log resource.  Implementations
          *    should use this in some form in their naming conventions for log files.
          */
         virtual void Open(const std::string &logResourceName) = 0;
         
         /**
          * Writes a game message to the stream.
          * @param msg The message to write.
          * @param timeStamp The time stamp matched to this message.
          */
         virtual void WriteMessage(const Message &msg, double timeStamp) = 0;
         
         /**
          * Reads a game message from the current stream.
          * @param This parameter will contain the time stamp of the read
          *    message.
          * @return The next game message in the stream.
          */
         virtual dtCore::RefPtr<Message> ReadMessage(double &timeStamp) = 0;
         
         /**
          * Creates and inserts a new tag into the current log stream.
          * @param newTag The new tag to insert.
          */
         virtual void InsertTag(LogTag &newTag) = 0;
         
         /**
          * Creates and inserts a new keyframe into the current log stream.
          * @param newKeyFrame The new keyframe to insert.
          */
         virtual void InsertKeyFrame(LogKeyframe &newKeyFrame) = 0;
         
         /**
          * Implementations of this method should adjust the current
          * stream such that the next call to ReadMessage() returns the 
          * first message corresponding to the requested keyframe.
          * @param keyFrame The keyframe to go to.
          * @note If the requested keyframe is not known to the stream
          *    an exception should be thrown.
          */
         virtual void JumpToKeyFrame(const LogKeyframe &keyFrame) = 0;
         
         /**
          * Gets the list of tags contained within the log stream.
          * @param tags This vector is filled with the tags currently in
          *    the log stream.
          */
         virtual void GetTagIndex(std::vector<LogTag> &tags) = 0;
         
         /**
          * Gets the list of keyframes contained within the log stream.
          * @param keyFrames This vector is filled with the tags currently
          *    in the log stream.
          */
         virtual void GetKeyFrameIndex(std::vector<LogKeyframe> &keyFrames) = 0;
         
         /**
          * This method should flush the stream, thereby writing any cached
          * data the stream might contain.
          */
         virtual void Flush() = 0;
         
         /**
          * @return True if the end of the log stream has been reached.
          */
         virtual bool IsEndOfStream() { return mEndOfStream; }
         
         /**
          * Gets a reference to this stream's message factory.
          * @return The message factory assigned to this stream.
          */
         MessageFactory &GetMessageFactory()
         {
            return *mMessageFactory;
         }
         
      protected:
      
          ///Empty Destructor...         
         virtual ~LogStream() { }   
         bool mEndOfStream;
         
      private:
         MessageFactory *mMessageFactory;                 
   };
   
}         

#endif
