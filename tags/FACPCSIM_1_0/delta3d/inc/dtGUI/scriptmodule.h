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

      /** \brief Add a callback handler.
        * @param name is the string representation of the handler function to be executed for the CEGUI::Event.
        * @param MemFun is the pointer to the member function to be called when the CEGUI::Event is activated.
        * @param instance is the pointer of type InstT which will be used to call the member function.
        */
      template<typename InstT>
      bool AddCallback(const std::string& name, bool (InstT::*MemFun)(const CEGUI::EventArgs&), InstT* instance)
      {
         HandlerFunctor hf(MemFun,instance);
         return mCallbacks.insert( CallbackRegistry::value_type( name , hf ) ).second;
      }

      /** \brief Add a callback handler.
        * @param name is the string representation of the handler function to be executed for the CEGUI::Event.
        * @param func is the pointer to the function to be called when the CEGUI::Event is activated.
        */
      bool AddCallback(const std::string& name, STATIC_FUNCTION func);

      /** \brief Add a callback handler.
        * @param name is the string representation of the handler function to be executed for the CEGUI::Event.
        * @param func is an instance of a function object to be called when the CEGUI::Event is activated.
        */
      bool AddCallback(const std::string& name, const HandlerFunctor& callback);

      /** Returns the StaticRegistry.*/
      const CallbackRegistry& GetRegistry() const { return mCallbacks; }

      // inherited methods
      virtual void executeScriptFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup = "");
      virtual int  executeScriptGlobal(const CEGUI::String& function_name);
      virtual void executeString(const CEGUI::String& str);
      virtual bool executeScriptedEventHandler(const CEGUI::String& handler_name, const CEGUI::EventArgs& ea);

   private:
      ScriptModule(const ScriptModule&);  // not implemented by design
      CallbackRegistry mCallbacks;
   };
};

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // DT_GUI_SCRIPTMODULE_INCLUDE
