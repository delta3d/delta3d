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
// Must be first because of a hawknl conflict with osg.  This is not a directly required include, but indirectly
#include <osgDB/Serializer>

#include "testechoserver.h"
#include <dtGame/gamemanager.h>
#include <dtCore/deltawin.h>
#include <dtNetGM/servernetworkcomponent.h>

#include <iostream>

//////////////////////////////////////////////////////////////////////////
EchoServer::EchoServer()
   : dtABC::Application("config.xml")
{
   SetName("EchoServer");
}

//////////////////////////////////////////////////////////////////////////
EchoServer::~EchoServer()
{
   Quit();
}

//////////////////////////////////////////////////////////////////////////
void EchoServer::Config()
{
   dtABC::Application::Config();
   dtABC::Application::GetWindow()->SetWindowTitle("testServer");
 
   mServerGM = new dtGame::GameManager(*GetScene());
   mServerGM->GetMachineInfo().SetName("EchoServer");
   mServerGM->SetApplication(*this);

   mServerComp = new dtNetGM::ServerNetworkComponent("EchoNetwork", 1, "echoserver.log");
   mServerGM->AddComponent(*mServerComp, dtGame::GameManager::ComponentPriority::HIGHER);

   mEchoMessageProcessor = new EchoMessageProcessor();
   mServerGM->AddComponent(*mEchoMessageProcessor, dtGame::GameManager::ComponentPriority::HIGHEST);

   bool bServer = mServerComp->SetupServer(5555);
   if (!bServer)
   {
      throw AppInitException("\nError setting up server!!", __FILE__, __LINE__);
   }
   else
   {
      std::cout << "\nServer running on port 5555\n";
   }
}


void EchoServer::Quit()
{
   //shutdown the networking
   mServerComp->ShutdownNetwork();

   Application::Quit();
}

////////////////////////////////////////////////////////////////////////////////
AppInitException::AppInitException(const std::string& message, const std::string& filename, unsigned int linenum) 
: dtUtil::Exception(message, filename, linenum)
{

}
