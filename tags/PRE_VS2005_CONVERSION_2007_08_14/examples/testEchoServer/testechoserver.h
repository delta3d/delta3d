/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2007, MOVES Institute
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
*  Pjotr van Amerongen
*/
#ifndef DELTA_EXAMPLE_TESTECHOSERVER
#define DELTA_EXAMPLE_TESTECHOSERVER

#include <dtABC/application.h>
#include <dtUtil/enumeration.h>

#include "echomessageprocessor.h"

namespace dtGame
{
   class GameManager;
}

namespace dtNetGM
{
	class ServerNetworkComponent;
}

/**
 * Exceptions that may be thrown by the demo.
 */
class AppException : public dtUtil::Enumeration
{
   DECLARE_ENUM(AppException);
   public:
      static AppException INIT_ERROR;
   private:
      AppException(const std::string &name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
};

/**
 * This Delta3D application tests the after review support of Delta3D.
 * After action review consists of two major components, task (objective)
 * tracking, and record/playback.  Therefore, this application tests these
 * two primary components.
 */
class EchoServer : public dtABC::Application
{
   public:

      /**
       * Constructs the test application.
       */
      EchoServer();

      /**
       * Destroys the test application.
       */
      virtual ~EchoServer();

      /**
       * Configures the application and prepares it to start the game/simulation.
       */
	  void Config();

	  virtual void Quit();

private:
	dtCore::RefPtr<dtGame::GameManager> mServerGM;
	dtCore::RefPtr<dtNetGM::ServerNetworkComponent> mServerComp;
	dtCore::RefPtr<EchoMessageProcessor> mEchoMessageProcessor;
};

#endif // DELTA_EXAMPLE_TESTECHOSERVER
