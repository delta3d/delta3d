// applicationbindings.cpp: Application binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtABC/application.h>
#include <dtCore/keyboard.h>

using namespace boost::python;
using namespace dtABC;
using namespace dtCore;

class ApplicationWrap : public Application, public wrapper<Application>
{
   public:

   ApplicationWrap( const std::string& configFilename = "") :
      Application(configFilename)
      {
      }

   virtual bool KeyPressed( const dtCore::Keyboard* keyboard,
                            Producer::KeyboardKey key,
                            Producer::KeyCharacter character )
   {
      if( override KeyPressed = this->get_override("KeyPressed") )
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         return call<bool>( KeyPressed.ptr(), boost::ref(keyboard), key, character );
         #else
         return KeyPressed( boost::ref(keyboard), key, character );
         #endif
      }
      return Application::KeyPressed( keyboard, key, character );
   }

   virtual bool DefaultKeyPressed( const dtCore::Keyboard* keyboard,
                                   Producer::KeyboardKey key,
                                   Producer::KeyCharacter character )
   {
      return this->Application::KeyPressed( keyboard, key, character );
   }

   protected:
      
      virtual void PreFrame(const double deltaFrameTime)
      {
         if( PyObject_HasAttrString( boost::python::detail::wrapper_base_::get_owner(*this),
                                     "PreFrame") )
         {
            if( override PreFrame = this->get_override("PreFrame") )
            {
               #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
               call<void>( PreFrame.ptr(), deltaFrameTime );
               #else
               PreFrame(deltaFrameTime);
               #endif
            }
            else
            {
               Application::PreFrame(deltaFrameTime);
            }
         }
      }
   
      virtual void Frame(const double deltaFrameTime)
      {
         if( PyObject_HasAttrString( boost::python::detail::wrapper_base_::get_owner(*this),
                                     "Frame") )
         {
            if( override Frame = this->get_override("Frame") )
            {
               #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
               call<void>( Frame.ptr(), deltaFrameTime );
               #else
               Frame(deltaFrameTime);
               #endif
            }
            else
            {
               Application::Frame(deltaFrameTime);
            }
         }
      }
   
      virtual void PostFrame(const double deltaFrameTime)
      {
         if( PyObject_HasAttrString( boost::python::detail::wrapper_base_::get_owner(*this),
                                     "PostFrame") )
         {
            if( override PostFrame = this->get_override("PostFrame") )
            {
               #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
               call<void>( PostFrame.ptr(), deltaFrameTime );
               #else
               PostFrame(deltaFrameTime);
               #endif
            }
            else
            {
               Application::PostFrame(deltaFrameTime);
            }
         }
      }      
};

void initApplicationBindings()
{
   Application* (*ApplicationGI1)(int) = &Application::GetInstance;
   Application* (*ApplicationGI2)(std::string) = &Application::GetInstance;

   class_<ApplicationWrap, bases<BaseABC>, dtCore::RefPtr<ApplicationWrap>, boost::noncopyable>("Application", init<optional<const std::string&> >())
      .def("GetInstanceCount", &Application::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ApplicationGI1, return_internal_reference<>())
      .def("GetInstance", ApplicationGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GenerateDefaultConfigFile", &Application::GenerateDefaultConfigFile)
      .staticmethod("GenerateDefaultConfigFile")
      .def("Run", &Application::Run)
      .def("KeyPressed",&Application::KeyPressed,&ApplicationWrap::DefaultKeyPressed)
      ;
}
