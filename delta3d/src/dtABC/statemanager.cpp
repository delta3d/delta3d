#include "dtABC/statemanager.h"
#include <dtUtil/stringutils.h>

using namespace dtABC;

// StateManager::EventType stuff
IMPLEMENT_ENUM(StateManager::EventType)
StateManager::EventType::EventType(const std::string& name) : Event::Type(name) { AddInstance(this); }
StateManager::EventType::~EventType() {}
const StateManager::EventType StateManager::EventType::TRANSITION_OCCURRED("TRANSITION_OCCURRED");

// StateManager implementation
StateManager::StateManager()
   : dtCore::Base("StateManager")
   , mCurrentState(0)
   , mLastEvent(0)
   , mStates()
   , mTransitions()
   , mSwitch(false)
   , mEventFactory(new EventFactory())
   , mStateFactory(new StateFactory())
{
   AddSender(&dtCore::System::GetInstance());
}

StateManager::~StateManager()
{
}

void StateManager::PreFrame(const double deltaFrameTime)
{
   if (mSwitch) //switch modes between frames
   {
      if (mLastEvent.valid())
      {
         const Event::Type* eventtype = mLastEvent->GetType();
         TransitionMap::key_type key(eventtype, mCurrentState.get());
         TransitionMap::iterator iter = mTransitions.find(key);

         if (iter != mTransitions.end())
         {
            State* to = (*iter).second.get();
            TransitionOccurredEvent* event = new TransitionOccurredEvent(mCurrentState.get(), to);
            mCurrentState->Shutdown();
            MakeCurrent( to );
            mSwitch = false;
            SendMessage( "event" , event );
         }
      }
   }

   if (mCurrentState.valid())
   {
      mCurrentState->PreFrame(deltaFrameTime);
   }
}

void StateManager::Frame(const double deltaFrameTime)
{
   if (mCurrentState.valid())
   {
      mCurrentState->Frame(deltaFrameTime);
   }
}

void StateManager::PostFrame(const double deltaFrameTime)
{
   if (mCurrentState.valid())
   {
      mCurrentState->PostFrame(deltaFrameTime);
   }
}

/** Pass the "preframe", "frame", and "postframe" to the current State.  If
  * the message is an "event", then:
  *  -if the message is from a State, rebroadcast it.
  *  -if the Event is in the transition table, process the transition
  *  -otherwise, pass the Event to the current State
  */
void StateManager::OnMessage(MessageData* data)
{
   if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      const double delta = *static_cast<const double*>(data->userData);
      PreFrame(delta);
   }
   else if (data->message == dtCore::System::MESSAGE_FRAME)
   {
      const double delta = *static_cast<const double*>(data->userData);
      Frame(delta);
   }
   else if (data->message == dtCore::System::MESSAGE_POST_FRAME)
   {
      const double delta = *static_cast<const double*>(data->userData);
      PostFrame(delta);
   }
   else if (data->message == "event")
   {
      Event* event = static_cast<Event*>(data->userData);

      //We don't want to have the State cause a transition directly.
      if (IS_A(data->sender, State*))
      {
         //Note: This should never happen as States don't send "events".
         //We'll leave this here as a safety in case the State does send
         //an "event" with the StateManager listening.
         SendMessage("event", static_cast<void*>(event));
      }
      //if the event/current state pair is in our list of transitions...
      else if (mTransitions.find(std::make_pair(event->GetType(), mCurrentState)) != mTransitions.end())
      {
         //then switch it up!
         mLastEvent = event;
         mSwitch = true;
      }
      else
      {
         //pass it to the current state
         State* state = GetCurrentState();
         if (state != 0)
         {
            state->HandleEvent(event);
         }
      }
   }
}

/** Insert the supplied State in to the internal list of States.  Also
  *  add the State as a message Sender to the StateManager.
  */
bool StateManager::AddState(State* state)
{
   if (!state)
   {
      return false;
   }

   // if we are are not already in the set of states...
   if (mStates.insert(state).second)
   {
      //AddSender(state); States should not communicate directly with the StateManager
      return true;
   }

   return false;
}

bool StateManager::RemoveState(State* state)
{
   if (!state)
   {
      return false;
   }

   //if we are already in the set of states...
   if (mStates.erase(state) != 0)
   {
      state->RemoveSender(this); // remove us as a sender
      //RemoveSender(state);   States should not communicate directly with the StateManager

      // remove transition to and from the remove state
      for (TransitionMap::iterator iter = mTransitions.begin(); iter != mTransitions.end();)
      {
         EventStatePtrPair pair = (*iter).first;

         State* from = pair.second.get();
         State* to = (*iter).second.get();

         //if "from" or "to" states equal the removed state, ditch the transition
         if (from == state || to == state)
         {
            mTransitions.erase(iter++);
         }
         else
         {
            iter++;
         }
      }

      return true;
   }

   return false;
}

