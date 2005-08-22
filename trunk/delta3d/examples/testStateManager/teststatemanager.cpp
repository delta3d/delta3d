#include "teststatemanager.h"
#include "MyEventTypes.h"
#include "MyStateTypes.h"

#include <vector>
#include <iostream>


StateWalker::StateWalker(dtABC::StateManager* gm) : BaseClass(), mStateManager(gm), mStartState(0)
{
   if( mStateManager.valid() )
      mStateManager->AddSender( this );

   AddSender( dtCore::System::Instance() );

   mStateManager->RegisterEvent<Alt>( &MyEventType::ALT );
   mStateManager->RegisterEvent<Start>( &MyEventType::START );

   mStateManager->RegisterState<Shell>( &MyStateType::SHELL );
   mStateManager->RegisterState<Options>( &MyStateType::OPTIONS );
   mStateManager->RegisterState<Game>( &MyStateType::GAME );

   if( dtABC::State* current = mStateManager->GetCurrentState() )
   {
      mStartState = current;
   }
}

StateWalker::~StateWalker()
{
   mStateManager->RemoveSender(this);
   RemoveSender( dtCore::System::Instance() );
}

void StateWalker::OnMessage(dtCore::Base::MessageData* msg)
{
   if( msg->message == "preframe" )
   {
      DisplayEventChoicesAndWaitForInput();
   }
}

StateWalker::EventPtrVec StateWalker::GetEvents(dtABC::State* from)
{
   EventPtrVec events;

   const dtABC::StateManager::TransitionMap& transitions = mStateManager->GetTransitions();
   unsigned int counter(0);
   for(dtABC::StateManager::TransitionMap::const_iterator iter=transitions.begin(); iter!=transitions.end(); iter++)
   {
      const dtABC::StateManager::TransitionMap::key_type::second_type state = (*iter).first.second;
      if( state == from )
      {
         events.push_back( (*iter).first.first );
      }
   }

   return events;
}

void StateWalker::DisplayEventChoicesAndWaitForInput()
{
   if( !mStateManager->GetCurrentState() )
   {
      std::cout << "StateWalker: No valid current State within StateManager, stopping the System." << std::endl;
      dtCore::System::Instance()->Stop();
      return;
   }

   std::cout << "For the State, " << mStateManager->GetCurrentState()->GetName() << ", the Event choices are:" << std::endl;

   std::vector<const dtABC::Event::Type*> eventvec = GetEvents( mStateManager->GetCurrentState() );
   unsigned int esize = eventvec.size();

   // display the Event choices for those transitions
   unsigned int i(0);
   for(; i<esize; i++)
   {
      std::cout << i << ": " << eventvec[i]->GetName() << std::endl;
   }

   // print "extra" event choices
   DisplayExtraEventChoices(i);

   // --------------------------- handle input --------------------------- //
   std::cout << "Please choose an Event:";

   unsigned int eventchoice;
   std::cin >> eventchoice;
   std::cout << "Your choice was: " << eventchoice << std::endl;

   if( eventchoice < esize )
   {
      const dtABC::Event::Type* eventtype = eventvec[eventchoice];
      std::cout << "StateWalker: Sending Event of type: " << eventtype->GetName() << std::endl;
      dtCore::RefPtr<dtABC::StateManager::EventFactory> ef = mStateManager->GetEventFactory();
      if( ef->IsTypeSupported( eventtype ) )
      {
         dtCore::RefPtr<dtABC::Event> event = ef->CreateObject( eventtype );
         SendMessage("event", event.get() );
      }
      else
      {  // some logging feedback
         std::cout << "StateWalker: Can not create Event of type: " << eventtype->GetName() << std::endl;
      }
   }

   else
   {  // honor the "extra" event choices
      HandleExtraEventChoices(esize,eventchoice);
   }
}

// --- "extra" choices --- //
void StateWalker::DisplayExtraEventChoices(unsigned int index)
{
   std::cout << index << ": " << "Quit" << std::endl;
}

void StateWalker::HandleExtraEventChoices(unsigned int eventvecsize, unsigned int choice)
{
   if( choice == eventvecsize )
   {
      std::cout << "StateWalker: Quit was chosen." << std::endl;

      ///\todo replace ::Instance() with ::Instance()
      dtCore::System::Instance()->Stop();
   }
}
