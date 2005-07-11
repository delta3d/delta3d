#include <dtGUI/scriptmodule.h>

#include <dtCore/notify.h>

using namespace dtGUI;

ScriptModule::ScriptModule(): CEGUI::ScriptModule(), mCallbacks()
{
}

ScriptModule::~ScriptModule()
{
}

void ScriptModule::executeScriptFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup)
{
   ///\todo Does support need to be added here?
}

int ScriptModule::executeScriptGlobal(const CEGUI::String& function_name)
{
  return 0;  ///\todo Does this need improvement?
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

bool ScriptModule::AddCallback(const std::string& name, FUNCTION f)
{
   return mCallbacks.insert( StaticRegistry::value_type( name , f ) ).second;
}
