// applicationbindings.cpp: Application binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtABC/application.h>
#include <dtCore/keyboard.h>
#include <dtCore/transformable.h>
#include <dtCore/scene.h>
#include <dtCore/refptr.h>
#include <dtCore/system.h>

using namespace boost::python;
using namespace dtABC;
using namespace dtCore;


struct PythonCollisionData
{
   PythonCollisionData(){}

   PythonCollisionData(dtCore::Scene::CollisionData* colData)
      : mBodies_0(colData->mBodies[0])
      , mBodies_1(colData->mBodies[1])
      , mLocation(colData->mLocation)
      , mNormal(colData->mNormal)
      , mDepth(colData->mDepth)
   {

   }

   Transformable* GetBody1()
   {
      return mBodies_0;
   }

   Transformable* GetBody2()
   {
      return mBodies_1;
   }

   Transformable* mBodies_0;
   Transformable* mBodies_1;
   osg::Vec3 mLocation; ///<The collision location
   osg::Vec3 mNormal; ///<The collision normal
   float mDepth; ///<The penetration depth

   
};


class ApplicationWrap : public Application, public wrapper<Application>
{
   public:

   ApplicationWrap( const std::string& configFilename = "") :
      Application(configFilename)
      {
                  
      }

   //////////////////////////////////////////////////////////////////////////
   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int kc )
   {
      if (override KeyPressed = this->get_override("KeyPressed"))
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         return call<bool>(KeyPressed.ptr(), boost::ref(keyboard), kc);
         #else
         return KeyPressed(boost::ref(keyboard), kc);
         #endif
      }
      return Application::KeyPressed(keyboard, kc);
   }
   
   //////////////////////////////////////////////////////////////////////////
   virtual bool DefaultKeyPressed(const dtCore::Keyboard* keyboard, int kc)
   {
      return this->Application::KeyPressed(keyboard, kc);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool KeyReleased(const dtCore::Keyboard* keyboard, int kc)
   {
      if (override KeyReleased = this->get_override("KeyReleased"))
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         return call<bool>(KeyReleased.ptr(), boost::ref(keyboard), kc);
         #else
         return KeyReleased(boost::ref(keyboard), kc);
         #endif
      }
      return Application::KeyReleased(keyboard, kc);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool DefaultKeyReleased(const dtCore::Keyboard* keyboard, int kc)
   {
      return this->Application::KeyReleased(keyboard, kc);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool MouseButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
   {
      if (override MouseButtonPressed = this->get_override("MouseButtonPressed"))
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         return call<bool>(MouseButtonPressed.ptr(), boost::ref(mouse), button);
         #else
         return MouseButtonPressed(boost::ref(mouse), button);
         #endif
      }
      return Application::MouseButtonPressed(mouse, button);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool DefaultMouseButtonPressed(const dtCore::Mouse* mouse,
                                          dtCore::Mouse::MouseButton button)
   {
      return this->Application::MouseButtonPressed(mouse, button);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool MouseButtonReleased(const dtCore::Mouse* mouse,
                                    dtCore::Mouse::MouseButton button)
   {
      if (override MouseButtonReleased = this->get_override("MouseButtonReleased"))
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         return call<bool>(MouseButtonReleased.ptr(), boost::ref(mouse), button);
         #else
         return MouseButtonReleased(boost::ref(mouse), button);
         #endif
      }
      return Application::MouseButtonReleased(mouse, button);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool DefaultMouseButtonReleased(const dtCore::Mouse* mouse,
                                           dtCore::Mouse::MouseButton button)
   {
      return this->Application::MouseButtonReleased(mouse, button);
   }
 
   //////////////////////////////////////////////////////////////////////////
   virtual bool MouseMoved(const dtCore::Mouse* mouse, float x, float y)
   {
      if (override MouseMoved = this->get_override("MouseMoved"))
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         return call<bool>(MouseMoved.ptr(), boost::ref(mouse), x, y);
         #else
         return MouseMoved(boost::ref(mouse), x, y);
         #endif
      }
      return Application::MouseMoved(mouse, x, y);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool DefaultMouseMoved(const dtCore::Mouse* mouse, float x, float y)
   {
      return this->Application::MouseMoved(mouse, x, y);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool MouseDragged(const dtCore::Mouse* mouse, float x, float y)
   {
      if (override MouseDragged = this->get_override("MouseDragged"))
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         return call<bool>(MouseDragged.ptr(), boost::ref(mouse), x, y);
         #else
         return MouseDragged(boost::ref(mouse), x, y);
         #endif
      }
      return Application::MouseDragged(mouse, x, y);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool DefaultMouseDragged(const dtCore::Mouse* mouse, float x, float y)
   {
      return this->Application::MouseDragged(mouse, x, y);
   }
   
   //////////////////////////////////////////////////////////////////////////
   virtual bool MouseScrolled(const dtCore::Mouse* mouse, int delta)
   {
      if (override MouseScrolled = this->get_override("MouseScrolled"))
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         return call<bool>(MouseScrolled.ptr(), boost::ref(mouse), delta);
         #else  
         return MouseScrolled(boost::ref(mouse), delta);
         #endif
      }
      return Application::MouseScrolled(mouse, delta);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool DefaultMouseScrolled(const dtCore::Mouse* mouse, int delta)
   {
      return this->Application::MouseScrolled(mouse, delta);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool MouseButtonDoubleClicked(const dtCore::Mouse* mouse,
                                         dtCore::Mouse::MouseButton button,
	                                      int clickCount)
   {
      if (override MouseButtonDoubleClicked = this->get_override("MouseButtonDoubleClicked"))
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         return call<bool>(MouseButtonDoubleClicked.ptr(), boost::ref(mouse), button, clickCount);
         #else
         return MouseButtonDoubleClicked(boost::ref(mouse), button, clickCount);
         #endif
      }
      return Application::MouseButtonDoubleClicked(mouse, button, clickCount);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool DefaultMouseButtonDoubleClicked(const dtCore::Mouse* mouse,
                                                dtCore::Mouse::MouseButton button,
                                                int clickCount)
   {
      return this->Application::MouseButtonDoubleClicked(mouse, button, clickCount);
   }

   //////////////////////////////////////////////////////////////////////////
   virtual void OnCollisionMessage(PythonCollisionData pData)
   {
      if(override pOverride = this->get_override("OnCollisionMessage"))
      {
         this->get_override("OnCollisionMessage")(pData);       
      }
   }

   protected:

      //////////////////////////////////////////////////////////////////////////
      virtual void EventTraversal(const double deltaFrameTime)
      {
         if (PyObject_HasAttrString(boost::python::detail::wrapper_base_::get_owner(*this),
                                    "EventTraversal"))
         {
            if (override EventTraversal = this->get_override("EventTraversal"))
            {
               #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
               call<void>(EventTraversal.ptr(), deltaFrameTime);
               #else
               EventTraversal(deltaFrameTime);
               #endif
            }
            else
            {
               Application::EventTraversal(deltaFrameTime);
            }
         }
         else
         {
            Application::EventTraversal(deltaFrameTime);
         }
      }

      //////////////////////////////////////////////////////////////////////////
      virtual void PreFrame(const double deltaFrameTime)
      {
         if (PyObject_HasAttrString( boost::python::detail::wrapper_base_::get_owner(*this),
                                    "PreFrame"))
         {
            if (override PreFrame = this->get_override("PreFrame"))
            {
               #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
               call<void>(PreFrame.ptr(), deltaFrameTime);
               #else
               PreFrame(deltaFrameTime);
               #endif
            }
            else
            {
               Application::PreFrame(deltaFrameTime);
            }
         }
         else
         {
            Application::PreFrame(deltaFrameTime);
         }
      }
   
      //////////////////////////////////////////////////////////////////////////
      virtual void Frame(const double deltaFrameTime)
      {
         if (PyObject_HasAttrString( boost::python::detail::wrapper_base_::get_owner(*this),
                                     "Frame"))
         {
            if (override Frame = this->get_override("Frame"))
            {
               #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
               call<void>(Frame.ptr(), deltaFrameTime);
               #else
               Frame(deltaFrameTime);
               #endif
            }
            else
            {
               Application::Frame(deltaFrameTime);
            }
         }
         else
         {
            Application::Frame(deltaFrameTime);
         }
      }
   
      //////////////////////////////////////////////////////////////////////////
      virtual void PostFrame(const double deltaFrameTime)
      {
         if (PyObject_HasAttrString( boost::python::detail::wrapper_base_::get_owner(*this),
                                     "PostFrame"))
         {
            if (override PostFrame = this->get_override("PostFrame"))
            {
               #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
               call<void>(PostFrame.ptr(), deltaFrameTime);
               #else
               PostFrame(deltaFrameTime);
               #endif
            }
            else
            {
               Application::PostFrame(deltaFrameTime);
            }
         }
         else
         {
            Application::PostFrame(deltaFrameTime);
         }
      }

      //////////////////////////////////////////////////////////////////////////
      void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

      {
         if (str == dtCore::System::MESSAGE_EVENT_TRAVERSAL)
         {
            EventTraversal(*static_cast<const double*>(data->userData));
         }
         else if (str == dtCore::System::MESSAGE_PRE_FRAME)
         {
            PreFrame(*static_cast<const double*>(data->userData));
         }
         else if (str == System::MESSAGE_FRAME)
         {
            Frame(*static_cast<const double*>(data->userData));
         }
         else if (str == dtCore::System::MESSAGE_POST_FRAME)
         {
            PostFrame(*static_cast<const double*>(data->userData));
         }
         else if (str == "collision")
         {
            OnCollisionMessage(PythonCollisionData(static_cast< Scene::CollisionData* >(data->userData)));
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
      .def("KeyPressed", &Application::KeyPressed,&ApplicationWrap::DefaultKeyPressed)
      .def("KeyReleased", &Application::KeyReleased,&ApplicationWrap::DefaultKeyReleased)
	   .def("MouseButtonPressed", &Application::MouseButtonPressed,&ApplicationWrap::DefaultMouseButtonPressed)
	   .def("MouseButtonReleased", &Application::MouseButtonReleased,&ApplicationWrap::DefaultMouseButtonReleased)
	   .def("MouseMoved", &Application::MouseMoved,&ApplicationWrap::DefaultMouseMoved)
	   .def("MouseDragged", &Application::MouseDragged,&ApplicationWrap::DefaultMouseDragged)
	   .def("MouseScrolled", &Application::MouseScrolled,&ApplicationWrap::DefaultMouseScrolled)
      .def("AddView", &Application::AddView)
      .def("RemoveView", &Application::RemoveView)
      ;

   class_<PythonCollisionData>("CollisionData")
      .def("GetBody1", &PythonCollisionData::GetBody1, return_value_policy<reference_existing_object>())
      .def("GetBody2", &PythonCollisionData::GetBody2, return_value_policy<reference_existing_object>())
      .def_readonly("mLocation", &PythonCollisionData::mLocation)
      .def_readonly("mNormal", &PythonCollisionData::mNormal)
      .def_readonly("mDepth", &PythonCollisionData::mDepth);
}
