// enveffectbindings.cpp: EnvEffect binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/enveffect.h"

using namespace boost::python;
using namespace dtCore;

class EnvEffectWrap : public EnvEffect
{
   public:

      EnvEffectWrap(PyObject* self, std::string name = "")
         : EnvEffect(name),
           mSelf(self)
      {}

      virtual void Repaint(sgVec3 skyColor, sgVec3 fogColor,
                           double sunAngle, double sunAzimuth,
                           double visibility)
      {
         call_method<void>(mSelf, "Repaint", skyColor, fogColor, sunAngle, sunAzimuth, visibility);
      }
      
      virtual osg::Group *GetNode(void)
      {
         return call_method<osg::Group*>(mSelf, "GetNode");
      }
      
   protected:

      PyObject* mSelf;
};

void initEnvEffectBindings()
{
   EnvEffect* (*EnvEffectGI1)(int) = &EnvEffect::GetInstance;
   EnvEffect* (*EnvEffectGI2)(std::string) = &EnvEffect::GetInstance;

   class_<EnvEffect, bases<Base>, osg::ref_ptr<EnvEffectWrap>, boost::noncopyable>("EnvEffect", init<optional<std::string> >())
      .def("GetInstanceCount", &EnvEffect::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", EnvEffectGI1, return_internal_reference<>())
      .def("GetInstance", EnvEffectGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("Repaint", &EnvEffect::Repaint)
      .def("GetNode", &EnvEffect::GetNode, return_internal_reference<>());
}
