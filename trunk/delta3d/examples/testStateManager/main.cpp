#include "teststatemanager.h"     // for app class
#include <dtCore/globals.h>

#include "MyEventTypes.h"
#include "MyStateTypes.h"

#include <iostream>

void Usage()
{
   std::cout << "Proper usage is like this : <program> <config>" << std::endl;
   std::cout << "Using defaults instead" << std::endl;
}

int main(unsigned int argc, char* argv[])
{
   dtUtil::Log::GetInstance().SetLogLevel( dtUtil::Log::LOG_DEBUG );
   dtCore::SetDataFilePathList( dtCore::GetDeltaRootPath()+"/examples/testStateManager" );

   dtCore::RefPtr<dtABC::StateManager> mgr = new dtABC::StateManager();
   dtCore::RefPtr<StateWalker> app = new StateWalker( mgr.get() );

   if( argc > 1 )
   {
      mgr->Load<MyEventType,MyStateType>( argv[1] );
   }

   else // load some defaults, but give feedback to user for correct usage.
   {
      Usage();
      dtCore::RefPtr<Game> game = new Game("Game");
      dtCore::RefPtr<Shell> shell = new Shell("Shell");
      dtCore::RefPtr<Options> options = new Options("Options");

      mgr->AddTransition( &MyEventType::ALT, shell.get(), options.get() );
      mgr->AddTransition( &MyEventType::ALT, options.get(), shell.get() );
      mgr->AddTransition( &MyEventType::START, shell.get(), game.get() );
      mgr->AddTransition( &MyEventType::START, options.get(), game.get() );

      mgr->MakeCurrent( shell.get() );

      app->SetStartState( mgr->GetCurrentState() );
   }

   dtCore::System::Instance()->Config();
   dtCore::System::Instance()->Run();

   return 0;
}
