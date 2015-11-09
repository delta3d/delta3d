/* -*-c++-*-
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
 * David Guthrie
 */

#ifndef DELTA_GAME_ENTRY_POINT
#define DELTA_GAME_ENTRY_POINT
#include <dtGame/export.h>
#include <dtCore/refptr.h>

namespace dtABC
{
   class BaseABC;
}

namespace dtGame
{
   class GameManager;

   /**
    * @class GameEntryPoint
    * A class for specifying the entry point into a Game so that the entire game can be defined in a
    * Game library and loaded at runtime.  Typically, a derivative of this
    * class is compiled into a dynamic library, which gets loaded by a
    * dtGame::GameApplicationLoader instance.
    *
    * Note: If a dtGame::GameApplicationLoader is used to load this as a dynamic
    * library, be sure to implement the two C functions as shown, typically
    * in the .cpp file:
    * @code
    * extern "C" API_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
    * {
    *   return new MyGameEntryPoint();
    * }
    *
    * extern "C" API_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint *entryPoint)
    * {
    *   delete entryPoint;
    * }
    * @endcode
    * @see dtGame::GameApplicationLoader::Config()
    */
   class DT_GAME_EXPORT GameEntryPoint
   {
   public:
      GameEntryPoint() {}

      /**
       * The destructor of the GameEntryPoint.
       */
      virtual ~GameEntryPoint()
      {
      }

      /**
       * Override this to create the application differently or use a different class.
       */
      virtual dtCore::RefPtr<dtABC::BaseABC> CreateApplication(const std::string& configFileName);

      /**
       * Called to initialize the game application.  This gets called from the
       * dtGame::GameApplication::Config() method, but before
       * dtABC::Application::Config().  This allows some defining of the
       * default Camera and Window, such as setting up stencil buffers.
       * @param app the current application
       * @param argc number of startup arguments.
       * @param argv array of string pointers to the arguments.
       */
      virtual void Initialize(dtABC::BaseABC& /*app*/, int /*argc*/, char ** /*argv*/) { }

      /**
       * Called after all startup related code is run.  At this point, the
       * dtGame::GameApplication has been configured and ready to be used.
       * Override this method to perform any specific start up functionality that
       * the GameEntryPoint needs to do.
       */
      virtual void OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gamemanager) = 0;

      /**
       * This is the notice to the GameEntryPoint that the application is
       * quitting.  Perform any cleanup that needs to be done here.
       * @note This is called from the GameApplication destructor and is followed
       * by the GameEntryPoint destructor.
       */
      virtual void OnShutdown(dtABC::BaseABC& /*app*/, dtGame::GameManager& /*gamemanager*/) { }

   };

} // namespace dtGame

#endif // DELTA_GAME_ENTRY_POINT
