#ifndef DELTA_TRACKER
#define DELTA_TRACKER

// tracker.h: Declaration of the Tracker class.
//
//////////////////////////////////////////////////////////////////////


#include <string>

#include "isense.h"

#include "inputdevice.h"


namespace dtCore
{
   /**
    * A tracker device.
    */
   class DT_EXPORT Tracker : public InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(Tracker)


      public:

         /**
          * Creates instances of Tracker corresponding to each
          * connected tracker device.
          */
         static void CreateInstances();

         /**
          * Destroys all Tracker instances.
          */
         static void DestroyInstances();

         /**
          * Polls all Tracker instances.
          */
         static void PollInstances();

         /**
          * Manually polls the state of this tracker, updating
          * all of its features.
          */
         void Poll();


      private:

         /**
          * Constructor.
          *
          * @param name the instance name
          * @param trackerHandle the handle of the tracker device
          */
         Tracker(std::string name, ISD_TRACKER_HANDLE trackerHandle);

         /**
          * Destructor.
          */
         virtual ~Tracker();

         /**
          * The InterSense tracker handle.
          */
         ISD_TRACKER_HANDLE mTrackerHandle;
   };
};


#endif // DELTA_TRACKER
