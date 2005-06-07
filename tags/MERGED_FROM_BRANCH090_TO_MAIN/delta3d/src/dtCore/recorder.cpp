// recorder.cpp: Implementation of the Recorder class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/recorder.h"
#include "dtCore/notify.h"
#include "dtCore/system.h"

using namespace std;
using namespace dtCore;


IMPLEMENT_MANAGEMENT_LAYER(Recorder)


/**
 * Constructor.
 *
 * @param name the instance name
 */
Recorder::Recorder(string name)
   : Base(name),
     mState(RecorderStopped)
{
   RegisterInstance(this);

   AddSender(System::GetSystem());
}

/**
 * Destructor.
 */
Recorder::~Recorder()
{
   DeregisterInstance(this);
   RemoveSender( System::GetSystem() );
}

/**
 * Adds an element to the list of objects to record.
 *
 * @param source the source to add
 */
void Recorder::AddSource(Recordable* source)
{
   mSources.insert(source);

   Base* base = dynamic_cast<Base*>(source);

   if(base != NULL)
   {
      AddSender( base );
   }

   if(mState == RecorderRecording)
   {
      mClock.update();

      double timeCode = mClock.getAbsTime();

      source->IncrementRecorderCount();

      mNextStateFrame = mTimeCodeStateFrameMap.insert(
         mNextStateFrame,
         pair<double, StateFrame*>(
            timeCode,
            source->GenerateKeyFrame()
         )
      );
   }
}

/**
 * Removes an element from the list of objects to record.
 *
 * @param source the source to remove
 */
void Recorder::RemoveSource(Recordable* source)
{
   mSources.erase(source);

   if(mState == RecorderRecording)
   {
      source->DecrementRecorderCount();
   }
}

/**
 * Starts recording.
 */
void Recorder::Record()
{
   mTimeCodeStateFrameMap.clear();

   mState = RecorderRecording;

   mNextStateFrame = mTimeCodeStateFrameMap.begin();

   mClock.reset();

   for(set<Recordable*>::iterator it = mSources.begin();
       it != mSources.end();
       it++)
   {
      (*it)->IncrementRecorderCount();

      mNextStateFrame = mTimeCodeStateFrameMap.insert(
         mNextStateFrame,
         pair<double, StateFrame*>(
            0.0,
            (*it)->GenerateKeyFrame()
         )
      );
   }
}

/**
 * Starts playing.
 */
void Recorder::Play()
{
   mState = RecorderPlaying;

   mNextStateFrame = mTimeCodeStateFrameMap.begin();

   mClock.reset();
}

/**
 * Stops recording or playing.
 */
void Recorder::Stop()
{
   if(mState == RecorderRecording)
   {
      mClock.update();

      mRecordingLength = mClock.getAbsTime();

      for(set<Recordable*>::iterator it = mSources.begin();
          it != mSources.end();
          it++)
      {
         (*it)->DecrementRecorderCount();
      }
   }

   mState = RecorderStopped;
}

/**
 * Returns the state of this recorder (stopped, recording, or playing).
 *
 * @return the current state
 */
RecorderState Recorder::GetState()
{
   return mState;
}

/**
 * Saves the recording to the specified file.
 *
 * @param filename the name of the file to save
 */
void Recorder::SaveFile(string filename)
{
   TiXmlElement root("RecordedFrames");

   char buf[256];

   sprintf(buf, "%g", mRecordingLength);

   root.SetAttribute("recordingLength", buf);

   for(multimap<double, StateFrame*>::iterator it = mTimeCodeStateFrameMap.begin();
       it != mTimeCodeStateFrameMap.end();
       it++)
   {
      TiXmlElement* element = (*it).second->Serialize();

      sprintf(buf, "%g", (*it).first);

      element->SetAttribute("timeCode", buf);

      Recordable* source = (*it).second->GetSource();

      element->SetAttribute(
         "source", 
         dynamic_cast<Base*>(source)->GetName().c_str()
      );

      root.LinkEndChild(element);
   }

   TiXmlDocument document;

   document.InsertEndChild(root);

   document.SaveFile(filename.c_str());
}

