/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2009 Alion Science and Technology
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
* Bradley Anderegg
*
*/

#ifndef DELTA_TYPETRAITS_H
#define DELTA_TYPETRAITS_H

#include <dtUtil/enumeration.h>
#include <dtCore/refptr.h>

namespace dtUtil
{

   //////////////////////////////////////////////////////////////////////////
   //IsPointer
   //////////////////////////////////////////////////////////////////////////   

   template <typename _Type>
   struct IsPointer
   {
   private:
      template <class U>
      struct _IsPointer_
      {
         enum { IS_POINTER = 0};
      };

      template <class U>
      struct _IsPointer_<U*>
      {
         enum {IS_POINTER = 1};
      };

   public:
      enum {IS_POINTER = _IsPointer_<_Type>::IS_POINTER};

   };

   //////////////////////////////////////////////////////////////////////////
   //IsReference
   //////////////////////////////////////////////////////////////////////////   

   template <typename _Type>
   struct IsReference
   {
   private:
      template <class U>
      struct _IsReference_
      {
         enum { IS_REFERENCE = 0};
      };

      template <class U>
      struct _IsReference_<U&>
      {
         enum {IS_REFERENCE = 1};
      };

   public:
      enum {IS_REFERENCE = _IsReference_<_Type>::IS_REFERENCE};

   };
   

   //////////////////////////////////////////////////////////////////////////
   //IsConst
   //////////////////////////////////////////////////////////////////////////   

   template <typename _Type>
   struct IsConst
   {
   private:
      template <class U>
      struct _IsConst_
      {
         enum { IS_CONST = 0};
      };

      template <class U>
      struct _IsConst_<const U>
      {
         enum {IS_CONST = 1};
      };

   public:
      enum {IS_CONST = _IsConst_<_Type>::IS_CONST};

   };

   //////////////////////////////////////////////////////////////////////////
   //Type Traits
   //////////////////////////////////////////////////////////////////////////
   template <typename _Type>
   struct TypeTraits
   {
   private:
      template <class U, class T = _Type>
      struct _TypeTraits_
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef const U& return_type;
         typedef const U& param_type;
      };

      template <class U, class T>
      struct _TypeTraits_<osg::ref_ptr<U>, T>
      {
         typedef osg::ref_ptr<U> value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef U* return_type;
         typedef U* param_type;
      };

      template <class U, class T>
      struct _TypeTraits_<dtCore::RefPtr<U>, T>
      {
         typedef dtCore::RefPtr<U> value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef U* return_type;
         typedef U* param_type;
      };

      template <class U, class T>
      struct _TypeTraits_<EnumerationPointer<U>, T>
      {
         typedef EnumerationPointer<U> value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef U& return_type;
         typedef U& param_type;
      };

      template <class T>
      struct _TypeTraits_<bool, T>
      {
         typedef bool value_type;
         typedef bool* pointer_type;
         typedef bool& reference;
         typedef const bool& const_reference;

         typedef bool return_type;
         typedef bool param_type;
      };

      template <class T>
      struct _TypeTraits_<signed char, T>
      {
         typedef signed char value_type;
         typedef signed char* pointer_type;
         typedef signed char& reference;
         typedef const signed char& const_reference;

         typedef signed char return_type;
         typedef signed char param_type;
      };

      template <class T>
      struct _TypeTraits_<float, T>
      {
         typedef float value_type;
         typedef float* pointer_type;
         typedef float& reference;
         typedef const float& const_reference;

         typedef float return_type;
         typedef float param_type;
      };

      template <class T>
      struct _TypeTraits_<double, T>
      {
         typedef double value_type;
         typedef double* pointer_type;
         typedef double& reference;
         typedef const double& const_reference;

         typedef double return_type;
         typedef double param_type;
      };

      template <class T>
      struct _TypeTraits_<int, T>
      {
         typedef int value_type;
         typedef int* pointer_type;
         typedef int& reference;
         typedef const int& const_reference;

         typedef int return_type;
         typedef int param_type;
      };

