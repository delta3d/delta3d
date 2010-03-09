#include "testdisapp.h"
#include <dtDIS/mastercomponent.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtActors/engineactorregistry.h>
#include <dtUtil/datapathutils.h>

////////////////////////////////////////////////////////////////////////////////
TestDISApp::TestDISApp(const std::string& connectionXml,
                       const std::string& actorTypeMappingXml)
: mConnectionXml(connectionXml)
, mActorTypeMapping(actorTypeMappingXml)
{

}

////////////////////////////////////////////////////////////////////////////////
TestDISApp::~TestDISApp()
{
   mGameManager->RemoveComponent(*mDISComponent);
   mGameManager->RemoveComponent( *mMessageProc );

}

////////////////////////////////////////////////////////////////////////////////
void TestDISApp::Config()
{
   dtDIS::SharedState* disConfig = NULL;

   disConfig = new dtDIS::SharedState(mConnectionXml, mActorTypeMapping);

   mDISComponent = new dtDIS::MasterComponent(disConfig);
   mMessageProc = new dtGame::DefaultMessageProcessor();

   mGameManager = new dtGame::GameManager(*this->GetScene());

   mGameManager->AddComponent(*mDISComponent);
   mGameManager->AddComponent(*mMessageProc, dtGame::GameManager::ComponentPriority::HIGHEST);
}
