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
 * Christopher DuBuc
 * William E. Johnson II
 */
#ifndef DELTA_TEST_AAR_ENTRY_POINT
#define DELTA_TEST_AAR_ENTRY_POINT

#include <dtGame/gameentrypoint.h>
#include "export.h"

// Foward declarations
namespace dtCore
{
   class MotionModel;
}

namespace dtGame
{
   class GameManager;
}

namespace dtLMS
{
   class LmsComponent;
}

class TEST_AAR_EXPORT TestAAR : public dtGame::GameEntryPoint
{

   public:

      /**
       * Constructs testAARLms.
       */
      TestAAR();

      /**
       * Destructs testAARLms, and disconnects from LMS if connected.
       */
      virtual ~TestAAR();

      /**
       * Called to initialize the game application.  You can pull any command line params here.
       */
      virtual void Initialize(dtGame::GameApplication& app, int argc, char **argv);

      /**
       * Create your game manager.
       */
      virtual dtCore::RefPtr<dtGame::GameManager> CreateGameManager(dtCore::Scene& scene);

      /**
       * Called just before your application's game loop starts.  This is your main 
       * opportunity to create components, load maps, create unique actors, etc...
       */
      virtual void OnStartup(dtGame::GameManager &gameManager);

   private:

      /**
       * Helper method to parse command line options
       * @note This method will parse command line options 
       * and set values as necessary. For instance, it will
       * set the data path of the application
       */
      void ParseCommandLineOptions(int argc, char **argv) const;

      dtCore::RefPtr<dtCore::MotionModel> mFMM;
      dtCore::RefPtr<dtLMS::LmsComponent> mLmsComponent;
      mutable bool mUseLMS;
};

#endif
