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
}

class TestDISApp : public dtABC::Application
{
public:
   TestDISApp(const std::string& connectionXml, const std::string& actorTypeMappingXml);
   virtual void Config();

protected:
   virtual ~TestDISApp();

private:
   dtCore::RefPtr<dtDIS::MasterComponent> mDISComponent;
   dtCore::RefPtr<dtGame::DefaultMessageProcessor> mMessageProc;
   dtCore::RefPtr<dtGame::GameManager> mGameManager;
   std::string mConnectionXml;
   std::string mActorTypeMapping;
};
#endif // testdisapp_h__
