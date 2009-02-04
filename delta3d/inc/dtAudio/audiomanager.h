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

// forward references
struct AudioConfigData;

namespace dtAudio
{

   /** dtAudio::AudioManager 
    *
    * dtAudio::AudioManager is the interface to the underlying audio-
    * engine; OpenAL.
    *
    * Before using, the user must instantiate and configure the
    * AudioManager:
    *
    *    AudioManager::Instantiate();
    *    AudioManager::GetInstance().Config();
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

   private:
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
      static const AudioConfigData _DefCfg;

   private:
      AudioManager(const std::string& name = "audiomanager");
      virtual ~AudioManager();

   public:
      
      /// create the singleton and initialize OpenAL
      static void Instantiate();

      /// destroy the singleton and shutdown OpenAL
      static void Destroy();

      /// access the AudioManager
      static AudioManager& GetInstance();

      /// access the global Listener
      static Listener* GetListener();

      ///Deprecated feb/02/2009 in favor of Sound::IsListenerRelative()
      DEPRECATE_FUNC bool GetListenerRelative(Sound* sound);

      /// initialize AudioManager
      virtual void Config(const AudioConfigData& data = _DefCfg);

      /// Returns true if initialized
      bool IsInitialized() const { return _Mgr != NULL; }

      /**
       * receive messages
       * handles the timeing messages (pre-post-frame) from the system
       * pushes sounds onto the command queue for later processing
       */
      virtual void OnMessage(MessageData* data);
      /// create or recycle a new sound for the user
      Sound* NewSound();

      /// free a sound that the user is finished with
      void FreeSound(Sound* sound);

      ///Deprecated feb/02/2009 in favor of Sound::GetSource()
      DEPRECATE_FUNC ALuint GetSource(Sound* sound);

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

      /// remove the buffer from a sound
      inline void UnloadSound(Sound* snd);
   private:
      ALvoid*             mEAXSet;
      ALvoid*             mEAXGet;
      
      unsigned int        mNumSounds;
      bool                mIsConfigured;         

      BUF_MAP             mBufferMap;

      SND_LST             mSoundList;

      SoundObjectStateMap mSoundStateMap; ///Maintains state of each Sound object
                                          ///prior to a system-wide pause message
   };
};

// configuration data
struct DT_AUDIO_EXPORT AudioConfigData
{
   enum DistanceModel
   {
      dmNONE     = AL_NONE,
      dmINVERSE  = AL_INVERSE_DISTANCE,
      dmINVCLAMP = AL_INVERSE_DISTANCE_CLAMPED
   };

   unsigned int numSources;
   bool         eax;
   unsigned int distancemodel;

   AudioConfigData(unsigned int ns = 16,
                   bool         ex = false,
                   unsigned int dm = dmINVERSE)
      : numSources(ns)
      , eax(ex)
      , distancemodel(dm)
   {}
};



#endif   // DELTA_AUDIOMANAGER
