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

#ifndef DELTA_TYPETOACTORPROPERTY_H
#define DELTA_TYPETOACTORPROPERTY_H

#include <dtCore/actoridactorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/doubleactorproperty.h>
#include <dtCore/enumactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/gameeventactorproperty.h>
#include <dtCore/groupactorproperty.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/longactorproperty.h>
#include <dtCore/namedactorparameter.h>
#include <dtCore/namedbooleanparameter.h>
#include <dtCore/namedcontainerparameter.h>
#include <dtCore/nameddoubleparameter.h>
#include <dtCore/namedenumparameter.h>
#include <dtCore/namedfloatparameter.h>
#include <dtCore/namedgameeventparameter.h>
#include <dtCore/namedgroupparameter.h>
#include <dtCore/namedintparameter.h>
#include <dtCore/namedunsignedintparameter.h>
#include <dtCore/namedshortintparameter.h>
#include <dtCore/namedunsignedshortintparameter.h>
#include <dtCore/namedlongintparameter.h>
#include <dtCore/namedunsignedlongintparameter.h>
#include <dtCore/namedunsignedcharparameter.h>
#include <dtCore/namedlongintparameter.h>
#include <dtCore/namedresourceparameter.h>
#include <dtCore/namedvectorparameters.h>
#include <dtCore/propertycontainer.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/vectoractorproperties.h>

#include <dtUtil/command.h>
#include <dtUtil/functor.h>
#include <dtUtil/typemanip.h>

///////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

namespace dtCore
{

   template <typename _Type>
   struct TypeToActorProperty
   {
   private:
      //if no specializations are used we will just do everything by copy
      template <bool isEnumSubclass, class U, typename T = _Type>
      struct _TypeToActorProperty_
      {
         typedef dtCore::ActorProperty value_type;
         typedef dtCore::NamedParameter named_parameter_type;
         typedef U* GetValueType;
         typedef U* SetValueType;

         typedef dtUtil::Functor<GetValueType, TYPELIST_0()> GetFuncType;
         typedef dtUtil::Functor<void, TYPELIST_1(SetValueType)> SetFuncType;
      };

      template <class U, typename T>
      struct _TypeToActorProperty_<true, U, T>
      {
         typedef dtCore::EnumActorProperty<U> value_type;
         typedef dtCore::NamedEnumParameter named_parameter_type;

         typedef U& GetValueType;
         typedef U& SetValueType;

         typedef typename value_type::GetFuncType GetFuncType;
         typedef typename value_type::SetFuncType SetFuncType;
      };
      //TODO- ActorActor (DEPRECATED ANYWAY), and ColorRGBA (Which uses a vec4, so there is no way to distinguish it)

      template <typename T>
      struct _TypeToActorProperty_<false, bool, T>
      {
         typedef dtCore::BooleanActorProperty value_type;
         typedef dtCore::NamedBooleanParameter named_parameter_type;

