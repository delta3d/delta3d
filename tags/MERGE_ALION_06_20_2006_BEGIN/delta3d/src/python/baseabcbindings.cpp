// baseabcbindings.cpp: BaseABC binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtABC/baseabc.h>
#include <dtDAL/map.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/scene.h>

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
            Config();
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
            Quit();
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
            AddDrawable( drawable );
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
            RemoveDrawable(drawable);
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
      
      virtual void PreFrame(const double deltaFrameTime)
      {
         this->get_override("PreFrame")(deltaFrameTime);
      }
      virtual void Frame(const double deltaFrameTime)
      {
         this->get_override("Frame")(deltaFrameTime);
      }
      virtual void PostFrame(const double deltaFrameTime)
      {
         this->get_override("PostFrame")(deltaFrameTime);
      }
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LoadMap_overloads, LoadMap, 1, 2)

void initBaseABCBindings()
{
   BaseABC* (*BaseABCGI1)(int) = &BaseABC::GetInstance;
   BaseABC* (*BaseABCGI2)(std::string) = &BaseABC::GetInstance;

   dtDAL::Map& (BaseABC::*LoadMap1)( const std::string&, bool ) = &BaseABC::LoadMap;
   void (BaseABC::*LoadMap2)( dtDAL::Map&, bool ) = &BaseABC::LoadMap;
   const dtCore::Mouse*(BaseABC::*GETMOUSE)() const =&BaseABC::GetMouse;
   
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
      .def("GetCamera", &BaseABC::GetCamera, return_internal_reference<>())
      .def("GetScene", &BaseABC::GetScene, return_internal_reference<>())
      .def("GetKeyboard", &BaseABC::GetKeyboard, return_internal_reference<>())
      .def("GetMouse", GETMOUSE, return_internal_reference<>())
      //.def("GetMouse", &BaseABC::GetMouse, return_internal_reference<>())
      .def("LoadMap", LoadMap1, LoadMap_overloads()[return_internal_reference<>()])
      .def("LoadMap", LoadMap2, LoadMap_overloads())
      ;
}
