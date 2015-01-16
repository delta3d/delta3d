/* -*-c++-*-
 * testClient - testClient (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007-2008, MOVES Institute
 * Copyright (C) 2014 Caper Holdings LLC
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
 */

#ifndef DELTA_EXAMPLE_TESTCLIENT
#define DELTA_EXAMPLE_TESTCLIENT

#include <dtABC/application.h>
#include <dtUtil/enumeration.h>

// forward declarations
namespace dtCore
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
   AppException(const std::string& name)
      : dtUtil::Enumeration(name)
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

   /**
    * Key press callback.
    *
    * @param keyboard the keyboard object generating the event
    * @param key the key pressed
    * @param character the corresponding character
    */
   virtual bool KeyPressed(const dtCore::Keyboard* kb, int key);

   virtual void Quit();

protected:
   virtual void PreFrame(const double deltaFrameTime);
   dtCore::RefPtr<dtCore::GameEvent> mEvent;

private:
   dtCore::RefPtr<dtGame::GameManager> mClientGM;
   dtCore::RefPtr<dtNetGM::ClientNetworkComponent> mClientNetwComp;
   dtCore::RefPtr<dtGame::DefaultMessageProcessor> mMessageProcessor;
};

#endif // DELTA_EXAMPLE_TESTCLIENT
