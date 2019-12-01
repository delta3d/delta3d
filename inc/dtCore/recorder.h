/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>           // for Walker settings

#include <dtUtil/xerceswriter.h>
#include <dtCore/timer.h>
#include <dtCore/refptr.h>
#include <dtCore/base.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/xerceserrorhandler.h>
#include <dtUtil/keyframedecoder.h>

namespace dtCore
{
   /**
    * A generic utility for recording frame information.
    * The Recorder class is generic utility used to
    * record interesting data for instances of arbitrary type.
    *
    * The class has been completely re-engineered by John K. Grant.
    *
    * @param RecorderableT is a type that supports the interfaces necessary for recording.  This class knows how to create and serialize FrameDataT types.
    * @param FrameDataT is the type to be stored in memory.
    */
   template<typename RecordableT, typename FrameDataT>
   class /*DT_CORE_EXPORT */Recorder : public dtCore::Base
   {
   public:
      typedef RecordableT                                    RecordableType;         /// The type of object of interest.  RecordableTypes know how to create, serialize, and deserialize FrameDataTypes.
      typedef FrameDataT                                     FrameDataType;          /// The data to be saved from the object of interest.
      typedef std::vector< dtCore::RefPtr<FrameDataType> >   FrameDataPtrContainer;  /// A container to hold each source's frame data.
      typedef std::pair<double,FrameDataPtrContainer>        KeyFrame;               /// The time stamp applied to the entire container of frame data.
      typedef std::vector<KeyFrame>                          KeyFrameContainer;      /// The container of KeyFrame data.
      typedef typename KeyFrameContainer::iterator           KeyFrameContainerIterator;
      typedef std::vector< dtCore::RefPtr<RecordableType> >  RecordablePtrContainer; /// The container of sources of frame data.

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
      Recorder(const std::string& name = "recorder"): Base(name), mState(Stopped)
      {
         dtCore::System::GetInstance().TickSignal.connect_slot(this, &Recorder::OnSystem);
      }

   protected:
      /**
        * Destructor.
        */
      virtual ~Recorder()
      {
      }

   public:
      /// Returns a const vector of KeyFrames.
      const KeyFrameContainer& GetKeyFrames() const { return mKeyFrames; }

      /// Returns a non const vector of KeyFrames.  Beware of the synchronization of key frames and sources.
      KeyFrameContainer& GetKeyFrames() { return mKeyFrames; }

      /// Returns a vector of the RecordableT sources.
      const RecordablePtrContainer& GetSources() const { return mSources; }

      /// Returns a vector of the RecordableT sources.  Beware of the synchronization of key frames and sources.
      RecordablePtrContainer& GetSources() { return mSources; }

      /**
        * Adds an element to the list of objects to record.
        *
        * @param source an instance of RecordableType, used to record data.
        */
      void AddSource(RecordableType* source)
      {
         if (mState == Recording || mState == Playing)
         {
            LOG_WARNING("Recorder does not support adding new sources while recording or playing");
         }
         else
         {
            mKeyFrames.clear();
            mSources.push_back( source );
         }
      }

      /**
        * Removes an element from the list of objects to record.
        *
        * @param source the source to be removed.
        */
      void RemoveSource(RecordableType* source)
      {
         if (mState == Recording || mState == Playing)
         {
            LOG_WARNING("Recorder does not support removing sources while recording or playing");
         }
         else
         {
            mKeyFrames.clear();
            for (typename RecordablePtrContainer::iterator i = mSources.begin();
                 i != mSources.end();
                 ++i)
            {
               if ( *i == source )
               {
                  mSources.erase(i);
               }
            }
         }
      }

