#ifndef DELTA_STATEMANAGER
#define DELTA_STATEMANAGER

#include <iostream>
#include <map>
#include <string>

#include <dtCore/refptr.h>
#include <dtABC/state.h>
#include <dtABC/event.h>

#include <xercesc/sax/HandlerBase.hpp>

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
      typedef std::pair< std::string, dtCore::RefPtr<State> >     EventStatePair;
      typedef std::map< EventStatePair, dtCore::RefPtr<State> >   EventMap;

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

      bool           AddTransition( std::string eventType, State* from, State* to );
      bool           RemoveTransition( std::string eventType, State* from, State* to );

      /// Returns the transition map
      inline const   EventMap& GetTransitions() const;

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
      Event*                                 mLastEvent;
      std::set< dtCore::RefPtr<State> >      mStates;
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
         State *mFromState;
         State *mToState;
         std::string mEventTypeName;
      };
      
   };

}

#endif // DELTA_STATEMANAGER
