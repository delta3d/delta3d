#include "teststatemanager.h"
#include "MyEventTypes.h"
#include "MyStateTypes.h"

#include <vector>
#include <iostream>

#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>

void LOG(const std::string& msg)
{
   dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ALWAYS, "", msg.c_str());
}

////////////////////////////////////////////////////////////////////////////////
StateWalker::StateWalker(dtABC::StateManager* gm)
   : BaseClass()
   , mStateManager(gm)
   , mStartState(0)
{
   if (mStateManager.valid())
   {
      mStateManager->AddSender(this);
   }

   AddSender(&dtCore::System::GetInstance());

   mStateManager->RegisterEvent<Alt>(&MyEventType::ALT);
   mStateManager->RegisterEvent<Start>(&MyEventType::START);

   mStateManager->RegisterState<Shell>(&MyStateType::SHELL);
   mStateManager->RegisterState<Options>(&MyStateType::OPTIONS);
   mStateManager->RegisterState<Game>(&MyStateType::GAME);

   if (dtABC::State* current = mStateManager->GetCurrentState())
   {
      mStartState = current;
   }
}

////////////////////////////////////////////////////////////////////////////////
StateWalker::~StateWalker()
{
   mStateManager->RemoveSender(this);
   RemoveSender(&dtCore::System::GetInstance());
}

////////////////////////////////////////////////////////////////////////////////
void StateWalker::OnMessage(dtCore::Base::MessageData* msg)
{
   if (msg->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      DisplayEventChoicesAndWaitForInput();
   }
}

////////////////////////////////////////////////////////////////////////////////
void StateWalker::DisplayEventChoicesAndWaitForInput()
{
   if (!mStateManager->GetCurrentState())
   {
      LOG_ERROR("No valid current State within StateManager, stopping the System.");
      dtCore::System::GetInstance().Stop();
      return;
   }

   LOG("Current State: " + mStateManager->GetCurrentState()->GetName());
   LOG("The Event choices are:");

   std::vector<const dtABC::Event::Type*> eventvec(mStateManager->GetNumOfEvents(mStateManager->GetCurrentState()));
   mStateManager->GetEvents(mStateManager->GetCurrentState(), eventvec);
   unsigned int esize = eventvec.size();

   // display the Event choices for those transitions
   unsigned int i(0);
   for (; i < esize; ++i)
   {
      LOG(dtUtil::ToString(i) + ": " + eventvec[i]->GetName());
   }

   // print "extra" event choices
   DisplayExtraEventChoices(i);

   // --------------------------- handle input --------------------------- //
   LOG("Please choose an Event:");

   unsigned int eventchoice;
   std::cin >> eventchoice;
   LOG("Your choice was: " + dtUtil::ToString(eventchoice));

   if (eventchoice < esize)
   {
      const dtABC::Event::Type* eventtype = eventvec[eventchoice];
      dtCore::RefPtr<dtABC::StateManager::EventFactory> ef = mStateManager->GetEventFactory();
      if (ef->IsTypeSupported(eventtype))
      {
         dtCore::RefPtr<dtABC::Event> event = ef->CreateObject(eventtype);
         if (event.valid())
         {
            LOG("Sending Event of type: " + eventtype->GetName());
            SendMessage("event", event.get());
         }
      }
      else
      {
         // some logging feedback
         LOG("Can not create Event of type: " + eventtype->GetName());
      }
   }
   else
   {
      // honor the "extra" event choices
      HandleExtraEventChoices(esize, eventchoice);
   }
}

////////////////////////////////////////////////////////////////////////////////
// --- "extra" choices --- //
void StateWalker::DisplayExtraEventChoices(unsigned int index)
{
   LOG(dtUtil::ToString(index) + ": " + "Quit");
}

////////////////////////////////////////////////////////////////////////////////
void StateWalker::HandleExtraEventChoices(unsigned int eventvecsize, unsigned int choice)
{
   if (choice == eventvecsize)
   {
      LOG("StateWalker: Quit was chosen.");
      dtCore::System::GetInstance().Stop();
   }
}