      /**
        * Starts recording events.
        */
      void Record()
      {
         mState = Recording;
         mStartTime = mClock.Tick();

         FrameDataPtrContainer sourcedata;
         sourcedata.reserve(mSources.size());
         typename RecordablePtrContainer::iterator iter = mSources.begin();
         typename RecordablePtrContainer::iterator enditer = mSources.end();
         while (iter != enditer)
         {
            // orders framedata the same as sources are ordered in the source container
            sourcedata.push_back((*iter)->CreateFrameData());
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
         if (mState == Recording)
         {
            mDeltaTime = mClock.Tick();
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
        */
      void SaveFile(const std::string& filename)
      {
         dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();
         writer->CreateDocument( "RecordedFrames" );
         XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();
         XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* root = doc->getDocumentElement();

         XMLCh* TIMECODE = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("TimeCode");
         XMLCh* FRAME = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("Frame");

         typename KeyFrameContainer::iterator kfiter = mKeyFrames.begin();
         typename KeyFrameContainer::iterator kfend = mKeyFrames.end();
         while (kfiter != kfend)
         {
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* frameelement = doc->createElement(FRAME);

            FrameDataPtrContainer& sourcedata = (*kfiter).second;
            typename FrameDataPtrContainer::iterator fditer = sourcedata.begin();

            typename RecordablePtrContainer::iterator srciter = mSources.begin();
            typename RecordablePtrContainer::iterator srcend = mSources.end();
            while (srciter != srcend)
            {
               // assumes an equal number of framedata iterators for source iterators
               FrameDataType *fdt = (*fditer).get();
               XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* dataelement = (*srciter)->Serialize(fdt,doc);
               frameelement->appendChild( dataelement );
               ++fditer;
               ++srciter;
            }

            double timestamp = (*kfiter).first;
            std::string timestring = dtUtil::ToString<double>(timestamp);

            XMLCh* TIMESTAMP = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(timestring.c_str());
            frameelement->setAttribute(TIMECODE , TIMESTAMP);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&TIMESTAMP);

            root->appendChild(frameelement);
            ++kfiter;
         }

         // write out the file
         writer->WriteFile(filename);

         // clean up memory
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&TIMECODE);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&FRAME);
      }

      /**
        * \brief Loads a recording from the specified file.
        * Loads an XML file, using DOM parsing because the nature of deserializing does not work well with SAX parsing.
        * A container of key frame data is filled from the data in the XML file.
        *
        *
        * @param filename the name of the file to load
        */
      void LoadFile(const std::string& filename)
      {
         // check to see if the file exits
         std::string file = dtUtil::FindFileInPathList(filename);
         if (file.empty())
         {
            LOG_WARNING("The file, " + filename + " was not found.")
            return;
         }

         mKeyFrames.clear();  // clear the current key frame data

         dtUtil::XercesErrorHandler ehandler;

         XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser parser;
         parser.setErrorHandler(&ehandler);
         parser.parse(file.c_str());

         XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = parser.getDocument();

         typedef dtUtil::KeyFrameDecoder<RecordableType,FrameDataType> SourceDecoder;
         SourceDecoder decoder(mSources,mKeyFrames);
         decoder.Walk(doc);
      }

      /**
        * Message handler.
        *
        * @param data the received message
        */
      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

      {
         switch (mState)
         {
         case Recording:
            {
               if (str == dtCore::System::MESSAGE_POST_FRAME)
               {
                  mDeltaTime = mClock.Tick();
                  double timeCode = mClock.DeltaSec(mStartTime, mDeltaTime);

                  FrameDataPtrContainer sourcedata;
                  sourcedata.reserve( mSources.size() );
                  typename RecordablePtrContainer::iterator iter = mSources.begin();
                  typename RecordablePtrContainer::iterator enditer = mSources.end();
                  while (iter != enditer)
                  {
                     // orders framedata the same as sources
                     sourcedata.push_back((*iter)->CreateFrameData());
                     iter++;
                  }
                  mKeyFrames.push_back(KeyFrame(timeCode,sourcedata));
               }
            } break;

            // for now, this just plays a frame-by-frame playback, no time stamp interpolation for FrameData
         case Playing:
            {
               if (str == dtCore::System::MESSAGE_PRE_FRAME)
               {
                  if (mKeyFrameIter != mKeyFrames.end())
                  {
                     // key frame stuff
                     //double timecode = (*mKeyFrameIter).first;
                     typename FrameDataPtrContainer::iterator framedataiter = (*mKeyFrameIter).second.begin();

                     // sources
                     typename RecordablePtrContainer::iterator srciter = mSources.begin();
                     typename RecordablePtrContainer::iterator srcend = mSources.end();
                     while (srciter != srcend)
                     {
                        // assumes sources are ordered the same as framedata
                        (*srciter)->UseFrameData( (*framedataiter).get() );
                        ++srciter;
                        ++framedataiter;
                     }
                     mKeyFrameIter++;
                  }
                  else
                  {
                     mState = Stopped;
                  }
               }
            } break;

         default:  // Stopped
            {
            } break;
         }
      }

   private:
      RecordablePtrContainer mSources;                  /// The object to record.
      RecorderState mState;                             /// The state of this recorder.
      dtCore::Timer mClock;                             /// The clock object.
      dtCore::Timer_t mDeltaTime, mStartTime;
      KeyFrameContainer mKeyFrames;
      KeyFrameContainerIterator mKeyFrameIter;
   };

}


#endif // DELTA_RECORDER
