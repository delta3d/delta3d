/* -*-c++-*-
 * testEchoServer - testechoserver (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007, MOVES Institute
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
 *  Pjotr van Amerongen
 */

#ifndef DELTA_EXAMPLE_TESTECHOSERVER
#define DELTA_EXAMPLE_TESTECHOSERVER

#include <dtABC/application.h>
#include <dtUtil/exception.h>

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
class AppInitException : public dtUtil::Exception
{
public:
	AppInitException(const std::string& message, const std::string& filename, unsigned int linenum);
	virtual ~AppInitException() {};
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
