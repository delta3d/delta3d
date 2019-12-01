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
 */

#ifndef DELTA_NAMED_VECTOR_PARAMETER
#define DELTA_NAMED_VECTOR_PARAMETER

#include <dtCore/namedgenericparameter.h>

#include <dtUtil/stringutils.h>

namespace dtCore
{
   /**
    * @class NamedVecParameter
    * This is a template for the vector types to make reading data from a string easier.
    */
   template <class ParamType>
   class NamedVecParameter: public NamedGenericParameter<ParamType>
   {
      public:
         virtual const std::string ToString() const
         {
            std::ostringstream ss;
            ss.precision(NamedGenericParameter<ParamType>::GetNumberPrecision());

            if (NamedGenericParameter<ParamType>::IsList())
            {
               const std::vector<ParamType>& values =
                  NamedGenericParameter<ParamType>::GetValueList();

               if (!values.empty())
               {
                  ss << values[0];
               }

               for (unsigned int i = 1; i < values.size(); ++i)
               {
                  ss << NamedGenericParameter<ParamType>::GetParamDelimeter() <<
                     values[i];
               }
               return ss.str();
            }
            else
            {
               ss << NamedGenericParameter<ParamType>::GetValue();
               return ss.str();
            }
         }

      protected:
         NamedVecParameter(DataType& dataType, const dtUtil::RefString& name, const ParamType& defaultValue,
            bool isList = false) : NamedGenericParameter<ParamType>(dataType, name, defaultValue, isList)
         {
               NamedGenericParameter<ParamType>::SetNumberPrecision(2 * sizeof(defaultValue[0]) + 1);
         }
         virtual ~NamedVecParameter() {}

         bool InternalFromString(const std::string& value, unsigned size)
         {
            if (NamedGenericParameter<ParamType>::IsList())
            {
               std::vector<ParamType>& result =
                  NamedGenericParameter<ParamType>::GetValueList();

               std::vector<std::string> stringList;
               dtUtil::IsDelimeter delimCheck(NamedGenericParameter<ParamType>::GetParamDelimeter());
               dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(stringList, value, delimCheck);

               result.clear();
               for (unsigned int i = 0; i < stringList.size(); ++i)
               {
                  ParamType v;
                  if (!dtUtil::ParseVec<ParamType>(stringList[i], v, size))
                  {
                     return false;
                  }
                  result.push_back(v);
               }

               return true;
            }
            else
            {
               ParamType newValue;
               if (dtUtil::ParseVec<ParamType>(value, newValue, size))
               {
                  NamedGenericParameter<ParamType>::SetValue(newValue);
                  return true;
               }
               else
               {
                  return false;
               }
            }
         }
   };

   /**
     * @class Vec2fMessageParameter
     */
   class DT_CORE_EXPORT NamedVec2fParameter: public NamedVecParameter<osg::Vec2f>
   {
      public:
         NamedVec2fParameter(const dtUtil::RefString& name,
             const osg::Vec2f& defaultValue = osg::Vec2f(0.0,0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 2); }

         virtual void SetFromProperty(const dtCore::ActorProperty& property);
         virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

         virtual bool operator==(const ActorProperty& toCompare) const;

      protected:
         virtual ~NamedVec2fParameter();
   };

   typedef NamedVec2fParameter NamedVec2Parameter;

   /**
     * @class Vec2dMessageParameter
     */
   class DT_CORE_EXPORT NamedVec2dParameter: public NamedVecParameter<osg::Vec2d>
   {
      public:
         NamedVec2dParameter(const dtUtil::RefString& name,
               const osg::Vec2d& defaultValue = osg::Vec2d(0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 2); }

         virtual void SetFromProperty(const dtCore::ActorProperty& property);
         virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

         virtual bool operator==(const ActorProperty& toCompare) const;

      protected:
         virtual ~NamedVec2dParameter();
   };


   /**
     * @class Vec3fMessageParameter
     */
   class DT_CORE_EXPORT NamedVec3fParameter: public NamedVecParameter<osg::Vec3f>
   {
      public:
         NamedVec3fParameter(const dtUtil::RefString& name,
               const osg::Vec3f& defaultValue = osg::Vec3f(0.0, 0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 3); }

         virtual void SetFromProperty(const dtCore::ActorProperty& property);
         virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

         virtual bool operator==(const ActorProperty& toCompare) const;

      protected:
         NamedVec3fParameter(DataType& dataType, const dtUtil::RefString& name,
            const osg::Vec3f& defaultValue, bool isList);
         virtual ~NamedVec3fParameter();
   };

   typedef NamedVec3fParameter NamedVec3Parameter;

   /**
     * @class Vec3dMessageParameter
     */
   class DT_CORE_EXPORT NamedVec3dParameter: public NamedVecParameter<osg::Vec3d>
   {
      public:
         NamedVec3dParameter(const dtUtil::RefString& name,
             const osg::Vec3d& defaultValue = osg::Vec3d(0.0, 0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 3); }

         virtual void SetFromProperty(const dtCore::ActorProperty& property);
         virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

         virtual bool operator==(const ActorProperty& toCompare) const;

      protected:
         virtual ~NamedVec3dParameter();
   };

   /**
    * @class Vec4fMessageParameter
    */
   class DT_CORE_EXPORT NamedVec4fParameter: public NamedVecParameter<osg::Vec4f>
   {
      public:
         NamedVec4fParameter(const dtUtil::RefString& name,
          const osg::Vec4f& defaultValue = osg::Vec4f(0.0, 0.0, 0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value)  {  return InternalFromString(value, 4); }

         virtual void SetFromProperty(const dtCore::ActorProperty& property);
         virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

         virtual bool operator==(const ActorProperty& toCompare) const;

      protected:
         NamedVec4fParameter(DataType& dataType, const dtUtil::RefString& name,
            const osg::Vec4f& defaultValue, bool isList);
         virtual ~NamedVec4fParameter();
   };

   typedef NamedVec4fParameter NamedVec4Parameter;

   /**
    * @class Vec4dMessageParameter
    */
   class DT_CORE_EXPORT NamedVec4dParameter: public NamedVecParameter<osg::Vec4d>
   {
      public:
         NamedVec4dParameter(const dtUtil::RefString& name,
            const osg::Vec4d& defaultValue = osg::Vec4d(0.0, 0.0, 0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 4); }

         virtual void SetFromProperty(const dtCore::ActorProperty& property);
         virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

         virtual bool operator==(const ActorProperty& toCompare) const;

      protected:
         virtual ~NamedVec4dParameter();
   };
}

#endif //DELTA_NAMED_VECTOR_PARAMETER
