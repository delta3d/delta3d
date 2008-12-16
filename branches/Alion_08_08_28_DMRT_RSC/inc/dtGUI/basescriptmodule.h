#ifndef DT_BASE_SCRIPTMODULE_INC
#define DT_BASE_SCRIPTMODULE_INC

#include <CEGUI/CEGUIScriptModule.h>        // for base class
#include <dtGUI/export.h>            // for Windows export symbols

namespace dtGUI
{
   /** \brief An abstract interface for classes used by CEUIDrawable.
     * BaseScriptModule is mean to be implemented to provide support
     * for Events triggered by the CEGUI Windows in a GUI scene.
     * \sa dtGUI::ScriptModule.
     */
   class DT_GUI_EXPORT BaseScriptModule : public CEGUI::ScriptModule
   {
   public:
      BaseScriptModule() {}
      virtual ~BaseScriptModule() {}

      // inherited methods
      virtual void executeScriptFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup = "")=0;
      virtual int  executeScriptGlobal(const CEGUI::String& function_name)=0;
      virtual void executeString(const CEGUI::String& str)=0;

      /** Overload this function to handle Events triggered from a CEGUI::Window.
        * @param handlerName the name of something to handle this event, typically a function name.
        * @param ea the CEGUI::EventArgs that can contain useful information about the CEGUI::Event that occurred.
        */
      virtual bool executeScriptedEventHandler(const CEGUI::String& handlerName, const CEGUI::EventArgs& ea)=0;

      #if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 5
      virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* target, const CEGUI::String& name, const CEGUI::String& subscriber_name)=0;
      virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* target, const CEGUI::String& name, CEGUI::Event::Group group, const CEGUI::String& subscriber_name)=0;
      #endif // CEGUI 0.5.0
   };
}

#endif  //  DT_BASE_SCRIPTMODULE_INC
