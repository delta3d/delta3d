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

#include "dtDAL/plugin_export.h"
#include "dtDAL/transformableactorproxy.h"
#include <dtDAL/exceptionenum.h>
#include <dtGame/gameactor.h>

namespace dtAudio
{
   class Sound;
}

namespace dtActors 
{

   class DT_PLUGIN_EXPORT SoundActor : public dtGame::GameActor
   {
   public:

      SoundActor(dtGame::GameActorProxy &proxy);

      dtAudio::Sound* GetSound();
      const dtAudio::Sound* GetSound() const;

   protected:
      virtual ~SoundActor();

   private:

      dtCore::RefPtr<dtAudio::Sound> mSound;
   };



    /**
     * @class SoundActorProxy
     * @brief This proxy wraps the Sound Delta3D object.
     */
    class DT_PLUGIN_EXPORT SoundActorProxy : public dtGame::GameActorProxy 
    {
       public:
          static const std::string PROPERTY_DIRECTION;
          static const std::string PROPERTY_GAIN;
          static const std::string PROPERTY_INITIAL_OFFSET_TIME;
          static const std::string PROPERTY_LISTENER_RELATIVE;
          static const std::string PROPERTY_LOOPING;
          static const std::string PROPERTY_MAX_DISTANCE;
          static const std::string PROPERTY_MIN_DISTANCE;
          static const std::string PROPERTY_MAX_GAIN;
          static const std::string PROPERTY_MIN_GAIN;
          static const std::string PROPERTY_MAX_RANDOM_TIME;
          static const std::string PROPERTY_MIN_RANDOM_TIME;
          static const std::string PROPERTY_PITCH;
          static const std::string PROPERTY_PLAY_AS_RANDOM;
          static const std::string PROPERTY_ROLLOFF_FACTOR;
          static const std::string PROPERTY_SOUND_EFFECT;
          static const std::string PROPERTY_VELOCITY;

           /**
            * Constructor
            */
           SoundActorProxy();
         
           /**
            * Adds the properties that are common to all Delta3D physical objects.
            */
           virtual void BuildPropertyMap();

           /**
            * Loads in a sound file
            * @param fileName The file to load
            */
           void LoadFile(const std::string &fileName);

           /**
            * Sets the direction
            * @param dir The direction to Set
            */
           void SetDirection(const osg::Vec3 &dir);

           /**
            * Gets the direction
            * @return The current direction
            */
           osg::Vec3 GetDirection();

           /**
            * Sets the velocity
            * @param vel The velocity to Set
            */
           void SetVelocity(const osg::Vec3 &vel);

           /**
            * Gets the velocity
            * @return The current velocity
            */
           osg::Vec3 GetVelocity();

           /**
            * Plays the sound
            */
           void Play();

           /**
            * Gets the render mode of sound actor proxies.
            * @return RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.  Although this may seem a little
            *  strange considering you cannot actually draw a sound, however, this informs the
            *  world that this proxy's actor and billboard should be represented in the scene.
            */
           virtual const ActorProxy::RenderMode& GetRenderMode() 
           {
               return ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
           }

           /**
            * Gets the billboard icon associated with sound actor proxies.
            * @return
            */
           virtual dtDAL::ActorProxyIcon* GetBillBoardIcon();

           /// Builds the invokables of this actor
           void BuildInvokables();

           // timers for the actor
           void HandleActorTimers(const dtGame::Message& msg);

           // makes initial timer start off
           void OnEnteredWorld();

           // Removes the allocated sound resource.
           void OnRemovedFromWorld();

           // properties for variables in this class
           bool IsARandomSoundEffect() const {return mRandomSoundEffect;}
           void SetToHaveRandomSoundEffect(bool value) {mRandomSoundEffect = value;}
           
           void SetMinRandomTime(int value) {mMinRandomTime = value;}
           int GetMinRandomTime() const {return mMinRandomTime;}
           
           void SetMaxRandomTime(int value) {mMaxRandomTime = value;}
           int GetMaxRandomTime() const {return mMaxRandomTime;}
           
           int GetOffsetTime() const {return mOffsetTime;}
           void SetOffsetTime(int value) {mOffsetTime = value;}

      protected:

        /**
         * Creates a new positional sound.
         */
        virtual void CreateActor();

        /**
         * Destructor
         */
        virtual ~SoundActorProxy();
      
      private:

         /// Does the sound effect play at a random time
         bool mRandomSoundEffect;

         /// What is the minimum amount for rand to calculate
         int mMinRandomTime;

         /// What is the maximum amount for rand to calculate
         int mMaxRandomTime;

         /// In case you have many objects all playing at random, this is so they wont overlap
         int mOffsetTime;
    };
}

#endif