         typedef bool GetValueType;
         typedef bool SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, GameEvent, T>
      {
         typedef dtCore::GameEventActorProperty value_type;
         typedef dtCore::NamedGameEventParameter named_parameter_type;

         typedef GameEvent* GetValueType;
         typedef GameEvent* SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, ResourceDescriptor, T>
      {
         typedef dtCore::ResourceActorProperty value_type;
         typedef dtCore::NamedResourceParameter named_parameter_type;

         typedef const ResourceDescriptor& GetValueType;
         typedef const ResourceDescriptor& SetValueType;

         typedef value_type::GetDescFuncType GetFuncType;
         typedef value_type::SetDescFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, unsigned char, T>
      {
         typedef dtCore::IntActorProperty value_type;
         typedef dtCore::NamedUnsignedCharParameter named_parameter_type;

         typedef unsigned char GetValueType;
         typedef unsigned char SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, char, T>
      {
         typedef dtCore::IntActorProperty value_type;
         typedef dtCore::NamedUnsignedCharParameter named_parameter_type;

         typedef char GetValueType;
         typedef char SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, unsigned short, T>
      {
         typedef dtCore::IntActorProperty value_type;
         typedef dtCore::NamedUnsignedShortIntParameter named_parameter_type;

         typedef unsigned short GetValueType;
         typedef unsigned short SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, short, T>
      {
         typedef dtCore::IntActorProperty value_type;
         typedef dtCore::NamedShortIntParameter named_parameter_type;

         typedef short GetValueType;
         typedef short SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, unsigned int, T>
      {
         typedef dtCore::IntActorProperty value_type;
         typedef dtCore::NamedUnsignedIntParameter named_parameter_type;

         typedef unsigned int GetValueType;
         typedef unsigned int SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, int, T>
      {
         typedef dtCore::IntActorProperty value_type;
         typedef dtCore::NamedIntParameter named_parameter_type;

         typedef int GetValueType;
         typedef int SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, long, T>
      {
         typedef dtCore::LongActorProperty value_type;
         typedef dtCore::NamedLongIntParameter named_parameter_type;

         typedef long GetValueType;
         typedef long SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, unsigned long, T>
      {
         typedef dtCore::LongActorProperty value_type;
         typedef dtCore::NamedUnsignedLongIntParameter named_parameter_type;

         typedef unsigned long GetValueType;
         typedef unsigned long SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, float, T>
      {
         typedef dtCore::FloatActorProperty value_type;
         typedef dtCore::NamedFloatParameter named_parameter_type;

         typedef float GetValueType;
         typedef float SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, double, T>
      {
         typedef dtCore::DoubleActorProperty value_type;
         typedef dtCore::NamedDoubleParameter named_parameter_type;

         typedef double GetValueType;
         typedef double SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, std::string, T>
      {
         typedef dtCore::StringActorProperty value_type;
         typedef dtCore::NamedStringParameter named_parameter_type;

         typedef std::string GetValueType;
         typedef const std::string& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, osg::Vec2f, T>
      {
         typedef dtCore::Vec2ActorProperty value_type;
         typedef dtCore::NamedVec2Parameter named_parameter_type;

         typedef osg::Vec2f GetValueType;
         typedef const osg::Vec2f& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, osg::Vec2d, T>
      {
         typedef dtCore::Vec2dActorProperty value_type;
         typedef dtCore::NamedVec2dParameter named_parameter_type;

         typedef osg::Vec2d GetValueType;
         typedef const osg::Vec2d& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, osg::Vec3f, T>
      {
         typedef dtCore::Vec3fActorProperty value_type;
         typedef dtCore::NamedVec3fParameter named_parameter_type;

         typedef osg::Vec3f GetValueType;
         typedef const osg::Vec3f& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, osg::Vec3d, T>
      {
         typedef dtCore::Vec3dActorProperty value_type;
         typedef dtCore::NamedVec3dParameter named_parameter_type;

         typedef osg::Vec3d GetValueType;
         typedef const osg::Vec3d& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, osg::Vec4f, T>
      {
         typedef dtCore::Vec4fActorProperty value_type;
         typedef dtCore::NamedVec4fParameter named_parameter_type;

         typedef osg::Vec4f GetValueType;
         typedef const osg::Vec4f& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, osg::Vec4d, T>
      {
         typedef dtCore::Vec4dActorProperty value_type;
         typedef dtCore::NamedVec4dParameter named_parameter_type;

         typedef osg::Vec4d GetValueType;
         typedef const osg::Vec4d& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, dtCore::UniqueId, T>
      {
         typedef dtCore::ActorIDActorProperty value_type;
         typedef dtCore::NamedActorParameter named_parameter_type;

         typedef dtCore::UniqueId GetValueType;
         typedef const dtCore::UniqueId& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<false, dtCore::NamedGroupParameter, T>
      {
         typedef dtCore::GroupActorProperty value_type;
         typedef dtCore::NamedGroupParameter named_parameter_type;

         typedef dtCore::RefPtr<dtCore::NamedGroupParameter> GetValueType;
         typedef const dtCore::NamedGroupParameter& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      // Need this one because the getter return is used in property macros, which has the refptr
      template <typename T>
      struct _TypeToActorProperty_<false, dtCore::RefPtr<dtCore::NamedGroupParameter>, T>
      {
         typedef dtCore::GroupActorProperty value_type;
         typedef dtCore::NamedGroupParameter named_parameter_type;

         typedef dtCore::RefPtr<dtCore::NamedGroupParameter> GetValueType;
         typedef const dtCore::NamedGroupParameter& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };


   public:
      enum { isEnum = DTUTIL_SUPERSUBCLASS_STRICT(dtUtil::Enumeration, _Type) };
      typedef typename _TypeToActorProperty_<isEnum, _Type>::value_type value_type;
      typedef typename _TypeToActorProperty_<isEnum, _Type>::named_parameter_type named_parameter_type;

      typedef typename _TypeToActorProperty_<isEnum, _Type>::SetValueType SetValueType;
      typedef typename _TypeToActorProperty_<isEnum, _Type>::GetValueType GetValueType;

      typedef typename _TypeToActorProperty_<isEnum, _Type>::SetFuncType SetFuncType;
      typedef typename _TypeToActorProperty_<isEnum, _Type>::GetFuncType GetFuncType;
   };

}//namespace dtCore

#endif //DELTA_TYPETOACTORPROPERTY_H
