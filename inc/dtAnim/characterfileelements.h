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

#ifndef DELTA_CHARACTER_FILE_ELEMENTS
#define DELTA_CHARACTER_FILE_ELEMENTS

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtUtil/refstring.h>



namespace dtAnim
{
   class CharacterFileElements
   {
   public:
      // Root Tag
      static const dtUtil::RefString CHARACTER_ELEMENT;

      // File Reference Tags
      static const dtUtil::RefString MIXED_RESOURCE_ELEMENT;
      static const dtUtil::RefString SKELETON_ELEMENT;
      static const dtUtil::RefString MESH_ELEMENT;
      static const dtUtil::RefString MATERIAL_ELEMENT;
      static const dtUtil::RefString ANIMATION_ELEMENT;
      static const dtUtil::RefString MORPH_ANIMATION_ELEMENT;
      static const dtUtil::RefString ATTACHMENT_POINT_ELEMENT;

      // File Reference Tag Attributes
      static const dtUtil::RefString NAME_ELEMENT;
      static const dtUtil::RefString FILENAME_ELEMENT;

      // Shader Tags
      static const dtUtil::RefString SKINNING_SHADER_ELEMENT;
      static const dtUtil::RefString SHADER_GROUP_ELEMENT;
      static const dtUtil::RefString SHADER_NAME_ELEMENT;
      static const dtUtil::RefString SHADER_MAX_BONES_ELEMENT;

      // LOD Tags
      static const dtUtil::RefString LOD_ELEMENT;
      static const dtUtil::RefString LOD_START_DISTANCE_ELEMENT;
      static const dtUtil::RefString LOD_END_DISTANCE_ELEMENT;
      static const dtUtil::RefString MAX_VISIBLE_DISTANCE_ELEMENT;

      // Scale Tags
      static const dtUtil::RefString SCALE_ELEMENT;
      static const dtUtil::RefString SCALE_FACTOR_ELEMENT;

      // Common Channel/Sequence Tags
      static const dtUtil::RefString CHANNEL_ELEMENT;
      static const dtUtil::RefString SEQUENCE_ELEMENT;
      static const dtUtil::RefString POSEMESH_ELEMENT;
      static const dtUtil::RefString ANIMATION_NAME_ELEMENT;
      static const dtUtil::RefString START_DELAY_ELEMENT;
      static const dtUtil::RefString FADE_IN_ELEMENT;
      static const dtUtil::RefString FADE_OUT_ELEMENT;
      static const dtUtil::RefString SPEED_ELEMENT;
      static const dtUtil::RefString BASE_WEIGHT_ELEMENT;
      static const dtUtil::RefString MAX_DURATION_ELEMENT;
      static const dtUtil::RefString IS_LOOPING_ELEMENT;
      static const dtUtil::RefString IS_ACTION_ELEMENT;
      static const dtUtil::RefString CHILD_ELEMENT;
      static const dtUtil::RefString CHILDREN_ELEMENT;

      // Sequence Tags
      static const dtUtil::RefString FOLLOW_PREV_ELEMENT;
      static const dtUtil::RefString FOLLOWS_ELEMENT;
      static const dtUtil::RefString CROSS_FADE_ELEMENT;
      static const dtUtil::RefString EVENT_ON_START_ELEMENT;
      static const dtUtil::RefString EVENT_ON_TIME_ELEMENT;
      static const dtUtil::RefString EVENT_ON_END_ELEMENT;

      // Sequence Tag Attributes
      static const dtUtil::RefString EVENT_ATTR;
      static const dtUtil::RefString OFFSET_ATTR;

      static const dtUtil::RefString BONE_NAME_ATTR;
      static const dtUtil::RefString OFFSET_X_ATTR;
      static const dtUtil::RefString OFFSET_Y_ATTR;
      static const dtUtil::RefString OFFSET_Z_ATTR;
      static const dtUtil::RefString OFFSET_RX_ATTR;
      static const dtUtil::RefString OFFSET_RY_ATTR;
      static const dtUtil::RefString OFFSET_RZ_ATTR;
      static const dtUtil::RefString RESOURCE_ATTR;


   private:
      CharacterFileElements(){}
      virtual ~CharacterFileElements(){}
   };
}
#endif // DELTA_CHARACTER_FILE_ELEMENTS
