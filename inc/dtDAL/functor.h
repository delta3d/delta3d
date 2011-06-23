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
 * Matthew W. Campbell
 * David Guthrie
*/
#ifndef DELTA_FUNCTOR
#define DELTA_FUNCTOR

#include <dtUtil/deprecationmgr.h>
#include <dtUtil/functor.h>

namespace dtDAL 
{

   /*
    * The functors in here have been removed in favor of the dtUtil/functor.  These functions here
    * are for backward compatibility, but you should not use them in any new code.  For actor properties
    * look at the typedefs in each class.
    * <p>
    * MakeFunctor(*this, &Class::SetTheInt)
    * <p>
    * Changes to
    * <p>
    * IntActorProperty::SetFuncType(this, &Class::SetTheInt)
    * <p>
    * Doing it this will is more flexible.  You can have a method return by const & and will still
    * work as the getter for an actor property.
    * <p>
    * If you have used a member function that has a non-void return type as the setter call for 
    * an actor property, a la MakeFunctor(*this, &Class::SetStuff), it will not work properly without
    * some tweaking.  You'll have to do
    * 
    * IntActorProperty::SetFuncType(this, (void (Class::*)(int))(&Class::SetStuff));
    * 
    * It's ugly, but it works.
    */

   template <typename CallType> inline 
   DEPRECATE_FUNC dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>
   MakeFunctor(CallType fun)
   {
      return dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>(fun);
   }
   template <typename CallType, class PObj> inline 
   DEPRECATE_FUNC dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>
   MakeFunctor(PObj& pobj, CallType memfun)
   {
      return dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>(&pobj, memfun);
   }
   template <typename CallType, class Fun> inline 
   DEPRECATE_FUNC dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>
   MakeFunctor(Fun const& fun)
   {
      return dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>(fun);
   }

   template <typename CallType> inline 
   DEPRECATE_FUNC dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>
   MakeFunctorRet(CallType fun)
   {
      return dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>(fun);
   }

   template <typename CallType, class PObj> inline 
   dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType> 
   DEPRECATE_FUNC MakeFunctorRet(PObj& pobj, CallType memfun)
   {
      return dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>(&pobj, memfun);
   }

   template <typename CallType, class Fun> inline 
   DEPRECATE_FUNC dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>
   MakeFunctorRet(Fun const& fun)
   {
      return dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>(fun);
   }

}

#endif
