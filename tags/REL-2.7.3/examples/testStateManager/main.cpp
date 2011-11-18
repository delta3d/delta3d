/* -*-c++-*-
 * testStateManager - main (.h & .cpp) - Using 'The MIT License'
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

#include "teststatemanager.h"     // for app class

#include "MyEventTypes.h"
#include "MyStateTypes.h"

#include <dtUtil/datapathutils.h>

#include <iostream>

void Usage()
{
   std::cout << "Proper usage is like this : <program> <config>" << std::endl;
   std::cout << "Using defaults instead" << std::endl;
}

int main(int argc, char* argv[])
{
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList() + ";" +
                               dtUtil::GetDeltaRootPath() + "/examples/testStateManager" + ";");

   // This is a console app, so we don't want to shutdown if there are no windows.
   dtCore::System::GetInstance().SetShutdownOnWindowClose(false);

   dtCore::RefPtr<dtABC::StateManager> mgr = new dtABC::StateManager();
   dtCore::RefPtr<StateWalker> app = new StateWalker(mgr.get());

   if (argc > 1)
   {
      mgr->Load<MyEventType,MyStateType>(argv[1]);
   }

   else // load some defaults, but give feedback to user for correct usage.
   {
      Usage();
      dtCore::RefPtr<Game> game = new Game("Game");
      dtCore::RefPtr<Shell> shell = new Shell("Shell");
      dtCore::RefPtr<Options> options = new Options("Options");

      mgr->AddTransition(&MyEventType::ALT,   shell.get(),   options.get());
      mgr->AddTransition(&MyEventType::ALT,   options.get(), shell.get());
      mgr->AddTransition(&MyEventType::START, shell.get(),   game.get());
      mgr->AddTransition(&MyEventType::START, options.get(), game.get());

      mgr->MakeCurrent(shell.get());

      app->SetStartState(mgr->GetCurrentState());
   }

   dtCore::System::GetInstance().Config();
   dtCore::System::GetInstance().Run();

   return 0;
}
