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

#ifndef MECHANICS_TYPE_H_
#define MECHANICS_TYPE_H_

#include <dtPhysics/physicsexport.h>
#include <dtUtil/enumeration.h>

namespace dtPhysics
{

   class DT_PHYSICS_EXPORT MechanicsType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(MechanicsType);
   public:
      static MechanicsType STATIC;
      static MechanicsType KINEMATIC;
      static MechanicsType DYNAMIC;

      int GetID();
   private:
      MechanicsType(const std::string &name, int id);
      int mId;
   };
}

#endif
