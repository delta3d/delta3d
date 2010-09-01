/* -*-c++-*-
* testAnim - testanim (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2007-2008, Alion Science and Technology Corporation
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
* Bradley Anderegg
*/
#ifndef DELTA_TEST_ANIM_ENTRY_POINT
#define DELTA_TEST_ANIM_ENTRY_POINT

#include <dtGame/gameentrypoint.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtCore/tripod.h>
#include "export.h"
#include "testaniminput.h"

// Foward declarations
namespace dtCore
{
   class FlyMotionModel;
   class Camera;
}

namespace dtGame
{
   class GameManager;
   class DefaultMessageProcessor;
}

namespace dtAnim
{
   class AnimationHelper;
   class AnimationComponent;
   class AnimationGameActorProxy;
}


class TEST_ANIM_EXPORT TestAnim : public dtGame::GameEntryPoint
{

   public:

      TestAnim();
      virtual ~TestAnim();

      /**
       * Called to initialize the game application.  You can pull any command line params here.
       */
      virtual void Initialize(dtGame::GameApplication& app, int argc, char **argv);

      /**
       * Called just before your application's game loop starts.  This is your main 
       * opportunity to create components, load maps, create unique actors, etc...
       */
      virtual void OnStartup(dtGame::GameApplication& app);

      virtual void OnShutdown(dtGame::GameApplication& app);

   private:

      void InitializeAnimationActor(dtAnim::AnimationGameActorProxy* gameProxy,
                                    dtAnim::AnimationComponent* animComp, bool isPlayer,
                                    dtCore::Camera *camera);

      void CreateAdditionalWindows();

      dtCore::ObserverPtr<dtAnim::AnimationHelper> mAnimationHelper;
      dtCore::RefPtr<dtCore::FlyMotionModel> mFMM;
      bool mPerformanceTest;

      dtCore::RefPtr<dtGame::DefaultMessageProcessor> mMessageProcComponent;
      dtCore::RefPtr<dtAnim::AnimationComponent> mAnimationComponent;
      dtCore::RefPtr<TestAnimInput> mInputComponent;

      dtCore::RefPtr<dtCore::Tripod> mTripod;
};

#endif
