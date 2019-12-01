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
 * David Guthrie
 */

#ifndef DELTA_INVOKABLE
#define DELTA_INVOKABLE

#include <string>
#include <osg/Referenced>
#include <dtGame/message.h>
#include <dtUtil/functor.h>

namespace dtGame
{
   class InvokableFunctorCallerBase: public osg::Referenced
   {
   public:
      virtual void Call(const Message& message) = 0;
   };

   template <typename Message_T = Message>
   class InvokableFunctorCaller: public InvokableFunctorCallerBase
   {
   public:
      typedef dtUtil::Functor<void, TYPELIST_1(const Message_T&)> InvokableFunc;

      InvokableFunctorCaller(InvokableFunc func)
      : mFunctor(func)
      {
      }

      virtual void Call(const Message& message)
      {
         mFunctor(static_cast<const Message_T&>(message));
      }
   private:
       InvokableFunc mFunctor;
   };

   /**
    * @class Invokable
    * @brief An Invokable is a queriable method interface that can be added to a dtGame::GameActorProxy just
    * as a dtCore::ActorProperty represents a data value on the actor.
    * Invoking the invokable requires a dtGame::Message as a parameter.  The actual code is executed by calling
    * a Functor that takes 1 parameter, the message, and returns nothing.  The method must take a const Method reference.
    * @see dtGame::Message
    */
   class DT_GAME_EXPORT Invokable : public osg::Referenced
   {
      public:
         typedef dtUtil::Functor<void, TYPELIST_1(const Message&)> InvokableFunc;

         template<typename Message_T>
         Invokable(const std::string& name, dtUtil::Functor<void, TYPELIST_1(const Message_T&)> toInvoke)
         : mName(name)
         , mCaller(new InvokableFunctorCaller<Message_T>(toInvoke))
         {
         }

         /**
          * @return the name of this invokable.
          */
         const std::string& GetName() const { return mName; }

         /**
          * Invoke this.
          * @param message the message to invoke.
          */
         void Invoke(const Message& message);
      protected:
         ///referenced classes should always have protected destructor
         virtual ~Invokable();
      private:
         std::string mName;

         dtCore::RefPtr<InvokableFunctorCallerBase> mCaller;

         Invokable(const Invokable&) {}
         Invokable& operator=(const Invokable&) { return *this; }
   };

}

#endif /*DELTA_INVOKABLE*/
