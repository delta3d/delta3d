#ifndef DELTA_STATEMANAGER
#define DELTA_STATEMANAGER

#include <cassert>
#include <iostream>
#include <functional>  // for std::binary_predicate
#include <map>
#include <string>

#include "osgDB/FileUtils"

#include <dtUtil/objectfactory.h>
#include <dtUtil/log.h>
#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtABC/event.h>
#include <dtABC/state.h>

#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <dtABC/statemanagereventtype.h>

namespace dtABC
{
   //had to place this outside of the template so gcc won't bitch
   template<typename T>
   struct RefPtrWithNameCompare : std::binary_function<T,T,bool>
   {
      // RefPtrWithNameCompare will make sure the State being added is
      // unique to the set based on its name AND based on the fact
      // that the State has a unique place in memory.
      // This makes sure that no one tried to submit a State that
      // had the same name as another State, or someone tried to
      // resubmit a State already in the set by changing its name.
      
      bool operator()(const T& lhs,const T& rhs) const
      {
         return lhs.get() != rhs.get() && lhs->GetName() < rhs->GetName();
      }
   };

   //had to place this outside of the template so gcc won't bitch
   template<typename T>
   struct PairRefPtrWithNameCompare : public std::binary_function<T,T,bool>
   {
      /** Re-implement the default comparison algorithm for std::pair<T1,T2>::operator<,
       * \sa http://www.sgi.com/tech/stl/pair.html ,
       * but add smart StatePtr comparison with the StatePtrLess predicate.
       */
      bool operator()(const T& x, const T& y) const
      {
         // try to use the first element
         bool first_less( x.first < y.first );
         if( first_less )
            return true;
         
         bool first_greater( y.first < x.first );
         if( first_greater )
            return false;
         
         // else, key off the second element, and use the StatePtr comparison
         RefPtrWithNameCompare<typename T::second_type> compare_them;
         return compare_them( x.second,y.second );
      }
   };

   ///Controls the switching of modes by starting and stoping the different states.  
   ///When a new state is started Config is called and Shutdown is called before 
   ///switching.
   template< typename T1, typename T2 >
   class StateManager : public dtCore::Base
   {
    private:
       typedef StateManager<T1,T2> ThisType;

       /** An Event class specific to StateManager. */
       class TransitionOccurredEvent : public dtABC::Event
       {
       public:
          TransitionOccurredEvent() : dtABC::Event(&StateManagerEventType::TRANSITION_OCCURRED) {}
       protected:
          virtual ~TransitionOccurredEvent() {}
       };

      ///Constructor creates an instance of each state.
      StateManager();

      ///Calls the destructor for each state instance by iterating mList.
      virtual ~StateManager();

   public:

      typedef T1 EventType;
      typedef T2 StateType;
   
      typedef dtCore::RefPtr<dtABC::State>                       StatePtr;
      typedef std::pair< const dtABC::Event::Type*, StatePtr >   EventStatePtrPair;

      typedef dtUtil::ObjectFactory< const dtABC::Event::Type*, dtABC::Event > EventFactory;
      typedef dtUtil::ObjectFactory< const dtABC::State::Type*, dtABC::State > StateFactory;

      typedef std::set< StatePtr, RefPtrWithNameCompare<StatePtr> > StatePtrSet;
      typedef std::map< EventStatePtrPair, StatePtr, PairRefPtrWithNameCompare<EventStatePtrPair> >    EventMap;

      static   StateManager<EventType,StateType>*  Instance();
      static   void                                Destroy();

      bool                    Load( std::string filename );

      void                    PreFrame( const double deltaFrameTime );
      void                    Frame( const double deltaFrameTime );
      void                    PostFrame( const double deltaFrameTime );

      void                    OnMessage( MessageData* data );

      bool                    AddState( State* state );
      /// Removes a State from the StatePtrSet and associated transitions from the EventMap
      bool                    RemoveState( State* state );  

      bool                    AddTransition( const Event::Type* eventType, State* from, State* to );
      bool                    RemoveTransition( const Event::Type* eventType, State* from, State* to );

