// effectmanagerbindings.cpp: EffectManager binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/effectmanager.h"

using namespace boost::python;
using namespace dtCore;

class EffectListenerWrap : public EffectListener
{
   public:
      
      EffectListenerWrap(PyObject* self)
         : mSelf(self)
      {}
      
      virtual void EffectAdded(EffectManager* effectManager, Effect* effect)
      {
         call_method<void>(mSelf, "EffectAdded", effectManager, effect);
      }
      
      void DefaultEffectAdded(EffectManager* effectManager, Effect* effect)
      {
         EffectListener::EffectAdded(effectManager, effect);
      }
      
      virtual void EffectRemoved(EffectManager* effectManager, Effect* effect)
      {
         call_method<void>(mSelf, "EffectRemoved", effectManager, effect);
      }
      
      void DefaultEffectRemoved(EffectManager* effectManager, Effect* effect)
      {
         EffectListener::EffectRemoved(effectManager, effect);
      }
      
   private:
      
      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(AD_overloads, AddDetonation, 1, 4)

void initEffectManagerBindings()
{
   EffectManager* (*EffectManagerGI1)(int) = &EffectManager::GetInstance;
   EffectManager* (*EffectManagerGI2)(std::string) = &EffectManager::GetInstance;

   enum_<DetonationType>("DetonationType")
      .value("HighExplosiveDetonation", HighExplosiveDetonation)
      .value("SmokeDetonation", SmokeDetonation)
      .export_values();
      
   class_<EffectManager, bases<Base, DeltaDrawable>, osg::ref_ptr<EffectManager> >("EffectManager", init<optional<std::string> >())
      .def("GetInstanceCount", &EffectManager::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", EffectManagerGI1, return_internal_reference<>())
      .def("GetInstance", EffectManagerGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("AddDetonationTypeMapping", &EffectManager::AddDetonationTypeMapping)
      .def("RemoveDetonationTypeMapping", &EffectManager::RemoveDetonationTypeMapping)
      .def("GetEffectCount", &EffectManager::GetEffectCount)
      .def("GetEffect", &EffectManager::GetEffect, return_internal_reference<>())
      .def("AddDetonation", &EffectManager::AddDetonation, AD_overloads()[return_internal_reference<>()])
      .def("RemoveEffect", &EffectManager::RemoveEffect)
      .def("AddEffectListener", &EffectManager::AddEffectListener)
      .def("RemoveEffectListener", &EffectManager::RemoveEffectListener);
      
   class_<EffectListener, EffectListenerWrap, boost::noncopyable>("EffectListener")
      .def("EffectAdded", &EffectListener::EffectAdded, &EffectListenerWrap::DefaultEffectAdded)
      .def("EffectRemoved", &EffectListener::EffectRemoved, &EffectListenerWrap::DefaultEffectRemoved);
      
   class_<Effect>("Effect", init<osg::Node*, double>())
      .def("GetNode", &Effect::GetNode, return_internal_reference<>())
      .def("SetTimeToLive", &Effect::SetTimeToLive)
      .def("GetTimeToLive", &Effect::GetTimeToLive)
      .def("SetDying", &Effect::SetDying)
      .def("IsDying", &Effect::IsDying);
      
   class_<Detonation, bases<Effect> >("Detonation", no_init)
      .def("GetPosition", &Detonation::GetPosition)
      .def("GetType", &Detonation::GetType)
      .def("GetParent", &Detonation::GetParent, return_internal_reference<>());
}
