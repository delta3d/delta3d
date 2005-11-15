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
 * @author David Guthrie
 */

#ifndef DELTA_INVOKABLE
#define DELTA_INVOKABLE

#include <string>
#include <osg/Referenced>
#include <dtDAL/functor.h>
#include "dtGame/message.h"

namespace dtGame
{
   /**
    * @class Invokable
    * @brief An Invokable is a queriable method interface that can be added to a dtGame::GameActorProxy just
    * as a dtDAL::ActorProperty represents a data value on the actor.  
    * Invoking the invokable requires a dtGame::Message as a parameter.  The actual code is executed by calling
    * a Functor that takes 1 parameter, the message, and returns nothing.  The method must take a const Method reference. 
    * @see dtGame::Message
    */
   class DT_GAME_EXPORT Invokable : public osg::Referenced
   {
      public:
      	Invokable(const std::string& name, dtDAL::Functor1<const Message&> toInvoke);
         
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
         ///refenceced classes should always have protected desctructor
         virtual ~Invokable();
      private:
         std::string mName;
         ///functor taking one parameter and optionally returning a value.
         dtDAL::Functor1<const Message&> mMethodToInvoke;

         Invokable(const Invokable& toCopy) {}
         Invokable& operator=(const Invokable& toAssign) { return *this; }         
   };
   
}

#endif /*DELTA_INVOKABLE*/