      State*                  GetState( const std::string& name );
      const State*            GetState( const std::string& name ) const;
      void                    RemoveAllStates();

      ///// Returns the set of states
      const    StatePtrSet&   GetStates() const { return mStates; }

      ///Returns the transition map
      const    EventMap&      GetTransitions() const { return mTransition; }

      /////Determines the number of events for the State
      unsigned int            GetNumOfEvents(const State* from) const;

      /////\brief This method should be used with \sa GetNumOfEvents
      void                    GetEvents(const State* from, std::vector<const Event::Type*>& events);

      ///Deprecated GetCurrentState()
      inline   State*         Current();
      ///Deprecated for GetCurrentState() const
      inline   const State*   Current() const;

      ///Returns pointer to current state.  Can be NULL.
      inline   State*         GetCurrentState();
      inline   const State*   GetCurrentState() const;

      void                    MakeCurrent( State* state );

      template<typename T>
      bool                    RegisterEvent( const Event::Type* eventType );

      template<typename T>
      bool                    RegisterState( const State::Type* stateType );

      StateFactory*           GetStateFactory() { return mStateFactory.get(); }
      EventFactory*           GetEventFactory() { return mEventFactory.get(); }

      //\todo: make this the << operater?
      ///Prints list of all states and transitions
      enum PrintOptions
      {
         PRINT_STATES,
         PRINT_TRANSITIONS
      };
      void           Print(PrintOptions options=PRINT_STATES) const;

   private:

      static dtCore::RefPtr< StateManager<EventType,StateType> >  mManager;
      dtCore::RefPtr<State>                                       mCurrentState;
      dtCore::RefPtr<Event>                                       mLastEvent;
      StatePtrSet                                                 mStates;
      EventMap                                                    mTransition;
      bool                                                        mSwitch;
      bool                                                        mStop;

      dtCore::RefPtr<EventFactory>                                mEventFactory;
      dtCore::RefPtr<StateFactory>                                mStateFactory;

      bool  ParseFile( std::string filename );

      class TransitionHandler : public XERCES_CPP_NAMESPACE_QUALIFIER HandlerBase
      {
      public:
      	TransitionHandler();
      	~TransitionHandler();
         virtual void startElement(const XMLCh* const, XERCES_CPP_NAMESPACE_QUALIFIER AttributeList&);
         virtual void endElement(const XMLCh* const name);

         virtual void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException&);
      private:
         dtCore::RefPtr<State> mFromState;
         dtCore::RefPtr<State> mToState;
         const Event::Type*    mEventType;
      };

   };
};

//implementation of template
namespace dtABC
{
   // private constructor
   template< typename T1, typename T2 >
   StateManager<T1,T2>::StateManager() : dtCore::Base("StateManager"),
     mCurrentState(0),
     mLastEvent(0),
     mStates(),
     mTransition(),
     mSwitch(false)
   {
      AddSender( dtCore::System::GetSystem() );

      mEventFactory = new EventFactory();
      mStateFactory = new StateFactory();
   }

   // private destructor
   template< typename T1, typename T2 >
   StateManager<T1,T2>::~StateManager()
   {
   }

   // create new StateManager and return, if already created just return it
   template< typename T1, typename T2 >
   StateManager<T1,T2>* StateManager<T1,T2>::Instance()
   {
      if( mManager.get() == 0 )
      {
         mManager = new StateManager;
      }

      return mManager.get();
   }

   template< typename T1, typename T2 >
   void StateManager<T1,T2>::Destroy()
   {
      if( mManager.get() )
      {
         mManager.release();
      }
   }

   template< typename T1, typename T2 >
   void StateManager<T1,T2>::PreFrame( const double deltaFrameTime )
   {
      if( mSwitch ) //switch modes between frames
      {
         EventMap::key_type key( mLastEvent->GetType(), mCurrentState.get() );
         EventMap::iterator iter = mTransition.find( key );

         if( iter != mTransition.end() )
         {
            MakeCurrent( (*iter).second.get() );
            mSwitch = false;
            SendMessage( "event" , new TransitionOccurredEvent() );
         }
      }

      if( mCurrentState.valid() )
      {
         mCurrentState->PreFrame( deltaFrameTime );
      }
   }

