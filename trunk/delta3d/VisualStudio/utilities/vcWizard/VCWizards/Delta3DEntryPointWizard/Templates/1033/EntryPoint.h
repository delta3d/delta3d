#ifndef EntryPoint_h__
#define EntryPoint_h__

#include "EntryPointExport.h"
#include <dtGame/gameentrypoint.h>

class ENTRY_POINT_EXPORT [!output PROJECT_NAME]EntryPoint : public dtGame::GameEntryPoint
{
public:
   [!output PROJECT_NAME]EntryPoint();

   virtual void Initialize(dtGame::GameApplication& app, int argc, char **argv);

   virtual void OnStartup(dtGame::GameApplication &app);

   virtual void OnShutdown(dtGame::GameApplication &app);

protected:
   virtual ~[!output PROJECT_NAME]EntryPoint();

private:

};

#endif // [!output PROJECT_NAME]EntryPoint_h__
