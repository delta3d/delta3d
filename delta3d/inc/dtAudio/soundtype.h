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

#ifndef soundtype_h__
#define soundtype_h__

#include <dtUtil/enumeration.h>
#include <dtAudio/export.h>

namespace dtAudio
{
   /**
    * SOUND TYPE ENUMERATION CODE
    * Used by the dtAudio::SoundComponent.
    */
   class DT_AUDIO_EXPORT SoundType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(SoundType);

   public:
      static SoundType SOUND_TYPE_DEFAULT;
      static SoundType SOUND_TYPE_MUSIC;
      static SoundType SOUND_TYPE_UI_EFFECT;
      static SoundType SOUND_TYPE_VOICE;
      static SoundType SOUND_TYPE_WORLD_EFFECT;

      static void AddNewType(SoundType& soundType);

   protected:
      virtual ~SoundType() {}

   private:
      SoundType(const std::string& name);
   };
} // namespace dtAudio

#endif // soundtype_h__
