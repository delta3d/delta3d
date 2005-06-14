#include <dtABC/statemanager.h>
#include <dtCore/system.h>
#include <dtCore/notify.h>

#include <osgDB/FileUtils>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/SAXParser.hpp>

using namespace dtABC;
using namespace dtCore;
XERCES_CPP_NAMESPACE_USE

IMPLEMENT_MANAGEMENT_LAYER(StateManager)

// static member variables
RefPtr<StateManager> StateManager::mManager = 0;

// private constructor
template< typename EventTypeClass, typename T2 >
StateManager::StateManager()
:  Base("StateManager"),
   mCurrentState(0),
   mLastEvent(0),
   mStates(),
   mTransition(),
   mSwitch(false)
{
   RegisterInstance( this );
   AddSender( System::GetSystem() );
}

// private destructor
template< typename T1, typename T2 >
StateManager::~StateManager()
{
   DeregisterInstance( this );
}

// create new StateManager and return, if already created just return it
template< typename T1, typename T2 >
StateManager* StateManager::Instance()
{
   if( mManager.get() == 0 )
   {
      mManager = new StateManager;
   }

   return mManager.get();
}

template< typename T1, typename T2 >
void StateManager::Destroy()
{
   if( mManager.get() )
   {
      mManager.release();
   }
}

template< typename T1, typename T2 >
void StateManager::PreFrame( const double deltaFrameTime )
{
   if( mSwitch ) //switch modes between frames
   {
      EventMap::key_type key( mLastEvent->GetType(),mCurrentState );
      EventMap::iterator iter = mTransition.find( key );
      if( iter != mTransition.end() )
      {
         MakeCurrent( (*iter).second.get() );
         mSwitch = false;
      }
   }

   if( mCurrentState.valid() )
   {
      mCurrentState->PreFrame( deltaFrameTime );
   }
}

template< typename T1, typename T2 >
void StateManager::Frame( const double deltaFrameTime )
{
   if( mCurrentState.valid() )
   {
      mCurrentState->Frame( deltaFrameTime );
   }
}

template< typename T1, typename T2 >
void StateManager::PostFrame( const double deltaFrameTime )
{
   if( mCurrentState.valid() )
   {
      mCurrentState->PostFrame( deltaFrameTime );
   }

   if( mSwitch ) //shutdown state if switched or stopped
   {
      if( mCurrentState.valid() )
      {
         mCurrentState->Shutdown();
      }
   }
}

template< typename T1, typename T2 >
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

template< typename T1, typename T2 >
bool StateManager::AddState( State* state )
{
   if( !state )
   {
      return false;
   }

   //if we are are not already in the set of states...
   if( mStates.insert(state).second )
   {
      state->AddSender(this);
      return true;
   }

   return false;
}

