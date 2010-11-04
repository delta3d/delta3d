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

#include <dtDAL/containeractorproperty.h>
#include <dtDAL/propertycontainer.h>
#include <dtDAL/typetoactorproperty.h>

#include <dtUtil/command.h>
#include <dtUtil/functor.h>
#include <dtUtil/getsetmacros.h>

///////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

namespace dtDAL
{

   template <class ContainerType, class FuncObj>
   struct PropertyRegHelper
   {
      //typedef PropertyRegHelper<ContainerType, FuncObj> Type;
      typedef FuncObj FunctorObjectType;
      typedef ContainerType ContainerObjectType;

      PropertyRegHelper(ContainerType con, FuncObj* objPtr, const dtUtil::RefString& groupName)
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

         mPropCon.AddProperty(new typename dtDAL::TypeToActorProperty<PropType>::value_type(name, label,
            typename dtDAL::TypeToActorProperty<PropType>::SetFuncType(mFuncObj, setter),
            typename dtDAL::TypeToActorProperty<PropType>::GetFuncType(mFuncObj, getter),
            desc, mGroup));
      }

      template <typename SetPtr, typename GetPtr>
      void RegisterResourceProperty(DataType& resourceType, SetPtr setter, GetPtr getter,
               const dtUtil::RefString& name, const dtUtil::RefString& label, const dtUtil::RefString& desc)
      {
         typedef typename dtUtil::FunTraits<GetPtr>::ResultType ResType;
         typedef typename dtUtil::TypeTraits<ResType>::value_type PropType;

         mPropCon.AddProperty(new dtDAL::ResourceActorProperty(resourceType, name, label,
            typename dtDAL::TypeToActorProperty<PropType>::SetFuncType(mFuncObj, setter),
            typename dtDAL::TypeToActorProperty<PropType>::GetFuncType(mFuncObj, getter),
            desc, mGroup));
      }

      template <typename SetPtr, typename GetPtr>
      void RegisterActorIdProperty(const dtUtil::RefString& baseClass, SetPtr setter, GetPtr getter,
               const dtUtil::RefString& name, const dtUtil::RefString& label, const dtUtil::RefString& desc)
      {
         typedef typename dtUtil::FunTraits<GetPtr>::ResultType ResType;
         typedef typename dtUtil::TypeTraits<ResType>::value_type PropType;

         mPropCon.AddProperty(new dtDAL::ActorIDActorProperty(name, label,
            typename dtDAL::TypeToActorProperty<PropType>::SetFuncType(mFuncObj, setter),
            typename dtDAL::TypeToActorProperty<PropType>::GetFuncType(mFuncObj, getter),
            baseClass, desc, mGroup));
      }

      ContainerType mPropCon;
      FuncObj* mFuncObj;

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
#PropertyName, PropertyLabel, DESC_ ## PropertyName);\

#define DT_REGISTER_RESOURCE_PROPERTY_WITH_NAME(DataType, PropertyName, PropertyStringName, PropertyLabel, PropertyDesc, RegHelperType_, RegHelperInstance) \
   static const dtUtil::RefString DESC_ ## PropertyName (PropertyDesc);\
   RegHelperInstance.RegisterResourceProperty(DataType,\
   DT_CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   DT_CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
PropertyStringName, PropertyLabel, DESC_ ## PropertyName);\

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

}//namespace dtDAL

#endif //DELTA_PROPERTYMACROS_H
