/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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
*/

#ifndef DELTA_RECORDER
#define DELTA_RECORDER

// recorder.h: Declaration of the Recorder class.
//
//////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>
#include <sstream> // for std::ostringstream

#include "dtUtil/xerceswriter.h"
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include "dtCore/timer.h"
#include "dtCore/refptr.h"
#include "dtCore/base.h"
#include "dtCore/system.h"
#include "dtUtil/log.h"

namespace dtCore
{
   /**
    * \brief A generic utility for recording frame information.
    * The Recorder class is generic utility used to
    * record interesting data for instances of arbitrary type.
    *
    * @param RecorderableT is a type that supports the interfaces necessary for recording.  This class knows how to create and serialize FrameDataT types.
    * @param FrameDataT is the type to be stored in memory.
    *
    * \todo make sure that adding a new source will invalidate the currently saved feyframes because of the assumed syncronization.
    */
   template<typename RecorderableT, typename FrameDataT>
   class /*DT_EXPORT */Recorder : public dtCore::Base
   {
   public:
      typedef RecorderableT                                  RecordableType;         /// The type of object of interest.  RecordableTypes know how to create, serialize, and deserialize FrameDataTypes
      typedef FrameDataT                                     FrameDataType;          /// The data to be saved from the object of interest
      typedef std::vector<FrameDataType>                     FrameDataContainer;     /// A container to hold each source's frame data
      typedef std::pair<double,FrameDataContainer>           KeyFrame;               /// The time stamp applied to the entire container of frame data
      typedef std::vector<KeyFrame>                          KeyFrameContainer;      /// The container of KeyFrame data.
      typedef std::vector< dtCore::RefPtr<RecordableType> >  RecordablePtrContainer; /// The container of sources of frame data.

      /** a utility function to convert a basic type into a string.
        * @param T the type being passed.
        * @param t the instance of the type to converted.
        */
      template<typename T>
      std::string ToString(T& t) const
      {
         std::ostringstream ss;
	 ss << t;
	 return ss.str();
      }

      /**
        * Represents the state of a recorder.
        */
      enum RecorderState
      {
         Stopped,
         Recording,
         Playing
      };

      /**
        * Constructor.
        *
        * @param name the instance name
        */
      Recorder(const std::string& name = "recorder"): Base(name), mState(Stopped), mWriter()
      {
         AddSender( dtCore::System::Instance() );
      }

   protected:
      /**
        * Destructor.
        */
      virtual ~Recorder()
      {
         RemoveSender( dtCore::System::Instance() );
      }

   public:
      /**
        * Adds an element to the list of objects to record.
        *
        * @param source an instance of RecordableType, used to record data.
        */
      void AddSource(RecordableType* source)
      {
         if( mState == Recording || mState == Playing )
         {
            LOG_WARNING("Recorder does not support adding new sources while recording or playing");
         }
         else
         {
            mSources.push_back( source );
         }
      }

      /**
        * Removes an element from the list of objects to record.
        *
        * @param source the source to be removed.
        *
        * \todo verify this function is working properly.
        */
      void RemoveSource(RecordableType* source)
      {
         if( mState == Recording || mState == Playing )
         {
            LOG_WARNING("Recorder does not support removing sources while recording or playing");
         }
         else
         {
            mSources.erase(source);
         }
      }

      /**
        * Starts recording events.
        */
      void Record()
      {
         mState = Recording;
         mStartTime = mClock.tick();

         FrameDataContainer sourcedata( mSources.size() );
         typename RecordablePtrContainer::iterator iter = mSources.begin();
         typename RecordablePtrContainer::iterator enditer = mSources.end();
         while( iter != enditer )
         {
            // orders framedata the same as sources
            sourcedata.push_back( (*iter)->CreateFrameData() );
            ++iter;
         }
         mKeyFrames.push_back( KeyFrame(0.0,sourcedata) );
      }

      /**
        * Starts playing events.
        */
      void Play()
      {
         mState = Playing;
      }

      /**
        * Stops recording or playing events.
        */
      void Stop()
      {
         if(mState == Recording)
         {
            mDeltaTime = mClock.tick();
         }

         mState = Stopped;
      }

      /**
        * Returns the state of this event recorder (stopped, recording, or playing).
        *
        * @return the current state
        */
      RecorderState GetState() { return mState; }

