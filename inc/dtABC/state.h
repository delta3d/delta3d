#ifndef DELTA_STATE
#define DELTA_STATE

#include <dtCore/base.h>
#include <dtUtil/enumeration.h>
#include <dtABC/export.h>

namespace dtABC
{
   // forward references
   class Event;

   ///Base class for allowing the application to switch between different modes such as the splash screen, menu, and the Game.
   class DT_ABC_EXPORT State : public dtCore::Base
   {
      public:

         typedef dtUtil::Enumeration Type;

         State( const Type* type, std::string name );

      protected:

         virtual ~State();

      public:

         virtual const Type* GetType() const { return mType; }

         ///This function must be implemented with desired functionality on an event transistion
         virtual void HandleEvent( Event* event ) = 0;

         ///The place to clean up memory when before switching to a different mode
         virtual void Shutdown();

         virtual void PreFrame( const double deltaFrameTime );
         virtual void Frame( const double deltaFrameTime );
         virtual void PostFrame( const double deltaFrameTime );

      protected:

         const Type* mType;

      private:

         State();  // not implemented by design
   };
}

#endif // DELTA_STATE
