#include "testdisapp.h"
#include <dtDIS/mastercomponent.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/coordinates.h>
#include <dtDAL/project.h>
#include <dtCore/rtsmotionmodel.h>

////////////////////////////////////////////////////////////////////////////////
TestDISApp::TestDISApp(const std::string& connectionXml,
                       const std::string& actorTypeMappingXml)
: mMotion(NULL)
, mConnectionXml(connectionXml)
, mActorTypeMapping(actorTypeMappingXml)

{

}

////////////////////////////////////////////////////////////////////////////////
TestDISApp::~TestDISApp()
{
   mGameManager->RemoveComponent(*mDISComponent);
   mGameManager->RemoveComponent( *mMessageProc );
   mMotion->SetTarget(NULL);
}

////////////////////////////////////////////////////////////////////////////////
void TestDISApp::Config()
{
   dtABC::Application::Config();

   mMotion = new dtCore::RTSMotionModel(GetKeyboard(), GetMouse());
   mMotion->SetTarget(GetCamera());

   dtDIS::SharedState* disConfig = NULL;

   disConfig = new dtDIS::SharedState(mConnectionXml, mActorTypeMapping);

   //Assuming that incoming positions are lat/lon and the local terrain is
   //centered around 0,0,0.  Note, this doesn't conform to DIS specs.
   disConfig->GetCoordinateConverter().SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC);
   disConfig->GetCoordinateConverter().SetFlatEarthOrigin(osg::Vec2(0.f, 0.f));

   mDISComponent = new dtDIS::MasterComponent(disConfig);
   mMessageProc = new dtGame::DefaultMessageProcessor(); //need this to get messages routed around

   mGameManager = new dtGame::GameManager(*this->GetScene());

   //need to set a ProjectContext so the ResouceActorProperty can find the StaticMesh resources
   const std::string context = dtUtil::FindFileInPathList("demoMap");
   dtDAL::Project::GetInstance().SetContext(context);
   mGameManager->ChangeMap("MyCoolMap");  //just for something to see

   mGameManager->AddComponent(*mDISComponent);
   mGameManager->AddComponent(*mMessageProc, dtGame::GameManager::ComponentPriority::HIGHEST);
}
