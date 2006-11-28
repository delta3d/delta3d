// applicationbindings.cpp: Application binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtABC/application.h>
#include <dtCore/keyboard.h>
#include <dtCore/transformable.h>
#include <dtCore/scene.h>
#include <dtCore/refptr.h>

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

   virtual void OnCollisionMessage(PythonCollisionData pData)
   {
      if( override pOverride = this->get_override("OnCollisionMessage") )
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
            call<>( this->get_override("OnCollisionMessage").ptr(), pData);
         #else
            this->get_override("OnCollisionMessage")(pData);
         #endif
      }
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

      void OnMessage( MessageData* data )
      {
         if( data->message == "preframe" )
         {
            PreFrame( *static_cast<const double*>(data->userData) );
         }
         else if( data->message == "frame" )
         {
            Frame( *static_cast<const double*>(data->userData) );
         }
         else if( data->message == "postframe" )
         {
            PostFrame( *static_cast<const double*>(data->userData) );
         }
         else if( data->message == "collision")
         {
             OnCollisionMessage(PythonCollisionData(static_cast< Scene::CollisionData* >( data->userData )));
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


   class_<PythonCollisionData>("CollisionData")
      .def("GetBody1", &PythonCollisionData::GetBody1, return_value_policy<reference_existing_object>())
      .def("GetBody2", &PythonCollisionData::GetBody2, return_value_policy<reference_existing_object>())
      .def_readonly("mLocation", &PythonCollisionData::mLocation)
      .def_readonly("mNormal", &PythonCollisionData::mNormal)
      .def_readonly("mDepth", &PythonCollisionData::mDepth);
}
