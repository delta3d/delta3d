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
 */

#ifndef genericactorproperty_h__
#define genericactorproperty_h__


#include <dtCore/actorproperty.h>
#include <dtCore/datatype.h>
#include <dtUtil/functor.h>
#include <dtUtil/log.h>

namespace dtCore
{

   /**
    * The GenericActorProperty class implements the set and get functionality
    * which defines the primary behavior of the ActorProperty.  The class is
    * templated such that it has a set type and a get type which correspond to
    * the set and get method signatures.
    * @note
    *     The set method signature excepts one parameter of any type and may
    *     or may not return a value.
    * @note
    *     The get method returns a value of any type and takes no parameters.
    */
   template <class SetType, class GetType>
   class GenericActorProperty : public ActorProperty
   {
   public:
      typedef GetType GetValueType;
      typedef SetType SetValueType;

      typedef dtUtil::Functor<void, TYPELIST_1(SetType)> SetFuncType;
      typedef dtUtil::Functor<GetType, TYPELIST_0()> GetFuncType;
      /**
       * Constructs the actor property.  Note, that functor objects must be
       * specified and match the set and get types of the GenericActorProperty.
       */
      GenericActorProperty(DataType& dataType,
         const dtUtil::RefString& name,
         const dtUtil::RefString& label,
         SetFuncType& set,
         GetFuncType& get,
         const dtUtil::RefString& desc,
         const dtUtil::RefString& groupName,
         bool readOnly = false)
         : ActorProperty(dataType, name,label,desc, groupName, readOnly)
         , SetPropFunctor(set)
         , GetPropFunctor(get)
      {
      }

      /**
       * This method allows a generic property of any type to be copied from
       * one to the other.
       * @param otherProp The property to copy from.
       * @note
       *     This method will only allow generic properties of the same
       *     type to be copied.  For example, a FloatActorProperty cannot
       *     be copied to an IntActorProperty.
       */
      virtual void CopyFrom(const ActorProperty& otherProp)
      {
         if (GetDataType() != otherProp.GetDataType())
         {
            LOG_ERROR(std::string("Property types are incompatible. Cannot copy from ") +
                     otherProp.GetLabel() + "(" +
                     otherProp.GetDataType().GetDisplayName() + ") to " +
                     GetLabel() + "(" +
                     GetDataType().GetDisplayName() + ")");
            return;
         }

         const GenericActorProperty<SetType,GetType>& prop =
            static_cast<const GenericActorProperty<SetType,GetType>& >(otherProp);

         SetValue(prop.GetValue());
      }

      /**
       * Sets the value of this property by calling the set functor
       * assigned to this property.
       */
      void SetValue(SetType value)
      {
         if (!IsReadOnly())
         {
            SetPropFunctor(value);
         }
         else
         {
            LOG_WARNING("SetValue has been called on a property that is read only.");
         }
      }

      /**
       * @return the value of this property be calling the get functor
       * assigned to this property.
       */
      GetType GetValue() const { return GetPropFunctor(); }

   protected:
      ///Keep destructors protected to ensure property smart pointer management.
      virtual ~GenericActorProperty() { }

   private:
      ///Set functor taking one parameter and optionally returning a value.
      SetFuncType SetPropFunctor;
      ///Get functor which returns a value and takes no parameters.
      GetFuncType GetPropFunctor;
   };

}

#endif // genericactorproperty_h__
