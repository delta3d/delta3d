#include <dtABC/statemanager.h>
#include <dtCore/system.h>
#include <dtCore/notify.h>

#include <osgDB/FileUtils>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/SAXParser.hpp>

#include <functional>  // for std::unary_function
#include <algorithm>   // for count_if

using namespace dtABC;
using namespace dtCore;
XERCES_CPP_NAMESPACE_USE

IMPLEMENT_MANAGEMENT_LAYER(StateManager)

// static member variables
RefPtr<StateManager> StateManager::mManager = 0;

// private constructor
StateManager::StateManager()
:  Base("StateManager"),
   mCurrentState(0),
   mLastEvent(0),
   mStates(),
   mTransition(),
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
         EventStatePtrPair pair = (*iter).first;

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

State* StateManager::GetState( const std::string& name )
{
   for( StatePtrSet::iterator iter = mStates.begin(); iter != mStates.end(); iter++ )
   {
      if( (*iter)->GetName() == name )
      {
         return const_cast<State*>((*iter).get());
      }
   }

   return 0;
}

bool StateManager::AddTransition(const std::string& eventType, State* from, State* to )
{
   /** Returns true if a transition was successfully added. */
   //lazy state addition
   mStates.insert(from);
   mStates.insert(to);

   // sync EventMap with the StatePtrSet algorithm:
   // 1) check to know if the std::pair<string,State::Ptr> is unique
   EventMap::key_type test_key( eventType, from );
   if( mTransition.find( test_key ) == mTransition.end() )
   {
      // 2) if that is unique, then check if the State is already in the std::set by using the State's string
      StatePtrSet::iterator set_iter = mStates.find( from );  // comparison predicate takes care of the string search
      if( set_iter == mStates.end() )      // 3a) add the new EventMap::value_type
         mTransition.insert( EventMap::value_type(test_key,to) );

      else                                 // 3b) then use the pointer from the set to form a pair
      {
         EventMap::key_type real_key(eventType,*set_iter);
         mTransition.insert( EventMap::value_type( real_key, to ) );
      }

      return true;
   }

   return false;
}

bool StateManager::RemoveTransition(const std::string& eventType, State* from, State* to )
{
   /** Returns true if more than one element was removed from the EventMap */
   EventMap::key_type key( eventType, from );

   // if key is in map...
   EventMap::iterator iter( mTransition.find(key) );
   if( iter != mTransition.end() )
   {
      //and if key maps to "to"
      if( iter->second == to )
         return mTransition.erase(key) > 0;
   }

   return false;
}

const StateManager::EventMap& StateManager::GetTransitions() const
{
   return mTransition;
}

unsigned int StateManager::GetNumOfEvents(const State* from) const
{
   unsigned int counter(0);
   for(EventMap::const_iterator iter=mTransition.begin(); iter!=mTransition.end(); iter++)
   {
      const EventMap::key_type::second_type state = (*iter).first.second;
      if( state->GetName() == from->GetName() )
         counter++;
   }
   return counter;
}

