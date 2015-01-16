/* -*-c++-*-
* Using 'The MIT License'
* Copyright (C) 2009, Alion Science and Technology Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* @author Bradley Anderegg
*/

#ifndef DELTA_PROPERTYMACROS_H
#define DELTA_PROPERTYMACROS_H

#include <dtCore/containeractorproperty.h>
#include <dtCore/propertycontainer.h>
#include <dtCore/typetoactorproperty.h>

#include <dtUtil/command.h>
#include <dtUtil/functor.h>
#include <dtUtil/getsetmacros.h>

///////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

namespace dtCore
{

   template <class ContainerType, class FuncObj = ContainerType>
   struct PropertyRegHelper
   {
      //typedef PropertyRegHelper<ContainerType, FuncObj> Type;
      typedef typename dtUtil::TypeTraits<FuncObj>::value_type FunctorObjectType;
      typedef typename dtUtil::TypeTraits<ContainerType>::reference ContainerObjectType;

      PropertyRegHelper(ContainerObjectType con, FunctorObjectType* objPtr, const dtUtil::RefString& groupName)
         : mPropCon(con)
         , mFuncObj(objPtr)
         , mGroup(groupName)
      {}

      template <typename SetPtr, typename GetPtr>
      void RegisterProperty(SetPtr setter, GetPtr getter, const dtUtil::RefString& name,
               const dtUtil::RefString& label, const dtUtil::RefString& desc)
      {
         typedef typename dtUtil::FunTraits<GetPtr>::ResultType ResType;
         typedef typename dtUtil::TypeTraits<ResType>::value_type PropType;

         mPropCon.AddProperty(new typename dtCore::TypeToActorProperty<PropType>::value_type(name, label,
            typename dtCore::TypeToActorProperty<PropType>::SetFuncType(mFuncObj, setter),
            typename dtCore::TypeToActorProperty<PropType>::GetFuncType(mFuncObj, getter),
            desc, mGroup));
      }

      template <typename SetPtr, typename GetPtr>
      void RegisterResourceProperty(DataType& resourceType, SetPtr setter, GetPtr getter,
               const dtUtil::RefString& name, const dtUtil::RefString& label, const dtUtil::RefString& desc,
               const dtUtil::RefString& editor)
      {
         typedef typename dtUtil::FunTraits<GetPtr>::ResultType ResType;
         typedef typename dtUtil::TypeTraits<ResType>::value_type PropType;

         mPropCon.AddProperty(new dtCore::ResourceActorProperty(resourceType, name, label,
            typename dtCore::TypeToActorProperty<PropType>::SetFuncType(mFuncObj, setter),
            typename dtCore::TypeToActorProperty<PropType>::GetFuncType(mFuncObj, getter),
            desc, mGroup, editor));
      }

      template <typename SetPtr, typename GetPtr>
      void RegisterActorIdProperty(const dtUtil::RefString& baseClass, SetPtr setter, GetPtr getter,
               const dtUtil::RefString& name, const dtUtil::RefString& label, const dtUtil::RefString& desc)
      {
         typedef typename dtUtil::FunTraits<GetPtr>::ResultType ResType;
         typedef typename dtUtil::TypeTraits<ResType>::value_type PropType;

         mPropCon.AddProperty(new dtCore::ActorIDActorProperty(name, label,
            typename dtCore::TypeToActorProperty<PropType>::SetFuncType(mFuncObj, setter),
            typename dtCore::TypeToActorProperty<PropType>::GetFuncType(mFuncObj, getter),
            baseClass, desc, mGroup));
      }

      ContainerObjectType mPropCon;
      FunctorObjectType* mFuncObj;

      const dtUtil::RefString mGroup;
   };

#define DT_REGISTER_PROPERTY(PropertyName, PropertyDesc, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   RegHelperInstance.RegisterProperty( \
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
#PropertyName, #PropertyName, DESC_ ## PropertyName);\

#define DT_REGISTER_PROPERTY_WITH_NAME(PropertyName, PropertyStringName, PropertyDesc, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   RegHelperInstance.RegisterProperty(\
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
PropertyStringName, PropertyStringName, DESC_ ## PropertyName);\

#define DT_REGISTER_PROPERTY_WITH_LABEL(PropertyName, PropertyLabel, PropertyDesc, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   RegHelperInstance.RegisterProperty(\
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
#PropertyName, PropertyLabel, DESC_ ## PropertyName);\

#define DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(PropertyName, PropertyStringName, PropertyLabel, PropertyDesc, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   RegHelperInstance.RegisterProperty(\
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
PropertyStringName, PropertyLabel, DESC_ ## PropertyName);\

#define DT_REGISTER_RESOURCE_PROPERTY(DataType, PropertyName, PropertyLabel, PropertyDesc, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   RegHelperInstance.RegisterResourceProperty(DataType,\
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
#PropertyName, PropertyLabel, DESC_ ## PropertyName, "");\

#define DT_REGISTER_RESOURCE_PROPERTY_WITH_NAME(DataType, PropertyName, PropertyStringName, PropertyLabel, PropertyDesc, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   RegHelperInstance.RegisterResourceProperty(DataType,\
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
PropertyStringName, PropertyLabel, DESC_ ## PropertyName, "");\

#define DT_REGISTER_RESOURCE_PROPERTY_WITH_EDITOR(DataType, PropertyName, PropertyLabel, PropertyDesc, PropertyEditor, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   RegHelperInstance.RegisterResourceProperty(DataType,\
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
#PropertyName, PropertyLabel, DESC_ ## PropertyName, PropertyEditor);\

#define DT_REGISTER_RESOURCE_PROPERTY_WITH_NAME_AND_EDITOR(DataType, PropertyName, PropertyStringName, PropertyLabel, PropertyDesc, PropertyEditor, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   RegHelperInstance.RegisterResourceProperty(DataType,\
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   PropertyStringName, PropertyLabel, DESC_ ## PropertyName, PropertyEditor);\

/**
* Macro used to register a dtCore::ActorIDActorProperty
* @param BaseClassString The string name of the type of class to store (e.g., "dtCore::Transformable")
* @param PropertyName The name of the Property to define (e.g., MyProperty)
* @param PropertyLabel The string used to as a label display of this Property. (e.g., "My Property")
* @param PropertyDesc The string used to display as the description of this Property (e.g., "This is the description")
* @param RegHelperType_ The concrete type definition of the dtCore::PropertyRegHelper
* @param RegHelperInstance An instance of the RegHelperType_
 */
#define DT_REGISTER_ACTOR_ID_PROPERTY(BaseClassString, PropertyName, PropertyLabel, PropertyDesc, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   static const dtUtil::RefString BASE_ ## PropertyName (BaseClassString);\
   RegHelperInstance.RegisterActorIdProperty(BASE_ ## PropertyName,\
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
#PropertyName, PropertyLabel, DESC_ ## PropertyName);\

#define DT_REGISTER_ACTOR_ID_PROPERTY_WITH_NAME(BaseClassString, PropertyName, PropertyStringName, PropertyLabel, PropertyDesc, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   static const dtUtil::RefString BASE_ ## PropertyName (BaseClassString);\
    RegHelperInstance.RegisterActorIdProperty(BASE_ ## PropertyName,\
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
PropertyStringName, PropertyLabel, DESC_ ## PropertyName);\

#define DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName)\
   &RegHelperType_::FunctorObjectType::Get ## PropertyName\

#define DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName)\
   &RegHelperType_::FunctorObjectType::Set ## PropertyName\

}//namespace dtCore

#endif //DELTA_PROPERTYMACROS_H
