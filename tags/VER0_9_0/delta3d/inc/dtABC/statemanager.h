#ifndef DELTA_STATEMANAGER
#define DELTA_STATEMANAGER

#include <iostream>
#include <map>
#include <string>

#include <dtCore/refptr.h>
#include <dtABC/state.h>
#include <dtABC/event.h>

#include <xercesc/sax/HandlerBase.hpp>

#include <functional>  // for std::binary_predicate

namespace dtABC
{
   ///Controls the switching of modes by starting and stoping the different states.  
   ///When a new state is started Config is called and Shutdown is called before 
   ///switching.
   class DT_EXPORT StateManager : public dtCore::Base
   {
      DECLARE_MANAGEMENT_LAYER(StateManager)

    private:

      ///Constructor creates an instance of each state.
      StateManager();

      ///Calls the destructor for each state instance by iterating mList.
      virtual ~StateManager();

   public:
      typedef dtCore::RefPtr<State>             StatePtr;
      typedef std::pair<std::string, StatePtr>  EventStatePtrPair;

      struct StatePtrLess : public std::binary_function<StatePtr,StatePtr,bool>
      {
         /** \brief Comparison object that keys off the name of a \class State .
           */
         bool operator()(const StatePtr& lhs,const StatePtr& rhs) const
         {
            return ( lhs->GetName() < rhs->GetName() );//&& (lhs->GetType() < rhs->GetType()))
         }
      };
      typedef std::set<StatePtr,StatePtrLess>                                 StatePtrSet;

      struct EventStatePtrPairLess : public std::binary_function<EventStatePtrPair,EventStatePtrPair,bool>
      {
         /** Re-implement the default comparison algorithm for std::pair<T1,T2>::operator<,
           * \sa http://www.sgi.com/tech/stl/pair.html ,
           * but add smart StatePtr comparison with the StatePtrLess predicate.
           */
         bool operator()(const EventStatePtrPair& x, const EventStatePtrPair& y) const
         {
            // try to use the first element
            bool first_less( x.first < y.first );
            if( first_less )
               return true;

            bool first_greater( y.first < x.first );
            if( first_greater )
               return false;

            // else, key off the second element, and use the StatePtr comparison
            StatePtrLess compare_them;
            return compare_them( x.second,y.second );
         }
      };

      typedef std::map<EventStatePtrPair, StatePtr, EventStatePtrPairLess>    EventMap;

      static   StateManager*  Instance();
      static   void           Destroy();

      bool           Load( std::string filename );

      void           PreFrame( const double deltaFrameTime );
      void           Frame( const double deltaFrameTime );
      void           PostFrame( const double deltaFrameTime );

      void           OnMessage( MessageData* data );

      bool           AddState( State* state );
      bool           RemoveState( State* state );
      State*         GetState( const std::string& name );

      bool           AddTransition(const std::string& eventType, State* from, State* to );
      bool           RemoveTransition(const std::string& eventType, State* from, State* to );

      /// Returns the set of states
      inline const   StatePtrSet&   GetStates() const;
      /// Returns the transition map
      inline const   EventMap&      GetTransitions() const;

      /// Determines the number of events for the State
      unsigned int   GetNumOfEvents(const State* from) const;

      /// \brief This method should be used with \sa GetNumOfEvents
      void           GetEvents(const State* from, std::vector<std::string>& events);

      ///Returns pointer to current state.
      inline   State*         Current();
      inline   const State*   Current() const;

      void           MakeCurrent( State* state );

      ///Calls shutdown on currently executing state.
      void           Stop();

      //\todo: make this the << operater?
      ///Prints list of all states and transitions
      void           Print( bool stateBased = false ) const;

   private:
      static dtCore::RefPtr<StateManager>    mManager;
      dtCore::RefPtr<State>                  mCurrentState;
      dtCore::RefPtr<Event>                  mLastEvent;
      StatePtrSet                            mStates;
      EventMap                               mTransition;
      bool                                   mSwitch;
      bool                                   mStop;

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
         std::string mEventTypeName;
      };
      
   };

}

#endif // DELTA_STATEMANAGER