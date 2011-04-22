// recorder.h: Declaration of the Recorder class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_RECORDER
#define DELTA_RECORDER

#include <map>
#include <set>

#include <sg.h>

#include <tinyxml.h>

#include "base.h"

namespace dtCore
{
   class Recordable;
   class StateFrame;
   

   /**
    * Represents the state of a recorder.
    */
   enum RecorderState
   {
      RecorderStopped,
      RecorderRecording,
      RecorderPlaying
   };


   /**
    * A recorder.
    */
   class Recorder : public Base
   {
      DECLARE_MANAGEMENT_LAYER(Recorder)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         Recorder(std::string name = "recorder");

         /**
          * Destructor.
          */
         virtual ~Recorder();

         /**
          * Adds an element to the list of objects to record.
          *
          * @param source the source to add
          */
         void AddSource(Recordable* source);

         /**
          * Removes an element from the list of objects to record.
          *
          * @param source the source to remove
          */
         void RemoveSource(Recordable* source);

         /**
          * Starts recording events.
          */
         void Record();

         /**
          * Starts playing events.
          */
         void Play();

         /**
          * Stops recording or playing events.
          */
         void Stop();

         /**
          * Returns the state of this event recorder (stopped, recording, or playing).
          *
          * @return the current state
          */
         RecorderState GetState();

         /**
          * Saves the recording to the specified file.
          *
          * @param filename the name of the file to save
          */
         void SaveFile(std::string filename);

         /**
          * Loads a recording from the specified file.
          *
          * @param filename the name of the file to load
          */
         void LoadFile(std::string filename);

         /**
          * Message handler.
          *
          * @param data the received message
          */
         virtual void OnMessage(MessageData *data);

         
      private:

         /**
          * The object to record.
          */
         std::set<Recordable*> mSources;

         /**
          * The state of this recorder.
          */
         RecorderState mState;

         /**
          * The clock object.
          */
         ulClock mClock;

         /**
          * Maps time codes to state frames.
          */
         std::multimap<double, StateFrame*> mTimeCodeStateFrameMap;

         /**
          * The next state frame to apply.
          */
         std::multimap<double, StateFrame*>::iterator mNextStateFrame;

         /**
          * The length of the recording, in seconds.
          */
         double mRecordingLength;
   };


   /**
    * An interface for recordable objects.  When recordable objects change,
    * they issue "stateFrame" messages with pointers to StateFrame objects
    * as their user data.
    */
   class Recordable
   {
      public:

         /**
          * Constructor.
          */
         Recordable();

         /**
          * Increments the recorder count.
          */
         void IncrementRecorderCount();

         /**
          * Decrements the recorder count.
          */
         void DecrementRecorderCount();

         /**
          * Checks whether this object is being recorded (that is, whether
          * the recorder count is greater than zero).
          *
          * @return true if this object is being recorded, false
          * otherwise
          */
         bool IsBeingRecorded() const;

         /**
          * Generates and returns a key frame that represents the
          * complete recordable state of this object.
          *
          * @return a new key frame
          */
         virtual StateFrame* GenerateKeyFrame() = 0;

         /**
          * Deserializes an XML element representing a state frame, turning it
          * into a new StateFrame instance.
          *
          * @param element the element that represents the frame
          * @return a newly generated state frame corresponding to the element
          */
         virtual StateFrame* DeserializeFrame(TiXmlElement* element) = 0;


      private:

         /**
          * The number of recorders currently recording this object's state.
          */
         int mRecorderCount;
   };


   /**
    * A recorded state update.
    */
   class StateFrame
   {
      public:
        
         /**
          * Constructor.
          *
          * @param source the source of this frame
          */
         StateFrame(Recordable* source);

         /**
          * Returns the source of this frame.
          *
          * @return the source of this frame
          */
         Recordable* GetSource() const;

         /**
          * Reapplies this frame to its source.
          */
         virtual void ReapplyToSource() = 0;

         /**
          * Serializes this frame, turning it into an XML element.
          *
          * @return an XML element representing the serialized frame
          */
         virtual TiXmlElement* Serialize() const = 0;


      private:

         /**
          * The source of this frame.
          */
         Recordable* mSource;
   };
};

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#if defined(_DEBUG)
#define _AUTOLIBNAME  "tinyxmld.lib"
#else
#define _AUTOLIBNAME  "tinyxml.lib"
#endif

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
#pragma message( "Will automatically link with " _AUTOLIBNAME )
#endif

#pragma comment(lib, _AUTOLIBNAME)

#endif // DELTA_RECORDER