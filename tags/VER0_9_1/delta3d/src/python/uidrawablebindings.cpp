// uidrawablebindings.cpp: UIDrawable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/uidrawable.h"

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CFF_overloads, CreateFixedFont, 2, 7)

void initUIDrawableBindings()
{
   UIDrawable* (*UIDrawableGI1)(int) = &UIDrawable::GetInstance;
   UIDrawable* (*UIDrawableGI2)(std::string) = &UIDrawable::GetInstance;

   void (UIDrawable::*CreateShader1)(const std::string&) = &UIDrawable::CreateShader;
   void (UIDrawable::*CreateShader2)(const std::string&, const osg::Vec4&) = &UIDrawable::CreateShader;
   void (UIDrawable::*CreateShader3)(const std::string&, sgVec4) = &UIDrawable::CreateShader;
   void (UIDrawable::*CreateShader4)(const std::string&, const std::string&) = &UIDrawable::CreateShader;
   void (UIDrawable::*CreateShader5)(const std::string&, const osg::Vec4&, const std::string&) = &UIDrawable::CreateShader;
   void (UIDrawable::*CreateShader6)(const std::string&, sgVec4, const std::string&) = &UIDrawable::CreateShader;
   
   void (UIDrawable::*CreateBorder1)(const std::string&, osg::Vec2*, int) = &UIDrawable::CreateBorder;
   void (UIDrawable::*CreateBorder2)(const std::string&, sgVec2*, int) = &UIDrawable::CreateBorder;

   class_<UIDrawable, bases<DeltaDrawable,MouseListener,KeyboardListener>, dtCore::RefPtr<UIDrawable> >("UIDrawable", init<optional<int, int> >())
      .def("GetInstanceCount", &UIDrawable::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", UIDrawableGI1, return_internal_reference<>())
      .def("GetInstance", UIDrawableGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetUI", &UIDrawable::GetUI, return_internal_reference<>())
      .def("LoadGUIFile", &UIDrawable::LoadGUIFile)
      .def("SetWindowResolution", &UIDrawable::SetWindowResolution)
      .def("GetRenderer", &UIDrawable::GetRenderer, return_internal_reference<>())
      .def("CreateShader", CreateShader1)
      .def("CreateShader", CreateShader2)
      .def("CreateShader", CreateShader3)
      .def("CreateShader", CreateShader4)
      .def("CreateShader", CreateShader5)
      .def("CreateShader", CreateShader6)
      .def("GetShader", &UIDrawable::GetShader, return_internal_reference<>())
      .def("CreateBorder", CreateBorder1)
      .def("CreateBorder", CreateBorder2)
      .def("GetBorder", &UIDrawable::GetBorder, return_internal_reference<>())
      .def("CreateFixedFont", &UIDrawable::CreateFixedFont, CFF_overloads())
      .def("GetFont", &UIDrawable::GetFont, return_internal_reference<>())
      .def("AddFrame", &UIDrawable::AddFrame)
      .def("AddRootFrame", &UIDrawable::AddRootFrame)
      .def("SetActiveRootFrame", &UIDrawable::SetActiveRootFrame)
      .def("GetActiveRootFrame", &UIDrawable::GetActiveRootFrame, return_internal_reference<>());
 
}
