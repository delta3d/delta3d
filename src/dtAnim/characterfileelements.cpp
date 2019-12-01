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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/characterfileelements.h>



namespace dtAnim
{
   const dtUtil::RefString CharacterFileElements::CHARACTER_ELEMENT("character");

   // File Reference Tags
   const dtUtil::RefString CharacterFileElements::MIXED_RESOURCE_ELEMENT("mixedResource");
   const dtUtil::RefString CharacterFileElements::SKELETON_ELEMENT("skeleton");
   const dtUtil::RefString CharacterFileElements::MESH_ELEMENT("mesh");
   const dtUtil::RefString CharacterFileElements::MATERIAL_ELEMENT("material");
   const dtUtil::RefString CharacterFileElements::ANIMATION_ELEMENT("animation");
   const dtUtil::RefString CharacterFileElements::MORPH_ANIMATION_ELEMENT("morph");
   const dtUtil::RefString CharacterFileElements::ATTACHMENT_POINT_ELEMENT("attachmentPoint");

   // File Reference Tag Attributes
   const dtUtil::RefString CharacterFileElements::NAME_ELEMENT("name");
   const dtUtil::RefString CharacterFileElements::FILENAME_ELEMENT("fileName");

   // Shader Tags
   const dtUtil::RefString CharacterFileElements::SKINNING_SHADER_ELEMENT("skinningShader");
   const dtUtil::RefString CharacterFileElements::SHADER_GROUP_ELEMENT("shaderGroup");
   const dtUtil::RefString CharacterFileElements::SHADER_NAME_ELEMENT("shaderName");
   const dtUtil::RefString CharacterFileElements::SHADER_MAX_BONES_ELEMENT("maxBones");

   // LOD Tags
   const dtUtil::RefString CharacterFileElements::LOD_ELEMENT("LOD");
   const dtUtil::RefString CharacterFileElements::LOD_START_DISTANCE_ELEMENT("lodStartDistance");
   const dtUtil::RefString CharacterFileElements::LOD_END_DISTANCE_ELEMENT("lodEndDistance");
   const dtUtil::RefString CharacterFileElements::MAX_VISIBLE_DISTANCE_ELEMENT("maxVisibleDistance");

   // Scale Tags
   const dtUtil::RefString CharacterFileElements::SCALE_ELEMENT("scale");
   const dtUtil::RefString CharacterFileElements::SCALE_FACTOR_ELEMENT("scalingFactor");

   // Common Channel/Sequence Tags
   const dtUtil::RefString CharacterFileElements::CHANNEL_ELEMENT("animationChannel");
   const dtUtil::RefString CharacterFileElements::SEQUENCE_ELEMENT("animationSequence");
   const dtUtil::RefString CharacterFileElements::POSEMESH_ELEMENT("poseMesh");
   const dtUtil::RefString CharacterFileElements::ANIMATION_NAME_ELEMENT("animationName");
   const dtUtil::RefString CharacterFileElements::START_DELAY_ELEMENT("startDelay");
   const dtUtil::RefString CharacterFileElements::FADE_IN_ELEMENT("fadeIn");
   const dtUtil::RefString CharacterFileElements::FADE_OUT_ELEMENT("fadeOut");
   const dtUtil::RefString CharacterFileElements::SPEED_ELEMENT("speed");
   const dtUtil::RefString CharacterFileElements::BASE_WEIGHT_ELEMENT("baseWeight");
   const dtUtil::RefString CharacterFileElements::MAX_DURATION_ELEMENT("maxDuration");
   const dtUtil::RefString CharacterFileElements::IS_LOOPING_ELEMENT("isLooping");
   const dtUtil::RefString CharacterFileElements::IS_ACTION_ELEMENT("isAction");
   const dtUtil::RefString CharacterFileElements::CHILD_ELEMENT("child");
   const dtUtil::RefString CharacterFileElements::CHILDREN_ELEMENT("children");

   // Sequence Tags
   const dtUtil::RefString CharacterFileElements::FOLLOW_PREV_ELEMENT("followPrevious");
   const dtUtil::RefString CharacterFileElements::FOLLOWS_ELEMENT("follows");
   const dtUtil::RefString CharacterFileElements::CROSS_FADE_ELEMENT("crossFade");
   const dtUtil::RefString CharacterFileElements::EVENT_ON_START_ELEMENT("eventOnStart");
   const dtUtil::RefString CharacterFileElements::EVENT_ON_TIME_ELEMENT("eventOnTime");
   const dtUtil::RefString CharacterFileElements::EVENT_ON_END_ELEMENT("eventOnEnd");

   // Sequence Tag Attributes
   const dtUtil::RefString CharacterFileElements::EVENT_ATTR("event");
   const dtUtil::RefString CharacterFileElements::OFFSET_ATTR("offset");

   const dtUtil::RefString CharacterFileElements::BONE_NAME_ATTR("bone");
   const dtUtil::RefString CharacterFileElements::OFFSET_X_ATTR("x");
   const dtUtil::RefString CharacterFileElements::OFFSET_Y_ATTR("y");
   const dtUtil::RefString CharacterFileElements::OFFSET_Z_ATTR("z");
   const dtUtil::RefString CharacterFileElements::OFFSET_RX_ATTR("rx");
   const dtUtil::RefString CharacterFileElements::OFFSET_RY_ATTR("ry");
   const dtUtil::RefString CharacterFileElements::OFFSET_RZ_ATTR("rz");
   const dtUtil::RefString CharacterFileElements::RESOURCE_ATTR("resource");



} // namespace dtAnim
