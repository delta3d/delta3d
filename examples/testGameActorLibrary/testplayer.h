/* -*-c++-*-
* testGameActorLibrary - testplayer (.h & .cpp) - Using 'The MIT License'
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
* Matthew W. Campbell
*/
#ifndef DELTA_TESTPLAYER
#define DELTA_TESTPLAYER

#include <osg/Vec3>
#include <string>
#include <dtGame/gameactor.h>
#include <dtCore/observerptr.h>
#include <dtCore/isector.h>
#include "export.h"

//namespace dtCore
//{
//   class Isector;
//}

class DT_EXAMPLE_EXPORT TestPlayer : public dtGame::GameActor
{
   public:

      TestPlayer(dtGame::GameActorProxy& parent);

      void SetModel(const std::string &fileName);
      std::string GetModel() const { return mModelFile; }

      void SetVelocity(float velocity);
      float GetVelocity() const;

      void SetTurnRate(float rate);
      float GetTurnRate() const;

      virtual void OnTickLocal(const dtGame::TickMessage& tickMessage);
      virtual void OnTickRemote(const dtGame::TickMessage& tickMessage);

      void HandleTick(double deltaSimTime, bool forceGroundClamp=false);

   protected:
      virtual ~TestPlayer();

   private:
      float mVelocity,mTurnRate;
      std::string mModelFile;
      dtCore::ObserverPtr<dtCore::Isector> mIsector;
};

class DT_EXAMPLE_EXPORT TestPlayerProxy : public dtGame::GameActorProxy
{
   public:
      TestPlayerProxy();

      virtual void BuildPropertyMap();
      virtual void BuildInvokables();

   protected:
      virtual ~TestPlayerProxy();
      virtual void CreateDrawable();
      virtual void OnEnteredWorld();

   private:
      bool mEnteredBefore;

};

#endif
