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
 * @author William E. Johnson II
 */
#ifndef DELTA_EXAMPLE_GAME_ACTOR1
#define DELTA_EXAMPLE_GAME_ACTOR1

#include <dtGame/gameactor.h>
#include "export.h"

class DT_EXAMPLE_EXPORT TestGameActor1 : public dtGame::GameActor
{
   public:
      /// Constructor
      TestGameActor1();

      /// Destructor
      virtual ~TestGameActor1();

      void FireOne(const dtGame::Message& message);
      void Reset(const dtGame::Message& message);

      /**
       * @see GameActorProxy#TickLocal
       */
      virtual void TickLocal(const dtGame::Message& tickMessage);
      
      /**
       * @see GameActorProxy#TickRemote
       */
      virtual void TickRemote(const dtGame::Message& tickMessage);
      
      bool OneIsFired() const { return fired; }
      void SetOneIsFired(bool newValue) { fired = newValue; }
   private:
      bool fired;
      unsigned tickLocals;
      unsigned tickRemotes;
};

class DT_EXAMPLE_EXPORT TestGameActorProxy1 : public dtGame::GameActorProxy
{
   public:

      /// Constructor
      TestGameActorProxy1();

      /// Destructor
      virtual ~TestGameActorProxy1();

      /**
       * Builds the properties associated with this proxy's actor
       */
      virtual void BuildPropertyMap();

      /**
       * Builds the invokable associated with this proxy.
       */
      virtual void BuildInvokables();
      
      void ToggleTicks(const dtGame::Message& message);
       
   protected:
      virtual void CreateActor();
   private:
      bool ticksEnabled;
};

#endif 

