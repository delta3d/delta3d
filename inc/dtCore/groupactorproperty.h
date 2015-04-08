/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
#ifndef DELTA_GROUP_ACTOR_PROPERTY
#define DELTA_GROUP_ACTOR_PROPERTY

#include <string>
#include <dtCore/actorproperty.h>
#include <dtCore/namedgroupparameter.h>
#include <dtCore/export.h>
#include <dtUtil/functor.h>

namespace dtCore
{
   /**
    * @brief An actor property that contains a structure of NamedParameter Objects.
    *
    * This actor property solves the concept of both structures and lists in actor properties.
    * This property gets and sets a NamedGroupParameter, which can hold any number of other NamedParameters.
    * This is passed to the functors for the actor property which must then handle the data in way that makes
    * sense the actor.  Additionally, a string name can be passed to the property in the constructor
    *
    * @note When loading and saving GroupActorProperties, only the NamedParameter types that have associated actor property types
    *       can be saved.  The rest will be ignored.  There are no plans no solve this issue.
    * @note Also, note that currently Vec3d and Vec3f parameters will be loaded as Vec3. There are plans to fix this.
    */
   class DT_CORE_EXPORT GroupActorProperty : public ActorProperty
   {
      public:
         typedef dtUtil::Functor<void, TYPELIST_1(const NamedGroupParameter&)> SetFuncType;
         typedef dtUtil::Functor<dtCore::RefPtr<NamedGroupParameter>, TYPELIST_0() > GetFuncType;
         /**
          * Creates a new group actor property.
          * @param name the name used to access this property.
          * @param label a more human readable name to use when displaying the property.
          * @param desc a longer description of the property.
          * @param set  The setting functor, a method/function taking a const reference to a NamedGroupParameter.
          * @param get  The getting functor, a method/function returning a ref ptr to a NamedGroupParameter  It returns
          *             a refptr because it is expected that the method returning the data will generating the parameter
          *             from internal data and won't be holding onto it.
          * @param groupName The property grouping to use when editing properties visually.
          * @param editorType a string specifying what type of editor to use for the data in this group.  These
          *                   will specify a UI to use in STAGE.
          * @param readOnly true if this property should not be editable.  Defaults to false.
          */
         GroupActorProperty(const std::string& name,
                            const std::string& label,
                            SetFuncType set,
                            GetFuncType get,
                            const std::string& desc,
                            const std::string& groupName,
                            const std::string& editorType = "",
                            bool readOnly = false);

         /**
          * Sets the value of the property based on a string.
          * @note This method will attempt to use the data to set the value, but it may return false if the data
          * could not be used.
          * @param value the value to set.
          * @return true if the string was usable to set the value, false if not.
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling SetStringValue.
          * @see #SetStringValue
          */
         virtual const std::string ToString() const;

         /**
          * This is overridden to make handle the fact that the get method returns a refptr.
          * @param otherProp The property to copy from.
          */
         virtual void CopyFrom(const ActorProperty& otherProp);

         /**
          * Sets the value of this property by calling the set functor
          * assigned to this property.
          */
         void SetValue(const NamedGroupParameter& value);

         /**
          * @return the value of this property be calling the get functor
          * assigned to this property.
          */
         dtCore::RefPtr<NamedGroupParameter> GetValue() const;

         const std::string& GetEditorType() const { return mEditorType; };

     protected:

         virtual ~GroupActorProperty();

      private:
         ///Set functor taking one parameter and optionally returning a value.
         SetFuncType mSetPropFunctor;

         ///Get functor which returns a value and takes no parameters.
         GetFuncType mGetPropFunctor;
         ///Used by stage
         const std::string mEditorType;
   };
}
#endif
