// scenebindings.cpp: Scene binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "scene.h"

using namespace boost::python;
using namespace dtCore;

void initSceneBindings()
{
   Scene* (*SceneGI1)(int) = &Scene::GetInstance;
   Scene* (*SceneGI2)(std::string) = &Scene::GetInstance;

   class_<Scene, bases<Base>, osg::ref_ptr<Scene> >("Scene", init<optional<std::string> >())
      .def("GetInstanceCount", &Scene::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SceneGI1, return_internal_reference<>())
      .def("GetInstance", SceneGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("AddDrawable", &Scene::AddDrawable);
}