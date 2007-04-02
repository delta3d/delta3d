#ifndef _dt_dis_plugin_entity_state_pdu_plugin_symbols_h_
#define _dt_dis_plugin_entity_state_pdu_plugin_symbols_h_

#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>

namespace dtDIS
{
   class IDISPlugin;

   extern "C" DT_DIS_DEFAULT_EXPORT IDISPlugin* CreateComponent();
   extern "C" DT_DIS_DEFAULT_EXPORT void DestroyComponent(IDISPlugin* plugin);
}

#endif  // _dt_dis_plugin_entity_state_pdu_plugin_symbols_h_
