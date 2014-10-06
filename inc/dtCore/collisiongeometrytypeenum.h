/* -*-c++-*-
 * Delta3D
 * Copyright 2013, David Guthrie
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

#ifndef COLLISIONGEOMETRYTYPEENUM_H_
#define COLLISIONGEOMETRYTYPEENUM_H_

#include <dtUtil/enumeration.h>
#include <dtCore/export.h>

namespace dtCore
{
   /**
    * We need an enumeration to allow the user to set which type
    * of collision geometry to use.  The other properties in this
    * proxy such as radius, length, etc. affect the current type
    * of collision geometry.
    * Ideally, this enum shouldn't be here, but rather in the collision class.
    * Its left here for backward compatibility.
    */
   class DT_CORE_EXPORT CollisionGeomType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(CollisionGeomType);

   public:
      static CollisionGeomType NONE;
      static CollisionGeomType SPHERE;
      static CollisionGeomType CYLINDER;
      static CollisionGeomType CCYLINDER;
      static CollisionGeomType CUBE;
      static CollisionGeomType RAY;
      static CollisionGeomType MESH;

   private:
      CollisionGeomType(const std::string& name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
   };
}

#endif /* COLLISIONGEOMETRYTYPEENUM_H_ */
