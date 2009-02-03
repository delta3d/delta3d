/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008, Alion Science and Technology, BMH Operation
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
 * @author Chris Rodgers
 */

#ifndef soundinfo_h__
#define soundinfo_h__

#include <dtAudio/export.h>
#include <osg/Referenced>
#include <dtAudio/soundtype.h>
#include <dtAudio/sound.h>

namespace dtAudio
{
   /**
    * SOUND INFO CODE
    * This class creates the relationship between a sound and a sound type.
    * It also handles play, stop and pause states that have been known to
    * be problems with the dtAudio sound system; the system has no way of
    * determining if a sound is playing, which is very bad.
    * Used by the dtAudio::SoundComponent
    */
   class DT_AUDIO_EXPORT SoundInfo : public osg::Referenced
   {
   public:
      SoundInfo(const dtAudio::SoundType& soundType, dtAudio::Sound& sound);

      const dtAudio::SoundType& GetType() const;

      dtAudio::Sound& GetSound();
      const dtAudio::Sound& GetSound() const;

   protected:
      virtual ~SoundInfo();

   private:
      const dtAudio::SoundType& mSoundType;
      dtAudio::Sound* mSound;
   };
} // namespace dtAudio

#endif // soundinfo_h__
