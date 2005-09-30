#ifndef DT_GUI_SCRIPTMODULE_INCLUDE
#define DT_GUI_SCRIPTMODULE_INCLUDE

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)     // for "warning C4251: 'dtGUI::ScriptModule::mCallbacks' : class 'std::map<_Kty,_Ty>' needs to have dll-interface to be used by clients of class 'dtGUI::ScriptModule' "
#endif

#include <dtGUI/basescriptmodule.h>   // for base class
#include <map>                        // for std::map type
#include <queue>                      // for member
#include <string>                     // for std::string type

namespace dtGUI
{
  /** \brief ScriptModule is the binding from CEGUI::Events to specific application callbacks.
    * Create an instance of this class, and provide it as a parameter during construction of a CEUIDrawable instance.
    * Add new handlers with the AddCallback function.
    */
   class DT_EXPORT ScriptModule : public BaseScriptModule
   {
   public:
      typedef void (*STATIC_FUNCTION)(const CEGUI::EventArgs &e);
      typedef std::map<std::string,STATIC_FUNCTION> StaticRegistry;

      ScriptModule();
      virtual ~ScriptModule();

      /** \brief Add a callback handler.
        * @param name is the string representation of the handler function to be executed for the CEGUI::Event.
        * @param func is the pointer to the function to be called when the CEGUI::Event is activated.
        */
      bool AddCallback(const std::string& name, STATIC_FUNCTION func);

      /** Returns the StaticRegistry.*/
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
