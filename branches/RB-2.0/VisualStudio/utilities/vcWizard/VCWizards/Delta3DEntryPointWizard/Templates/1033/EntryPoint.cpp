#include "[!output PROJECT_NAME]EntryPoint.h"
#include <dtGame/gameapplication.h>
[!if DMP_CHECKBOX]
#include <dtGame/defaultmessageprocessor.h>
[!endif]
[!if DNPC_CHECKBOX]
#include <dtGame/defaultnetworkpublishingcomponent.h>
[!endif]

[!if INPUT_CHECKBOX]
#include "[!output PROJECT_NAME]Input.h"
[!endif]

extern "C" ENTRY_POINT_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new [!output PROJECT_NAME]EntryPoint;
}

extern "C" ENTRY_POINT_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}

[!output PROJECT_NAME]EntryPoint::[!output PROJECT_NAME]EntryPoint()
{

}

[!output PROJECT_NAME]EntryPoint::~[!output PROJECT_NAME]EntryPoint()
{

}

void [!output PROJECT_NAME]EntryPoint::Initialize( dtGame::GameApplication& app, int argc, char **argv )
{

}

void [!output PROJECT_NAME]EntryPoint::OnStartup( dtGame::GameApplication &app )
{
   //Create and add GMComponents
[!if DMP_CHECKBOX]
   mMessageProc = new dtGame::DefaultMessageProcessor();
   app.GetGameManager()->AddComponent( *mMessageProc, dtGame::GameManager::ComponentPriority::HIGHEST);
[!endif]

[!if DNPC_CHECKBOX]
   mNetworkComponent = new dtGame::DefaultNetworkPublishingComponent();
   app.GetGameManager()->AddComponent( *mNetworkComponent, dtGame::GameManager::ComponentPriority::NORMAL);
[!endif]

[!if INPUT_CHECKBOX]
   mInputComponent = new [!output PROJECT_NAME]Input();
   app.GetGameManager()->AddComponent( *mInputComponent, dtGame::GameManager::ComponentPriority::NORMAL );
[!endif]

}

void [!output PROJECT_NAME]EntryPoint::OnShutdown( dtGame::GameApplication &app )
{
[!if DMP_CHECKBOX]
   app.GetGameManager()->RemoveComponent( *mMessageProc );
[!endif]
[!if DNPC_CHECKBOX]
   app.GetGameManager()->RemoveComponent( *mNetworkComponent );
[!endif]
[!if INPUT_CHECKBOX]
   app.GetGameManager()->RemoveComponent( *mInputComponent );
[!endif]
}
