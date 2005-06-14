#include "teststatemanager.h"
#include <vector>

IMPLEMENT_ENUM(MyEventType);
const MyEventType MyEventType::ALT("ALT");
const MyEventType MyEventType::START("START");

IMPLEMENT_ENUM(MyStateType);
const MyStateType MyStateType::SHELL("SHELL");
const MyStateType MyStateType::OPTIONS("OPTIONS");
const MyStateType MyStateType::GAME("GAME");

Alt::Alt() : dtABC::Event( &MyEventType::ALT ) {}
Start::Start() : dtABC::Event( &MyEventType::START ) {}

Game::Game( std::string name ) : dtABC::State( name, &MyStateType::GAME ) {}
Shell::Shell( std::string name ) : dtABC::State( name, &MyStateType::SHELL ) {}
Options::Options( std::string name ) : dtABC::State( name, &MyStateType::OPTIONS ) {}

IMPLEMENT_MANAGEMENT_LAYER(TestStateManager)

//static member variables
const int TestStateManager::mBufferSize = 256;
dtCore::RefPtr<TestStateManager::MyStateManager> TestStateManager::MyStateManager::mManager = 0;

TestStateManager::TestStateManager(const std::string& config): Base( "TestStateManager" ), mStartState(0)
{
   RegisterInstance(this);
   AddSender( dtCore::System::GetSystem() );

   MyStateManager* sm = MyStateManager::Instance();
   sm->AddSender(this);

   sm->RegisterEvent<Alt>( &MyEventType::ALT );
   sm->RegisterEvent<Start>( &MyEventType::START );

   sm->RegisterState<Shell>( &MyStateType::SHELL );
   sm->RegisterState<Options>( &MyStateType::OPTIONS );
   sm->RegisterState<Game>( &MyStateType::GAME );

   if( !sm->Load( config ) )
   {      
      //setup some defaults if XML loading fails
      dtCore::RefPtr<Game> game = new Game("Game");
      dtCore::RefPtr<Shell> shell = new Shell("Shell");
      dtCore::RefPtr<Options> options = new Options("Options");

      sm->AddTransition( &MyEventType::ALT, shell.get(), options.get() );
      sm->AddTransition( &MyEventType::ALT, options.get(), shell.get() );
      sm->AddTransition( &MyEventType::START, shell.get(), game.get() );
      sm->AddTransition( &MyEventType::START, options.get(), game.get() );

      sm->MakeCurrent( shell.get() );
   }

   mStartState = sm->Current();
}

TestStateManager::~TestStateManager()
{
   DeregisterInstance(this);
}

int TestStateManager::GetInput()
{
   std::cout << "q) Quit" << std::endl
      << "r) Restart" << std::endl
      << "p) Print StateManager contents" << std::endl;  //\todo take out
   std::cout << "Send event: ";

   char buffer[mBufferSize];
   std::cin.getline( buffer, mBufferSize );

   if( strcmp( buffer, "q" ) == 0 )
   {
      dtCore::System::GetSystem()->Stop();
      return -1;
   }
   else if( strcmp( buffer, "r" ) == 0 )
   {
      std::cout << "Returning to starting State, ";
      MyStateManager* sm = MyStateManager::Instance();
      sm->MakeCurrent( mStartState );
      std::cout << sm->Current()->GetName() << "." <<std::endl;
      return -1;
   }
   else if( strcmp( buffer, "p" ) == 0 )   //\todo take out
   {
      MyStateManager* mgr = MyStateManager::Instance();

      const MyStateManager::StatePtrSet& setstates = mgr->GetStates();
      std::cout << std::endl << "StateManager's set of States:" << std::endl;
      for(MyStateManager::StatePtrSet::const_iterator iter=setstates.begin(); iter!=setstates.end(); iter++)
      {
         std::cout << (*iter)->GetName() << std::endl;
      }

      const MyStateManager::EventMap& transitions = mgr->GetTransitions();
      std::cout << std::endl << "StateManager's transitions:" << std::endl;
      for(MyStateManager::EventMap::const_iterator iter=transitions.begin(); iter!=transitions.end(); iter++)
      {
         MyStateManager::EventMap::key_type kv = iter->first;
         MyStateManager::EventMap::key_type::first_type event = kv.first;
         MyStateManager::EventMap::key_type::second_type from = kv.second;
         MyStateManager::EventMap::value_type::second_type to = iter->second;
         std::cout << "<" << event->GetName() << "," << from->GetName() << "> : " << to->GetName() << std::endl;
      }
   }

   int choice = atoi(buffer);
   return choice;
}

void TestStateManager::OnMessage( MessageData* data )
{
   if( data->message == "preframe" )
   {
      const dtABC::State* state = MyStateManager::Instance()->Current();
      std::cout << std::endl << "From the State, " << state->GetName() << ", ";

      // iterate over the valid range
      EventTypeVector eventsTypes = GetEvents( state );
      std::cout << "please choose which event to send:" << std::endl;
      unsigned int counter(0);
      for(EventTypeVector::const_iterator iter=eventsTypes.begin(); iter!=eventsTypes.end(); iter++)
         std::cout << counter++ << ") " << (*iter)->GetName() << std::endl;

      int choice = GetInput();

      if( (choice >= 0) && (choice < int(eventsTypes.size())) && dtCore::System::GetSystem()->IsRunning() )
      {
         //get event from factory
         dtCore::RefPtr<MyStateManager::EventFactory> factory = MyStateManager::Instance()->GetEventFactory();
         dtCore::RefPtr<dtABC::Event> newEvent = factory->CreateObject(eventsTypes[choice]); 
         assert(newEvent.get());

         SendMessage("event",(void*)(newEvent.get()));
      }
   }
}

TestStateManager::EventTypeVector TestStateManager::GetEvents(const dtABC::State* state)
{
   unsigned int msize = MyStateManager::Instance()->GetNumOfEvents(state);
   EventTypeVector events(msize);   // allocates the array
   MyStateManager::Instance()->GetEvents(state,events);    // fills the array
   return events;
}
