#include <dtGUI/scriptmodule.h>
#include <dtUtil/log.h>
#include <CEGUI/CEGUIEventSet.h>
#include <CEGUI/CEGUIEvent.h>

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

   CEGUI::Event::Connection c;
   if (groupName)
   {
       c = window->subscribeEvent(eventName, groupName, iter->second);
   }
   else
   {
      c = window->subscribeEvent(eventName, iter->second);
   }

   mConnections.push_back(c);
   return c;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptModule::AddCallback(const std::string& callbackName, CEGUI::SubscriberSlot subscriberSlot)
{ 
   return mCallbacks.insert(std::pair<std::string, CEGUI::SubscriberSlot>(callbackName.c_str(), subscriberSlot)).second; 
}

////////////////////////////////////////////////////////////////////////////
//bool ScriptModule::RemoveCallback(const std::string& callbackName)
//{
//   const std::map<std::string, CEGUI::SubscriberSlot>::iterator iter = mCallbacks.find(callbackName.c_str());
//
//   if (iter == mCallbacks.end())
//   {
//      LOG_WARNING(std::string(callbackName.c_str() ) + "is no valid callback.");
//      return false;
//   }
//
//   CEGUI::SubscriberSlot slot = iter->second;
//   slot.cleanup();
//
//   mCallbacks.erase(iter);
//
//   int count = (int)mConnections.size();
//   for (int index = 0; index < count; ++index)
//   {
//      CEGUI::Event::Connection c = mConnections[index];
//      if (!c.isValid())
//      {
//         mConnections.erase(mConnections.begin() + index);
//         index--;
//         count--;
//      }
//   }
//
//   return true;
//}

////////////////////////////////////////////////////////////////////////////////
CEGUI::Event::Connection dtGUI::ScriptModule::subscribeEvent(CEGUI::EventSet* window,
                                                             const CEGUI::String& eventName,
                                                             const CEGUI::String& subscriberName)
{
   return subscribeEvent(window, eventName, 0, subscriberName);
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

////////////////////////////////////////////////////////////////////////////////
const dtGUI::ScriptModule::CallbackRegistry& dtGUI::ScriptModule::GetRegistry() const
{
   return mCallbacks;
}

////////////////////////////////////////////////////////////////////////////////
dtGUI::ScriptModule::~ScriptModule()
{

}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::ScriptModule::destroyBindings(void)
{
   mCallbacks.clear();

   std::vector<CEGUI::Event::Connection>::iterator itr = mConnections.begin();

   while(itr != mConnections.end())
   {
      if (itr->isValid())
      {
         if ((*itr)->connected())
         {
            (*itr)->disconnect();
         }
         ++itr;
      }
   }

   mConnections.clear();
}
