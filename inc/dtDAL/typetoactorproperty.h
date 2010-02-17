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

   template <typename _Type>
   struct TypeToActorProperty
   {
   private:
      //if no specializations are used we will just do everything by copy
      template <class U, typename T = _Type>
      struct _TypeToActorProperty_
      {
         typedef U value_type;
         typedef value_type GetValueType;
         typedef value_type SetValueType;

         typedef dtUtil::Functor<GetValueType, TYPELIST_0()> GetFuncType;
         typedef dtUtil::Functor<void, TYPELIST_1(SetValueType)> SetFuncType;
      };

      //TODO- ActorActor, GameEvent, Resource, Enumeration, and ColorRGBA

      template <typename T>
      struct _TypeToActorProperty_<bool, T>
      {
         typedef dtDAL::BooleanActorProperty value_type;

         typedef bool GetValueType;
         typedef bool SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<int, T>
      {
         typedef dtDAL::IntActorProperty value_type;

         typedef int GetValueType;
         typedef int SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<long, T>
      {
         typedef dtDAL::LongActorProperty value_type;

         typedef long GetValueType;
         typedef long SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<float, T>
      {
         typedef dtDAL::FloatActorProperty value_type;

         typedef float GetValueType;
         typedef float SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<double, T>
      {
         typedef dtDAL::DoubleActorProperty value_type;

         typedef double GetValueType;
         typedef double SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<std::string, T>
      {
         typedef dtDAL::StringActorProperty value_type;

         typedef std::string GetValueType;
         typedef const std::string& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<osg::Vec2f, T>
      {
         typedef dtDAL::Vec2ActorProperty value_type;

         typedef const osg::Vec2f& GetValueType;
         typedef const osg::Vec2f& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<osg::Vec2d, T>
      {
         typedef dtDAL::Vec2dActorProperty value_type;

         typedef const osg::Vec2d& GetValueType;
         typedef const osg::Vec2d& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<osg::Vec3f, T>
      {
         typedef dtDAL::Vec3fActorProperty value_type;

         typedef const osg::Vec3f& GetValueType;
         typedef const osg::Vec3f& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<osg::Vec3d, T>
      {
         typedef dtDAL::Vec3dActorProperty value_type;

         typedef const osg::Vec3d& GetValueType;
         typedef const osg::Vec3d& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<osg::Vec4f, T>
      {
         typedef dtDAL::Vec4fActorProperty value_type;

         typedef const osg::Vec4f& GetValueType;
         typedef const osg::Vec4f& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<osg::Vec4d, T>
      {
         typedef dtDAL::Vec4dActorProperty value_type;

         typedef const osg::Vec4d& GetValueType;
         typedef const osg::Vec4d& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };

      template <typename T>
      struct _TypeToActorProperty_<dtCore::UniqueId, T>
      {
         typedef dtDAL::ActorIDActorProperty value_type;

         typedef const dtCore::UniqueId GetValueType;
         typedef const dtCore::UniqueId& SetValueType;

         typedef value_type::GetFuncType GetFuncType;
         typedef value_type::SetFuncType SetFuncType;
      };


   public:
      typedef typename _TypeToActorProperty_<_Type>::value_type value_type;

      typedef typename _TypeToActorProperty_<_Type>::GetValueType GetValueType;
      typedef typename _TypeToActorProperty_<_Type>::SetValueType SetValueType;

      typedef typename _TypeToActorProperty_<_Type>::SetFuncType SetFuncType;
      typedef typename _TypeToActorProperty_<_Type>::GetFuncType GetFuncType;
   };

}//namespace dtDAL

#endif //DELTA_TYPETOACTORPROPERTY_H
