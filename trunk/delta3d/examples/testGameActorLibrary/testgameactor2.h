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
#ifndef DELTA_EXAMPLE_GAME_ACTOR2
#define DELTA_EXAMPLE_GAME_ACTOR2

#include "export.h"
#include <dtGame/gameactor.h>

class DT_EXAMPLE_EXPORT TestGameActor2 : public dtGame::GameActor
{
   public:
   
      /// Constructor
      TestGameActor2(dtGame::GameActorProxy& proxy);
   
      /// Destructor
      virtual ~TestGameActor2();
      
      int GetActorDeletedCount() const { return mActorDeletedCount; }
      void SetActorDeletedCount(int newValue)  { mActorDeletedCount = newValue; } 

      int GetActorPublishedCount() const { return mActorPublishedCount; }
      void SetActorPublishedCount(int newValue)  { mActorPublishedCount = newValue; } 

      int GetMapLoadedCount() const { return mMapLoadedCount; }
      void SetMapLoadedCount(int newValue)  { mMapLoadedCount = newValue; } 

      // tests setting an invokable with a new name - does +1 on each message counter
      void LogMessage(const dtGame::Message& message);

      // tests the default processor - does +2 on each message counter
      void ProcessMessage(const dtGame::Message& message);

   private:
      int mActorDeletedCount;
      int mActorPublishedCount;
      int mMapLoadedCount;
};

class DT_EXAMPLE_EXPORT TestGameActorProxy2 : public dtGame::GameActorProxy
{
   public:
   
      /// Constructor
      TestGameActorProxy2();
   
      /// Destructor
      virtual ~TestGameActorProxy2();
   
      /**
      * Builds the properties associated with this proxy's actor
      */
      virtual void BuildPropertyMap();
   
      /**
      * Builds the invokable associated with this proxy.
      */
      virtual void BuildInvokables();
   protected:
      virtual void CreateActor();

};
#endif
