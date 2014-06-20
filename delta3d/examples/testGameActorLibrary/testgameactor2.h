/* -*-c++-*-
* testGameActorLibrary - testgameactor2 (.h & .cpp) - Using 'The MIT License'
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
#ifndef DELTA_EXAMPLE_GAME_ACTOR2
#define DELTA_EXAMPLE_GAME_ACTOR2

#include "export.h"
#include <dtGame/gameactor.h>

class DT_EXAMPLE_EXPORT TestGameActor2 : public dtGame::GameActor
{
   public:
   
      /// Constructor
      TestGameActor2(dtGame::GameActorProxy& parent);
   
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
      virtual void CreateDrawable();

};
#endif
