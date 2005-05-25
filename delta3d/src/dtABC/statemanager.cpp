#include <dtCore/system.h>

#include <dtABC/statemanager.h>

using namespace dtABC;
using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(StateManager)

// static member variables
RefPtr<StateManager> StateManager::mManager = 0;

// private constructor
StateManager::StateManager()
:  Base("StateManager"),
   mCurrentState(0),
   mLastEvent(0),
   mSwitch(false),
   mStop(false)
{
   RegisterInstance( this );
   AddSender( System::GetSystem() );
}

// private destructor
StateManager::~StateManager()
{ 
   DeregisterInstance( this );  
}

// create new StateManager and return, if already created just return it
StateManager* StateManager::Instance()
{
   if( mManager.get() == 0 )
   {
      mManager = new StateManager;
   }

   return mManager.get();
}

void StateManager::Destroy()
{
   if( mManager.get() )
   {
      mManager.release();
   }
}

void StateManager::PreFrame( const double deltaFrameTime )
{
   if( mSwitch ) //switch modes between frames
   {
      MakeCurrent( mTransition[ std::make_pair( mLastEvent->GetType(), mCurrentState ) ].get() );
      mSwitch = false;
   }

   if( mCurrentState.valid() )
   {
      mCurrentState->PreFrame( deltaFrameTime );
   }
}

void StateManager::Frame( const double deltaFrameTime )
{
   if( mCurrentState.valid() )
   {
      mCurrentState->Frame( deltaFrameTime );
   }
}

void StateManager::PostFrame( const double deltaFrameTime )
{
   if( mCurrentState.valid() )
   {
      mCurrentState->PostFrame( deltaFrameTime );
   }

   if( mSwitch || mStop ) //shutdown state if switched or stopped
   {
      if( mCurrentState.valid() )
      {
         mCurrentState->Shutdown();
      }

      if( mStop )
      {
         System::GetSystem()->Stop();
      }
   }
}

void StateManager::OnMessage( MessageData* data )
{
   
   if( data->message == "preframe" )
   {
      const double delta = *reinterpret_cast<const double*>(data->userData); 
      PreFrame(delta);
   }
   else if( data->message == "frame" )
   {
      const double delta = *reinterpret_cast<const double*>(data->userData); 
      Frame(delta);
   }
   else if( data->message == "postframe" )
   {
      const double delta = *reinterpret_cast<const double*>(data->userData); 
      PostFrame(delta);
   }
   else if( data->message == "event" )
   {
      Event* event = reinterpret_cast<Event*>( data->userData );

      //if the event/current state pair is in our list of transitions, then switch it up!
      if( mTransition.find( std::make_pair( event->GetType(), mCurrentState ) ) != mTransition.end() )
      {
         mLastEvent = event;
         mSwitch = true;
      }
   }
}

bool StateManager::AddState( State* state )
{
   //if we are are not already in the set of states...
   if( mStates.insert(state).second )
   {
      state->AddSender(this);
      return true;
   }

   return false;
}

bool StateManager::RemoveState( State* state )
{
   //if we are already in the set of states...
   if( mStates.erase(state) != 0 )
   {
      state->RemoveSender(this);

      //remove transition to and from the remove state
      for( EventMap::iterator iter = mTransition.begin(); iter != mTransition.end(); )
      {
         EventStatePair pair = (*iter).first;

         State* from = pair.second.get();
         State* to = (*iter).second.get();

         //if "from" or "to" states equal the removed state, ditch the transition
         if( from == state || to == state )
         {
            mTransition.erase(iter++);
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

bool StateManager::AddTransition( std::string eventType, State* from, State* to )
{
   //lazy state addition
   mStates.insert(from);
   mStates.insert(to);

   EventStatePair key = std::make_pair( eventType, from );
   
   //if key is not alread in map...
   if( mTransition.find(key) == mTransition.end() )
   {
      //add it
      mTransition[key] = to;
      return true;
   }

   return false;
}

bool StateManager::RemoveTransition( std::string eventType, State* from, State* to )
{
   EventStatePair key = std::make_pair( eventType, from );

   //if key is in map...
   if( mTransition.find(key) != mTransition.end() )
   {
      //and if key maps to "to"
      if( mTransition[key] == to )
      {
         return mTransition.erase(key) != 0;
      }
   }

   return false;
}

State* StateManager::Current()
{
   return mCurrentState.get();
}

const State* StateManager::Current() const
{
   return mCurrentState.get();
}

void StateManager::MakeCurrent( State* state )
{
   mCurrentState = state;

   if( mCurrentState.valid() )
   {
      mCurrentState->Enable( mLastEvent );
   }
}

void StateManager::Stop()
{
   mStop = true;
}

void StateManager::Print( bool stateBased ) const
{
   if( stateBased )
   {
      //iterate over all states
      for( std::set< RefPtr<State> >::const_iterator iter = mStates.begin(); iter != mStates.end(); iter++ )
      {
         const State* printState = (*iter).get();

         std::string printStateName = printState->GetName();
         if( printState == Current() )
         {
            printStateName.insert(0,"*");
         }

         std::cout << printStateName;

         //find all transitions that relate to the state we are currently printing
         for( EventMap::const_iterator iter =  mTransition.begin(); iter != mTransition.end(); iter++ )
         {
            const std::string& event = (*iter).first.first;
            const State* from = (*iter).first.second.get();
            const State* to = (*iter).second.get();

            std::string fromName = from->GetName();
            if( from == Current() )
            {
               fromName.insert(0,"*");
            }

            std::string toName = to->GetName();
            if( to == Current() )
            {
               toName.insert(0,"*");
            }

            if( from == printState || to == printState )
            {
               std::cout << " (" << event << ":" << fromName << ","<< toName << ")";
            }

         }

         std::cout << std::endl;
      }
   }
   else
   {
      for( EventMap::const_iterator iter =  mTransition.begin(); iter != mTransition.end(); iter++ )
      {
         const std::string& event = (*iter).first.first;
         const State* from = (*iter).first.second.get();
         const State* to = (*iter).second.get();

         std::string fromName = from->GetName();
         if( from == Current() )
         {
            fromName.insert(0,"*");
         }

         std::string toName = to->GetName();
         if( to == Current() )
         {
            toName.insert(0,"*");
         }

         std::cout << event << "(" << fromName << "," << toName << ")" << std::endl;
      }
   }
}
