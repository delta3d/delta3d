#ifndef EntryPoint_h__
#define EntryPoint_h__

#include "EntryPointExport.h"
#include <dtGame/gameentrypoint.h>


[!if DMP_CHECKBOX || DNPC_CHECKBOX]
namespace dtGame
{
[!if DMP_CHECKBOX]
   class DefaultMessageProcessor;
[!endif]
[!if DNPC_CHECKBOX]
   class DefaultNetworkPublishingComponent;
[!endif]
}
[!endif]

[!if INPUT_CHECKBOX]
class [!output PROJECT_NAME]Input;
[!endif]

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
[!if DMP_CHECKBOX]
   dtCore::RefPtr<dtGame::DefaultMessageProcessor> mMessageProc;
[!endif]
[!if DNPC_CHECKBOX]
   dtCore::RefPtr<dtGame::DefaultNetworkPublishingComponent> mNetworkComponent;
[!endif]
[!if INPUT_CHECKBOX]
   dtCore::RefPtr<[!output PROJECT_NAME]Input> mInputComponent;
[!endif]
};

#endif // [!output PROJECT_NAME]EntryPoint_h__
