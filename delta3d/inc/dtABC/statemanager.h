#ifndef DELTA_STATEMANAGER
#define DELTA_STATEMANAGER

#include <iostream>
#include <map>
#include <string>

#include <dtCore/refptr.h>
#include <dtABC/state.h>
#include <dtABC/event.h>

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

      static   StateManager*  Instance();
      static   void           Destroy();

               void           PreFrame( const double deltaFrameTime );
               void           Frame( const double deltaFrameTime );
               void           PostFrame( const double deltaFrameTime );

               void           OnMessage( MessageData* data );

               bool           AddState( State* state );
               bool           RemoveState( State* state );

               bool           AddTransition( std::string eventType, State* from, State* to );
               bool           RemoveTransition( std::string eventType, State* from, State* to );

               ///Returns pointer to current state.
      inline   State*         Current();
      inline   const State*   Current() const;

               void           MakeCurrent( State* state );

               ///Calls shutdown on currently executing state.
               void           Stop();

               //TODO: make this the << operater?
               ///Prints list of all states and transitions
               void           Print( bool stateBased = false ) const;

   private:

      typedef std::pair< std::string, dtCore::RefPtr<State> >     EventStatePair;
      typedef std::map< EventStatePair, dtCore::RefPtr<State> >   EventMap;

      static dtCore::RefPtr<StateManager>    mManager;
      dtCore::RefPtr<State>                  mCurrentState;
      Event*                                 mLastEvent;
      std::set< dtCore::RefPtr<State> >      mStates;
      EventMap                               mTransition;
      bool                                   mSwitch;
      bool                                   mStop;
 
   };

}

#endif // DELTA_STATEMANAGER
