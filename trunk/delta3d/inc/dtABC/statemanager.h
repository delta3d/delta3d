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

namespace dtABC
{
   //had to place this outside of the template so gcc won't gripe.
   /** Compares 2 referenced pointer objects by pointer value and name.*/
   template<typename T>
   struct RefPtrWithNameCompare : std::binary_function<T,T,bool>
   {
      /** RefPtrWithNameCompare will make sure the State being added is
        * unique to the set based on its name AND based on the fact
        * that the State has a unique place in memory.
        * This makes sure that no one tried to submit a State that
        * had the same name as another State, or someone tried to
        * resubmit a State already in the set by changing its name.
        */
      bool operator()(const T& lhs,const T& rhs) const
      {
         return lhs.get() != rhs.get() && lhs->GetName() < rhs->GetName();
      }
   };

   //had to place this outside of the template so gcc won't gripe.
   /** Compares a pair, but assumes the 2nd type is a referenced pointer.*/
   template<typename T>
   struct PairRefPtrWithNameCompare : public std::binary_function<T,T,bool>
   {
      /** Re-implement the default comparison algorithm for std::pair<T1,T2>::operator<,
        * but add smart StatePtr comparison with the RefPtrWithNameCompare predicate.
        * \sa http://www.sgi.com/tech/stl/pair.html
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
         
         // else, key off the second element, and use the RefPtrWithNameCompare comparison
         RefPtrWithNameCompare<typename T::second_type> compare_them;
         return compare_them( x.second,y.second );
      }
   };

   /** \brief A class to manage State transitions due to an Event.
     *
     * Controls the switching of modes by starting and stopping the different states.  
     * When a new state is started Config is called and Shutdown is called before 
     * switching.  The class is implemented as a singleton.  It derives from dtCore::Base
     * so that it can fire Events.
     */
   class DT_EXPORT StateManager : public dtCore::Base
   {
    private:
       /** \brief An Event class specific to StateManager. 
         *
         * TransitionOccurredEvent is an Event which can be watched
         * by classes interested in such an Event.
         */
       class TransitionOccurredEvent : public dtABC::Event
       {
       public:
          TransitionOccurredEvent() : dtABC::Event(&StateManager::EventType::TRANSITION_OCCURRED) {}
       protected:
          virtual ~TransitionOccurredEvent() {}
       };

      /// Event::Type instances to be used by StateManager
      class EventType : public dtABC::Event::Type
      {
         DECLARE_ENUM(EventType);
      protected:
         EventType(const std::string& name);
         virtual ~EventType();

      public:
         static const EventType TRANSITION_OCCURRED;
      };

      /** A class to handle XML elements from the SAX parser.
         * It is used when ParseFile is called.
         */
      template< typename ET, typename ST >
      class TransitionHandler : public XERCES_CPP_NAMESPACE_QUALIFIER HandlerBase
      {
      public:
         typedef ET EventType;
         typedef ST StateType;
         TransitionHandler(StateManager* sm);
         ~TransitionHandler();

         virtual void startElement(const XMLCh* const, XERCES_CPP_NAMESPACE_QUALIFIER AttributeList&);
         virtual void endElement(const XMLCh* const name);
         virtual void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException&);
         virtual void error( const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException &exc );
         virtual void warning( const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException &exc );

      private:
         TransitionHandler(); /// not implemented by design
         StateManager* mManager;
         dtCore::RefPtr<State> mFromState;
         dtCore::RefPtr<State> mToState;
         const Event::Type*    mEventType;
      };

   public:
      ///Constructor creates an instance of each state.
      StateManager();

      /** \brief a convenience typedef.*/
      typedef dtCore::RefPtr<dtABC::State> StatePtr;

      /** \brief a convenience typedef.*/
      typedef std::pair< const dtABC::Event::Type*, StatePtr > EventStatePtrPair;

      /** An ObjectFactory used to create concrete instances of user-defined Events.*/
      typedef dtUtil::ObjectFactory< const dtABC::Event::Type*, dtABC::Event > EventFactory;

      /** An ObjectFactory used to create concrete instances of user-defined States.*/
      typedef dtUtil::ObjectFactory< const dtABC::State::Type*, dtABC::State > StateFactory;

      /** The set of unique States.*/
      typedef std::set< StatePtr, RefPtrWithNameCompare<StatePtr> > StatePtrSet;

      /** \brief The map of transitions.
        * The transition map defined by the unique pair, composed of the 'from' State and the Event,
        * which maps to a 'to' State.*/
      typedef std::map< EventStatePtrPair, StatePtr, PairRefPtrWithNameCompare<EventStatePtrPair> > TransitionMap;

      /** Overloaded for desired actions to occur before drawing.*/
      void PreFrame( const double deltaFrameTime );

