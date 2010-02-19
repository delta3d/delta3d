#include <dtGUI/scriptmodule.h>
#include <dtUtil/log.h>
#include <CEGUI/CEGUIEventSet.h>


using namespace dtGUI;

//////////////////////////////////////////////////////////////////////////
bool ScriptModule::executeScriptedEventHandler(const CEGUI::String& handlerName, const CEGUI::EventArgs& eventArgs)
{
   const std::map<std::string, CEGUI::SubscriberSlot>::iterator iter = mCallbacks.find( handlerName.c_str());

   if (iter == mCallbacks.end())
   {
      LOG_WARNING(std::string(handlerName.c_str() ) + "is no valid function.");
      return false;
   }
   
   return iter->second(eventArgs);
}


//////////////////////////////////////////////////////////////////////////
CEGUI::Event::Connection ScriptModule::subscribeEvent(CEGUI::EventSet* window,
                                                      const CEGUI::String& eventName,
                                                      CEGUI::Event::Group groupName,
                                                      const CEGUI::String& subscriberName)
{
   const std::map<std::string, CEGUI::SubscriberSlot>::iterator iter = mCallbacks.find(subscriberName.c_str());

   if (iter == mCallbacks.end())
   {
      LOG_WARNING(std::string(subscriberName.c_str() ) + "is no valid callback.");
      return NULL;
   }

   if (groupName)
      return window->subscribeEvent(eventName, groupName, iter->second);
   else
      return window->subscribeEvent(eventName, iter->second);
}

//////////////////////////////////////////////////////////////////////////
bool ScriptModule::AddCallback(const std::string& callbackName, CEGUI::SubscriberSlot subscriberSlot)
{ 
   return mCallbacks.insert(std::pair<std::string, CEGUI::SubscriberSlot>(callbackName.c_str(), subscriberSlot)).second; 
}

////////////////////////////////////////////////////////////////////////////////
CEGUI::Event::Connection dtGUI::ScriptModule::subscribeEvent(CEGUI::EventSet* window,
                                                             const CEGUI::String& eventName,
                                                             const CEGUI::String& subscriberName)
{
   return subscribeEvent(window, eventName, NULL, subscriberName);
}

////////////////////////////////////////////////////////////////////////////////
bool dtGUI::ScriptModule::NotSupported(const std::string& methodName)
{
   LOG_WARNING( methodName + " not handled."); return false;
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::ScriptModule::executeScriptFile(const CEGUI::String &fileName, const CEGUI::String &resourceGroup/*="" */)
{
   NotSupported( __FUNCTION__ );
}

////////////////////////////////////////////////////////////////////////////////
int dtGUI::ScriptModule::executeScriptGlobal(const CEGUI::String& functionName)
{
   return NotSupported( __FUNCTION__ );
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::ScriptModule::executeString(const CEGUI::String &str)
{
   NotSupported( __FUNCTION__ );
}