void StateManager::RemoveAllStates()
{
   mStates.clear();
   mTransitions.clear();
}

State* StateManager::GetState(const std::string& name)
{
   for (StatePtrSet::iterator iter = mStates.begin(); iter != mStates.end(); iter++)
   {
      if ((*iter)->GetName() == name)
      {
         return const_cast<State*>((*iter).get());
      }
   }

   return 0;
}

const State* StateManager::GetState(const std::string& name) const
{
   for (StatePtrSet::const_iterator iter = mStates.begin(); iter != mStates.end(); iter++)
   {
      if ((*iter)->GetName() == name)
      {
         return (*iter).get();
      }
   }

   return 0;
}

// Returns true if a transition was successfully added.
bool StateManager::AddTransition(const Event::Type* eventType, State* from, State* to)
{
   if (!eventType || !from || !to)
   {
      return false;
   }

   //lazy state addition
   AddState(from);
   AddState(to);

   // checking the set of States
   State* realFrom = GetState(from->GetName());
   if (!realFrom)
   {
      realFrom = from;
   }

   State* realTo = GetState(to->GetName());
   if (!realTo)
   {
      realTo = to;
   }

   // checking the transition map's keys
   TransitionMap::key_type key(eventType, realFrom);
   std::pair<TransitionMap::iterator,bool> returnpair = mTransitions.insert(TransitionMap::value_type(key, realTo));
   return returnpair.second;
}

bool StateManager::RemoveTransition(const Event::Type* eventType, State* from, State* to)
{
   if (!eventType || !from || !to)
   {
      return false;
   }

   // Returns true if any elements were removed from the EventMap
   TransitionMap::key_type key(eventType, from);

   // if key is in map...
   TransitionMap::iterator iter(mTransitions.find(key));
   if (iter != mTransitions.end())
   {
      // and if key maps to "to"
      if (iter->second == to)
      {
         return mTransitions.erase(key) > 0;
      }
   }

   return false;
}

unsigned int StateManager::GetNumOfEvents(const State* from) const
{
   unsigned int counter(0);
   for (TransitionMap::const_iterator iter=mTransitions.begin(); iter!=mTransitions.end(); iter++)
   {
      const TransitionMap::key_type::second_type state = (*iter).first.second;
      if (state == from)
      {
         counter++;
      }
   }
   return counter;
}

void StateManager::GetEvents(const State* from, std::vector<const Event::Type*>& events)
{
   /**
    * Be sure to have correctly resized @param Events before calling this function
    * with the GetNumOfEvents member function.
    * \sa GetNumOfEvents
    */
   unsigned int counter(0);
   for (TransitionMap::const_iterator iter = mTransitions.begin(); iter != mTransitions.end(); iter++)
   {
      const TransitionMap::key_type::second_type state = (*iter).first.second;
      if (state == from)
      {
         if (events.size() > counter)
         {
            events[counter++] = (*iter).first.first;
         }
         // else throw exception?
      }
   }

   assert(events.size() == counter);
}

State* StateManager::GetCurrentState()
{
   return mCurrentState.get();
}

const State* StateManager::GetCurrentState() const
{
   return mCurrentState.get();
}

void StateManager::MakeCurrent(State* state)
{
   mCurrentState = state;

   if (mCurrentState.valid())
   {
      //immediately pass the event to the new current state
      mCurrentState->HandleEvent(mLastEvent.get());
   }
}

void StateManager::PrintStates() const
{
   LOG_ALWAYS("StateManager::set<State> contents:")

   //iterate over all states
   unsigned int counter(0);
   for (StatePtrSet::const_iterator iter = mStates.begin(); iter != mStates.end(); iter++)
      LOG_ALWAYS("State[" + dtUtil::ToString(counter++) + "]=" + (*iter)->GetName())
}

void StateManager::PrintTransitions() const
{
   LOG_ALWAYS("StateManager::map<<Event::Type,State> : State> contents:" )

   //iterate over all states
   unsigned int counter(0);
   for (TransitionMap::const_iterator iter = mTransitions.begin(); iter != mTransitions.end(); iter++)
   {
      LOG_ALWAYS("Transition[" + dtUtil::ToString(counter++) + "]=<" + (*iter).first.first->GetName() + "," + (*iter).first.second->GetName() + "> : " + (*iter).second->GetName())
   }
}