/**
 * Loads a recording from the specified file.
 *
 * @param filename the name of the file to load
 */
void Recorder::LoadFile(string filename)
{
   TiXmlDocument document;

   if(document.LoadFile(filename.c_str()))
   {
      mTimeCodeStateFrameMap.clear();

      mNextStateFrame = mTimeCodeStateFrameMap.begin();

      TiXmlElement* root = document.RootElement();

      const char* buf = root->Attribute("recordingLength");

      sscanf(buf, "%lg", &mRecordingLength);

      for(TiXmlNode* child = root->FirstChild();
          child != NULL;
          child = child->NextSibling())
      {
         if(child->ToElement())
         {
            TiXmlElement* element = (TiXmlElement*)child;

            const char* buf = element->Attribute("timeCode");

            if(buf != NULL)
            {
               double timeCode;

               sscanf(buf, "%lg", &timeCode);
               
               string name = element->Attribute("source");

               Recordable* source = NULL;

               for(set<Recordable*>::iterator it = mSources.begin();
                   it != mSources.end();
                   it++)
               {
                  Base* base = dynamic_cast<Base*>(*it);
                  
                  if(base != NULL && base->GetName() == name)
                  {
                     source = *it;
                     break;
                  }
               }

               if(source != NULL)
               {
                  mNextStateFrame = mTimeCodeStateFrameMap.insert(
                     mNextStateFrame,
                     pair<double, StateFrame*>(
                        timeCode,
                        source->DeserializeFrame(element)
                     )
                  );
               }
            }
         }
      }
   }
}

/**
 * Message handler.
 *
 * @param data the received message
 */
void Recorder::OnMessage(MessageData *data)
{
   if(data->message == "frame" &&
      mState == RecorderPlaying)
   {
      mClock.update();

      double timeCode = mClock.getAbsTime();

      if(timeCode >= mRecordingLength)
      {
         mState = RecorderStopped;
      }
      else if(mNextStateFrame != mTimeCodeStateFrameMap.end())
      {
         while(mNextStateFrame != mTimeCodeStateFrameMap.end() &&
               timeCode >= (*mNextStateFrame).first)
         {
            (*mNextStateFrame).second->ReapplyToSource();

            mNextStateFrame++;
         }
      }
   }
   else if(data->message == "stateFrame" &&
           mState == RecorderRecording)
   {
      Recordable* source = dynamic_cast<Recordable*>(data->sender);

      if(mSources.count(source) > 0)
      {
         mClock.update();

         double timeCode = mClock.getAbsTime();

         mNextStateFrame = mTimeCodeStateFrameMap.insert(
            mNextStateFrame,
            pair<double, StateFrame*>(
               timeCode,
               (StateFrame*)data->userData
            )
         );
      }
   }
}


/**
 * Constructor.
 */
Recordable::Recordable()
   : mRecorderCount(0)
{}

/**
 * Increments the recorder count.
 */
void Recordable::IncrementRecorderCount()
{
   mRecorderCount++;
}

/**
 * Decrements the recorder count.
 */
void Recordable::DecrementRecorderCount()
{
   mRecorderCount--;
}

/**
 * Checks whether this object is being recorded (that is, whether
 * the recorder count is greater than zero).
 *
 * @return true if this object is being recorded, false
 * otherwise
 */
bool Recordable::IsBeingRecorded() const
{
   return mRecorderCount > 0;
}


/**
 * Constructor.
 *
 * @param source the source of this frame
 */
StateFrame::StateFrame(Recordable* source)
   : mSource(source)
{}

/**
 * Returns the source of this frame.
 *
 * @return the source of this frame
 */
Recordable* StateFrame::GetSource() const
{
   return mSource;
}
