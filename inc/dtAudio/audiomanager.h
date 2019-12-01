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

#ifndef  DELTA_AUDIOMANAGER
#define  DELTA_AUDIOMANAGER

#include <cstddef>
#include <vector>
#include <queue>
#include <map>
#include <string>

#if defined(_MSC_VER)
#   include <alc.h>
#   include <al.h>
#elif defined(__APPLE__)
#   include <OpenAL/alc.h>
#   include <OpenAL/al.h>
#else
#   include <AL/al.h>
#   include <AL/alc.h>
#endif

#include <dtCore/base.h>
#include <dtCore/transformable.h>
#include <dtAudio/listener.h>
#include <dtAudio/sound.h>
#include <dtAudio/export.h>

#include <osg/Vec3>


namespace dtAudio
{

   /**
    * dtAudio::AudioManager
    *
    * dtAudio::AudioManager is the interface to the underlying audio-
    * engine; OpenAL.
    *
    * Before using, the user must instantiate and configure the
    * AudioManager:
    *
    *    AudioManager::Instantiate();
    *
    * Optionally the user can create an AudioConfigData structure
    * to pass to the AudioManager when configuring set some of the
    * base functionalit of the manager.  Currently only the number
    * of sources is set this way.  It is encouraged, but not required,
    * that the user know how many channels their audio hardware uses
    * and set the number of sources = the number of channels.
    *
    * After user is finished with all sound, the AudioManager should
    * be freed:
    *
    *    AudioManager::Destroy();
    *
    * FOR THE USER:
    * Sounds are not created by the user, but they are requested from
    * the AudioManager.  After getting a sound from the AudioManager
    * the user then calls the sound's functions.  When the user is
    * finished with the sound, the sound should be passed back to
    * the AudioManager for free the resource.
    *
    * There is one global listener which the user also requests from
    * the AudioManager.  After getting the listener from the AudioManager
    * the usere then calls the listener's functions.  When finished
    * with the listener, the user does NOT free it.  The listener is
    * just an interface to AudioManager's protected object and the
    * AudioManager will handle it's resources.
    *
    * In many cases it's more efficeint to preload the sounds into
    * the AudioManager before loading them into the individual sounds.
    *
    * FOR THE DEVELOPER:
    * The AudioManager is a repository for all sounds objects and
    * their corresponding buffer.
    *
    * At pre-frame, the AudioManager process all the sounds waiting for
    * command processing.
    * State commands are commnads to change the state of the sound
    * (play, stop, pause, etc.), but not the value of any of the sounds
    * attributes (gain, pitch, etc.).  Value commands change the value of
    * a sound's attributes.  State commands are pushed onto the Sound's
    * command queue for further processing at the appropriate time (frame change).
    *
    * At frame time, AudioManager process all Sounds with commands in their
    * respective queues.
    *
    */
   class DT_AUDIO_EXPORT AudioManager : public dtCore::Base
   {
      DECLARE_MANAGEMENT_LAYER(AudioManager)

   public:
      /**
       * BufferData is an internal structure
       * used to identify an OpenAL buffer and
       * hold reference data associated with it
       */
      struct BufferData
      {
         ALuint       buf;
         const char*  file;
         ALboolean    loop;
         unsigned int use;
         ALenum       format;
         ALsizei      freq;
         ALsizei      size;

         BufferData()
            : buf(0L)
            , file("")
            , loop(AL_FALSE)
            , use(0L)
         {}
      };

   private:
      typedef dtCore::RefPtr<AudioManager>       MOB_ptr;
      typedef dtCore::RefPtr<Sound>              SOB_PTR;
      typedef dtCore::RefPtr<Listener>           LOB_PTR;

      typedef std::map<std::string, BufferData*> BUF_MAP;

      typedef std::vector<SOB_PTR>               SND_LST;

      enum SoundState
      {
         PAUSED,
         PLAYING,
         STOPPED
      };

      typedef std::map<Sound*, SoundState> SoundObjectStateMap;

   private:
      static MOB_ptr               _Mgr;
      static LOB_PTR               _Mic;
      static const char*           _EaxVer;
      static const char*           _EaxSet;
      static const char*           _EaxGet;

