#ifndef DT_GUI_SCRIPTMODULE_INCLUDE
#define DT_GUI_SCRIPTMODULE_INCLUDE

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)     // for "warning C4251: 'dtGUI::ScriptModule::mCallbacks' : class 'std::map<_Kty,_Ty>' needs to have dll-interface to be used by clients of class 'dtGUI::ScriptModule' "
#endif

#include <CEGUIScriptModule.h>        // for base class
#include <map>                        // for std::map type
#include <string>                     // for std::string type
#include <dtCore/export.h>

namespace dtGUI
{
  /** ScriptModule is the binding from CEGUI Events to
    * specific application callbacks.  This class requires
    * the GUIHandler to support some interfaces.
    */
   class DT_EXPORT ScriptModule : public CEGUI::ScriptModule
   {
   public:
      typedef void (*FUNCTION)(const CEGUI::EventArgs &e);
      typedef std::map<std::string,FUNCTION> StaticRegistry;

      ScriptModule();
      virtual ~ScriptModule();

      bool AddCallback(const std::string& name, FUNCTION f);
      const StaticRegistry& GetRegistry() const { return mCallbacks; }

      // inherited methods
      virtual void executeScriptFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup = "");
      virtual int  executeScriptGlobal(const CEGUI::String& function_name);
      virtual void executeString(const CEGUI::String& str);
      virtual bool executeScriptedEventHandler(const CEGUI::String& handler_name, const CEGUI::EventArgs& ea);

   private:
      ScriptModule(const ScriptModule&);  // not implemented by design
      StaticRegistry mCallbacks;
   };
};

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // DT_GUI_SCRIPTMODULE_INCLUDE
