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
 * @author Matthew W. Campbell
 */
#include "dtDAL/datatype.h"

namespace dtDAL 
{
   IMPLEMENT_ENUM(DataType);
   DataType DataType::UNKNOWN("UNKNOWN", "Unknown", false, 0);
   DataType DataType::CHAR("CHAR", "Char", false, 1);
   DataType DataType::UCHAR("UCHAR", "Unsigned Char", false, 2);
   DataType DataType::FLOAT("FLOAT", "Float", false, 3);
   DataType DataType::DOUBLE("DOUBLE", "Double", false, 4);
   DataType DataType::INT("INT", "Int", false, 5);
   DataType DataType::UINT("UINT", "Unsigned Int", false, 6);
   DataType DataType::LONGINT("LONG", "Long");
   DataType DataType::ULONGINT("ULONG", "Unsigned Long", false, 7);
   DataType DataType::SHORTINT("SHORT", "Unsigned Short", false, 8);
   DataType DataType::USHORTINT("USHORT", "Unsigned short", false, 9);
   DataType DataType::STRING("STRING", "String", false, 10);
   DataType DataType::BOOLEAN("BOOLEAN", "Boolean", false, 11);
   DataType DataType::VEC2("VEC2", "Vector 2", false, 12);
   DataType DataType::VEC3("VEC3", "Vector 3", false, 13);
   DataType DataType::VEC4("VEC4", "Vector 4", false, 14);
   DataType DataType::VEC2F("VEC2F", "Vector 2f", false, 15);
   DataType DataType::VEC3F("VEC3F", "Vector 3f", false, 16);
   DataType DataType::VEC4F("VEC4F", "Vector 4f", false, 17);
   DataType DataType::VEC2D("VEC2D", "Vector 2d", false, 18);
   DataType DataType::VEC3D("VEC3D", "Vector 3d", false, 19);
   DataType DataType::VEC4D("VEC4D", "Vector 4d", false, 20);
   DataType DataType::RGBCOLOR("RGBCOLOR", "RGB Color", false, 21);
   DataType DataType::RGBACOLOR("RGBACOLOR", "RGBA Color", false, 22);
   DataType DataType::ENUMERATION("ENUMERATION",  "Enumeration", false, 23);
   DataType DataType::GAME_EVENT("GAME_EVENT","Game Event",false,24);
   DataType DataType::GROUP("GROUP", "Group", false, 25);
   DataType DataType::ACTOR("ACTOR", "Actor", false, 26);

   DataType DataType::STATIC_MESH("StaticMeshes", "Static Mesh", true, 27);
   DataType DataType::TEXTURE("Textures", "Texture", true, 28);
   DataType DataType::CHARACTER("Characters", "Character", true, 29);
   DataType DataType::TERRAIN("Terrains", "Terrain", true, 30);
   DataType DataType::SOUND("Sounds", "Sound", true, 31);
   DataType DataType::PARTICLE_SYSTEM("Particles", "Particle", true, 32);

}
