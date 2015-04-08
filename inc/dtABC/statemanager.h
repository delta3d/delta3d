#ifndef DELTA_STATEMANAGER
#define DELTA_STATEMANAGER

#include <cassert>
#include <iostream>
#include <functional>  // for std::binary_predicate
#include <map>
#include <string>

#include <dtCore/base.h>  // for base class
#include <dtABC/event.h>
#include <dtABC/state.h>
#include <dtUtil/log.h>
#include <dtUtil/objectfactory.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/xercesutils.h>

#include <xercesc/sax2/ContentHandler.hpp>  // for a base class
#include <xercesc/util/XMLString.hpp>

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
         bool first_less(x.first < y.first);
         if (first_less)
         {
            return true;
         }

         bool first_greater(y.first < x.first);
         if (first_greater)
         {
            return false;
         }

         // else, key off the second element, and use the RefPtrWithNameCompare comparison
         RefPtrWithNameCompare<typename T::second_type> compare_them;
         return compare_them(x.second,y.second);
      }
   };

   /** \brief A class to manage State transitions due to an Event.
     *
     * Controls the switching of modes by starting and stopping the different states.
     * When a new state is started Config is called and Shutdown is called before
     * switching. It derives from dtCore::Base
     * so that it can fire Events.
     */
   class DT_ABC_EXPORT StateManager : public dtCore::Base
   {
   public:
       /** \brief An Event class specific to StateManager.
         *
         * TransitionOccurredEvent is an Event which can be watched
         * by classes interested in such an Event.
         */
       class TransitionOccurredEvent : public dtABC::Event
       {
       public:
          TransitionOccurredEvent(State* from, State* to)
             : dtABC::Event(&StateManager::EventType::TRANSITION_OCCURRED)
             , mFrom(from)
             , mTo(to)
          {
          }

          dtCore::RefPtr<const State> mFrom, mTo;

       protected:
          virtual ~TransitionOccurredEvent()
          {
          }
       };

      /// Event::Type instances to be used by StateManager
      class DT_ABC_EXPORT EventType : public dtABC::Event::Type
      {
         DECLARE_ENUM(EventType);
      protected:
         EventType(const std::string& name);
         virtual ~EventType();

      public:
         static const EventType TRANSITION_OCCURRED;
      };

   private:
      /** A class to handle XML elements from the SAX parser.
        * It is used when ParseFile is called.
        */
      template< typename ET, typename ST >
      class TransitionHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
      {
      public:
         typedef ET EventType;
         typedef ST StateType;

         TransitionHandler(dtABC::StateManager* mgr): mManager(mgr) {}
         ~TransitionHandler() {}

         // inherited pure virtual functions
#if XERCES_VERSION_MAJOR < 3
         virtual void characters(const XMLCh* const chars, const unsigned int length) {}
         virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}
#else
         virtual void characters(const XMLCh* const /*chars*/, const XMLSize_t /*length*/) {}
         virtual void ignorableWhitespace(const XMLCh* const /*chars*/, const XMLSize_t /*length*/) {}
#endif
         virtual void endDocument() {}
         virtual void processingInstruction(const XMLCh* const /*target*/, const XMLCh* const /*data*/) {}
         virtual void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const /*locator*/) {}
         virtual void startDocument() {}
         virtual void startPrefixMapping(const XMLCh* const /*prefix*/,const XMLCh* const /*uri*/) {}
         virtual void endPrefixMapping(const XMLCh* const /*prefix*/) {}
         virtual void skippedEntity(const XMLCh* const /*name*/) {}

         virtual void startElement(const XMLCh* const uri,
                                   const XMLCh* const localname,
                                   const XMLCh* const qname,
                                   const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);

         virtual void endElement(const XMLCh* const /*uri*/,
                                 const XMLCh* const localname,
                                 const XMLCh* const /*qname*/)
         {
            char* ename = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(localname);
            std::string elementName(ename);

            if (elementName == "Transition")
            {
               mManager->AddTransition(mEventType, mFromState.get(), mToState.get());
            }

            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&ename);
         }

      private:
         TransitionHandler();   /// not implemented by design
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
      typedef std::pair<const dtABC::Event::Type*, StatePtr> EventStatePtrPair;

      /** An ObjectFactory used to create concrete instances of user-defined Events.*/
      typedef dtUtil::ObjectFactory<const dtABC::Event::Type*, dtABC::Event> EventFactory;

      /** An ObjectFactory used to create concrete instances of user-defined States.*/
      typedef dtUtil::ObjectFactory<const dtABC::State::Type*, dtABC::State> StateFactory;

      /** The set of unique States.*/
      typedef std::set< StatePtr, RefPtrWithNameCompare<StatePtr> > StatePtrSet;

      /** \brief The map of transitions.
        * The transition map defined by the unique pair, composed of the 'from' State and the Event,
        * which maps to a 'to' State.*/
      typedef std::map< EventStatePtrPair, StatePtr, PairRefPtrWithNameCompare<EventStatePtrPair> > TransitionMap;

      /** Overloaded for desired actions to occur before drawing.*/
      void PreFrame(const double deltaFrameTime);

      /** Overloaded for desired actions to occur during drawing.*/
      void Frame(const double deltaFrameTime);

      /** Overloaded for desired actions to occur during drawing.*/
      void PostFrame(const double deltaFrameTime);

      /** Overloaded to handle messages.*/
      void OnMessage(MessageData* data);

      /** Loads an XML file specifying State Transitions.
        * The parser will add transitions to this StateManager instance, based on the XML file.
        * EventT is the user defined Event::Type type and StateT is the user defined State::Type type.
        *
        * @param filename is the complete file path.
        */
      template<typename EventT, typename StateT>
      bool Load(const std::string& filename);

      /** Add a new State to the set of States.*/
      bool AddState(State* state);

      /** Removes a State from the set of States and associated transitions from the TransitionMap.*/
      bool RemoveState(State* state);

      /** Add a new transition to the map of transitions.*/
      bool AddTransition(const Event::Type* eventType, State* from, State* to);

      /** Remove a transition from the map of transitions.*/
      bool RemoveTransition(const Event::Type* eventType, State* from, State* to);

      /** Return a non-const State by specifying the name.*/
      State* GetState(const std::string& name);

      /** Return a const State by specifying its name.*/
      const State* GetState(const std::string& name) const;

      /** Clear the set of States.*/
      void RemoveAllStates();

      /** Returns the set of states.*/
      const StatePtrSet& GetStates() const { return mStates; }

      /** Returns the transition map.*/
      const TransitionMap& GetTransitions() const { return mTransitions; }

      /** Determines the number of events for the State.*/
      unsigned int GetNumOfEvents(const State* from) const;

      /** \brief Fills a vector of Events which cause transitions for the specified State.
        * This method should be used with GetNumOfEvents.
        * \sa GetNumOfEvents.
        *
        * @param from is the State of interest.
        * @param events is an already allocated std::vector of Event::Type pointers.
        */
      void GetEvents(const State* from, std::vector<const Event::Type*>& events);

      /** Returns a pointer to current state.  Can be 0 if no current state is assigned. */
      State* GetCurrentState();

      /** Returns a pointer to current state.  Can be 0 if no current state is assigned. */
      const State* GetCurrentState() const;

      /** Forces the given State to now be the 'current' State.*/
      void MakeCurrent(State* state);

      /** \brief Register a user defined, concrete Event.
        * Register the user defined Event so that the EventFactory can create such an Event, especially when needed for XML loading.
        * T is the user defined, concrete event, to be registered.
        * @param eventType is the user defined unique identifier for to Event being registered.
        */
      template<typename T>
      bool RegisterEvent(const Event::Type* eventType)
      {
         return mEventFactory->template RegisterType<T>(eventType);
      }

      /** \brief Register a user defined, concrete State.
        * Register the user defined State so that the StateFactory can create such a State, specifically needed for XML loading.
        * T is the user defined, concrete State, to be registered.
        * @param stateType is the user defined unique identifier for to State being registered.
        */
      template<typename T>
      bool RegisterState(const State::Type* stateType)
      {
         return mStateFactory->template RegisterType<T>(stateType);
      }

      /** Return the non-const instance of the StateFactory.*/
      StateFactory* GetStateFactory() { return mStateFactory.get(); }

      /** Return the const instance of the StateFactory.*/
      const StateFactory* GetStateFactory() const { return mStateFactory.get(); }

      /** Return the non-const instance of the StateFactory.*/
      EventFactory* GetEventFactory() { return mEventFactory.get(); }

      /** Return the const instance of the StateFactory.*/
      const EventFactory* GetEventFactory() const { return mEventFactory.get(); }

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
      template<typename EventT, typename StateT>
      bool ParseFile(const std::string& filename);

   private:
      dtCore::RefPtr<State>        mCurrentState;  /// The handle to the current State.
      dtCore::RefPtr<Event>        mLastEvent;     /// The last Event used causing transition???
      StatePtrSet                  mStates;        /// The set of States.
      TransitionMap                mTransitions;   /// The map of transitions.
      bool                         mSwitch;        /// a helper to know when to actually perform a transition
      dtCore::RefPtr<EventFactory> mEventFactory;  /// the ObjectFactory of Events.
      dtCore::RefPtr<StateFactory> mStateFactory;  /// the ObjectFactory of States.
   };

   template<typename EventT, typename StateT>
   bool StateManager::Load(const std::string& filename)
   {
      std::string fullFileName = dtUtil::FindFileInPathList(filename);
      if (fullFileName.empty())
      {
         LOG_WARNING("StateManager - Can't find file " + filename)
         return false;
      }
      return ParseFile<EventT,StateT>(fullFileName);
   }

   template<typename EventT, typename StateT>
   bool StateManager::ParseFile(const std::string& filename)
   {
      typedef TransitionHandler<EventT,StateT> XMLElementHandler;  // a convenience typedef
      XMLElementHandler handler(this);                             // the element handler

      dtUtil::XercesParser parser;
      return parser.Parse(filename, handler, "transitionlist.xsd");
   }

   template<typename T1, typename T2>
   void StateManager::TransitionHandler<T1,T2>::startElement(const XMLCh* const /*uri*/,
                                                             const XMLCh* const localname,
                                                             const XMLCh* const /*qname*/,
                                                             const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
   {
      char* elementName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(localname);
      std::string ename(elementName);
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&elementName);

      dtUtil::AttributeSearch attrsearch;
      dtUtil::AttributeSearch::ResultMap results = attrsearch(attrs);

      dtUtil::AttributeSearch::ResultMap::iterator typeiter = results.find("Type");
      dtUtil::AttributeSearch::ResultMap::iterator nameiter = results.find("Name");

      if (ename == "Transition")
      {
         LOG_INFO("Found a State Transition XML element.")
      }
      else if (ename == "Event")
      {
         if (typeiter != results.end())
         {
            mEventType = EventType::GetValueForName((*typeiter).second);
         }
         else
         {
            LOG_ERROR("Transition file not structured properly at Event, "+ename+", requires Type attributes.")
         }
      }
      else if (ename == "FromState")
      {
         if ((typeiter==results.end()) || (nameiter==results.end()))
         {
            LOG_ERROR("Transition file not structured properly at State, "+ename+", requires Type and Name attributes.")
         }

         std::string stateType("default"), stateName("default");
         if (typeiter != results.end())
         {
            stateType = (*typeiter).second;
         }
         if (nameiter != results.end())
         {
            stateName = (*nameiter).second;
         }

         //Do a check to see if a State exists with the same name and type name
         //If it does, use it, otherwise create a new State
         State *s = mManager->GetState(stateName);
         if (s == 0)
         {
            dtCore::RefPtr<StateManager::StateFactory> sf = mManager->GetStateFactory();
            mFromState = sf->CreateObject(StateType::GetValueForName(stateType));
         }
         else if (s->GetType()->GetName() != stateType)
         {
            dtCore::RefPtr<StateManager::StateFactory> sf = mManager->GetStateFactory();
            mFromState = sf->CreateObject(StateType::GetValueForName(stateType));
         }
         else
         {
            mFromState = s;
         }

         if (mFromState.valid())
         {
            mFromState->SetName(stateName);
         }
         else
         {
            LOG_WARNING(std::string("Loading can't create FromState, ") + stateType)
         }
      }
      else if (ename == "ToState")
      {
         if ((typeiter==results.end()) || (nameiter==results.end()))
         {
            LOG_ERROR("Transition file not structured properly at State, "+ename+", requires Type and Name attributes.")
         }

         std::string stateType("default"), stateName("default");
         if (typeiter != results.end())
         {
            stateType = (*typeiter).second;
         }
         if (nameiter != results.end())
         {
            stateName = (*nameiter).second;
         }

         //Do a check to see if a State exists with the same name and type name
         //If it does, use it, otherwise create a new State
         State *s = mManager->GetState(stateName);
         if (s == 0)
         {
            dtCore::RefPtr<StateManager::StateFactory> sf = mManager->GetStateFactory();
            mToState = sf->CreateObject(StateType::GetValueForName(stateType));
         }
         else if (s->GetType()->GetName() != stateType)
         {
            dtCore::RefPtr<StateManager::StateFactory> sf = mManager->GetStateFactory();
            mToState = sf->CreateObject(StateType::GetValueForName(stateType));
         }
         else
         {
            mToState = s;
         }

         if (mToState.valid())
         {
            mToState->SetName(stateName);
         }
         else
         {
            LOG_WARNING(std::string("StateManager Load() can't create ToState, ") + stateType)
         }
      }
      else if (ename == "StartState")
      {
         std::string stateName("default");
         if (nameiter != results.end())
         {
            stateName = (*nameiter).second;
         }
         else
         {
            LOG_ERROR("Transition file not structured properly at Start State, "+ename+", requires a Name attribute.")
         }
         mManager->MakeCurrent(mManager->GetState(stateName));
      }
   }

}  // end dtABC namespace

#endif // DELTA_STATEMANAGER
