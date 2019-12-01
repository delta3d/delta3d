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

#ifndef VECTOR_ACTOR_PROPERTIES
#define VECTOR_ACTOR_PROPERTIES

#include <dtCore/datatype.h>
#include <dtCore/export.h>
#include <dtCore/genericactorproperty.h>

#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/Vec4>
#include <osg/Vec4d>
#include <osg/Vec2>
#include <osg/Vec2d>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 2 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT Vec2fActorProperty :
      public GenericActorProperty<const osg::Vec2f&, osg::Vec2f>
   {
      public:
         typedef GenericActorProperty<const osg::Vec2f&, osg::Vec2f> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec2fActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "");

         /**
          * Sets the value of the property based on a string.
          * The string should be two floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~Vec2fActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 2 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT Vec2dActorProperty :
      public GenericActorProperty<const osg::Vec2d&, osg::Vec2d>
   {
      public:
         typedef GenericActorProperty<const osg::Vec2d&, osg::Vec2d> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec2dActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "");

         /**
          * Sets the value of the property based on a string.
          * The string should be two floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~Vec2dActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 3 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT Vec3fActorProperty :
      public GenericActorProperty<const osg::Vec3f&, osg::Vec3f>
   {
      public:
         typedef GenericActorProperty<const osg::Vec3f&, osg::Vec3f> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec3fActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "");

         /**
          * Sets the value of the property based on a string.
          * The string should be three floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~Vec3fActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 3 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT Vec3dActorProperty :
      public GenericActorProperty<const osg::Vec3d&, osg::Vec3d>
   {
      public:
         typedef GenericActorProperty<const osg::Vec3d&, osg::Vec3d> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec3dActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "");

         /**
          * Sets the value of the property based on a string.
          * The string should be three floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~Vec3dActorProperty() { }
   };

      ////////////////////////////////////////////////////////////////////////////
      /**
       * This actor property represents a vector data member with 4 values.
       */
      ////////////////////////////////////////////////////////////////////////////
      class DT_CORE_EXPORT Vec4fActorProperty :
         public GenericActorProperty<const osg::Vec4f&, osg::Vec4f>
      {
         public:
            typedef GenericActorProperty<const osg::Vec4f&, osg::Vec4f> BaseClass;
            typedef BaseClass::SetFuncType SetFuncType;
            typedef BaseClass::GetFuncType GetFuncType;

            Vec4fActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                              SetFuncType set, GetFuncType get,
                              const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "",
                              dtCore::DataType& type = DataType::VEC4F);

            /**
             * Sets the value of the property based on a string.
             * The string should be four floating point values as a strings separated by spaces.
             * This will return false if the values can't be set.
             * @note Returns false it the property is read only
             * @param value the value to set.
             * @return true
             */
            virtual bool FromString(const std::string& value);


            /**
             * @return a string version of the data.  This value can be used when calling FromString.
             * @see #FromString
             */
            virtual const std::string ToString() const;

         protected:

            virtual ~Vec4fActorProperty() { }
      };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 4 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT Vec4dActorProperty :
      public GenericActorProperty<const osg::Vec4d&, osg::Vec4d>
   {
      public:
         typedef GenericActorProperty<const osg::Vec4d&, osg::Vec4d> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec4dActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "");

         /**
          * Sets the value of the property based on a string.
          * The string should be four floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~Vec4dActorProperty() { }
   };

   /// TODO make this auto change based on if the Vec[2,3,4] is has a float or a double.
   typedef Vec2fActorProperty Vec2ActorProperty;
   typedef Vec3fActorProperty Vec3ActorProperty;
   typedef Vec4fActorProperty Vec4ActorProperty;
}

#endif //VECTOR_ACTOR_PROPERTIES
