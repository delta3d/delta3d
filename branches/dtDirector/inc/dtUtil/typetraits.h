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
      template <class U>
      struct _TypeTraits_
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef U const_return_type;
         typedef U return_type;    
      };

      template <class U>
      struct _TypeTraits_<const U&>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef const U& const_return_type;
         typedef U& return_type;    
      };

      template <class U>
      struct _TypeTraits_<U&>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef const U& const_return_type;
         typedef U& return_type;    
      };

      template <class U>
      struct _TypeTraits_<const U*>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef const U* const_return_type;
         typedef U* return_type;
      };

      template <class U>
      struct _TypeTraits_<U* const>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef U& const_reference;

         typedef const U* const const_return_type;
         typedef U* const return_type;
      };

      template <class U>
      struct _TypeTraits_<const U* const>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef const U* const const_return_type;
         typedef const U* const return_type;
      };


      template <class U>
      struct _TypeTraits_<U*>
      {
         typedef U value_type;
         typedef U* pointer_type;
         typedef U& reference;
         typedef const U& const_reference;

         typedef const U* const_return_type;
         typedef U* return_type;
      };

   public:
      typedef typename _TypeTraits_<_Type>::value_type value_type;
      typedef typename _TypeTraits_<_Type>::pointer_type pointer_type;
      typedef typename _TypeTraits_<_Type>::reference reference;
      typedef typename _TypeTraits_<_Type>::const_reference const_reference;


      typedef typename _TypeTraits_<_Type>::const_return_type const_param_type;
      typedef typename _TypeTraits_<_Type>::return_type param_type;

      typedef typename _TypeTraits_<_Type>::const_return_type const_return_type;
      typedef typename _TypeTraits_<_Type>::return_type return_type;

   };



}//namespace dtUtil


#endif //__TYPETRAITS_H__

// ---------------------------------------------------------------------------------------------------------------------------------
// TypeTraits.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

