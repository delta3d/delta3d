// baseabcbindings.cpp: BaseABC binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "baseabc.h"

using namespace boost::python;
using namespace dtABC;
using namespace dtCore;

class BaseABCWrap : public BaseABC
{
   public:

      BaseABCWrap(PyObject* self)
         : mSelf(self)
      {}

      virtual void Config()
      {
         call_method<void>(mSelf, "Config");
      }
      
      void DefaultConfig()
      {
         BaseABC::Config();
      }
      
      virtual void Quit()
      {
         call_method<void>(mSelf, "Quit");
      }
      
      void DefaultQuit()
      {
         BaseABC::Quit();
      }
      
      virtual void AddDrawable(Drawable* drawable)
      {
         call_method<void>(mSelf, "AddDrawable", drawable);
      }
      
      void DefaultAddDrawable(Drawable* drawable)
      {
         BaseABC::AddDrawable(drawable);
      }
      
      virtual void RemoveDrawable(Drawable* drawable)
      {
         call_method<void>(mSelf, "RemoveDrawable", drawable);
      }
      
      void DefaultRemoveDrawable(Drawable* drawable)
      {
         BaseABC::RemoveDrawable(drawable);
      }
      
   protected:
      
      virtual void PreFrame(const double deltaFrameTime)
      {
         if(PyObject_HasAttrString(mSelf, "PreFrame"))
         {
            call_method<void>(mSelf, "PreFrame", deltaFrameTime);
         }
         else
         {
            BaseABC::PreFrame(deltaFrameTime);
         }
      }
      
      virtual void Frame(const double deltaFrameTime)
      {
         if(PyObject_HasAttrString(mSelf, "Frame"))
         {
            call_method<void>(mSelf, "Frame", deltaFrameTime);
         }
         else
         {
            BaseABC::Frame(deltaFrameTime);
         }
      }
      
      virtual void PostFrame(const double deltaFrameTime)
      {
         if(PyObject_HasAttrString(mSelf, "PostFrame"))
         {
            call_method<void>(mSelf, "PostFrame", deltaFrameTime);
         }
         else
         {
            BaseABC::PostFrame(deltaFrameTime);
         }
      }
      
   private:
      
      PyObject* mSelf;
};

void initBaseABCBindings()
{
   BaseABC* (*BaseABCGI1)(int) = &BaseABC::GetInstance;
   BaseABC* (*BaseABCGI2)(std::string) = &BaseABC::GetInstance;

   class_<BaseABC, bases<Base>, osg::ref_ptr<BaseABCWrap>, boost::noncopyable>("BaseABC", no_init)
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
      .def("GetMouse", &BaseABC::GetMouse, return_internal_reference<>());
}