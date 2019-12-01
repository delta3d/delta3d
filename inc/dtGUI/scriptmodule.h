#ifndef DT_GUI_SCRIPTMODULE_INCLUDE
#define DT_GUI_SCRIPTMODULE_INCLUDE


#include <dtGUI/export.h>
#include <dtUtil/functor.h>           // for typedef, member
#include <CEGUI/CEGUIScriptModule.h>
#include <map>
#include <string>
#include <vector>

namespace dtGUI
{
  /** \brief ScriptModule is the binding from CEGUI::Events to specific application callbacks.
    * Create an instance of this class, and provide it as a parameter during construction of a CEUIDrawable instance.
    * Add new handlers with the AddCallback function.
    */
   class DT_GUI_EXPORT ScriptModule : public CEGUI::ScriptModule
   {
      public:
         virtual ~ScriptModule();

         virtual void destroyBindings(void);


         virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* window,
                                                         const CEGUI::String& eventName,
                                                         CEGUI::Event::Group groupName,
                                                         const CEGUI::String& subscriberName);

         virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* window,
                                                         const CEGUI::String& eventName,
                                                         const CEGUI::String& subscriberName);

         virtual bool executeScriptedEventHandler(const CEGUI::String& handlerName,
                                                  const CEGUI::EventArgs& eventArgs);

         virtual void executeScriptFile (const CEGUI::String &fileName,
                                        const CEGUI::String &resourceGroup="");

         virtual int executeScriptGlobal (const CEGUI::String& functionName);

         virtual void executeString(const CEGUI::String &str);
 
         ///Deprecated 2/23/10
         typedef dtUtil::Functor<bool,TYPELIST_1(const CEGUI::EventArgs&)> HandlerFunctor;

         /**
         * Add a callback handler.
         * Example:
         * @code
         * class MyClass
         * {
         *   ...
         *   static bool OnClickStatic (const CEGUI::EventArgs &e);
         *   bool OnClick (const CEGUI::EventArgs &e);
         *   ...
         * }
         *
         * mScriptModule->AddCallback("OnDoSomething", &OnClickStatic);
         * mScriptModule->AddCallback("OnDoSomething", CEGUI::SubscriberSlot(&OnClick, this));
         * @endcode
         *
         * @param callbackName is the string representation of the handler function to be executed for the CEGUI::Event.
         * @param subscriberSlot function to be called when the CEGUI::Event is activated.
         */
         bool AddCallback(const std::string& callbackName, CEGUI::SubscriberSlot subscriberSlot);
         //bool RemoveCallback(const std::string& callbackName);

         typedef std::map<std::string,CEGUI::SubscriberSlot> CallbackRegistry;
         
         /**
          * Returns the StaticRegistry.
          */
         const CallbackRegistry& GetRegistry() const;

      private:
         bool NotSupported(const std::string& methodName);

         CallbackRegistry mCallbacks;
         std::vector<CEGUI::Event::Connection> mConnections; 
   };
}

#endif  // DT_GUI_SCRIPTMODULE_INCLUDE
