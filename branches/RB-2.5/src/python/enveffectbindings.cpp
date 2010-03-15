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

      EnvEffectWrap(PyObject* self, const std::string& name = "")
         : EnvEffect(name),
           mSelf(self)
      {}

      virtual void Repaint(const osg::Vec3& skyColor, const osg::Vec3& fogColor,
                           double sunAngle, double sunAzimuth,
                           double visibility)
      {
         call_method<void>(mSelf, "Repaint", skyColor, fogColor, sunAngle, sunAzimuth, visibility);
      }
      
   protected:

      PyObject* mSelf;
};

void initEnvEffectBindings()
{
   EnvEffect* (*EnvEffectGI1)(int) = &EnvEffect::GetInstance;
   EnvEffect* (*EnvEffectGI2)(std::string) = &EnvEffect::GetInstance;

   class_<EnvEffect, bases<DeltaDrawable>, dtCore::RefPtr<EnvEffectWrap>, boost::noncopyable>("EnvEffect", init<optional<const std::string&> >())
      .def("GetInstanceCount", &EnvEffect::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", EnvEffectGI1, return_internal_reference<>())
      .def("GetInstance", EnvEffectGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("Repaint", &EnvEffectWrap::Repaint);
}
