#ifndef DELTA_STATE
#define DELTA_STATE

#include <dtCore/base.h>

namespace dtABC
{
   // forward references
   class    Event;
   class    StateManager;
   struct   StateData;

   ///Base class for allowing the application to switch between different modes such as the splash screen, menu, and the Game.
   class DT_EXPORT State : public dtCore::Base
   {
      DECLARE_MANAGEMENT_LAYER(State)

      friend class StateManager;

      public:

         State( std::string type = "state" );
         virtual ~State();

      protected:

         ///Called after construction to pass in static data (perhaps from XML)
         virtual void Config( StateData* data = 0 ) = 0;

         ///Called after switching into a particular mode.  Used to setup states of the mode before executing main loop.
         virtual void Enable( Event* data = 0 ) = 0;

         ///The place to clean up memory when before switching to a different mode
         virtual void Shutdown() {};

         virtual void PreFrame( const double deltaFrameTime ) {}
         virtual void Frame( const double deltaFrameTime ) {}
         virtual void PostFrame( const double deltaFrameTime ) {}

   };
}

#endif // DELTA_STATE
