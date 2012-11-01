// baseabcbindings.cpp: BaseABC binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtABC/baseabc.h>
#include <dtCore/map.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/scene.h>
#include <dtCore/base.h>
using namespace boost::python;
using namespace dtABC;
using namespace dtCore;

class BaseABCWrap : public BaseABC, public wrapper<BaseABC>
{
   public:

      BaseABCWrap( const std::string& name )
         : BaseABC(name)
      {
      }

      virtual void Config()
      {
         if( override Config = this->get_override("Config") )
         {
             #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
             call<void>( Config.ptr() );
             #else
             Config();
             #endif
         }
         else
         {
            BaseABC::Config();
         }  
      }
      
      void DefaultConfig()
      {
         this->BaseABC::Config();
      }
      
      virtual void Quit()
      {
         if( override Quit = this->get_override("Quit") )
         {
            #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
            call<void>( Quit.ptr() );
            #else
            Quit();
            #endif
         }
         else
         {
            BaseABC::Quit();
         }
      }
      
      void DefaultQuit()
      {
         this->BaseABC::Quit();
      }
      
      virtual void AddDrawable(DeltaDrawable* drawable)
      {
         if( override AddDrawable = this->get_override("AddDrawable") )
         {
            #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
            call<void>( AddDrawable.ptr(), drawable );
            #else
            AddDrawable( drawable );
            #endif
         }
         else
         {
            BaseABC::AddDrawable( drawable );
         }
      }
      
      void DefaultAddDrawable(DeltaDrawable* drawable)
      {
         BaseABC::AddDrawable( drawable );
      }
      
      virtual void RemoveDrawable(DeltaDrawable* drawable)
      {
         if( override RemoveDrawable = this->get_override("RemoveDrawable") )
         {
            #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
            call<void>( RemoveDrawable.ptr(), drawable );
            #else
            RemoveDrawable(drawable);
            #endif
         }
         else
         {
            BaseABC::RemoveDrawable(drawable);
         }
      }
      
      void DefaultRemoveDrawable(DeltaDrawable* drawable)
      {
         BaseABC::RemoveDrawable(drawable);
      }

      
   protected:
      
      virtual void EventTraversal(const double deltaFrameTime)
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<void>( this->get_override("EventTraversal").ptr(), deltaFrameTime );
         #else
         this->get_override("EventTraversal")(deltaFrameTime);
         #endif
      }

      virtual void PreFrame(const double deltaFrameTime)
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<void>( this->get_override("PreFrame").ptr(), deltaFrameTime );
         #else
         this->get_override("PreFrame")(deltaFrameTime);
         #endif
      }
      virtual void Frame(const double deltaFrameTime)
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<void>( this->get_override("Frame").ptr(), deltaFrameTime );
         #else
         this->get_override("Frame")(deltaFrameTime);
         #endif
      }
      virtual void PostFrame(const double deltaFrameTime)
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<void>( this->get_override("PostFrame").ptr(), deltaFrameTime );
         #else
         this->get_override("PostFrame")(deltaFrameTime);
         #endif
      }
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LoadMap_overloads, LoadMap, 1, 2)

void initBaseABCBindings()
{
   BaseABC* (*BaseABCGI1)(int) = &BaseABC::GetInstance;
   BaseABC* (*BaseABCGI2)(std::string) = &BaseABC::GetInstance;

   dtCore::Camera       *(BaseABC::*GetCameraNonConst)()       = &BaseABC::GetCamera;
   const dtCore::Camera *(BaseABC::*GetCameraConst   )() const = &BaseABC::GetCamera;

   dtCore::Map& (BaseABC::*LoadMap1)( const std::string&, bool ) = &BaseABC::LoadMap;
   void (BaseABC::*LoadMap2)( dtCore::Map&, bool ) = &BaseABC::LoadMap;
   dtCore::Mouse*(BaseABC::*GETMOUSE)() =&BaseABC::GetMouse;
   dtCore::View*(BaseABC::*GETVIEW)() =&BaseABC::GetView;

   class_<BaseABCWrap, bases<Base>, dtCore::RefPtr<BaseABCWrap>, boost::noncopyable>("BaseABC", no_init)
      .def("GetInstanceCount", &BaseABC::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", BaseABCGI1, return_internal_reference<>())
      .def("GetInstance", BaseABCGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("Config", &BaseABC::Config, &BaseABCWrap::DefaultConfig)
      .def("Quit", &BaseABC::Quit, &BaseABCWrap::DefaultQuit)
      .def("AddDrawable", &BaseABC::AddDrawable, &BaseABCWrap::DefaultAddDrawable)
      .def("RemoveDrawable", &BaseABC::RemoveDrawable, &BaseABCWrap::DefaultRemoveDrawable)
      .def("GetWindow", &BaseABC::GetWindow, return_internal_reference<>())
      .def("GetCamera", GetCameraNonConst, return_internal_reference<>())
      .def("GetCamera", GetCameraConst,    return_internal_reference<>())
      .def("GetScene", &BaseABC::GetScene, return_internal_reference<>())
      .def("GetKeyboard", &BaseABC::GetKeyboard, return_internal_reference<>())
      .def("GetMouse", GETMOUSE, return_internal_reference<>())
	  .def("GetView", GETVIEW, return_internal_reference<>())
      .def("LoadMap", LoadMap1, LoadMap_overloads()[return_internal_reference<>()])
      .def("LoadMap", LoadMap2, LoadMap_overloads())
      ;
}
