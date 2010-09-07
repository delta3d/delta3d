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

#ifndef MESSAGEMACROS_H_
#define MESSAGEMACROS_H_

#include <dtDAL/typetoactorproperty.h>

#define DECLARE_MESSAGE_BEGIN(CLS, BASE, EXPORT_MACRO) \
   class EXPORT_MACRO CLS : public BASE \
   { \
   public: \
      CLS();\
   protected: \
      virtual ~CLS() {} \
   public: \

#define DECLARE_PARAMETER_INLINE(Type, ParamName) \
      typedef dtDAL::TypeToActorProperty<Type>::named_parameter_type TYPE_##ParamName; \
      \
      void Set ## ParamName(dtUtil::TypeTraits<Type>::param_type value)\
      {\
         static const dtUtil::RefString NAME_TO_LOOKUP(#ParamName); \
         return static_cast<TYPE_##ParamName*>(GetParameter(NAME_TO_LOOKUP))->SetValue(value);\
      };\
      \
      dtUtil::TypeTraits<Type>::return_type Get ## ParamName() const\
      {\
         static const dtUtil::RefString NAME_TO_LOOKUP(#ParamName); \
         return static_cast<const TYPE_##ParamName*>(GetParameter(NAME_TO_LOOKUP))->GetValue();\
      };\

#define DECLARE_MESSAGE_END() };

#define IMPLEMENT_MESSAGE_BEGIN(CLS) \
         CLS::CLS() {

#define ADD_PARAMETER(Type, ParamName) \
   typedef dtDAL::TypeToActorProperty<Type>::named_parameter_type TYPE_##ParamName; \
   static const dtUtil::RefString PARAM_NAME_##ParamName(#ParamName); \
   AddParameter(new TYPE_##ParamName(PARAM_NAME_##ParamName));

#define ADD_PARAMETER_WITH_DEFAULT(Type, ParamName, Default) \
   typedef dtDAL::TypeToActorProperty<Type>::named_parameter_type TYPE_##ParamName; \
   static const dtUtil::RefString PARAM_NAME_##ParamName(#ParamName); \
   AddParameter(new TYPE_##ParamName(PARAM_NAME_##ParamName, Default));

#define IMPLEMENT_MESSAGE_END() };


#endif /* MESSAGEMACROS_H_ */
