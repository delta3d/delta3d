#include <dtDIS/plugins/default/pluginsymbols.h>
#include <dtDIS/plugins/default/defaultplugin.h>

// should not need to include the following files:
#include <dtGame/gamemanager.h>
#include <dtDAL/actortype.h>

dtDIS::IDISPlugin* dtDIS::CreateComponent()
{
   return new dtDIS::DefaultPlugin();
}

void dtDIS::DestroyComponent(dtDIS::IDISPlugin* plugin)
{
   delete plugin;
}

