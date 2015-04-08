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
 * William E. Johnson II, Allen Danklefsen, Chris Rodgers
 */

#ifndef DELTA_SOUND_ACTOR_PROXY
#define DELTA_SOUND_ACTOR_PROXY

#include <dtAudio/export.h>

#include <dtGame/gameactor.h>
#include <dtCore/resourcedescriptor.h>

namespace dtGame
{
   class TimerElapsedMessage;
}

namespace dtAudio
{
   class Sound;

   /**
    * @class SoundActor
    * @brief This proxy wraps the Sound Delta3D object.
    *
    * This version works around an issue in the Delta3D version of this class where it allocates an
    * OpenAL source as soon as the actor is created.  Since OpenAL frequently limits applications to creating
    * no more than 16 or 32 sources, applications can easily into the situation that they can't add any more
    * sounds.  This works around it by only allocating a dtAudio sound when it starts playing, and deleting that
    * sound when it stops.
    *
    * Plans exist to change delta3d so that it only allocations sources for sounds that are playing, and to
    * later add support to choose each frame the N sounds that should have sources based on distance and
    * perceived volume.  That is, this class is temporary, and some changes in the class will eventually be migrated
    * into the sound actor proxy in dtAudio.
    *
    * If you are not running out of sources with the current sound actor in dtAudio, you should continue using that class.
    *
    */
   class DT_AUDIO_EXPORT SoundActor : public dtGame::GameActorProxy
   {
   public:
      static const dtUtil::RefString CLASS_NAME;
      static const dtUtil::RefString INVOKABLE_TIMER_HANDLER;
      static const dtUtil::RefString PROPERTY_DIRECTION;
      static const dtUtil::RefString PROPERTY_GAIN;
      static const dtUtil::RefString PROPERTY_INITIAL_OFFSET_TIME;
      static const dtUtil::RefString PROPERTY_LISTENER_RELATIVE;
      static const dtUtil::RefString PROPERTY_LOOPING;
      static const dtUtil::RefString PROPERTY_MAX_DISTANCE;
      static const dtUtil::RefString PROPERTY_REFERENCE_DISTANCE;
      static const dtUtil::RefString PROPERTY_MAX_GAIN;
      static const dtUtil::RefString PROPERTY_MAX_RANDOM_TIME;
      static const dtUtil::RefString PROPERTY_MIN_RANDOM_TIME;
      static const dtUtil::RefString PROPERTY_PITCH;
      static const dtUtil::RefString PROPERTY_PLAY_AS_RANDOM;
      static const dtUtil::RefString PROPERTY_PLAY_AT_STARTUP;
      static const dtUtil::RefString PROPERTY_ROLLOFF_FACTOR;
      static const dtUtil::RefString PROPERTY_SOUND_EFFECT;
      static const dtUtil::RefString PROPERTY_VELOCITY;
      static const dtUtil::RefString TIMER_NAME;
      static const dtUtil::RefString PLAY_END_TIMER_NAME;

      static const float DEFAULT_RANDOM_TIME_MAX;
      static const float DEFAULT_RANDOM_TIME_MIN;

      /**
       * Constructor
       */
      SoundActor();

      /**
       * Adds the properties that are common to all Delta3D physical objects.
       */
      virtual void BuildPropertyMap();

      /**
       * Assigns the sound resource
       */
      void SetSoundResource(const dtCore::ResourceDescriptor& soundResource);

      /**
       * @return the sound resource
       */
      const dtCore::ResourceDescriptor& GetSoundResource() const;

      /**
       * Sets the direction
       * @param dir The direction to Set
       */
      void SetDirection(const osg::Vec3& dir);

      /**
       * Gets the direction
       * @return The current direction
       */
      osg::Vec3 GetDirection();

      /**
       * Sets the velocity
       * @param vel The velocity to Set
       */
      void SetVelocity(const osg::Vec3& vel);

      /**
       * Gets the velocity
       * @return The current velocity
       */
      osg::Vec3 GetVelocity();

      /**
       * Plays the sound immediately without using timers.
       */
      void Play();

      /// Stops playing the sound and cleans it up.
      void Stop();

      /**
       * Play the sound using the random time range properties set on this sound actor.
       * A sound timer is created in the Game Manager to trigger the sound to play.
       * The sound will play once the timer elapses while this actor is contained
       * in the Game Manager.
       * @param offsetSeconds Time in seconds to wait prior to playing the sound.
       *        The offset is added with randomized time if this actor is set as random.
       */
      void PlayQueued(float offsetSeconds = 0.0f);

