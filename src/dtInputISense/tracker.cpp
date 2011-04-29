// tracker.cpp: Implementation of the Tracker class.
//
//////////////////////////////////////////////////////////////////////
//#include <prefix/dtcoreprefix.h>
#include <dtInputISense/tracker.h>
#include <sstream>

using namespace dtInputISense;

IMPLEMENT_MANAGEMENT_LAYER(Tracker)

/**
 * Creates instances of Tracker corresponding to each
 * connected tracker device.
 */
void Tracker::CreateInstances()
{
   ISD_TRACKER_HANDLE trackerHandle;

   trackerHandle = ISD_OpenTracker(0, 0, FALSE, FALSE);

   if (trackerHandle > 0)
   {
      new Tracker("tracker", trackerHandle);
   }
}

/**
 * Destroys all Tracker instances.
 */
void Tracker::DestroyInstances()
{
   while (GetInstanceCount() > 0)
   {
      delete GetInstance(0);
   }
}

/**
 * Polls all Tracker instances.
 */
void Tracker::PollInstances()
{
   for (int i = 0; i < GetInstanceCount(); ++i)
   {
      GetInstance(i)->Poll();
   }
}

/**
 * Constructor.
 *
 * @param name the instance name
 * @param trackerHandle the handle of the tracker device
 */
Tracker::Tracker(const std::string& name, ISD_TRACKER_HANDLE trackerHandle)
   : InputDevice(name)
   , mTrackerHandle(trackerHandle)
{
   RegisterInstance(this);

   for (int i = 0; i < ISD_MAX_STATIONS; ++i)
   {
      std::ostringstream bufs[6];

      bufs[0] << GetName() << " station " << i << ", x axis";

      AddFeature(
         new dtCore::Axis(this, bufs[0].str())
      );

      bufs[1] << GetName() << " station " << i << ", y axis";

      AddFeature(
         new dtCore::Axis(this, bufs[1].str())
      );

      bufs[2] << GetName() << " station " << i << ", z axis";

      AddFeature(
         new dtCore::Axis(this, bufs[2].str())
      );

      bufs[3] << GetName() << " station " << i << ", azimuth";

      AddFeature(
         new dtCore::Axis(this, bufs[3].str())
      );

      bufs[4] << GetName() << " station " << i << ", elevation";

      AddFeature(
         new dtCore::Axis(this, bufs[4].str())
      );

      bufs[5] << GetName() << " station " << i << ", roll";

      AddFeature(
         new dtCore::Axis(this, bufs[5].str())
      );
   }
}

/**
 * Destructor.
 */
Tracker::~Tracker()
{
   DeregisterInstance(this);

   ISD_CloseTracker(mTrackerHandle);
}

/**
 * Manually polls the state of this tracker, updating
 * all of its features.
 */
void Tracker::Poll()
{
   if (mTrackerHandle > 0)
   {
      ISD_TRACKING_DATA_TYPE data;

      ISD_GetTrackingData(mTrackerHandle, &data);

      for (int i = 0; i < ISD_MAX_STATIONS; ++i)
      {
         GetAxis(i*6+0)->SetState(data.Station[i].Position[0]);

         GetAxis(i*6+1)->SetState(data.Station[i].Position[1]);

         GetAxis(i*6+2)->SetState(data.Station[i].Position[2]);

         GetAxis(i*6+3)->SetState(data.Station[i].Euler[0]);

         GetAxis(i*6+4)->SetState(data.Station[i].Euler[1]);

         GetAxis(i*6+5)->SetState(data.Station[i].Euler[2]);
      }
   }
}
