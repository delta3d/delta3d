#ifndef testdisapp_h__
#define testdisapp_h__

#include <dtABC/application.h>
#include <dtCore/refptr.h>

namespace dtDIS
{
   class MasterComponent;
}

namespace dtGame
{
   class GameManager;
   class DefaultMessageProcessor;
   class DefaultNetworkPublishingComponent;
}

namespace dtCore
{
   class RTSMotionModel;
}

namespace dtCore
{
   class BaseActorObject;
}

class TestDISApp : public dtABC::Application
{
public:
   TestDISApp(const std::string& connectionXml, const std::string& actorTypeMappingXml);
   virtual void Config();

protected:
   virtual ~TestDISApp();
   virtual void PostFrame(const double deltaSimTime);

private:
   dtCore::RefPtr<dtDIS::MasterComponent> mDISComponent;
   dtCore::RefPtr<dtGame::DefaultMessageProcessor> mMessageProc;
   dtCore::RefPtr<dtGame::DefaultNetworkPublishingComponent> mNetworkingRouter;
   dtCore::RefPtr<dtGame::GameManager> mGameManager;
   dtCore::RefPtr<dtCore::RTSMotionModel> mMotion;
   std::string mConnectionXml;
   std::string mActorTypeMapping;
   dtCore::ActorPtrVector mActorsToPublish;

   void FindActorsAndAddComponents();
};
#endif // testdisapp_h__
