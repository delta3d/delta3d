#include "[!output PROJECT_NAME]EntryPoint.h"

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

}

void [!output PROJECT_NAME]EntryPoint::OnShutdown( dtGame::GameApplication &app )
{

}