      /**
       * Gets the render mode of sound actor proxies.
       * @return RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.  Although this may seem a little
       *  strange considering you cannot actually draw a sound, however, this informs the
       *  world that this proxy's actor and billboard should be represented in the scene.
       */
      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
      }

      /**
       * Gets the billboard icon associated with sound actor proxies.
       * @return
       */
      virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

      /// Builds the invokables of this actor
      void BuildInvokables();

      // timers for the actor
      void HandleActorTimers(const dtGame::TimerElapsedMessage& msg);

      // makes initial timer start off
      void OnEnteredWorld();

      // Removes the allocated sound resource.
      void OnRemovedFromWorld();

      // properties for variables in this class
      bool IsARandomSoundEffect() const {return mRandomSoundEffect;}
      void SetToHaveRandomSoundEffect(bool value) {mRandomSoundEffect = value;}

      /// Indicates whether or not sound is played at application startup.
      bool IsPlayedAtStartup() const {return mPlaySoundAtStartup;}

      /// Set property that determines if a sound is played at startup
      void SetPlayAtStartup(bool val) {mPlaySoundAtStartup = val;}

      /**
       * Set the minimum seconds to wait between random executions of the sound.
       * @param seconds Minimum time to wait in whole and/or partial seconds.
       */
      void SetMinRandomTime(float seconds) {mMinRandomTime = seconds;}

      /**
       * @return Minimum time to wait in whole and/or partial seconds
       *         prior to playing the sound.
       */
      float GetMinRandomTime() const {return mMinRandomTime;}

      /**
       * Set the maximum seconds to wait between random executions of the sound.
       * @param seconds Maximum time to wait in whole and/or partial seconds.
       */
      void SetMaxRandomTime(float seconds) {mMaxRandomTime = seconds;}

      /**
       * @return Maximum time to wait in whole and/or partial seconds
       *         prior to playing the sound.
       */
      float GetMaxRandomTime() const {return mMaxRandomTime;}

      /**
       * Set the seconds to wait before the initial execution of the sound.
       * Note that this is only used when the sound actor enters the Game Manager.
       * @param seconds Time to wait in whole and/or partial seconds.
       */
      float GetOffsetTime() const {return mOffsetTime;}

      /**
       * @return Time to wait in whole and/or partial seconds
       *         prior to playing the sound for the first time
       *         when the actor enters the Game Manager.
       */
      void SetOffsetTime(float seconds) {mOffsetTime = seconds;}

      /**
       * Access the sound object directly without having to grab a hold
       * of the associated Sound Actor.
       * @return Sound object held by the associated Sound Actor.
       */
      dtAudio::Sound* GetSound();

      /**
       * Access the sound object directly without having to grab a hold
       * of the associated Sound Actor.
       * @return Sound object held by the associated Sound Actor.
       */
      const dtAudio::Sound* GetSound() const;

      void SetLooping(bool looping);
      bool IsLooping() const;

      void SetGain(float gain);
      float GetGain() const;

      void SetPitch(float pitch);
      float GetPitch() const;

      void SetListenerRelative(bool lisrel);
      bool IsListenerRelative() const;

      void SetMaxDistance(float max);
      float GetMaxDistance() const;

      void SetReferenceDistance(float max);
      float GetReferenceDistance() const;

      void SetRolloffFactor(float rolloff);
      float GetRolloffFactor() const;

      void SetMaxGain(float max);
      float GetMaxGain() const;

   protected:

      /**
       * Creates a new positional sound.
       */
      virtual void CreateDrawable();

      /**
       * Destructor
       */
      virtual ~SoundActor();

   private:

      dtCore::ResourceDescriptor mSoundResource;

      osg::Vec3 mDirection, mVelocity;

      /// Does the sound effect play at a random time
      bool mRandomSoundEffect;

      /// Minimum seconds for rand to calculate
      float mMinRandomTime;

      /// Maximum seconds for rand to calculate
      float mMaxRandomTime;

      /**
       * Seconds to wait before play the sound.
       * In case you have many objects all playing at random,
       * this is so they wont overlap.
       */
      float mOffsetTime;

      /// Flag that determines whether or not the sound begins at app startup
      bool mPlaySoundAtStartup;
   };

   class DT_AUDIO_EXPORT SoundDrawable : public dtGame::GameActor
   {
   public:

      SoundDrawable(dtGame::GameActorProxy& parent);

      void CreateSound();
      void DestroySound();

      dtAudio::Sound* GetSound();
      const dtAudio::Sound* GetSound() const;

   protected:
      virtual ~SoundDrawable();

   private:

      dtCore::RefPtr<dtAudio::Sound> mSound;
   };

} // namespace dtAudio

#endif
