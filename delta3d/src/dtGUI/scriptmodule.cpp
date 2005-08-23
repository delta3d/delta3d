#include <dtGUI/scriptmodule.h>

#include <dtCore/notify.h>

using namespace dtGUI;

ScriptModule::ScriptModule(): BaseScriptModule(), mCallbacks()
{
}

ScriptModule::~ScriptModule()
{
}

void ScriptModule::executeScriptFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup)
{
}

int ScriptModule::executeScriptGlobal(const CEGUI::String& function_name)
{
  return 0;
}

void ScriptModule::executeString(const CEGUI::String& str)
{
}

/** Uses the map of strings to functions to execute a function to handle the EventArgs.
 * This function is called by CEGUI::System when a CEGUI::Window throws an CEGUI::Event*/
bool ScriptModule::executeScriptedEventHandler(const CEGUI::String& handler_name, const CEGUI::EventArgs& ea)
{
   StaticRegistry::iterator iter = mCallbacks.find( handler_name.c_str() );
   if( iter != mCallbacks.end() )
   {
      StaticRegistry::value_type::second_type aFunction = (*iter).second;
      aFunction( ea );
      return true;
   }
   else
   {
      dtCore::Notify(dtCore::WARN,"ScriptModule: function '%s' not found in registry.", handler_name.c_str() );
      return false;
   }
}

bool ScriptModule::AddCallback(const std::string& name, STATIC_FUNCTION f)
{
   return mCallbacks.insert( StaticRegistry::value_type( name , f ) ).second;
}
