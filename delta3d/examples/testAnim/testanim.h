/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * Bradley Anderegg
 */
#ifndef DELTA_TEST_ANIM_ENTRY_POINT
#define DELTA_TEST_ANIM_ENTRY_POINT

#include <dtGame/gameentrypoint.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include "export.h"

// Foward declarations
namespace dtCore
{
   class FlyMotionModel;
   class Camera;
}

namespace dtGame
{
   class GameManager;
}

namespace dtAnim
{
   class AnimationHelper;
   class AnimationComponent;
}

namespace dtActors
{
   class AnimationGameActorProxy2;
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

      void InitializeAnimationActor(dtActors::AnimationGameActorProxy2* actor,
                                    dtAnim::AnimationComponent* animComp, bool isPlayer,
                                    dtCore::Camera *camera);

      dtCore::RefPtr<dtAnim::AnimationHelper> mAnimationHelper;
      dtCore::RefPtr<dtCore::FlyMotionModel> mFMM;
      bool mPerformanceTest;
};

#endif