      /** Overloaded for desired actions to occur during drawing.*/
      void Frame( const double deltaFrameTime );

      /** Overloaded for desired actions to occur during drawing.*/
      void PostFrame( const double deltaFrameTime );

      /** Overloaded to handle messages.*/
      void OnMessage( MessageData* data );

      /** Loads an XML file specifying State Transitions.
        * The parser will add transitions to this StateManager instance, based on the XML file.
        * @param filename is the complete file path.
        */
      template< typename EventT, typename StateT>
      bool Load(const std::string& filename );

      /** Add a new State to the set of States.*/
      bool AddState( State* state );

      /** Removes a State from the set of States and associated transitions from the TransitionMap.*/
      bool RemoveState( State* state );  

      /** Add a new transition to the map of transitions.*/
      bool AddTransition( const Event::Type* eventType, State* from, State* to );

      /** Remove a transition from the map of transitions.*/
      bool RemoveTransition( const Event::Type* eventType, State* from, State* to );

      /** Return a non-const State by specifying the name.*/
      State* GetState( const std::string& name );

      /** Return a const State by specifying its name.*/
      const State* GetState( const std::string& name ) const;

      /** Clear the set of States.*/
      void RemoveAllStates();

      /** Returns the set of states.*/
      const StatePtrSet& GetStates() const { return mStates; }

      /** Returns the transition map.*/
      const TransitionMap& GetTransitions() const { return mTransitions; }

      /** Determines the number of events for the State.*/
      unsigned int GetNumOfEvents(const State* from) const;

      /** \brief Fills a vector of Events which cause transitions for the specified State.
        * @param from is the State of interest.
        * @param events is an already allocated std::vector of Event::Type pointers.
        * This method should be used with GetNumOfEvents.
        * \sa GetNumOfEvents.
        */
      void GetEvents(const State* from, std::vector<const Event::Type*>& events);

      /** Returns a pointer to current state.  Can be 0 if no current state is assigned. */
      State* GetCurrentState();

      /** Returns a pointer to current state.  Can be 0 if no current state is assigned. */
      const State* GetCurrentState() const;

      /** Forces the given State to now be the 'current' State.*/
      void MakeCurrent( State* state );

      /** \brief Register a user defined, concrete Event.
        * @param T is the user defined, concrete event, to be registered.
        * @param eventType is the user defined unique identifier for to Event being registered.
        */
      template<typename T>
      bool RegisterEvent( const Event::Type* eventType )
      {
         return mEventFactory->RegisterType<T>( eventType );     
      }

      /** \brief Register a user defined, concrete State.
        * @param T is the user defined, concrete State, to be registered.
        * @param stateType is the user defined unique identifier for to State being registered.
        */
      template<typename T>
      bool RegisterState( const State::Type* stateType )
      {
         return mStateFactory->RegisterType<T>( stateType );
      }

      /** Return the non-const instance of the StateFactory.*/
      StateFactory* GetStateFactory() { return mStateFactory.get(); }

      /** Return the const instance of the StateFactory.*/
      const StateFactory* GetStateFactory() const { return mStateFactory.get(); }

      /** Return the non-const instance of the StateFactory.*/
      EventFactory* GetEventFactory() { return mEventFactory.get(); }

      /** Return the const instance of the StateFactory.*/
      const EventFactory* GetEventFactory() const { return mEventFactory.get(); }

      /** options to be used with the Print function.*/
      enum PrintOptions
      {
         PRINT_STATES,
         PRINT_TRANSITIONS
      };

      /** Print the States names.*/
      void PrintStates() const;

      /** Print the Transition map.*/
      void PrintTransitions() const;

   protected:
      /** The protected virtual destructor for reference counted classes.*/
      virtual ~StateManager();

      /** The real parsing function.
        * Called from Load.
        * @param filename is the file path pointing to the XML file containing State Transitions.
        * \sa StateManager::Load
        */
      template< typename EventT, typename StateT >
      bool ParseFile(const std::string& filename );

   private:
      dtCore::RefPtr<State>        mCurrentState;  /// The handle to the current State.
      dtCore::RefPtr<Event>        mLastEvent;     /// The last Event used causing transition???
      StatePtrSet                  mStates;        /// The set of States.
      TransitionMap                mTransitions;   /// The map of transitions.
      bool                         mSwitch;        /// a helper to know when to actually perform a transition
      dtCore::RefPtr<EventFactory> mEventFactory;  /// the ObjectFactory of Events.
      dtCore::RefPtr<StateFactory> mStateFactory;  /// the ObjectFactory of States.
   };