      template <class T>
      struct _TypeTraits_<short, T>
      {
         typedef short value_type;
         typedef short* pointer_type;
         typedef short& reference;
         typedef const short& const_reference;

         typedef short return_type;
         typedef short param_type;
      };

      template <class T>
      struct _TypeTraits_<char, T>
      {
         typedef char value_type;
         typedef char* pointer_type;
         typedef char& reference;
         typedef const char& const_reference;

         typedef char return_type;
         typedef char param_type;
      };

      template <class T>
      struct _TypeTraits_<long, T>
      {
         typedef long value_type;
         typedef long* pointer_type;
         typedef long& reference;
         typedef const long& const_reference;

         typedef long return_type;
         typedef long param_type;
      };

      template <class T>
      struct _TypeTraits_<long long, T>
      {
         typedef long long value_type;
         typedef long long* pointer_type;
         typedef long long& reference;
         typedef const long long& const_reference;

         typedef long long return_type;
         typedef long long param_type;
      };

      template <class T>
      struct _TypeTraits_<unsigned, T>
      {
         typedef unsigned value_type;
         typedef unsigned* pointer_type;
         typedef unsigned& reference;
         typedef const unsigned& const_reference;

         typedef unsigned return_type;
         typedef unsigned param_type;
      };

      template <class T>
      struct _TypeTraits_<unsigned short, T>
      {
         typedef unsigned short value_type;
         typedef unsigned short* pointer_type;
         typedef unsigned short& reference;
         typedef const unsigned short& const_reference;

         typedef unsigned short return_type;
         typedef unsigned short param_type;
      };

      template <class T>
      struct _TypeTraits_<unsigned char, T>
      {
         typedef unsigned char value_type;
         typedef unsigned char* pointer_type;
         typedef unsigned char& reference;
         typedef const unsigned char& const_reference;

         typedef unsigned char return_type;
         typedef unsigned char param_type;
      };

      template <class T>
      struct _TypeTraits_<unsigned long, T>
      {
         typedef unsigned long value_type;
         typedef unsigned long* pointer_type;
         typedef unsigned long& reference;
         typedef const unsigned long& const_reference;

         typedef unsigned long return_type;
         typedef unsigned long param_type;
      };

      template <class T>
      struct _TypeTraits_<unsigned long long, T>
      {
         typedef unsigned long long value_type;
         typedef unsigned long long* pointer_type;
         typedef unsigned long long& reference;
         typedef const unsigned long long& const_reference;

         typedef unsigned long long return_type;
         typedef unsigned long long param_type;
      };

      template <class U, class T>
      struct _TypeTraits_<const U&, T>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef const U& return_type;
         typedef const U& param_type;
      };

      template <class U, class T>
      struct _TypeTraits_<U&, T>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef const U& return_type;
         typedef const U& param_type;
      };

      template <class U, class T>
      struct _TypeTraits_<const U*, T>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef const U* return_type;
         typedef const U* param_type;
      };

      template <class U, class T>
      struct _TypeTraits_<U* const, T>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef U& const_reference;

         typedef U* const return_type;
         typedef U* const param_type;
      };

      template <class U, class T>
      struct _TypeTraits_<const U* const, T>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef const U* return_type;
         typedef const U* param_type;
      };


      template <class U, class T>
      struct _TypeTraits_<U*, T>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef U* return_type;
         typedef U* param_type;
      };

   public:
      typedef typename _TypeTraits_<_Type>::value_type value_type;
      typedef typename _TypeTraits_<_Type>::pointer_type pointer_type;
      typedef typename _TypeTraits_<_Type>::reference reference;
      typedef typename _TypeTraits_<_Type>::const_reference const_reference;

      typedef typename _TypeTraits_<_Type>::param_type const_param_type;
      typedef typename _TypeTraits_<_Type>::param_type param_type;

      typedef typename _TypeTraits_<_Type>::return_type return_type;

   };



}//namespace dtUtil


#endif //__TYPETRAITS_H__

// ---------------------------------------------------------------------------------------------------------------------------------
// TypeTraits.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

