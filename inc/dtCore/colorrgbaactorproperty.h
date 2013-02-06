/* -*-c++-*-
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
 * William E. Johnson II
 * Eric Heine
 */

#ifndef COLOR_RGBA_ACTOR_PROPERTY
#define COLOR_RGBA_ACTOR_PROPERTY

#include <dtCore/export.h>
#include <dtCore/vectoractorproperties.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a color data member with 4 floats.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT ColorRgbaActorProperty : public Vec4ActorProperty
   {
      public:
         typedef Vec4ActorProperty BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         ColorRgbaActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                              SetFuncType set, GetFuncType get,
                              const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "");

         // This is a work around a bug in Visual Studio where the Unit Tests would fail at runtime because
         // it couldn't find these functions in this class, even though they are inherited.
         virtual bool FromString(const std::string& value);
         virtual const std::string ToString() const;

      protected:
         virtual ~ColorRgbaActorProperty() { }
   };
}

#endif //COLOR_RGBA_ACTOR_PROPERTY
