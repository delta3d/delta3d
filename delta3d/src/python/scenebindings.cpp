// scenebindings.cpp: Scene binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/scene.h"

using namespace boost::python;
using namespace dtCore;

void initSceneBindings()
{
   Scene* (*SceneGI1)(int) = &Scene::GetInstance;
   Scene* (*SceneGI2)(std::string) = &Scene::GetInstance;

   void (Scene::*SetGravity1)(sgVec3) = &Scene::SetGravity;
   void (Scene::*SetGravity2)(float, float, float) = &Scene::SetGravity;
   
   void (Scene::*GetGravity1)(sgVec3) = &Scene::GetGravity;
   void (Scene::*GetGravity2)(float*, float*, float*) = &Scene::GetGravity;
   
   scope sceneScope = class_<Scene, bases<Base>, osg::ref_ptr<Scene> >("Scene", init<optional<std::string> >())
      .def("GetInstanceCount", &Scene::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SceneGI1, return_internal_reference<>())
      .def("GetInstance", SceneGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetSceneHandler", &Scene::GetSceneHandler, return_internal_reference<>())
      .def("GetSceneNode", &Scene::GetSceneNode, return_internal_reference<>())
      .def("AddDrawable", &Scene::AddDrawable)
      .def("RemoveDrawable", &Scene::AddDrawable)
      .def("GetHeightOfTerrain", &Scene::GetHeightOfTerrain)
      .def("GetSpaceID", &Scene::GetSpaceID, return_value_policy<return_opaque_pointer>())
      .def("GetWorldID", &Scene::GetWorldID, return_value_policy<return_opaque_pointer>())
      .def("SetGravity", SetGravity1)
      .def("SetGravity", SetGravity2)
      .def("GetGravity", GetGravity1)
      .def("GetGravity", GetGravity2)
      .def("SetNextStatisticsType", &Scene::SetNextStatisticsType)
      .def("SetStatisticsType", &Scene::SetStatisticsType)
      .def("SetPhysicsStepSize", &Scene::SetPhysicsStepSize)
      .def("GetPhysicsStepSize", &Scene::GetPhysicsStepSize)
      .def("AddLight", &Scene::AddLight)
      .def("RemoveLight", &Scene::RemoveLight)
      .def("GetLight", &Scene::GetLight, return_internal_reference<>())
      .def("UseSceneLight", &Scene::UseSceneLight);
      
   class_<Scene::CollisionData>("CollisionData")
      .def_readonly("mBodies", &Scene::CollisionData::mBodies)
      .def_readonly("mLocation", &Scene::CollisionData::mLocation)
      .def_readonly("mNormal", &Scene::CollisionData::mNormal)
      .def_readonly("mDepth", &Scene::CollisionData::mDepth);
}
