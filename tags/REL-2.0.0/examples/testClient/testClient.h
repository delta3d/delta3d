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
*/
#ifndef DELTA_EXAMPLE_TESTCLIENT
#define DELTA_EXAMPLE_TESTCLIENT

#include <dtABC/application.h>
#include <dtUtil/enumeration.h>

//forward declarations
namespace dtDAL
{
   class GameEvent;
}

namespace dtGame
{
   class GameManager;
   class DefaultMessageProcessor;
}

namespace dtNetGM
{
   class ClientNetworkComponent;
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

class ClientMessageProcessor;

class EchoClient : public dtABC::Application
{
public:

   /**
   * Constructs the test application.
   */
   EchoClient();

   /**
   * Destroys the test application.
   */
   virtual ~EchoClient();

   /**
   * Configures the application and prepares it to start the game/simulation.
   */
   void Config();

   virtual void Quit();

protected:
   virtual void PreFrame(const double deltaFrameTime);
   dtCore::RefPtr<dtDAL::GameEvent> mEvent;

private:
   dtCore::RefPtr<dtGame::GameManager> mClientGM;
   dtCore::RefPtr<dtNetGM::ClientNetworkComponent> mClientNetwComp;
   dtCore::RefPtr<dtGame::DefaultMessageProcessor> mMessageProcessor;
};

#endif // DELTA_EXAMPLE_TESTCLIENT
