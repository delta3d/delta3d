// scenebindings.cpp: Scene binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#include "scene.h"

using namespace boost::python;
using namespace P51;

void initSceneBindings()
{
   Scene* (*SceneGI1)(int) = &Scene::GetInstance;
   Scene* (*SceneGI2)(std::string) = &Scene::GetInstance;

   class_<Scene, bases<Base> >("Scene", init<optional<std::string> >())
      .def("GetInstanceCount", &Scene::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SceneGI1, return_internal_reference<>())
      .def("GetInstance", SceneGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("AddDrawable", &Scene::AddDrawable);
}