// positionallightbindings.cpp: PositionalLight binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/positionallight.h"

using namespace boost::python;
using namespace dtCore;

class PositionalLightWrap : public PositionalLight
{
public:

   PositionalLightWrap(PyObject* self, int number, std::string name = "defaultPositonalLight", LightingMode mode = GLOBAL )
      :  PositionalLight( number, name, mode ),
         mSelf(self)
   {}

   virtual osg::Node* GetOSGNode()
   {
      call_method<void>(mSelf, "GetOSGNode");
   } 

   void DefaultGetOSGNode()
   {
      PositionalLight::GetOSGNode();
   }

   virtual void AddChild( DeltaDrawable *child )
   {
      call_method<void>(mSelf, "AddChild");
   }

   void DefaultAddChild( DeltaDrawable *child )
   {
      PositionalLight::AddChild(child);
   }

   virtual void RemoveChild( DeltaDrawable *child )
   {
      call_method<void>(mSelf, "RemoveChild");
   }

   void DefaultRemoveChild( DeltaDrawable *child )
   {
      PositionalLight::RemoveChild(child);
   }

   virtual void AddedToScene( Scene *scene )
   {
      call_method<void>(mSelf, "AddedToScene"); 
   }

   void DefaultAddedToScene( Scene *scene )
   {
      PositionalLight::AddedToScene(scene);
   }

protected:

   PyObject* mSelf;
};

void initPositionalLightBindings()
{
   class_<PositionalLight, bases<Light,Transformable>, osg::ref_ptr<PositionalLightWrap> >("PositionalLight", init<int, optional<std::string, Light::LightingMode> >())
      .def("SetAttenuation", &PositionalLight::SetAttenuation)
      .def("GetAttenuation", &PositionalLight::GetAttenuation)
      .def("GetOSGNode", &PositionalLight::GetOSGNode, return_internal_reference<>()) 
      .def("AddChild", &PositionalLight::AddChild, with_custodian_and_ward<1, 2>())
      .def("RemoveChild", &PositionalLight::RemoveChild, &PositionalLightWrap::RemoveChild)
      .def("AddedToScene", &PositionalLight::AddedToScene, &PositionalLightWrap::AddedToScene);
}
