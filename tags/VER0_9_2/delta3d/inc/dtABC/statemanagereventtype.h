#ifndef _dtabc_statemanagerevent_
#define _dtabc_statemanagerevent_

#include "dtABC/event.h"  // for base class
#include <string>

namespace dtABC
{
   /// Event::Type instances to be used by StateManager
   class DT_EXPORT StateManagerEventType : public dtABC::Event::Type
   {
   private:
      StateManagerEventType(const std::string& name) : dtABC::Event::Type(name)
      {
         AddInstance(this); 
      }

   public:
      DECLARE_ENUM(StateManagerEventType);

      static const StateManagerEventType TRANSITION_OCCURRED;
   };
};

#endif  // _dtabc_statemanagerevent_
