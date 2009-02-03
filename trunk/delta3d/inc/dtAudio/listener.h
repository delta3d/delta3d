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

#ifndef  DELTA_LISTENER
#define  DELTA_LISTENER

#include <dtCore/transformable.h>
#include <dtAudio/export.h>

#include <osg/Vec3>

namespace dtAudio
{
   /** dtAudio::Listener
    *
    * dtAudio::Listener is just an interface to the global listener
    * object held within (and protected) by the dtAudio::AudioManager.
    *
    * The listener is not usually created directly by the user (new/delete).
    * Instead the user requests the listener from the AudioManager:
    *
    *    Listener* global_ear = AudioManager::GetInstance().GetListener();
    *
    * The user can then call any of the Listener interface functions.
    * After the user is finished with the Listener, there is no need to
    * free it up.  The underlying listener object is a global singular
    * which lasts as long as the AudioManager exists.
    *
    * Listener is a transformable, so it can be a child of other
    * transformables (ie. the camera)  When a Listener is child of
    * another object, it automatically gets positioned in scene-space
    * relative to the parent object every frame, so there is no need to
    * update the Listener's position.  The Listener position can be set
    * manually in scene-space without having to make it a child of another
    * object, but any position updates must then be made manually.
    */
   class DT_AUDIO_EXPORT Listener : public  dtCore::Transformable
   {
      DECLARE_MANAGEMENT_LAYER(Listener)

      public:
         /**
          * Constructor, user does not create directly
          * instead requests the listener from AudioManager
          */
         Listener();

      protected:
         /**
          * Destructor, user does not delete directly
          * AudioManager handles destruction
          */
         virtual ~Listener();

      public:
         /**
          * Set the velocity of the listener.
          *
          * @param velocity to set
          */
         void SetVelocity(const osg::Vec3f& velocity);

         /**
          * Get the velocity of the listener.
          *
          * @param velocity to get
          */
         void GetVelocity(osg::Vec3f& velocity) const;

         /**
          * Sets the master volume of the listener.
          *
          * @param gain the new gain
          */
         void SetGain(float gain);

         /**
          * Returns the master volume of the listener.
          *
          * @return the current volume
          */
         float GetGain() const;

         /**
          * Message handler's main job is to reposition listener if it's a child
          * of a Transformable in scene-space.
          */
         virtual void OnMessage(MessageData* data);

         /// clean up listener
         void Clear(void);
   };
} // namespace dtAudio

#endif   // DELTA_LISTENER
