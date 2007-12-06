#include "[!output PROJECT_NAME]EntryPoint.h"
#include <dtGame/gameapplication.h>
[!if DMP_CHECKBOX]
#include <dtGame/defaultmessageprocessor.h>
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
   dtGame::DefaultMessageProcessor *messageProc = new dtGame::DefaultMessageProcessor();
   app.GetGameManager()->AddComponent( *messageProc, dtGame::GameManager::ComponentPriority::HIGHEST);
[!endif]

[!if INPUT_CHECKBOX]
   [!output PROJECT_NAME]Input *inp = new [!output PROJECT_NAME]Input();
   app.GetGameManager()->AddComponent( *inp, dtGame::GameManager::ComponentPriority::NORMAL );
[!endif]

}

void [!output PROJECT_NAME]EntryPoint::OnShutdown( dtGame::GameApplication &app )
{

}
