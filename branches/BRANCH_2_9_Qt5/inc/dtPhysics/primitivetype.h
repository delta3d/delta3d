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

#ifndef PRIMITIVE_TYPE_H_
#define PRIMITIVE_TYPE_H_

#include <dtPhysics/physicsexport.h>
#include <dtUtil/enumeration.h>

namespace dtPhysics
{
   /////////////////////////////////////////////////////////////////////
   class DT_PHYSICS_EXPORT PrimitiveType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(PrimitiveType);
   public:
      static PrimitiveType BOX;
      static PrimitiveType SPHERE;
      static PrimitiveType CYLINDER;
      static PrimitiveType CAPSULE;
      static PrimitiveType HEIGHTFIELD; // Not yet supported
      static PrimitiveType CONVEX_HULL;
      static PrimitiveType TRIANGLE_MESH;
      static PrimitiveType TERRAIN_MESH;
      static PrimitiveType CUSTOM_CONCAVE_MESH;

      /// @return true if this primitive type instance represents a static terrain type.
      bool IsTerrainType() const;

      /// @return if this primitive type refers to a simple shape type as opposed to a mesh.
      bool IsSimpleShape() const;
   private:
      PrimitiveType(const std::string& name, bool terrainType, bool simpleShape);
      bool mTerrainType;
      bool mSimpleShape;
   };
}
#endif /* PRIMITIVE_TYPE_H_ */