void StateManager::GetEvents(const State* from, std::vector<std::string>& events)
{
   /**
     * Be sure to have correctly resized \param events before calling this function
     * with the \sa GetNumOfEvents member function.
     */
   unsigned int counter(0);
   for(EventMap::const_iterator iter=mTransition.begin(); iter!=mTransition.end(); iter++)
   {
      const EventMap::key_type::second_type state = (*iter).first.second;
      if( state->GetName() == from->GetName() )
      {
         if( events.size() > counter )
            events[counter++] = (*iter).first.first;
         // else throw exception?
      }
   }

   if( events.size() != counter )
      assert( 0 );
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
      mCurrentState->Enable( mLastEvent.get() );
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
      for( StatePtrSet::const_iterator iter = mStates.begin(); iter != mStates.end(); iter++ )
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

bool StateManager::Load( std::string filename )
{
   bool retVal = false;

   std::string fullFileName = osgDB::findDataFile(filename);

   if (!fullFileName.empty())
   {
      ParseFile(fullFileName);
   }
   else
   {
      Notify(WARN, "StateManager - Can't find file '%s'",filename.c_str());
      retVal = false;
   }
   return retVal;
}

///Private
bool StateManager::ParseFile( std::string filename )
{
   bool retVal(false);
   try
   {
      XMLPlatformUtils::Initialize();
   }
   catch (const XMLException& toCatch) 
   {
      Notify(WARN) << toCatch.getMessage() << std::endl;
      return 1;
   }

   SAXParser* parser = new SAXParser();
   parser->setDoValidation(true);    // optional.
   parser->setDoNamespaces(true);    // optional

   TransitionHandler* docHandler = new TransitionHandler();
   parser->setDocumentHandler(docHandler);

   ErrorHandler* errHandler = (ErrorHandler*) docHandler;
   parser->setErrorHandler(errHandler);

   try
   {
      parser->parse(filename.c_str());
      retVal = true;
   }
   catch (const XMLException& toCatch) 
   {
      char* message = XMLString::transcode(toCatch.getMessage());
      Notify(WARN) << "Exception message is: \n"
         << message << "\n";
      XMLString::release(&message);
      return -1;
   }
   catch (const SAXParseException& toCatch) {
      char* message = XMLString::transcode(toCatch.getMessage());
      Notify(WARN) << "Exception message is: \n"
         << message << "\n";
      XMLString::release(&message);
      return -1;
   }
   catch (...) 
   {
      Notify(WARN) << "Unexpected Exception \n" ;
      return -1;
   }

   delete parser;
   delete docHandler;

   return retVal;
}


StateManager::TransitionHandler::TransitionHandler()
{
}

StateManager::TransitionHandler::~TransitionHandler()
{
}

void StateManager::TransitionHandler::startElement(const XMLCh* const name,
                                                   AttributeList& attributes)
{
   std::string elementName = XMLString::transcode(name);

   if (elementName == "Transition")
   {
      //start of a Transition
   }
   else if (elementName == "Event")
   {
      std::string eventTypeName = XMLString::transcode(attributes.getValue("TypeName"));
      Notify(DEBUG_INFO, "Create event. typeName:'%s'", eventTypeName.c_str() );

      mEventTypeName = eventTypeName;
   }
   else if (elementName == "FromState")
   {
      std::string stateType = XMLString::transcode(attributes.getValue("Type"));
      std::string stateName = XMLString::transcode(attributes.getValue("Name"));

      Notify(DEBUG_INFO, "Create FromState. type:'%s', name:'%s' ", 
             stateType.c_str(), stateName.c_str() );

      //lookup
      mFromState = new State(stateType);

      mFromState->SetName(stateName);

   }
   else if (elementName == "ToState")
   {
      std::string stateType = XMLString::transcode(attributes.getValue("Type"));
      std::string stateName = XMLString::transcode(attributes.getValue("Name"));

      Notify(DEBUG_INFO, "Create ToState. type:'%s', name:'%s'",
             stateType.c_str(), stateName.c_str() );

      mToState = new State(stateType);
      mToState->SetName(stateName);
   }
   else if (elementName == "StartState")
   {
      std::string stateName = XMLString::transcode(attributes.getValue("Name"));

      Notify(DEBUG_INFO, "Set StartState: '%s'", stateName.c_str());
      StateManager::Instance()->MakeCurrent( 
         StateManager::Instance()->GetState(stateName) );
   }

}

void StateManager::TransitionHandler::fatalError(const SAXParseException& exception)
{
   char* message = XMLString::transcode(exception.getMessage());
   Notify(WARN) << "Fatal Error: " << message
      << " at line: " << exception.getLineNumber()
      << std::endl;
}

void StateManager::TransitionHandler::endElement(const XMLCh* const name)
{
   std::string elementName = XMLString::transcode(name);

   if (elementName == "Transition")
   {
      Notify(DEBUG_INFO, "AddTransition('%s', '%s', '%s')",
         mEventTypeName.c_str(),
         mFromState->GetName().c_str(),
         mToState->GetName().c_str() );

      StateManager::Instance()->AddTransition(mEventTypeName, mFromState.get(), mToState.get() );
   }
}
