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
 * Matthew W. Campbell
 * David Guthrie
 */
#include <prefix/dtdalprefix-src.h>
#include <dtDAL/datatype.h>

namespace dtDAL 
{
   IMPLEMENT_ENUM(DataType);
   DataType DataType::UNKNOWN("UNKNOWN", "Unknown", false, UNKNOWN_ID);
   DataType DataType::CHAR("CHAR", "Char", false, CHAR_ID);
   DataType DataType::UCHAR("UCHAR", "Unsigned Char", false, UCHAR_ID);
   DataType DataType::FLOAT("FLOAT", "Float", false, FLOAT_ID);
   DataType DataType::DOUBLE("DOUBLE", "Double", false, DOUBLE_ID);
   DataType DataType::INT("INT", "Int", false, INT_ID);
   DataType DataType::UINT("UINT", "Unsigned Int", false, UINT_ID);
   DataType DataType::LONGINT("LONG", "Long", false, LONGINT_ID);
   DataType DataType::ULONGINT("ULONG", "Unsigned Long", false, ULONGINT_ID);
   DataType DataType::SHORTINT("SHORT", "Unsigned Short", false, SHORTINT_ID);
   DataType DataType::USHORTINT("USHORT", "Unsigned short", false, USHORTINT_ID);
   DataType DataType::STRING("STRING", "String", false, STRING_ID);
   DataType DataType::BOOLEAN("BOOLEAN", "Boolean", false, BOOLEAN_ID);
   
   DataType DataType::VEC2("VEC2", "Vector 2", false, VEC2_ID);
   DataType DataType::VEC3("VEC3", "Vector 3", false, VEC3_ID);
   DataType DataType::VEC4("VEC4", "Vector 4", false, VEC4_ID);
   
   DataType DataType::VEC2F("VEC2F", "Vector 2f", false, VEC2F_ID);
   DataType DataType::VEC3F("VEC3F", "Vector 3f", false, VEC3F_ID);
   DataType DataType::VEC4F("VEC4F", "Vector 4f", false, VEC4F_ID);
   
   DataType DataType::VEC2D("VEC2D", "Vector 2d", false, VEC2D_ID);
   DataType DataType::VEC3D("VEC3D", "Vector 3d", false, VEC3D_ID);
   DataType DataType::VEC4D("VEC4D", "Vector 4d", false, VEC4D_ID);
   
   DataType DataType::RGBCOLOR("RGBCOLOR", "RGB Color", false, RGBCOLOR_ID);
   DataType DataType::RGBACOLOR("RGBACOLOR", "RGBA Color", false, RGBACOLOR_ID);
   
   DataType DataType::ENUMERATION("ENUMERATION",  "Enumeration", false, ENUMERATION_ID);
   DataType DataType::GAME_EVENT("GAME_EVENT", "Game Event", false, GAMEEVENT_ID);
   DataType DataType::GROUP("GROUP", "Group", false, GROUP_ID);
   DataType DataType::ACTOR("ACTOR", "Actor", false, ACTOR_ID);

   DataType DataType::STATIC_MESH("StaticMeshes", "Static Mesh", true, STATICMESH_ID);
   DataType DataType::TEXTURE("Textures", "Texture", true, TEXTURE_ID);
   DataType DataType::CHARACTER("Characters", "Character", true, CHARACTER_ID);
   DataType DataType::TERRAIN("Terrains", "Terrain", true, TERRAIN_ID);
   DataType DataType::SOUND("Sounds", "Sound", true, SOUND_ID);
   DataType DataType::PARTICLE_SYSTEM("Particles", "Particle", true, PARTICLESYSTEM_ID);
}
