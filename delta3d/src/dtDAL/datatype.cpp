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
    DataType DataType::UNKNOWN("UNKNOWN", "Unknown");
    DataType DataType::FLOAT("FLOAT", "Float");
    DataType DataType::DOUBLE("DOUBLE", "Double");
    DataType DataType::INT("INT", "Int");
    DataType DataType::LONGINT("LONG", "Long");
    DataType DataType::STRING("STRING", "String");
    DataType DataType::BOOLEAN("BOOLEAN", "Boolean");
    DataType DataType::VEC2("VEC2", "Vector 2");
    DataType DataType::VEC3("VEC3", "Vector 3");
    DataType DataType::VEC4("VEC4", "Vector 4");
    DataType DataType::RGBCOLOR("RGBCOLOR", "RGB Color");
    DataType DataType::RGBACOLOR("RGBACOLOR", "RGBA Color");
    DataType DataType::ENUMERATION("ENUMERATION",  "Enumeration");
    DataType DataType::GROUP("GROUP", "Group");
    DataType DataType::ACTOR("ACTOR", "Actor");

    DataType DataType::STATIC_MESH("StaticMeshes", "Static Mesh", true);
    DataType DataType::TEXTURE("Textures", "Texture", true);
    DataType DataType::CHARACTER("Characters", "Character", true);
    DataType DataType::TERRAIN("Terrains", "Terrain", true);
    DataType DataType::SOUND("Sounds", "Sound", true);
    DataType DataType::PARTICLE_SYSTEM("Particles", "Particle", true);

}
