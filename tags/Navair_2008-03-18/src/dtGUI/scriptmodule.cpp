#include <dtGUI/scriptmodule.h>

#include <dtUtil/log.h>
#include <CEGUI/CEGUIEventSet.h>

using namespace dtGUI;

ScriptModule::ScriptModule(): BaseScriptModule(), mCallbacks()
{
   d_identifierString = "dtGUI/ScriptModule";
}

ScriptModule::~ScriptModule()
{
}

void ScriptModule::executeScriptFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup)
{
   LOG_WARNING("Script File Event not handled");
}

int ScriptModule::executeScriptGlobal(const CEGUI::String& function_name)
{
   LOG_WARNING("Script Global Event not handled");
   return 0;
}

void ScriptModule::executeString(const CEGUI::String& str)
{
   LOG_WARNING("String Event not handled");
}

/** Uses the map of strings to functions to execute a function to handle the EventArgs.
 * This function is called by CEGUI::System when a CEGUI::Window throws an CEGUI::Event*/
bool ScriptModule::executeScriptedEventHandler(const CEGUI::String& handler_name, const CEGUI::EventArgs& ea)
{
   CallbackRegistry::iterator iter = mCallbacks.find( handler_name.c_str() );
   if( iter != mCallbacks.end() )
   {
      CallbackRegistry::value_type::second_type aFunction = (*iter).second;
      return aFunction( ea );
   }
   else
   {
      dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FUNCTION__,
         "ScriptModule: function '%s' not found in registry.", handler_name.c_str() );
      return false;
   }
}

#if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 5
CEGUI::Event::Connection ScriptModule::subscribeEvent(   CEGUI::EventSet* target,
                                                         const CEGUI::String& name,
                                                         const CEGUI::String& subscriber_name)
{
   CEGUI::Event::Connection con;
   
   CallbackRegistry::iterator iter = mCallbacks.find( subscriber_name.c_str() );
   if( iter != mCallbacks.end() )
   {
      CallbackRegistry::value_type::second_type aFunction = (*iter).second;
      con = target->subscribeEvent(name, CEGUI::Event::Subscriber(aFunction));
   }
   else
   {
      dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FUNCTION__,
         "ScriptModule: function '%s' not found in registry.", subscriber_name.c_str() );
   }

   return con;
}

CEGUI::Event::Connection ScriptModule::subscribeEvent(CEGUI::EventSet* target,
                                                      const CEGUI::String& name,
                                                      CEGUI::Event::Group group,
                                                      const CEGUI::String& subscriber_name)
{
   CEGUI::Event::Connection con;

   CallbackRegistry::iterator iter = mCallbacks.find( subscriber_name.c_str() );
   if( iter != mCallbacks.end() )
   {
      CallbackRegistry::value_type::second_type aFunction = (*iter).second;
      con = target->subscribeEvent(name, group, CEGUI::Event::Subscriber(aFunction));
   }
   else
   {
      dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FUNCTION__,
         "ScriptModule: function '%s' not found in registry.", subscriber_name.c_str() );
   }

   return con;
}
#endif // CEGUI 0.5.0

bool ScriptModule::AddCallback(const std::string& name, STATIC_FUNCTION f)
{
   HandlerFunctor hf( f );
   return mCallbacks.insert( CallbackRegistry::value_type( name , hf ) ).second;
}

bool ScriptModule::AddCallback(const std::string& name, const HandlerFunctor& callback)
{
   return mCallbacks.insert( CallbackRegistry::value_type( name , callback ) ).second;
}
