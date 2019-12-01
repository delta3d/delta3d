/* -*-c++-*-
 * Delta3D
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * 
 * David Guthrie
 */

#ifndef PROPERTYCONTAINERACTORPROPERTY_H_
#define PROPERTYCONTAINERACTORPROPERTY_H_

#include <dtCore/export.h>

#include <dtCore/actorproperty.h>
#include <dtCore/propertycontainer.h>
#include <dtUtil/functor.h>

namespace dtCore
{

   /**
    * @brief supports a property container or as a property
    */
   class DT_CORE_EXPORT BasePropertyContainerActorProperty : public ActorProperty
   {
   public:
      BasePropertyContainerActorProperty(
               const dtUtil::RefString& name,
               const dtUtil::RefString& label,
               const dtUtil::RefString& desc,
               const dtUtil::RefString& groupName);

      virtual dtCore::PropertyContainer* GetValue() const = 0;
      virtual void SetValue(dtCore::PropertyContainer* value) = 0;

      virtual void CreateNew() = 0;

      virtual void CopyFrom(const ActorProperty& otherProp);

      virtual const std::string ToString() const;

      virtual bool FromString(const std::string& value);

   protected:

      virtual ~BasePropertyContainerActorProperty();
   };

   /**
    * This is the main property container actor property implementation, but it still leaves up to the developer
    * the means of creating a new one via the "CreateNew" pure virtual function.
    * It could be extended so that it could create various types.
    */
   template <typename PropContainerT>
   class PropertyContainerActorProperty : public BasePropertyContainerActorProperty
   {
   public:
      typedef typename dtUtil::Functor<void, TYPELIST_1(PropContainerT*)> SetFuncType;
      typedef typename dtUtil::Functor<PropContainerT*, TYPELIST_0()> GetFuncType;


      PropertyContainerActorProperty(
               const dtUtil::RefString& name,
               const dtUtil::RefString& label,
               SetFuncType set,
               GetFuncType get,
               const dtUtil::RefString& desc,
               const dtUtil::RefString& groupName)
      : BasePropertyContainerActorProperty(name, label, desc, groupName)
      , mSetFunc(set)
      , mGetFunc(get)
      {
      }

      virtual dtCore::PropertyContainer* GetValue() const
      {
         return mGetFunc();
      }

      virtual void SetValue(dtCore::PropertyContainer* value)
      {
         PropContainerT* pc = dynamic_cast<PropContainerT*>(value);
         if (pc != NULL)
         {
            mSetFunc(pc);
         }
      }

   protected:

      virtual ~PropertyContainerActorProperty() {}

      SetFuncType mSetFunc;
      GetFuncType mGetFunc;
   };

   /**
    * This is a concrete PropertyContainerActorProperty template that assumes it will only
    * use one single type for the property container.
    */
   template <typename T>
   class SimplePropertyContainerActorProperty : public PropertyContainerActorProperty<T>
   {
   public:
	  typedef PropertyContainerActorProperty<T> BaseClass;
      typedef typename BaseClass::SetFuncType SetFuncType;
      typedef typename BaseClass::GetFuncType GetFuncType;

      SimplePropertyContainerActorProperty(
               const dtUtil::RefString& name,
               const dtUtil::RefString& label,
               SetFuncType set,
               GetFuncType get,
               const dtUtil::RefString& desc,
               const dtUtil::RefString& groupName)
      : PropertyContainerActorProperty<T>(name, label, set, get, desc, groupName)
      {
      }

      virtual void CreateNew() { BaseClass::SetValue(new T()); }
   protected:
      virtual ~SimplePropertyContainerActorProperty() {}
   };


}
#endif /* PROPERTYCONTAINERACTORPROPERTY_H_ */
