/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * MG
 */

#ifndef TEST_DIRECTOR_ENTRY_POINT
#define TEST_DIRECTOR_ENTRY_POINT

#include <dtGame/gameentrypoint.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>

#include <vector>

#include "testdirectorexport.h"

////////////////////////////////////////////////////////////////////////////////

namespace dtCore { class MotionModel; }
namespace dtGame { class GameManager; }

////////////////////////////////////////////////////////////////////////////////
class TEST_DIRECTOR_EXPORT TestDirector : public dtGame::GameEntryPoint
{
public:

   TestDirector();
   virtual ~TestDirector();

   /**
    * Called to initialize the game application.  You can pull any command line params here.
    */
   virtual void Initialize(dtABC::BaseABC& app, int argc, char** argv);

   /**
    * Called just before your application's game loop starts.  This is your main
    * opportunity to create components, load maps, create unique actors, etc...
    */
   virtual void OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gameManager);
private:
   std::vector<std::string> mMapNames;
};

#endif // TEST_DIRECTOR_ENTRY_POINT