   private:
      AudioManager(const std::string& name = "audiomanager",
                   ALCdevice* dev = NULL, ALCcontext* cntxt = NULL, bool shutdownPassedInContexts = true);
      virtual ~AudioManager();

   public:

      /** Create the singleton and initialize OpenAL and ALUT.
       *
       *  Specifying the OpenAL device and context is considered an advanced,
       *  operation.  Be sure that you know what you're doing before you try it!
       *  Usually you don't need to specify the device and context.  Typically,
       *  the AudioManager creates them automatically.
       *
       *  Note that if you are going to supply a custom device you must ALSO
       *  supply a custom context, and vice versa.  When you supply the custom
       *  device and context, the AudioManager assumes that they've been opened,
       *  initialized, and that the context has been set to the current OpenAL
       *  context.
       *
       *@param name - The name of the AudioManager instance.
       *@param dev  - The OpenAL device to be used by the AudioManager.
       *@param cntxt - The OpenAL context to be used by the AudioManager.
       *@param shutdownPassedInContexts - this doesn't matter if cntxt or dev are NULL, but if true, it will close
       *                                  the passed in context when the audio manager destructor is called.
       */
      static void Instantiate(const std::string& name = "audiomanager",
                              ALCdevice* dev = NULL, ALCcontext* cntxt = NULL, bool shutdownPassedInContexts = true);

      /// destroy the singleton and shutdown OpenAL
      static void Destroy();

      /// returns the OpenAL sound device the AudioManager is using
      ALCdevice* GetDevice() { return mDevice; }

      /// returns the OpenAL context the AudioManager is using
      ALCcontext* GetContext() { return mContext; }

      /// Pause all sounds.
      void PauseSounds();

      /// UnPause all sounds.
      void UnPauseSounds();

      /** This is an advanced operation that normally isn't necessary! Typically
       *  the default device and context are sufficient.  If you do use this
       *  method make sure you brush up on your OpenAL first!
       *
       *  Set the OpenAL device the AudioManger is using.
       *  Also sets up an OpenAL context for this device.
       *  (Preexisting context and device are closed before doing the preceeding)
       *
       */
      void SetOpenALDevice(const ALCchar* deviceName);

      /// access the AudioManager
      static AudioManager& GetInstance();

      /// access the global Listener
      static Listener* GetListener();

      /**
       * Sets the OpenAL distance model.  Possible parameter values are:
       * AL_INVERSE_DISTANCE, AL_INVERSE_DISTANCE_CLAMPED, AL_LINEAR_DISTANCE,
       * AL_LINEAR_DISTANCE_CLAMPED, AL_EXPONENT_DISTANCE,
       * AL_EXPONENT_DISTANCE_CLAMPED, or AL_NONE
       */
      void SetDistanceModel(ALenum dm);

      /**
       * Set the OpenAL Doppler factor.
       *
       * 0 disables Doppler effect.
       * Values between 0.0 and 1.0 tend to minimize the Doppler effect.
       * 1.0 is the default value.
       * Values greater than 1.0 ted to maximimze the Doppler effect.
       * Negative values raise an AL_INVALID_VALUE error.
       *
       * An very simiplified summary of OpenAL calculation for the Doppler effect:
       *
       * shift = DOPPLER_FACTOR * freq * (DOPPLER_VELOCITY - l.velocity) / (DOPPLER_VELOCITY + s.velocity)
       *
       * where l is the listener and s is a sound source.
       *
       * More detailed calculation specs are in the OpenAL Programmers Guide available at:
       * http://connect.creativelabs.com/openal
       *
       */
      void SetDopplerFactor(float f);

      /**
       * Set the speed of sound used in OpenAL Doppler calculations.
       * Note that OpenAL's default value is 343.3
       */
      void SetSpeedOfSound(float s);

      /// Returns true if initialized
      static bool IsInitialized() { return _Mgr != NULL; }

      /**
       * receive messages
       * handles the timeing messages (pre-post-frame) from the system
       * pushes sounds onto the command queue for later processing
       */
      void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal);
      void OnSoundCommand(const dtUtil::RefString& command, Sound* snd);

      /// create or recycle a new sound for the user
      Sound* NewSound();