   template< typename T1, typename T2 >
   void StateManager<T1,T2>::Frame( const double deltaFrameTime )
   {
      if( mCurrentState.valid() )
      {
         mCurrentState->Frame( deltaFrameTime );
      }
   }

   template< typename T1, typename T2 >
   void StateManager<T1,T2>::PostFrame( const double deltaFrameTime )
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

   /** 
    * Pass the "preframe", "frame", and "postframe" to the current State.  If 
    * the message is an "event", then:
    *  -if the message is from a State, rebroadcast it.
    *  -if the Event is in the transition table, process the transition
    *  -otherwise, pass the Event to the current State
    */
   template< typename T1, typename T2 >
   void StateManager<T1,T2>::OnMessage( MessageData* data )
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

         //We don't want to have the State cause a transition directly.  
         if (IS_A(data->sender, State*))
         {
            //Note: This should never happen as States don't send "events".
            //We'll leave this here as a safety in case the State does send
            //an "event" with the StateManager listening.
            SendMessage("event", static_cast<void*>(event));
         }
         //if the event/current state pair is in our list of transitions...
         else if( mTransition.find( std::make_pair( event->GetType(), mCurrentState ) ) != mTransition.end() )
         {
            //then switch it up!
            mLastEvent = event;
            mSwitch = true;
         }
         else
         {
            //pass it to the current state
            State *state = GetCurrentState();
            if (state!=0)
            {
               state->HandleEvent( event );
            }
         }
      }
   }

   /** Insert the supplied State in to the internal list of States.  Also
    *  add the State as a message Sender to the StateManager.
    */
   template< typename T1, typename T2 >
   bool StateManager<T1,T2>::AddState( State* state )
   {
      if( !state )
      {
         return false;
      }

      //if we are are not already in the set of states...
      if( mStates.insert(state).second )
      {             
         //AddSender(state); States should not communicate directly with the StateManager
         return true;
      }

      return false;
   }

   template< typename T1, typename T2 >
   bool StateManager<T1,T2>::RemoveState( State* state )
   {
      if( !state )
      {
         return false;
      }

      //if we are already in the set of states...
      if( mStates.erase(state) != 0 )
      {
         state->RemoveSender(this); //remove us as a sender
         //RemoveSender(state);   States should not communicate directly with the StateManager

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
   void StateManager<T1,T2>::RemoveAllStates()
   {
      mStates.clear();
      mTransition.clear();
   }

   template< typename T1, typename T2 >
   State* StateManager<T1,T2>::GetState( const std::string& name )
   {
      for( StatePtrSet::iterator iter = mStates.begin(); iter != mStates.end(); iter++ )
      {
         if( (*iter)->GetName() == name )
         {
            return const_cast<State*>( (*iter).get() );
         }
      }

      return 0;
   }

   template< typename T1, typename T2 >
   const State* StateManager<T1,T2>::GetState( const std::string& name ) const
   {
      for( StatePtrSet::const_iterator iter = mStates.begin(); iter != mStates.end(); iter++ )
      {
         if( (*iter)->GetName() == name )
         {
            return (*iter).get();
         }
      }

      return 0;
   }

   // Returns true if a transition was successfully added.
   template< typename T1, typename T2 >
   bool StateManager<T1,T2>::AddTransition( const Event::Type* eventType, State* from, State* to )
   {
      if( !eventType || !from || !to )
      {
         return false;
      }

      //lazy state addition
      AddState(from);
      AddState(to);

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
   bool StateManager<T1,T2>::RemoveTransition( const Event::Type* eventType, State* from, State* to )
   {
      if( !eventType || !from || !to )
      {
         return false;
      }

      // Returns true if any elements were removed from the EventMap
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
   unsigned int StateManager<T1,T2>::GetNumOfEvents(const State* from) const
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
   void StateManager<T1,T2>::GetEvents(const State* from, std::vector<const Event::Type*>& events)
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
      State* StateManager<T1,T2>::Current()
   {
      DEPRECATE(  "State* StateManager<T1,T2>::Current()",
                  "State* StateManager<T1,T2>::GetCurrentState()" )
      return GetCurrentState();
   }

   template< typename T1, typename T2 >
      const State* StateManager<T1,T2>::Current() const
   {
      DEPRECATE(  "const State* StateManager<T1,T2>::Current() const",
                  "const State* StateManager<T1,T2>::GetCurrentState() const" )
      return GetCurrentState();
   }

   template< typename T1, typename T2 >
   State* StateManager<T1,T2>::GetCurrentState()
   {
      return mCurrentState.get();
   }

   template< typename T1, typename T2 >
   const State* StateManager<T1,T2>::GetCurrentState() const
   {
      return mCurrentState.get();
   }

   template< typename T1, typename T2 >
   void StateManager<T1,T2>::MakeCurrent( State* state )
   {
      mCurrentState = state;
      
      if( mCurrentState.valid() )
      {
         //immediately pass the event to the new current state
         mCurrentState->HandleEvent( mLastEvent.get() );
      }
   }

   template<typename T1, typename T2>
   template<typename T3>
   bool StateManager<T1,T2>::RegisterEvent( const Event::Type* eventType )
   {
      return mEventFactory->RegisterType<T3>( eventType );     
   }

   template<typename T1, typename T2>
   template<typename T3>
   bool StateManager<T1,T2>::RegisterState( const State::Type* stateType )
   {
      return mStateFactory->RegisterType<T3>( stateType );
   }

   template< typename T1, typename T2 >
   void StateManager<T1,T2>::Print(PrintOptions options) const
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
   bool StateManager<T1,T2>::Load( std::string filename )
   {
      bool retVal = false;

      std::string fullFileName = osgDB::findDataFile(filename);

      if (!fullFileName.empty())
      {
         retVal = ParseFile(fullFileName);
      }
      else
      {
         dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__, 
               "StateManager - Can't find file '%s'",filename.c_str());
         retVal = false;
      }
      return retVal;
   }

   ///Private
   template< typename T1, typename T2 >
   bool StateManager<T1,T2>::ParseFile( std::string filename )
   {
      bool retVal(false);
      try
      {
         XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
      }
      catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch) 
      {
         char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(toCatch.getMessage());
         dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__, message);
         return 1;
      }

      XERCES_CPP_NAMESPACE::SAXParser* parser = new XERCES_CPP_NAMESPACE::SAXParser();
      parser->setDoValidation(true);    // optional.
      parser->setDoNamespaces(true);    // optional

      TransitionHandler* docHandler = new TransitionHandler();
      parser->setDocumentHandler(docHandler);

      XERCES_CPP_NAMESPACE::ErrorHandler* errHandler = (XERCES_CPP_NAMESPACE::ErrorHandler*) docHandler;
      parser->setErrorHandler(errHandler);

      try
      {
         parser->parse(filename.c_str());
         retVal = true;
      }
      catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch) 
      {
         char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(toCatch.getMessage());
         dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__, "Exception message is '%s",  message );
         XERCES_CPP_NAMESPACE::XMLString::release(&message);
         return -1;
      }
      catch (const XERCES_CPP_NAMESPACE::SAXParseException& toCatch) {
         char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(toCatch.getMessage());
         dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__, "Exception message is '%s'", message);
         XERCES_CPP_NAMESPACE::XMLString::release(&message);
         return -1;
      }
      catch (...) 
      {
         dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__, "Statemanager::ParseFile() Unexpected Exception");
         return -1;
      }

      delete parser;
      delete docHandler;

      return retVal;
   }

   template< typename T1, typename T2 >
   StateManager<T1,T2>::TransitionHandler::TransitionHandler()
   {
   }

   template< typename T1, typename T2 >
   StateManager<T1,T2>::TransitionHandler::~TransitionHandler()
   {
   }

   template< typename T1, typename T2 >
   void StateManager<T1,T2>::TransitionHandler::startElement(const XMLCh* const name, XERCES_CPP_NAMESPACE::AttributeList& attributes)
   {
      std::string elementName = XERCES_CPP_NAMESPACE::XMLString::transcode(name);

      if (elementName == "Transition")
      {
         //start of a Transition
      }
      else if (elementName == "Event")
      {
         std::string eventTypeName = XERCES_CPP_NAMESPACE::XMLString::transcode(attributes.getValue("TypeName"));
         mEventType = EventType::GetValueForName( eventTypeName );
      }
      else if (elementName == "FromState")
      {
         std::string stateType = XERCES_CPP_NAMESPACE::XMLString::transcode(attributes.getValue("Type"));
         std::string stateName = XERCES_CPP_NAMESPACE::XMLString::transcode(attributes.getValue("Name"));

         //Do a check to see if a State exists with the same name and type name
         //If it does, use it, otherwise create a new State
         State *s = mManager->GetState( stateName );
         if ( s == 0)
         {
            dtCore::RefPtr<StateFactory> sf = mManager->GetStateFactory();
            mFromState = sf->CreateObject( StateType::GetValueForName( stateType ) );
         }
         else if ( s->GetType()->GetName() != stateType )
         {
            dtCore::RefPtr<StateFactory> sf = mManager->GetStateFactory();
            mFromState = sf->CreateObject( StateType::GetValueForName( stateType ) );
         }
         else
         {
            mFromState = s;
         }

         if( mFromState.valid() )
         {
            mFromState->SetName(stateName);
         }
         else dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__, 
            "StateManager Load() can't create FromState '%s'",
             stateType.c_str() );

      }
      else if (elementName == "ToState")
      {
         std::string stateType = XERCES_CPP_NAMESPACE::XMLString::transcode(attributes.getValue("Type"));
         std::string stateName = XERCES_CPP_NAMESPACE::XMLString::transcode(attributes.getValue("Name"));

         //Do a check to see if a State exists with the same name and type name
         //If it does, use it, otherwise create a new State
         State *s = mManager->GetState( stateName );
         if ( s == 0)
         {
            dtCore::RefPtr<StateFactory> sf = mManager->GetStateFactory();
            mToState = sf->CreateObject( StateType::GetValueForName( stateType ) );
         }
         else if ( s->GetType()->GetName() != stateType )
         {
            dtCore::RefPtr<StateFactory> sf = mManager->GetStateFactory();
            mToState = sf->CreateObject( StateType::GetValueForName( stateType ) );
         }
         else
         {
            mToState = s;
         }

         if( mToState.valid() )
         {
            mToState->SetName(stateName);
         }
         else dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__, 
            "StateManager Load() can't create ToState '%s'",
            stateType.c_str() );

      }
      else if (elementName == "StartState")
      {
         std::string stateName = XERCES_CPP_NAMESPACE::XMLString::transcode(attributes.getValue("Name"));

         dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__,
            "Set StartState: '%s'", stateName.c_str());
         StateManager::Instance()->MakeCurrent( StateManager<T1,T2>::Instance()->GetState(stateName) );
      }

   }

   template< typename T1, typename T2 >
   void StateManager<T1,T2>::TransitionHandler::fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& exception)
   {
      char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(exception.getMessage());
      dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__, "Fatal Error:%s, at line %d",message, exception.getLineNumber());
   }

   template< typename T1, typename T2 >
   void StateManager<T1,T2>::TransitionHandler::endElement(const XMLCh* const name)
   {
      std::string elementName = XERCES_CPP_NAMESPACE::XMLString::transcode(name);

      if (elementName == "Transition")
      {
         ///\todo : Is 'elementName' correct here?
         StateManager<T1,T2>::Instance()->AddTransition( mEventType, mFromState.get(), mToState.get() );
      }
   }
}

#endif // DELTA_STATEMANAGER
