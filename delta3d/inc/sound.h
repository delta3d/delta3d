// sound.h: Declaration of the Sound class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_SOUND
#define DELTA_SOUND

#include <al.h>
#include <alut.h>

#include "recorder.h"
#include "transformable.h"

namespace dtCore
{
   /**
    * A sound.
    */
   class DT_EXPORT Sound : public Transformable,
                           public Recordable
   {
      DECLARE_MANAGEMENT_LAYER(Sound)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         Sound(std::string name = "sound");

         /**
          * Destructor.
          */
         virtual ~Sound();

         /**
          * Sets the transform of the listener.
          *
          * @param transform the new transform
          */
         static void SetListenerTransform(Transform* transform);

         /**
          * Retrieves the transform of the listener.
          *
          * @param transform the transform to fill
          */
         static void GetListenerTransform(Transform* transform);

         /**
          * Sets the listener gain.
          *
          * @param gain the new gain
          */
         static void SetListenerGain(float gain);

         /**
          * Returns the listener gain.
          *
          * @return the current gain
          */
         static float GetListenerGain();

         /**
          * Loads the specified sound file.
          *
          * @param filename the name of the file to load
          */
         void LoadFile(std::string filename);

         /**
          * Returns the name of the loaded sound file.
          *
          * @return the name of the loaded file
          */
         std::string GetFilename() const;

         /**
          * Sets the gain of the sound source.
          *
          * @param gain the new gain
          */
         void SetGain(float gain);

         /**
          * Returns the gain of the sound source.
          *
          * @return the current gain
          */
         float GetGain() const;

         /// Set the pitch multiplier of the sound source.
         void SetPitch( float pitch );

         /// Get the current pitch multiplier of the sound source
         float GetPitch() const;

         /**
          * Sets whether or not to play the sound in a continuous loop.
          *
          * @param looping true to play the sound in a loop, false
          * otherwise
          */
         void SetLooping(bool looping);

         /**
          * Checks whether or not the sound plays in a continuous loop.
          *
          * @return true if the sound plays in a loop, false otherwise
          */
         bool IsLooping() const;

         /**
          * Starts playing this sound.
          */
         void Play();

         /**
          * Checks whether or not the sound is playing.
          *
          * @return true if the sound is playing, false otherwise
          */
         bool IsPlaying() const;

         /**
          * Pauses playing this sound.
          */
         void Pause();

         /**
          * Stops playing this sound.
          */
         void Stop();

         /**
          * Rewinds to the beginning of this sound.
          */
         void Rewind();

         /**
          * Message handler.
          *
          * @param data the received message
          */
         virtual void OnMessage(MessageData *data);

         /**
          * Generates and returns a key frame that represents the
          * complete recordable state of this object.
          *
          * @return a new key frame
          */
         virtual StateFrame* GenerateKeyFrame();

         /**
          * Deserializes an XML element representing a state frame, turning it
          * into a new StateFrame instance.
          *
          * @param element the element that represents the frame
          * @return a newly generated state frame corresponding to the element
          */
         virtual StateFrame* DeserializeFrame(TiXmlElement* element);

         
      private:

         /**
          * Whether or not ALUT has been initialized.
          */
         static bool mALUTInitialized;

         /**
          * The transform of the listener.
          */
         static Transform mListenerTransform;

         /**
          * The gain of the listener.
          */
         static float mListenerGain;

         /**
          * The name of the sound buffer.
          */
         ALuint mBuffer;

         /**
          * The name of the sound source.
          */
         ALuint mSource;

         /**
          * The name of the file.
          */
         std::string mFilename;

         /**
          * The gain of the sound source.
          */
         float mGain;

         /**
          * Whether or not the sound plays in a loop.
          */
         bool mLooping;

         float mPitch; ///<The current pitch multiplier

   };
};

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "ALut.lib")

#endif // DELTA_SOUND
