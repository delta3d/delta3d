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
 * @author David Guthrie
 */

#include <prefix/dtcoreprefix.h>

#include <dtCore/namedvectorparameters.h>

#include <dtCore/vectoractorproperties.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec2fParameter::NamedVec2fParameter(const dtUtil::RefString& name,
      const osg::Vec2f& defaultValue, bool isList)
      : NamedVecParameter<osg::Vec2f>(dtCore::DataType::VEC2F, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec2fParameter::~NamedVec2fParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec2fParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::Vec2fActorProperty *ap = static_cast<const dtCore::Vec2fActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec2fParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::Vec2fActorProperty *ap = static_cast<dtCore::Vec2fActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedVec2fParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return GetValue() == static_cast<const dtCore::Vec2fActorProperty*>(&toCompare)->GetValue();
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec2dParameter::NamedVec2dParameter(const dtUtil::RefString& name,
      const osg::Vec2d& defaultValue, bool isList)
      : NamedVecParameter<osg::Vec2d>(dtCore::DataType::VEC2D, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec2dParameter::~NamedVec2dParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec2dParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::Vec2dActorProperty *ap = static_cast<const dtCore::Vec2dActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec2dParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::Vec2dActorProperty *ap = static_cast<dtCore::Vec2dActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedVec2dParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return GetValue() == static_cast<const dtCore::Vec2dActorProperty*>(&toCompare)->GetValue();
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3fParameter::NamedVec3fParameter(const dtUtil::RefString& name,
      const osg::Vec3f& defaultValue, bool isList)
      : NamedVecParameter<osg::Vec3f>(dtCore::DataType::VEC3F, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3fParameter::NamedVec3fParameter(DataType& dataType, const dtUtil::RefString& name,
      const osg::Vec3f& defaultValue, bool isList)
      : NamedVecParameter<osg::Vec3f>(dataType, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3fParameter::~NamedVec3fParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec3fParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::Vec3fActorProperty *ap = static_cast<const dtCore::Vec3fActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec3fParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::Vec3fActorProperty *ap = static_cast<dtCore::Vec3fActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedVec3fParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return GetValue() == static_cast<const dtCore::Vec3fActorProperty*>(&toCompare)->GetValue();
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3dParameter::NamedVec3dParameter(const dtUtil::RefString& name,
      const osg::Vec3d& defaultValue, bool isList)
      : NamedVecParameter<osg::Vec3d>(dtCore::DataType::VEC3D, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3dParameter::~NamedVec3dParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec3dParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::Vec3dActorProperty *ap = static_cast<const dtCore::Vec3dActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec3dParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::Vec3dActorProperty *ap = static_cast<dtCore::Vec3dActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedVec3dParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return GetValue() == static_cast<const dtCore::Vec3dActorProperty*>(&toCompare)->GetValue();
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4fParameter::NamedVec4fParameter(const dtUtil::RefString& name,
      const osg::Vec4f& defaultValue, bool isList)
      : NamedVecParameter<osg::Vec4f>(dtCore::DataType::VEC4F, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4fParameter::NamedVec4fParameter(DataType& dataType, const dtUtil::RefString& name,
      const osg::Vec4f& defaultValue, bool isList) :
   NamedVecParameter<osg::Vec4f>(dataType, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4fParameter::~NamedVec4fParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec4fParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::Vec4fActorProperty *ap = static_cast<const dtCore::Vec4fActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec4fParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::Vec4fActorProperty *ap = static_cast<dtCore::Vec4fActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedVec4fParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return GetValue() == static_cast<const dtCore::Vec4fActorProperty*>(&toCompare)->GetValue();
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4dParameter::NamedVec4dParameter(const dtUtil::RefString& name,
      const osg::Vec4d& defaultValue, bool isList)
      : NamedVecParameter<osg::Vec4d>(dtCore::DataType::VEC4D, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4dParameter::~NamedVec4dParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec4dParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::Vec4dActorProperty *ap = static_cast<const dtCore::Vec4dActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec4dParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::Vec4dActorProperty *ap = static_cast<dtCore::Vec4dActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedVec4dParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return GetValue() == static_cast<const dtCore::Vec4dActorProperty*>(&toCompare)->GetValue();
      }
      return false;
   }
}