      /// free a sound that the user is finished with
      void FreeSound(Sound* sound);

      /*
       * Pre-load a sound file into a buffer. We only support .wav's
       * Return the OpenAL buffer ID of the loaded sound data.
       *
       * Testing if the returned buffer is invalid can be done with like this:
       *    if (alIsBuffer(buf) == AL_FALSE)
       *
       * This method is in the AudioManager rather than the Sound class
       * because one of the AudioManager's main jobs is to manage sound data
       * buffers.  If muliple sounds use the same buffers then the
       * AudioManager ensures that these redundant data buffers are shared
       * by the Sounds and not loaded into memory multiple times.
       *
       * @param file File to be loaded into a buffer.
       * @return OpenAL buffer ID.
       */
      ALint LoadFile(const std::string& file);

      /// un-load a sound file from a buffer (if use-count is zero)
      bool UnloadFile(const std::string& file);

   private:
      /// process commands of all sounds in the sound list
      inline void PreFrame(const double deltaFrameTime);

      /// check if manager has been configured
      inline bool Configured() const;

      /// get the eax function pointers from OpenAL (nothing done with them yet)
      inline bool ConfigEAX(bool eax);

      /**
       * This method loads the sound buffer from the Sound specified
       * into the AudioManager's std::map of sound buffers.
       * @param snd Sound object that specifies the sound file to load.
       *        It also receives a reference to a new or an existing
       *        sound buffer.
       * @return Count of references to the loaded sound buffer; 0 if failed.
       */
      inline int LoadSoundBuffer(Sound& snd);

      /**
       * This method reduces the reference count for the buffer referenced
       * by the specified sound object. If the reference count results to 0,
       * the associated sound buffer will be deleted.
       * @param snd Sound object that specifies the sound file to be unloaded.
       * @return Remaining count of references to the loaded sound buffer; -1 if failed.
       */
      inline int UnloadSound(Sound* snd);

      /**
       * This method ensures that the sound object is properly stopped and its
       * source released. Calls to this method usually appear before deleting
       * the sound object's shared sound buffer.
       * @param snd Sound object that is to have its source properly released.
       * @param errorMessae Error message that should be logged if the operation fails.
       * @param callerFunctionName Name of the function/method that is calling
       *        this method. This is needed for checking OpenAL error state.
       * @param callerFunctionLineNum Line number of the function/method that
       *        is calling this method. This is needed for checking OpenAL error state.
       * @return TRUE if release was successful.
       */
      inline bool ReleaseSoundSource(Sound& snd, const std::string& errorMessage,
         const std::string& callerFunctionName, int callerFunctionLineNum );

      /**
       * This method performs the actual deletion of the the sound buffer.
       * @param bufferHandle Handle of the sound buffer to be deleted.
       * @param errorMessae Error message that should be logged if the operation fails.
       * @param callerFunctionName Name of the function/method that is calling
       *        this method. This is needed for checking OpenAL error state.
       * @param callerFunctionLineNum Line number of the function/method that
       *        is calling this method. This is needed for checking OpenAL error state.
       * @return TRUE if release was successful.
       */
      inline bool ReleaseSoundBuffer(ALuint bufferHandle, const std::string& errorMessage,
         const std::string& callerFunctionName, int callerFunctionLineNum );

      /// Open an OpenAL device for the AudioManager to use
      void OpenDevice(const ALCchar* deviceName = 0);

      /// Create an OpenAL context using the current device
      void CreateContext();

      /// close and cleanup current OpenAL sound device the AudioManager is using
      void CloseDevice();

      /// close and cleanup current OpenAL sound context the AudioManager is using
      void CloseContext();

   private:
      ALvoid*             mEAXSet;
      ALvoid*             mEAXGet;

      unsigned int        mNumSounds;
      bool                mIsConfigured;

      BUF_MAP             mBufferMap;

      SND_LST             mSoundList;

      //SoundObjectStateMap mSoundStateMap; ///Maintains state of each Sound object
      //                                    ///prior to a system-wide pause message

      ALCdevice*          mDevice;
      ALCcontext*         mContext;

      bool mShutdownContexts;
   };
};



#endif   // DELTA_AUDIOMANAGER
