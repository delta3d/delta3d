// applicationbindings.cpp: Application binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "application.h"

using namespace boost::python;
using namespace dtABC;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(DDG_overloads, DisplayDebugGUI, 0, 1)

class ApplicationWrap : public Application
{
   public:

      ApplicationWrap(PyObject* self, std::string configFilename = "")
         : Application(configFilename),
           mSelf(self)
      {}

   protected:
      
      virtual void PreFrame(const double deltaFrameTime)
      {
         if(PyObject_HasAttrString(mSelf, "PreFrame"))
         {
            call_method<void>(mSelf, "PreFrame", deltaFrameTime);
         }
         else
         {
            Application::PreFrame(deltaFrameTime);
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
            Application::Frame(deltaFrameTime);
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
            Application::PostFrame(deltaFrameTime);
         }
      }
      
   private:
      
      PyObject* mSelf;
};

void initApplicationBindings()
{
   Application* (*ApplicationGI1)(int) = &Application::GetInstance;
   Application* (*ApplicationGI2)(std::string) = &Application::GetInstance;

   class_<Application, bases<BaseABC>, osg::ref_ptr<ApplicationWrap>, boost::noncopyable>("Application", init<optional<std::string> >())
      .def("GetInstanceCount", &Application::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ApplicationGI1, return_internal_reference<>())
      .def("GetInstance", ApplicationGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("Run", &Application::Run)
      .def("DisplayDebugGUI", &Application::DisplayDebugGUI, DDG_overloads());
}