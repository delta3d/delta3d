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

#include <dtDAL/typetoactorproperty.h>
#include <dtDAL/propertycontainer.h>
#include <dtDAL/containeractorproperty.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/command.h>
#include <dtUtil/functor.h>

///////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

namespace dtDAL
{

   template <class ContainerType, class FuncObj>
   struct PropertyRegHelper
   {
      typedef PropertyRegHelper<ContainerType, FuncObj> Type;
      typedef FuncObj FunctorObjectType;
      typedef ContainerType ContainerObjectType;

      typedef dtDAL::Vec3ActorProperty Vec3;

      PropertyRegHelper(ContainerType con, FuncObj* objPtr, const std::string& str)
         : mPropCon(con)
         , mFuncObj(objPtr)
         , mGroup(str)
      {}

      template <class PropType, typename SetPtr, typename GetPtr>
      void RegisterProperty(PropType prop, SetPtr setter, GetPtr getter, const std::string& name, const std::string& desc)
      {

         mPropCon.AddProperty(new typename dtDAL::TypeToActorProperty<PropType>::value_type(name, name,
            typename dtDAL::TypeToActorProperty<PropType>::SetFuncType(mFuncObj, setter),
            typename dtDAL::TypeToActorProperty<PropType>::GetFuncType(mFuncObj, getter),
            desc, mGroup));
      }

      ContainerType mPropCon;
      FuncObj* mFuncObj;

      const std::string& mGroup;
   };

   #define DECLARE_PROPERTY_INLINE(PropertyType, PropertyName) \
   private:\
      PropertyType  m ## PropertyName; \
   public: \
      \
      void Set ## PropertyName(dtDAL::TypeToActorProperty<PropertyType>::SetValueType value)\
   {\
      m ## PropertyName = value; \
   };\
      \
      dtDAL::TypeToActorProperty<PropertyType>::GetValueType Get ## PropertyName() const\
   {\
      return m ## PropertyName;\
   };\
      \

   #define DECLARE_PROPERTY(PropertyType, PropertyName) \
   private:\
      PropertyType  m ## PropertyName; \
   public: \
      \
      void Set ## PropertyName(dtDAL::TypeToActorProperty<PropertyType>::SetValueType value);\
      \
      dtDAL::TypeToActorProperty<PropertyType>::GetValueType Get ## PropertyName() const;\
      \

   #define IMPLEMENT_PROPERTY_GETTER(ClassName, PropertyType, PropertyName) \
      dtDAL::TypeToActorProperty<PropertyType>::GetValueType ClassName::Get ## PropertyName() const\
      {\
         return m ## PropertyName;\
      }\
      \

   #define IMPLEMENT_PROPERTY_SETTER(ClassName, PropertyType, PropertyName) \
      void ClassName::Set ## PropertyName(dtDAL::TypeToActorProperty<PropertyType>::SetValueType value)\
      {\
         m ## PropertyName = value;\
      }\
      \

   #define IMPLEMENT_PROPERTY(ClassName, PropertyType, PropertyName) \
      IMPLEMENT_PROPERTY_SETTER(ClassName, PropertyType, PropertyName)\
      \
      IMPLEMENT_PROPERTY_GETTER(ClassName, PropertyType, PropertyName)\
      \


   #define CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName)\
      &RegHelperType_::FunctorObjectType::Get ## PropertyName\

   #define CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName)\
      &RegHelperType_::FunctorObjectType::Set ## PropertyName\


   #define REGISTER_PROPERTY(PropertyName, PropertyDesc, RegHelperType_, RegHelperInstance) \
      RegHelperInstance.RegisterProperty(m ## PropertyName, \
      CREATE_PROPERTY_SETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
      CREATE_PROPERTY_GETTER_HELPER_MACRO(RegHelperType_, PropertyName), \
   #PropertyName, #PropertyDesc);\

}//namespace dtDAL

#endif //DELTA_PROPERTYMACROS_H
