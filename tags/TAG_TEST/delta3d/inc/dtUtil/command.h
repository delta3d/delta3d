/*
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
 * @author John K. Grant
*/

#ifndef _DTUTIL_COMMAND_H_
#define _DTUTIL_COMMAND_H_

#include <osg/Referenced>    // for base class
#include <dtUtil/functor.h>
#include <dtUtil/generic.h>
#include <dtUtil/typetraits.h>

namespace dtUtil
{

   namespace details
   {
      template<typename U>
        struct TypeTraits
      {
      private:
         template<typename T>
         struct _Traits
         {
            typedef T NonConstNoRef;
            typedef T& NonConstRef;
            typedef const T& ConstRef;
         };

        //      template<typename T>
        //      struct _Traits<T*>
        //      {
        //         typedef *T NonConstNoRef;
        //         typedef *T NonConstRef;
        //         typedef const *T ConstRef;
        //      };

   /*       template<typename T> */
   /*       struct _Traits<const T*> */
   /*       { */
   /*          typedef *T NonConstNoRef; */
   /*          typedef *T NonConstRef; */
   /*          typedef const *T ConstRef; */
   /*       }; */

         template<typename T>
         struct _Traits<const T&>
         {
            typedef T NonConstNoRef;
            typedef T& NonConstRef;
            typedef const T& ConstRef;
         };

         template<typename T>
         struct _Traits<T&>
         {
            typedef T NonConstNoRef;
            typedef T& NonConstRef;
            typedef const T& ConstRef;
         };

      public:
         typedef typename _Traits<U>::NonConstNoRef NonConstNoRef;
         typedef typename _Traits<U>::NonConstRef NonConstRef;
         typedef typename _Traits<U>::ConstRef ConstRef;
      };
   }

   /** An abstract class for all types which
     * provide a uniform interface for executing
     * Functors.  These classes will hold the
     * client-defined arguments.  This makes it
     * easy to have a container of Commands which
     * can be executed at a chosen time.  This has
     * been proven to be useful when catching GUI
     * events and processing them during the System's
     * preframe so that modifications to the graphics
     * thread can be contolled.
     *
     * The template parameter should be as follows:
     * - RetT The return type for the interface class AND function signature.
     */
   template<typename RetT>
   class Command : public osg::Referenced
   {
   public:
      typedef RetT ReturnType;
      Command() : osg::Referenced() {}

      virtual ReturnType operator ()() = 0;

   protected:
      virtual ~Command() {}
   };

   /** A Command which does not need arguments and has no return value.
     * The template parameter should be as follows:
     * - RetT The return type for the interface class AND function signature.
     */
   template<typename RetT>
   class Command0 : public Command<RetT>
   {
   public:
      typedef dtUtil::Functor<RetT,TYPELIST_0()> FunctorType;

      Command0(const FunctorType& f) : Command<RetT>(),
         mFunctor( f )
      {
      }

      /*virtual*/ RetT operator ()()
      {
         return mFunctor();
      }

   protected:
      /*virtual*/ ~Command0()
      {
      }

   private:
      FunctorType mFunctor;
   };

   /** A Command which uses one argument and has no return value.
     * The template parameters shoudl be as follows:
     * - RetT The return type for the interface class AND function signature.
     * - ArgTMember1 The type used to store the first parameter.
     * \todo Use Loki::TypeTraits to eliminate the need for the ArgTMember template parameter.
     */
   template<typename RetT, typename ArgT>
   class Command1 : public Command<RetT>
   {
   public:
      typedef typename details::TypeTraits<ArgT>::NonConstNoRef MemberType;
      typedef TYPELIST_1(ArgT) Types;
      typedef dtUtil::Functor<RetT,Types> FunctorType;
      typedef typename FunctorType::ParmsListType Params;
      typedef MemberType Param0;

      Command1(const FunctorType& f,Param0 arg) : Command<RetT>(),
         mFunctor( f ),
         mArg(arg)
      {
      }

      void SetArg1(typename TypeTraits<MemberType>::param_type arg)
      {
         mArg = arg;
      }

      typename TypeTraits<MemberType>::return_type GetArg1() const
      {
         return mArg;
      }

      /*virtual*/ RetT operator ()()
      {
         return mFunctor( mArg );
      }

   protected:
      /*virtual*/ ~Command1()
      {
      }

   private:
      FunctorType mFunctor;
      MemberType mArg;
   };

   /** A Command which uses two arguments and has no return value.
     * The template parameters shoudl be as follows:
     * - RetT The return type for the interface class AND function signature.
     * - ArgTMember1 The type used to store the first parameter.
     * - ArgTMember2 The type used to store the second parameter.
     */
   template<typename RetT,typename ArgTMember1,typename ArgTMember2>
   class Command2 : public Command<RetT>
   {
   public:
      typedef typename details::TypeTraits<ArgTMember1>::NonConstNoRef MemberType1;
      typedef typename details::TypeTraits<ArgTMember2>::NonConstNoRef MemberType2;
      typedef TYPELIST_2(MemberType1,MemberType2) Types;
      typedef dtUtil::Functor<RetT,Types> FunctorType;
      typedef typename dtUtil::CallParms<Types>::ParmsListType Params;

      ///\todo make these typedefs work!  they are needed for the argument types in the ctor
      //typedef dtUtil::TypeAt<Params::Head,0>::Result Param0;
      //typedef dtUtil::TypeAt<Params,1>::Result Param1;

      ///\todo take this out when the above is fixed
      typedef MemberType1 Param0;
      typedef MemberType2 Param1;

      Command2(const FunctorType& f, Param0 arg1, Param1 arg2) : Command<RetT>(),
         mFunctor( f ),
         mArg1(arg1),
         mArg2(arg2)
      {
      }

      void SetArg1(typename TypeTraits<MemberType1>::param_type arg)
      {
         mArg1 = arg;
      }

      typename TypeTraits<MemberType1>::return_type GetArg1() const
      {
         return mArg1;
      }

      void SetArg2(typename TypeTraits<MemberType2>::param_type arg)
      {
         mArg2 = arg;
      }

      typename TypeTraits<MemberType2>::return_type GetArg2() const
      {
         return mArg2;
      }

      /*virtual*/ RetT operator ()()
      {
         return mFunctor( mArg1 , mArg2 );
      }

   private:
      FunctorType mFunctor;
      MemberType1 mArg1;
      MemberType2 mArg2;
   };
}

#endif  // _DTUTIL_COMMAND_H_
