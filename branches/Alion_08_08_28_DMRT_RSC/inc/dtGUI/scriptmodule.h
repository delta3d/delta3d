#ifndef DT_GUI_SCRIPTMODULE_INCLUDE
#define DT_GUI_SCRIPTMODULE_INCLUDE

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4251)     // for "warning C4251: 'dtGUI::ScriptModule::mCallbacks' : class 'std::map<_Kty,_Ty>' needs to have dll-interface to be used by clients of class 'dtGUI::ScriptModule' "
#endif

#include <dtGUI/basescriptmodule.h>   // for base class
#include <map>                        // for std::map type
#include <queue>                      // for member
#include <string>                     // for std::string type
#include <dtUtil/functor.h>           // for typedef, member

namespace dtGUI
{
  /** \brief ScriptModule is the binding from CEGUI::Events to specific application callbacks.
    * Create an instance of this class, and provide it as a parameter during construction of a CEUIDrawable instance.
    * Add new handlers with the AddCallback function.
    */
   class DT_GUI_EXPORT ScriptModule : public BaseScriptModule
   {
   public:
      ///\todo test to know if this typedef can support just NonMemberFunction, not requiring them to be static.
      typedef bool (*STATIC_FUNCTION)(const CEGUI::EventArgs &e);
      typedef dtUtil::Functor<bool,TYPELIST_1(const CEGUI::EventArgs&)> HandlerFunctor;
      typedef std::map<std::string,HandlerFunctor> CallbackRegistry;

      ScriptModule();
      virtual ~ScriptModule();

      /**
       * Add a static callback handler.
       * Example:
       * @code
       * class MyClass
       * {
       *   ...
       *   static bool OnClick( const CEGUI::EventArgs &e );
       *   ...
       * }
       *
       *  ...
       *  mScriptModule->AddCallback("OnDoSomething", &OnClick);
       *  ...
       * @endcode
       * @param name is the string representation of the handler function to be executed for the CEGUI::Event.
       * @param func is the pointer to the function to be called when the CEGUI::Event is activated.
       */
      bool AddCallback(const std::string& name, STATIC_FUNCTION func);

      /**
       * Add a non-static callback handler.
       * Example:
       * @code
       * class App
       * {
       *   ...
       *  public:
       *   bool OnClick( const CEGUI::EventArgs &e );
       *   ...
       * }
       *
       *  ...
       *  App *mApp = new App();
       *
       *  dtGUI::ScriptModule::HandlerFunctor handler( dtUtil::MakeFunctor( &App::OnClick, mApp ) );
       *  mScriptModule->AddCallback("OnDoSomething", handler );
       * ...
       * @endcode
       * @param name is the string representation of the handler function to be executed for the CEGUI::Event.
       * @param func is an instance of a function object to be called when the CEGUI::Event is activated.
       * @attention An attempt was make an implemenation with the signature template<typename InstT> bool
       * AddCallback(const std::string& name, bool (InstT::*MemFun)(const CEGUI::EventArgs&), InstT* instance)
       * but somehow creating the HandleFunctor intenal to the function caused some problems.
       */
      bool AddCallback(const std::string& name, const HandlerFunctor& callback);

      /**
       * Returns the StaticRegistry.
       */
      const CallbackRegistry& GetRegistry() const { return mCallbacks; }

      // inherited methods
      virtual void executeScriptFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup = "");
      virtual int  executeScriptGlobal(const CEGUI::String& function_name);
      virtual void executeString(const CEGUI::String& str);
      virtual bool executeScriptedEventHandler(const CEGUI::String& handler_name, const CEGUI::EventArgs& ea);

      #if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 5
      virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* target, const CEGUI::String& name, const CEGUI::String& subscriber_name);
      virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* target, const CEGUI::String& name, CEGUI::Event::Group group, const CEGUI::String& subscriber_name);
      #endif // CEGUI 0.5.0

   private:
      ScriptModule(const ScriptModule&);  // not implemented by design
      CallbackRegistry mCallbacks;
   };
}

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif  // DT_GUI_SCRIPTMODULE_INCLUDE
