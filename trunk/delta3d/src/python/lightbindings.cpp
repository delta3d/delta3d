// lightbindings.cpp: Light binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/light.h"

using namespace boost::python;
using namespace dtCore;

class LightWrap : public Light
{
public:

   LightWrap(PyObject* self, int number, LightingMode mode, osg::LightSource* lightSource = NULL )
      : Light( number, mode, lightSource ),
        mSelf(self)
   {}

   virtual void AddedToScene(Scene* scene)
   {
      call_method<void>(mSelf, "AddedToScene");
   }

   void DefaultAddedToScene(Scene* scene)
   {
      Light::AddedToScene(scene);
   }

protected:

   PyObject* mSelf;
};

void initLightBindings()
{
   scope LightScope = class_<Light, bases<DeltaDrawable>, osg::ref_ptr<LightWrap> >("Light", no_init)
      .def("GetLightSource", &Light::GetLightSource, return_internal_reference<>())
      .def("SetLightingMode", &Light::SetLightingMode)
      .def("GetLightingMode", &Light::GetLightingMode)
      .def("SetEnabled", &Light::SetEnabled)
      .def("GetEnabled", &Light::GetEnabled)
      .def("SetLightModel", &Light::SetLightModel)
      .def("SetNumber", &Light::SetNumber)
      .def("GetNumber", &Light::GetNumber)
      .def("SetAmbient", &Light::SetAmbient)
      .def("GetAmbient", &Light::GetAmbient)
      .def("SetDiffuse", &Light::SetDiffuse)
      .def("GetDiffuse", &Light::GetDiffuse)
      .def("SetSpecular", &Light::SetSpecular)
      .def("GetSpecular", &Light::GetSpecular)
      .def("AddedToScene", &Light::AddedToScene, &LightWrap::DefaultAddedToScene);

   enum_<Light::LightingMode>("LightingMode")
      .value("GLOBAL", Light::GLOBAL)
      .value("LOCAL", Light::LOCAL)
      .export_values();
}
