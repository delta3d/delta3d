// scenebindings.cpp: Scene binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include <dtCore/scene.h>
#include <dtCore/light.h>
#include <osg/Group>

using namespace boost::python;
using namespace dtCore;

void initSceneBindings()
{
   Scene* (*SceneGI1)(int) = &Scene::GetInstance;
   Scene* (*SceneGI2)(std::string) = &Scene::GetInstance;

   float (Scene::*GetHeightOfTerrain1)( float x, float y ) = &Scene::GetHeightOfTerrain;
   
   void (Scene::*SetGravity1)(const osg::Vec3&) = &Scene::SetGravity;
   void (Scene::*SetGravity2)(float, float, float) = &Scene::SetGravity;
   
   void (Scene::*GetGravity1)(osg::Vec3&) const = &Scene::GetGravity;
   void (Scene::*GetGravity2)(float&, float&, float&) const = &Scene::GetGravity;

   Light* (Scene::*GetLight1)(int) = &Scene::GetLight;
   const Light* (Scene::*GetLight2)(int) const = &Scene::GetLight;
   Light* (Scene::*GetLight3)(const std::string&) = &Scene::GetLight;
   const Light* (Scene::*GetLight4)(const std::string&) const = &Scene::GetLight;
   
   scope sceneScope = class_<Scene, bases<Base>, dtCore::RefPtr<Scene>, boost::noncopyable >("Scene", init<optional<const std::string&> >())
      .def("GetInstanceCount", &Scene::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SceneGI1, return_internal_reference<>())
      .def("GetInstance", SceneGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetSceneNode", &Scene::GetSceneNode, return_internal_reference<>())
      .def("AddDrawable", &Scene::AddDrawable)
      .def("RemoveDrawable", &Scene::RemoveDrawable)
      .def("GetDrawable", &Scene::GetDrawable, return_internal_reference<>())
      .def("SetRenderState", &Scene::SetRenderState)
      .def("GetHeightOfTerrain", GetHeightOfTerrain1)
      //.def("GetSpaceID", &Scene::GetSpaceID, return_value_policy<return_opaque_pointer>())
      //.def("GetWorldID", &Scene::GetWorldID, return_value_policy<return_opaque_pointer>())
      .def("SetGravity", SetGravity1)
      .def("SetGravity", SetGravity2)
      .def("GetGravity", GetGravity1)
      .def("GetGravity", GetGravity2)
      .def("SetPhysicsStepSize", &Scene::SetPhysicsStepSize)
      .def("GetPhysicsStepSize", &Scene::GetPhysicsStepSize)
      .def("GetLight", GetLight1, return_internal_reference<>())
      .def("GetLight", GetLight2, return_internal_reference<>())
      .def("GetLight", GetLight3, return_internal_reference<>())
      .def("GetLight", GetLight4, return_internal_reference<>())
      .def("UseSceneLight", &Scene::UseSceneLight)
      .def("GetDrawableIndex", &Scene::GetDrawableIndex)
      .def("GetNumberOfAddedDrawable", &Scene::GetNumberOfAddedDrawable)
      .def("EnablePaging", &Scene::EnablePaging)
      .def("DisablePaging", &Scene::DisablePaging);

   enum_<Scene::Face>("Face")
      .value("FRONT", Scene::FRONT)
      .value("BACK", Scene::BACK)
      .value("FRONT_AND_BACK", Scene::FRONT_AND_BACK)
      .export_values();

   enum_<Scene::Mode>("Mode")
      .value("POINT", Scene::POINT)
      .value("LINE", Scene::LINE)
      .value("FILL", Scene::FILL)
      .export_values();    

   
}
