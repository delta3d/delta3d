/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 MOVES Institute
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
 * Chris Rodgers
 */

#ifndef DELTA_CHARACTER_FILE_WRITER
#define DELTA_CHARACTER_FILE_WRITER

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <dtCore/basexml.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtAnim
{
   class Animatable;
   class AnimationChannel;
   class AnimationSequence;
   class BaseModelData;
}



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
namespace dtAnim
{
   class DT_ANIM_EXPORT CharacterFileWriter : public dtCore::BaseXMLWriter
   {
   public:
      typedef dtCore::BaseXMLWriter BaseClass;

      CharacterFileWriter();

      bool Write(const dtAnim::BaseModelData& modelData, std::ostream& stream);

   protected:
      virtual ~CharacterFileWriter();

   private:
      void WriteFileTag(const dtAnim::BaseModelData& modelData, int fileType);
      void WriteSkeleton(const dtAnim::BaseModelData& modelData);
      void WriteMeshes(const dtAnim::BaseModelData& modelData);
      void WriteMaterials(const dtAnim::BaseModelData& modelData);
      void WriteAnimations(const dtAnim::BaseModelData& modelData);
      void WriteMorphs(const dtAnim::BaseModelData& modelData);
      void WriteShader(const dtAnim::BaseModelData& modelData);
      void WriteScale(const dtAnim::BaseModelData& modelData);
      void WriteLOD(const dtAnim::BaseModelData& modelData);
      void WritePoseMesh(const dtAnim::BaseModelData& modelData);
      void WriteChannelsAndSequences(const dtAnim::BaseModelData& modelData);
      void WriteChannel(const AnimationChannel& channel);
      void WriteSequence(const AnimationSequence& sequence);
      void WriteChildAnimatable(const Animatable& anim);
      void WriteAnimatable(const Animatable& anim, bool isChannel);
      void WriteAnimatableEvents(const Animatable& anim);

      void Write(bool b);
      void Write(int n);
      void Write(float n);
      void Write(double n);
      void Write(const std::string& s);
   };
}

#endif // DELTA_CHARACTER_FILE_WRITER
