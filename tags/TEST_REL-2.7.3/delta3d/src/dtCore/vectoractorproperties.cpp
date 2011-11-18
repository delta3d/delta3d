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
 * Eric Heine
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/vectoractorproperties.h>
#include <dtUtil/stringutils.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////////
   Vec2fActorProperty::Vec2fActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/)
      : BaseClass(DataType::VEC2F, name, label, set, get, desc, groupName)
   {
      SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec2fActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec2f newValue;

      if (dtUtil::ParseVec<osg::Vec2f>(value, newValue, 2))
      {
         SetValue(newValue);
         return true;
      }
      else return false;

   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec2fActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec2dActorProperty::Vec2dActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/)
      : BaseClass(DataType::VEC2D, name, label, set, get, desc, groupName)
   {
      SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec2dActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec2d newValue;

      if (dtUtil::ParseVec<osg::Vec2d>(value, newValue, 2))
      {
         SetValue(newValue);
         return true;
      }
      else return false;

   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec2dActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec3fActorProperty::Vec3fActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/)
      : BaseClass(DataType::VEC3F, name, label, set, get, desc, groupName)
   {
      SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec3fActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec3f newValue;

      if (dtUtil::ParseVec<osg::Vec3f>(value, newValue, 3))
      {
         SetValue(newValue);
         return true;
      }
      else return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec3fActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec3dActorProperty::Vec3dActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/)
      : BaseClass(DataType::VEC3D, name, label, set, get, desc, groupName)
   {
      SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec3dActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec3d newValue;

      if (dtUtil::ParseVec<osg::Vec3d>(value, newValue, 3))
      {
         SetValue(newValue);
         return true;
      }
      else return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec3dActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec4fActorProperty::Vec4fActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/,
      dtCore::DataType& type /*= DataType::VEC4F*/)
      : BaseClass(type, name, label, set, get, desc, groupName)
   {
      SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec4fActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec4f newValue;

      if (dtUtil::ParseVec<osg::Vec4f>(value, newValue, 4))
      {
         SetValue(newValue);
         return true;
      }
      else return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec4fActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec4dActorProperty::Vec4dActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/)
      : BaseClass(DataType::VEC4D, name, label, set, get, desc, groupName)
   {
      SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool Vec4dActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec4d newValue;

      if (dtUtil::ParseVec<osg::Vec4d>(value, newValue, 4))
      {
         SetValue(newValue);
         return true;
      }
      else return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string Vec4dActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());
      stream << GetValue();
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////
}

