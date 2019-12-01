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

#include <dtPhysics/mechanicstype.h>
#include <pal/palBodies.h>
namespace dtPhysics
{
   IMPLEMENT_ENUM(MechanicsType);
   MechanicsType MechanicsType::STATIC("Static", int(PALBODY_STATIC));
   MechanicsType MechanicsType::KINEMATIC("Kinematic", int(PALBODY_KINEMATIC));
   MechanicsType MechanicsType::DYNAMIC("Dynamic", int(PALBODY_DYNAMIC));

   MechanicsType::MechanicsType(const std::string& name, int id)
   : dtUtil::Enumeration(name)
   , mId(id)
   {
      AddInstance(this);
   }

   int MechanicsType::GetID()
   {
      return mId;
   }
}
