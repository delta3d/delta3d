/* -*-c++-*-
* testGameActorLibrary - testgameactor (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* William E. Johnson II
*/
#ifndef DELTA_EXAMPLE_GAME_ACTOR1
#define DELTA_EXAMPLE_GAME_ACTOR1

#include <dtGame/gameactor.h>
#include "export.h"

class DT_EXAMPLE_EXPORT TestGameActor1 : public dtGame::GameActor
{
   public:
      /// Constructor
      TestGameActor1(dtGame::GameActorProxy& proxy);

      /// Destructor
      virtual ~TestGameActor1();

      void FireOne(const dtGame::Message& message);
      void Reset(const dtGame::Message& message);

      /**
       * @see GameActorProxy#TickLocal
       */
      virtual void OnTickLocal(const dtGame::TickMessage& tickMessage);

      /**
       * @see GameActorProxy#TickRemote
       */
      virtual void OnTickRemote(const dtGame::TickMessage& tickMessage);

      bool OneIsFired() const { return fired; }
      void SetOneIsFired(bool newValue) { fired = newValue; }

      int GetTickLocals() const { return tickLocals; }
      void SetTickLocals(int newTickLocals) { tickLocals = newTickLocals; }

      int GetTickRemotes() const { return tickRemotes; }
      void SetTickRemotes(int newTickRemotes) { tickRemotes = newTickRemotes; }
   private:
      bool fired;
      int tickLocals;
      int tickRemotes;
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

