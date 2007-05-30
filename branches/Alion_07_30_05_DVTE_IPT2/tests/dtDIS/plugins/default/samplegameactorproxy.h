#ifndef _plugin_sample_game_actor_proxy_h_
#define _plugin_sample_game_actor_proxy_h_

#include <dtGame/gameactor.h>                  // for base class

#include <DIS/Orientation.h>                   // for member
#include <DIS/Vector3Double.h>                 // for member

namespace dtTest
{
   class SampleGameActorProxy : public dtGame::GameActorProxy
   {
   public:
      // properties
      SampleGameActorProxy();

      int mAppearance;
      DIS::Vector3Double mLocation;
      DIS::Orientation mOrientation;
      unsigned short mEntityIDEntity;
      unsigned short mEntityIDApplication;
      unsigned short mEntityIDSite;

      void CreateActor();
      void BuildPropertyMap();
      void BuildInvokables();

   protected:
      ~SampleGameActorProxy();
   };
}

#endif  // _plugin_sample_game_actor_proxy_h_
