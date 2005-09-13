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

#include <osgDB/FileUtils>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>           // for Walker settings

#include "dtUtil/xerceswriter.h"
#include "dtCore/timer.h"
#include "dtCore/refptr.h"
#include "dtCore/base.h"
#include "dtCore/system.h"
#include "dtUtil/log.h"
#include "dtUtil/stringutils.h"
#include "dtUtil/xerceserrorhandler.h"
#include "dtUtil/keyframedecoder.h"
//#include "dtUtil/keyframehandler.h"
// @param ContentHandlerT is the type to handle loading XML representations.

namespace dtCore
{
   /**
    * \brief A generic utility for recording frame information.
    * The Recorder class is generic utility used to
    * record interesting data for instances of arbitrary type.
    *
    * The class has been completely re-engineered by John K. Grant.
    *
    * @param RecorderableT is a type that supports the interfaces necessary for recording.  This class knows how to create and serialize FrameDataT types.
    * @param FrameDataT is the type to be stored in memory.
    * @param ContentHandlerT the type enforcing a parsing algorithm, used to populate key frame data from a file.
    *
    * \todo Verify that adding a new source will invalidate the currently saved keyframes because of the assumed syncronization.
    * \todo Make the loading done via a policy class which can impose deserialization interfaces as needed to the RecordableType.
    * \todo Make the saving done via a policy class which can impose serialization interfaces as needed to the RecordableType.
    */
   template<typename RecordableT, typename FrameDataT>
   class /*DT_EXPORT */Recorder : public dtCore::Base
   {
   public:
      typedef RecordableT                                    RecordableType;         /// The type of object of interest.  RecordableTypes know how to create, serialize, and deserialize FrameDataTypes.
      typedef FrameDataT                                     FrameDataType;          /// The data to be saved from the object of interest.
      typedef std::vector< dtCore::RefPtr<FrameDataType> >   FrameDataPtrContainer;  /// A container to hold each source's frame data.
      typedef std::pair<double,FrameDataPtrContainer>        KeyFrame;               /// The time stamp applied to the entire container of frame data.
      typedef std::vector<KeyFrame>                          KeyFrameContainer;      /// The container of KeyFrame data.
      typedef typename KeyFrameContainer::iterator           KeyFrameContainerIterator;
      typedef std::vector< dtCore::RefPtr<RecordableType> >  RecordablePtrContainer; /// The container of sources of frame data.
      //typedef ContentHandlerT                                ContentHandlerType;     /// The type of algorithm used for parsing XML files.

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
      Recorder(const std::string& name = "recorder"): Base(name), mState(Stopped), mWriter(0)
      {
         AddSender( dtCore::System::Instance() );

         try
         {
            XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
         }

         catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& /*toCatch*/)
         {
            LOG_ERROR("There was a problem initializing the Xerces XMLPlatformUtils.");
            /// \todo disable support for loading/saving

            //char *pMsg = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.getMessage());
            //XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
            //      << "  Exception message:"
            //      << pMsg;
            //XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&pMsg);
         }

         mWriter = new dtUtil::XercesWriter();
      }

