#ifndef _dtdis_net_idisplugin_h_
#define _dtdis_net_idisplugin_h_

#include <vector>                       // for typedef, param
#include <dtCore/refptr.h>              // for typedef, param
#include <dtGame/gameactor.h>           // for typedef, param

#include <dtDIS/dtdisexport.h>          // for library export symbols

namespace DIS
{
   class IncomingMessage;
}

namespace dtGame
{
   class GameManager;
   class GameActorProxy;
}

namespace dtDIS
{
   class OutgoingMessage;
   class SharedState;

   /// the interface for all DIS plugins
   class DT_DIS_EXPORT IDISPlugin
   {
   public:
      virtual ~IDISPlugin();

      /// plugin is informed of the state of the game.
      /// called after being successfully loaded.
      /// @param incoming used to attach packet handlers.
      /// @param outgoing used to attach message handlers.
      /// @param gm used to deliver the state of the game.
      /// @param config used to share connection and setup data.
      virtual void Start(DIS::IncomingMessage& incoming,
                         dtDIS::OutgoingMessage& outgoing,
                         dtGame::GameManager* gm,
                         dtDIS::SharedState* config)=0;

      /// plugin should forget the state of the game.
      /// called before unloading.
      virtual void Finish(DIS::IncomingMessage& incoming,
                          dtDIS::OutgoingMessage& outgoing)=0;
   };
}

#endif  //  _dtdis_net_idisplugin_h_