      /**
        * Saves the recording to the specified file.
        *
        * @param filename the name of the file to save
        *
        * \todo handle Xerces exceptions.
        */
      void SaveFile(const std::string& filename)
      {
         mWriter.CreateDocument( "RecordedFrames" );
         XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = mWriter.GetDocument();
         XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* root = doc->getDocumentElement();

         std::vector<XMLCh*> cleanupxmlstring;

         XMLCh* TIMECODE = dtUtil::XercesWriter::ConvertToTranscode("TimeCode");
         cleanupxmlstring.push_back( TIMECODE );

         XMLCh* FRAME = dtUtil::XercesWriter::ConvertToTranscode("Frame");
         cleanupxmlstring.push_back( FRAME );

         typename KeyFrameContainer::iterator kfiter = mKeyFrames.begin();
         typename KeyFrameContainer::iterator kfend = mKeyFrames.end();
         while( kfiter != kfend )
         {
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* frameelement = doc->createElement( FRAME );

            FrameDataContainer& sourcedata = (*kfiter).second;
            typename FrameDataContainer::iterator fditer = sourcedata.begin();

            typename RecordablePtrContainer::iterator srciter = mSources.begin();
            typename RecordablePtrContainer::iterator srcend = mSources.end();
            while( srciter != srcend )
            {
               // assumes an equal number of framedata iterators for source iterators
               XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* dataelement = (*srciter)->Serialize( (*fditer),doc );
               frameelement->appendChild( dataelement );
               ++fditer;
               ++srciter;
            }

            double timestamp = (*kfiter).first;
            std::string timestring = ToString<double>( timestamp );

            XMLCh* TIMESTAMP = dtUtil::XercesWriter::ConvertToTranscode(timestring.c_str());
            cleanupxmlstring.push_back( TIMESTAMP );

            frameelement->setAttribute( TIMECODE , TIMESTAMP );
            root->appendChild( frameelement );

            ++kfiter;
         }

         // write out the file
         mWriter.WriteFile( filename );

         // clean up memory
         dtUtil::XercesWriter::ReleaseTranscode( TIMECODE );
         dtUtil::XercesWriter::ReleaseTranscode( FRAME );
         //std::for_each( cleanupxmlstring.begin(), cleanupxmlstring.end(), &dtUtil::XercesWriter::ReleaseTranscode );
      }

      /**
        * Loads a recording from the specified file.
        *
        * @param filename the name of the file to load
        */
      void LoadFile(const std::string& filename)
      {
      }

      /**
        * Message handler.
        *
        * @param data the received message
        */
      virtual void OnMessage(dtCore::Base::MessageData *data)
      {
         switch( mState )
         {
         case Recording:
            {
               if( data->message == "postframe" )
               {
                  mDeltaTime = mClock.tick();
                  double timeCode = mClock.delta_s(mStartTime, mDeltaTime);

                  FrameDataContainer sourcedata( mSources.size() );
                  typename RecordablePtrContainer::iterator iter = mSources.begin();
                  typename RecordablePtrContainer::iterator enditer = mSources.end();
                  while( iter != enditer )
                  {
                     // orders framedata the same as sources
                     sourcedata.push_back( (*iter)->CreateFrameData() );
                     ++iter;
                  }
                  mKeyFrames.push_back( KeyFrame(timeCode,sourcedata) );
               }
            } break;

            // plays a frame-by-frame playback, no interpolation for FrameData
         case Playing:
            {
               if( data->message == "preframe" )
               {
                  // key frame stuff
                  //double timecode = (*mKeyFrameIter).first;
                  typename FrameDataContainer::iterator frameiter = (*mKeyFrameIter).second.begin();

                  // sources
                  typename RecordablePtrContainer::iterator srciter = mSources.begin();
                  typename RecordablePtrContainer::iterator srcend = mSources.end();
                  while( srciter != srcend )
                  {
                     // assumes sources are ordered the same as framedata
                     (*srciter)->UseFrameData( (*frameiter) );
                     ++srciter;
                     ++frameiter;
                  }
               }
            } break;
         }

      //   if(data->message == "frame" &&
      //      mState == Playing)
      //   {
      //      mDeltaTime = mClock.tick();

      //      double timeCode = mClock.delta_s(mStartTime, mDeltaTime);

      //      if(timeCode >= mRecordingLength)
      //      {
      //         mState = RecorderStopped;
      //      }
      //      else if(mNextStateFrame != mTimeCodeStateFrameMap.end())
      //      {
      //         while(mNextStateFrame != mTimeCodeStateFrameMap.end() &&
      //               timeCode >= (*mNextStateFrame).first)
      //         {
      //            (*mNextStateFrame).second->ReapplyToSource();

      //            mNextStateFrame++;
      //         }
      //      }
      //   }
      //   else if(data->message == "stateFrame" &&
      //         mState == Recording)
      //   {
      //      Recordable* source = dynamic_cast<Recordable*>(data->sender);

      //      if(mSources.count(source) > 0)
      //      {
      //         mDeltaTime = mClock.tick();
      //         double timeCode = mClock.delta_s(mStartTime, mDeltaTime);
      //         mNextStateFrame = mTimeCodeStateFrameMap.insert( mNextStateFrame, TimeCodeStateFrameMap::value_type(timeCode,(StateFrame*)data->userData) );
      //      }
      //   }
      }

   private:
      RecordablePtrContainer mSources;                  /// The object to record.
      RecorderState mState;                             /// The state of this recorder.
      dtCore::Timer mClock;                                     /// The clock object.
      dtCore::Timer_t mDeltaTime, mStartTime;
      KeyFrameContainer mKeyFrames;
      typename KeyFrameContainer::iterator mKeyFrameIter;
      dtUtil::XercesWriter mWriter;
      //dtUtil::XercesReader mReader;
   };
};


#endif // DELTA_RECORDER