   protected:
      /**
        * Destructor.
        */
      virtual ~Recorder()
      {
         RemoveSender( dtCore::System::Instance() );

         // shutdown the system
         ///\todo this should not be here.
         XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
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

         FrameDataPtrContainer sourcedata( mSources.size() );
         typename RecordablePtrContainer::iterator iter = mSources.begin();
         typename RecordablePtrContainer::iterator enditer = mSources.end();
         while( iter != enditer )
         {
            // orders framedata the same as sources are ordered in the source container
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
         mKeyFrameIter = mKeyFrames.begin();
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
         mWriter->CreateDocument( "RecordedFrames" );
         XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = mWriter->GetDocument();
         XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* root = doc->getDocumentElement();

         std::vector<XMLCh*> cleanupxmlstring;

         XMLCh* TIMECODE = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("TimeCode");
         cleanupxmlstring.push_back( TIMECODE );

         XMLCh* FRAME = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("Frame");
         cleanupxmlstring.push_back( FRAME );

         typename KeyFrameContainer::iterator kfiter = mKeyFrames.begin();
         typename KeyFrameContainer::iterator kfend = mKeyFrames.end();
         while( kfiter != kfend )
         {
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* frameelement = doc->createElement( FRAME );

            FrameDataPtrContainer& sourcedata = (*kfiter).second;
            typename FrameDataPtrContainer::iterator fditer = sourcedata.begin();

            typename RecordablePtrContainer::iterator srciter = mSources.begin();
            typename RecordablePtrContainer::iterator srcend = mSources.end();
            while( srciter != srcend )
            {
               // assumes an equal number of framedata iterators for source iterators
               XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* dataelement = (*srciter)->Serialize( (*fditer).get(),doc );
               frameelement->appendChild( dataelement );
               ++fditer;
               ++srciter;
            }

            double timestamp = (*kfiter).first;
            std::string timestring = dtUtil::ToString<double>( timestamp );

            XMLCh* TIMESTAMP = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode( timestring.c_str() );
            cleanupxmlstring.push_back( TIMESTAMP );

            frameelement->setAttribute( TIMECODE , TIMESTAMP );
            root->appendChild( frameelement );

            ++kfiter;
         }

         // write out the file
         mWriter->WriteFile( filename );

         // clean up memory
         dtUtil::XercesWriter::ReleaseTranscode( TIMECODE );
         dtUtil::XercesWriter::ReleaseTranscode( FRAME );
         //std::for_each( cleanupxmlstring.begin(), cleanupxmlstring.end(), &dtUtil::XercesWriter::ReleaseTranscode );
      }

      /**
        * \brief Loads a recording from the specified file.
        * Loads an XML file, using DOM parsing because the nature of deserializing does not work well with SAX parsing.
        * A container of key frame data is filled from the data in the XML file.
        *
        * \todo verify that the number of frames loaded equals the number of frames were (recorded & saved).
        *
        * @param ContentHandlerT is the XML loading handler object type.
        * @param filename the name of the file to load
        */
      void LoadFile(const std::string& filename)
      {
         // check to see if the file exits
         std::string file = osgDB::findDataFile( filename );
         if( file.empty() )
         {
            LOG_WARNING("The file, " + filename + " was not found.")
            return;
         }

         // --- use DOM parsing (completing now) --- //
         dtUtil::XercesErrorHandler ehandler;

         XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser parser;
         parser.setErrorHandler( &ehandler );
         parser.parse( file.c_str() );

         XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = parser.getDocument();

         typedef dtUtil::KeyFrameDecoder<RecordableType,FrameDataType> SourceDecoder;
         SourceDecoder decoder(mSources,mKeyFrames);
         decoder.Walk( doc );

         //// --- use sax parsing (NOT COMPLETE) --- //
         //// build a vector of frame data
         //XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader* parser = XERCES_CPP_NAMESPACE_QUALIFIER XMLReaderFactory::createXMLReader();
         //try
         //{
         //   ContentHandlerType handler(mSources,mKeyFrames);
         //   XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader* parser = XERCES_CPP_NAMESPACE_QUALIFIER XMLReaderFactory::createXMLReader();
         //   parser->setContentHandler(&handler);
         //   //parser->setErrorHandler(&handler);
         //   parser->parse( XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(file.c_str()) );
         //   //errorCount = parser->getErrorCount();
         //}
         //catch (const XERCES_CPP_NAMESPACE_QUALIFIER OutOfMemoryException& e)
         //{
         //   LOG_ERROR("Out of memory exception occurred while loading file" + file + ", with message: " + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(e.getMessage())) )
         //}
         //catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& e)
         //{
         //   LOG_ERROR("An error occurred while parsing file, " + file + ", with message: " + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(e.getMessage())) )
         //}
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

                  FrameDataPtrContainer sourcedata( mSources.size() );
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

            // for now, this just plays a frame-by-frame playback, no time stamp interpolation for FrameData
         case Playing:
            {
               if( data->message == "preframe" )
               {
                  if( mKeyFrameIter != mKeyFrames.end() )
                  {
                     // key frame stuff
                     double timecode = (*mKeyFrameIter).first;
                     typename FrameDataPtrContainer::iterator framedataiter = (*mKeyFrameIter).second.begin();

                     // sources
                     typename RecordablePtrContainer::iterator srciter = mSources.begin();
                     typename RecordablePtrContainer::iterator srcend = mSources.end();
                     while( srciter != srcend )
                     {
                        // assumes sources are ordered the same as framedata
                        (*srciter)->UseFrameData( (*framedataiter).get() );
                        ++srciter;
                        ++framedataiter;
                     }
                  }
                  else
                  {
                     mState = Stopped;
                  }
               }
            } break;

         default:  // Stopped
            {
            }
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
      KeyFrameContainerIterator mKeyFrameIter;
      osg::ref_ptr<dtUtil::XercesWriter> mWriter;
      //KeyFrameIter mKeyFrameIter;
   };

};


#endif // DELTA_RECORDER
