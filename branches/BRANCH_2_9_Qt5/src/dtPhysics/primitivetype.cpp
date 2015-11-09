/* -*-c++-*-
 * dtPhysics
 * Copyright 2007-2008, Alion Science and Technology
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
 * David Guthrie
 */

#include <dtPhysics/primitivetype.h>

namespace dtPhysics
{
   IMPLEMENT_ENUM(PrimitiveType);
   PrimitiveType PrimitiveType::BOX("Box", false, true);
   PrimitiveType PrimitiveType::SPHERE("Sphere", false, true);
   PrimitiveType PrimitiveType::CYLINDER("Cylinder", false, true);
   PrimitiveType PrimitiveType::CAPSULE("Capsule", false, true);
   PrimitiveType PrimitiveType::CONVEX_HULL("Convex Hull", false, false);
   PrimitiveType PrimitiveType::HEIGHTFIELD("Heightfield", true, false);
   PrimitiveType PrimitiveType::TRIANGLE_MESH("Triangle Mesh", false, false);
   PrimitiveType PrimitiveType::TERRAIN_MESH("Terrain Mesh", true, false);
   PrimitiveType PrimitiveType::CUSTOM_CONCAVE_MESH("Custom Concave Mesh", false, false);

   PrimitiveType::PrimitiveType(const std::string& name, bool terrainType, bool simpleShape)
   : dtUtil::Enumeration(name)
   , mTerrainType(terrainType)
   , mSimpleShape(simpleShape)
   {
      AddInstance(this);
   }

   bool PrimitiveType::IsTerrainType() const
   {
      return mTerrainType;
   }

   bool PrimitiveType::IsSimpleShape() const
   {
      return mSimpleShape;
   }

}