template< typename T1, typename T2 >
bool StateManager::RemoveState( State* state )
{
   if( !state )
   {
      return false;
   }

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

template< typename T1, typename T2 >
void StateManager::RemoveAllStates()
{
   mStates.clear();
   mTransition.clear();
}

template< typename T1, typename T2 >
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

template< typename T1, typename T2 >
bool StateManager::AddTransition( const Event::Type* eventType, State* from, State* to )
{
   if( !eventType || !from || !to )
   {
      return false;
   }

   /** Returns true if a transition was successfully added. */
   //lazy state addition
   mStates.insert(from);
   mStates.insert(to);

   // checking the set of States
   State* realFrom = GetState( from->GetName() );
   if( !realFrom )
   {
      realFrom = from;
   }

   State* realTo = GetState( to->GetName() );
   if( !realTo )
   {
      realTo = to;
   }

   // checking the transition map's keys
   EventMap::key_type key( eventType, realFrom );
   std::pair<EventMap::iterator,bool> returnpair = mTransition.insert( EventMap::value_type( key , realTo ) );
   return returnpair.second;
}

template< typename T1, typename T2 >
bool StateManager::RemoveTransition( const Event::Type* eventType, State* from, State* to )
{
   if( !eventType || !from || !to )
   {
      return false;
   }

   /** Returns true if any elements were removed from the EventMap */
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

template< typename T1, typename T2 >
const StateManager::StatePtrSet& StateManager::GetStates() const
{
   return mStates;
}

template< typename T1, typename T2 >
const StateManager::EventMap& StateManager::GetTransitions() const
{
   return mTransition;
}

template< typename T1, typename T2 >
unsigned int StateManager::GetNumOfEvents(const State* from) const
{
   unsigned int counter(0);
   for(EventMap::const_iterator iter=mTransition.begin(); iter!=mTransition.end(); iter++)
   {
      const EventMap::key_type::second_type state = (*iter).first.second;
      if( state == from )
         counter++;
   }
   return counter;
}

template< typename T1, typename T2 >
void StateManager::GetEvents(const State* from, std::vector<const Event::Type*>& events)
{
   /**
     * Be sure to have correctly resized \param events before calling this function
     * with the \sa GetNumOfEvents member function.
     */
   unsigned int counter(0);
   for(EventMap::const_iterator iter=mTransition.begin(); iter!=mTransition.end(); iter++)
   {
      const EventMap::key_type::second_type state = (*iter).first.second;
      if( state == from )
      {
         if( events.size() > counter )
            events[counter++] = (*iter).first.first;
         // else throw exception?
      }
   }

   if( events.size() != counter )
      assert( 0 );
}

template< typename T1, typename T2 >
State* StateManager::Current()
{
   return mCurrentState.get();
}

template< typename T1, typename T2 >
const State* StateManager::Current() const
{
   return mCurrentState.get();
}

template< typename T1, typename T2 >
void StateManager::MakeCurrent( State* state )
{
   mCurrentState = state;

   if( mCurrentState.valid() )
   {
      mCurrentState->Enable( mLastEvent.get() );
   }
}

template< typename T1, typename T2 >
void StateManager::Print(PrintOptions options) const
{
   ///\todo print to Log file instead of only to std::cout
   if( options == PRINT_STATES )
   {
      std::cout << "StateManager::set<State> contents:" << std::endl;
      //iterate over all states
      unsigned int counter(0);
      for( StatePtrSet::const_iterator iter = mStates.begin(); iter != mStates.end(); iter++ )
         std::cout << "State[" << counter++ << "]=" << (*iter)->GetName() << std::endl;
   }
   else   // PRINT_TRANSITIONS
   {
      std::cout << "StateManager::map<<Event,State> : State> contents:" << std::endl;
      //iterate over all states
      unsigned int counter(0);
      for( EventMap::const_iterator iter = mTransition.begin(); iter != mTransition.end(); iter++ )
         std::cout << "Transition[" << counter++ << "]=<" << (*iter).first.first << "," << (*iter).first.second->GetName() << "> : " << (*iter).second->GetName() << std::endl;
   }
}

template< typename T1, typename T2 >
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
template< typename T1, typename T2 >
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

template< typename T1, typename T2 >
StateManager::TransitionHandler::TransitionHandler()
{
}

template< typename T1, typename T2 >
StateManager::TransitionHandler::~TransitionHandler()
{
}

template< typename T1, typename T2 >
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
      Notify( DEBUG_INFO, "Create event. typeName:'%s'", eventTypeName.c_str() );

      mEventType = EventType::GetValueForName( eventTypeName );
   }
   else if (elementName == "FromState")
   {
      std::string stateType = XMLString::transcode(attributes.getValue("Type"));
      std::string stateName = XMLString::transcode(attributes.getValue("Name"));

      Notify(DEBUG_INFO, "Create FromState. type:'%s', name:'%s' ", 
             stateType.c_str(), stateName.c_str() );

      dtCore::RefPtr<StateManager::StateFactory> sf = StateManager::Instance();
      mFromState = sf->CreateObject( StateType::GetValueForName( stateType ) );

      if( mFromState.valid() )
      {
         mFromState->SetName(stateName);
      }

   }
   else if (elementName == "ToState")
   {
      std::string stateType = XMLString::transcode(attributes.getValue("Type"));
      std::string stateName = XMLString::transcode(attributes.getValue("Name"));

      Notify(DEBUG_INFO, "Create ToState. type:'%s', name:'%s'",
             stateType.c_str(), stateName.c_str() );

      dtCore::RefPtr<StateManager::StateFactory> sf = StateManager::Instance();
      mToState = sf->CreateObject( StateType::GetValueForName( stateType ) );

      if( mToState.valid() )
      {
         mToState->SetName(stateName);
      }
   }
   else if (elementName == "StartState")
   {
      std::string stateName = XMLString::transcode(attributes.getValue("Name"));

      Notify(DEBUG_INFO, "Set StartState: '%s'", stateName.c_str());
      StateManager::Instance()->MakeCurrent( 
         StateManager::Instance()->GetState(stateName) );
   }

}

template< typename T1, typename T2 >
void StateManager::TransitionHandler::fatalError(const SAXParseException& exception)
{
   char* message = XMLString::transcode(exception.getMessage());
   Notify(WARN) << "Fatal Error: " << message
      << " at line: " << exception.getLineNumber()
      << std::endl;
}

template< typename T1, typename T2 >
void StateManager::TransitionHandler::endElement(const XMLCh* const name)
{
   std::string elementName = XMLString::transcode(name);

   if (elementName == "Transition")
   {
      Notify(DEBUG_INFO, "AddTransition('%s', '%s', '%s')",
         mEventTypeName.c_str(),
         mFromState->GetName().c_str(),
         mToState->GetName().c_str() );

      AddTransition( mEventType.get(), mFromState.get(), mToState.get() );
   }
}
