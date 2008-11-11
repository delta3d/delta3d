#ifndef DELTA_EVENT
#define DELTA_EVENT

#include <dtCore/base.h>
#include <dtUtil/enumeration.h>
#include <dtABC/export.h>

namespace dtABC
{

   /**
   *  The Event class is specific to the state manager where an
   *  event is an object whose Type can cause a Transition.
   *  This class is meant to be derived from, to see a complete
   *  usage example, goto: testStateManager
   */
   class DT_ABC_EXPORT Event : public osg::Referenced
   {
      public:

         /**
         * This Type is the internal key to events.
         * A specific Type is what causes a transition
         * between states.
         */
         typedef dtUtil::Enumeration Type;

         Event( const Type* type ) : mType(type) {}
        
         virtual const Type* GetType() const { return mType; }

      protected:

         //pure virutal destructor to force abstract class
         virtual ~Event() = 0;

         const Type* mType;

   };

}

#endif //DELTA_EVENT