   template< typename EventT, typename StateT >
   bool StateManager::Load(const std::string& filename )
   {
      bool retVal = false;
      std::string fullFileName = osgDB::findDataFile(filename);

      if (!fullFileName.empty())
      {
         retVal = ParseFile<EventT,StateT>(fullFileName);
      }
      else
      {
         dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__, "StateManager - Can't find file '%s'",filename.c_str());
         retVal = false;
      }
      return retVal;
   }

   template< typename EventT, typename StateT >
   bool StateManager::ParseFile(const std::string& filename )
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
      parser->setDoSchema(true);

      typedef TransitionHandler<EventT,StateT> XMLElementHandler;
      XMLElementHandler* docHandler = new XMLElementHandler(this);
      parser->setDocumentHandler(docHandler);

      XERCES_CPP_NAMESPACE::ErrorHandler* errHandler = (XERCES_CPP_NAMESPACE::ErrorHandler*) docHandler;
      parser->setErrorHandler(errHandler);

      try
      {
         parser->parse(filename.c_str());
         retVal = true;
      }
      catch (const XERCES_CPP_NAMESPACE::XMLException& e)
      {
         char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(e.getMessage());
         LOG_ERROR(std::string("Exception message is: ") + message)
         XERCES_CPP_NAMESPACE::XMLString::release(&message);
         retVal = false;
      }
      catch (const XERCES_CPP_NAMESPACE::SAXParseException& e)
      {
         //The error will already be logged by the errorHandler
         char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(e.getMessage());
         LOG_ERROR(std::string("An exception occurred while parsing file, ") + filename + std::string(", with message: ") + message)
         XERCES_CPP_NAMESPACE::XMLString::release(&message);
         retVal = false;;
      }
      catch (...) 
      {
         LOG_ERROR("An exception occurred while parsing file, " + filename)
         retVal = false;
      }

      delete parser;
      delete docHandler;

      return retVal;
   }

   template< typename T1, typename T2 >
   StateManager::TransitionHandler<T1,T2>::TransitionHandler(StateManager* sm): mManager(sm)
   {
   }

   template< typename T1, typename T2 >
   StateManager::TransitionHandler<T1,T2>::~TransitionHandler()
   {
   }

   template< typename T1, typename T2 >
   void StateManager::TransitionHandler<T1,T2>::startElement(const XMLCh* const name, XERCES_CPP_NAMESPACE::AttributeList& attributes)
   {
      std::string elementName = XERCES_CPP_NAMESPACE::XMLString::transcode(name);

      if (elementName == "Transition")
      {
         //start of a Transition
      }
      else if (elementName == "Event")
      {
         ///\todo change this to use "Type" because all Enumeration representations should be found via a "Type"
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
            dtCore::RefPtr<StateManager::StateFactory> sf = mManager->GetStateFactory();
            mFromState = sf->CreateObject( StateType::GetValueForName( stateType ) );
         }
         else if ( s->GetType()->GetName() != stateType )
         {
            dtCore::RefPtr<StateManager::StateFactory> sf = mManager->GetStateFactory();
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
            dtCore::RefPtr<StateManager::StateFactory> sf = mManager->GetStateFactory();
            mToState = sf->CreateObject( StateType::GetValueForName( stateType ) );
         }
         else if ( s->GetType()->GetName() != stateType )
         {
            dtCore::RefPtr<StateManager::StateFactory> sf = mManager->GetStateFactory();
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
         mManager->MakeCurrent( mManager->GetState(stateName) );
      }
   }

   template< typename T1, typename T2 >
   void StateManager::TransitionHandler<T1,T2>::fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& exception)
   {
      char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(exception.getMessage());
      dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_ERROR, __FUNCTION__, "Fatal:%s, at line %d",
                                             message, exception.getLineNumber());
      throw exception;
   }

   template< typename T1, typename T2 >
      void StateManager::TransitionHandler<T1,T2>::error(const XERCES_CPP_NAMESPACE::SAXParseException& exception)
   {
      char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(exception.getMessage());
      dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_ERROR, __FUNCTION__, "%s, at line %d",
                                             message, exception.getLineNumber());
      throw exception;
   }

   template< typename T1, typename T2 >
      void StateManager::TransitionHandler<T1,T2>::warning(const XERCES_CPP_NAMESPACE::SAXParseException& exception)
   {
      char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(exception.getMessage());
      dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, "---", "%s, at line %d",
                                              message, exception.getLineNumber());

   }


   template< typename T1, typename T2 >
   void StateManager::TransitionHandler<T1,T2>::endElement(const XMLCh* const name)
   {
      std::string elementName = XERCES_CPP_NAMESPACE::XMLString::transcode(name);

      if (elementName == "Transition")
      {
         ///\todo : Is 'elementName' correct here?
         mManager->AddTransition( mEventType, mFromState.get(), mToState.get() );
      }
   }

};  // end dtABC namespace

#endif // DELTA_STATEMANAGER
