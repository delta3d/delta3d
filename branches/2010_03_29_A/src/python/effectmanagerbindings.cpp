// effectmanagerbindings.cpp: EffectManager binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/effectmanager.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>
#include <osg/Node>

using namespace boost::python;
using namespace dtCore;

class EffectManagerWrap : public EffectManager, public wrapper<EffectManager>
{
   public:

      EffectManagerWrap( const std::string& name = "effectManager") : EffectManager(name)
      {
      }

      Detonation* AddDetonation1( const osg::Vec3& position )
      {
         return AddDetonation( position );
      }

      Detonation* AddDetonation2( const osg::Vec3& position, const std::string& type )
      {
         return AddDetonation( position, type );
      }

      Detonation* AddDetonation3( const osg::Vec3& position, const std::string& type, double timeToLive )
      {
         return AddDetonation( position, type, timeToLive );
      }

      Detonation* AddDetonation4( const osg::Vec3& position, const std::string& type, double timeToLive, Transformable* parent )
      {
         return AddDetonation( position, type, timeToLive, parent );
      }
};

void initEffectManagerBindings()
{
   EffectManager* (*EffectManagerGI1)(int) = &EffectManager::GetInstance;
   EffectManager* (*EffectManagerGI2)(std::string) = &EffectManager::GetInstance;

   enum_<DetonationType>("DetonationType")
      .value("HighExplosiveDetonation", HighExplosiveDetonation)
      .value("SmokeDetonation", SmokeDetonation)
      .export_values();

   void (EffectManager::*AddDetonationTypeMapping2)(const std::string&, const std::string&) = &EffectManager::AddDetonationTypeMapping;

   void (EffectManager::*RemoveDetonationTypeMapping2)(const std::string&) = &EffectManager::RemoveDetonationTypeMapping;

   class_<EffectManagerWrap, bases<DeltaDrawable>, dtCore::RefPtr<EffectManagerWrap>, boost::noncopyable >("EffectManager", init<optional<const std::string&> >())
      .def("GetInstanceCount", &EffectManager::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", EffectManagerGI1, return_internal_reference<>())
      .def("GetInstance", EffectManagerGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("AddDetonationTypeMapping", AddDetonationTypeMapping2)
      .def("RemoveDetonationTypeMapping", RemoveDetonationTypeMapping2)
      .def("GetEffectCount", &EffectManager::GetEffectCount)
      .def("GetEffect", &EffectManager::GetEffect, return_internal_reference<>())
      .def("AddDetonation", &EffectManagerWrap::AddDetonation1, return_internal_reference<>())
      .def("AddDetonation", &EffectManagerWrap::AddDetonation2, return_internal_reference<>())
      .def("AddDetonation", &EffectManagerWrap::AddDetonation3, return_internal_reference<>())
      .def("AddDetonation", &EffectManagerWrap::AddDetonation4, return_internal_reference<>())
      .def("RemoveEffect", &EffectManager::RemoveEffect)
      .def("AddEffectListener", &EffectManager::AddEffectListener)
      .def("RemoveEffectListener", &EffectManager::RemoveEffectListener);

   class_<EffectListener, dtCore::RefPtr<EffectListener>, boost::noncopyable>("EffectListener", no_init)
      .def("EffectAdded", pure_virtual(&EffectListener::EffectAdded) )
      .def("EffectRemoved", pure_virtual(&EffectListener::EffectRemoved) );

   osg::Node* (Effect::*GetNode1)() = &Effect::GetNode;
   const osg::Node* (Effect::*GetNode2)() const = &Effect::GetNode;

   class_<Effect, dtCore::RefPtr<Effect> >("Effect", init<osg::Node*, double>())
      .def("GetNode", GetNode1, return_internal_reference<>())
      .def("GetNode", GetNode2, return_internal_reference<>())
      .def("SetTimeToLive", &Effect::SetTimeToLive)
      .def("GetTimeToLive", &Effect::GetTimeToLive)
      .def("SetDying", &Effect::SetDying)
      .def("IsDying", &Effect::IsDying);

   void (Detonation::*GetPosition1)(osg::Vec3& res) const = &Detonation::GetPosition;
   const osg::Vec3& (Detonation::*GetPosition2)() const = &Detonation::GetPosition;

   Transformable* (Detonation::*GetParent1)() = &Detonation::GetParent;
   const Transformable* (Detonation::*GetParent2)() const = &Detonation::GetParent;

   const std::string& (Detonation::*GetType1)() = &Detonation::GetType;
   void (Detonation::*GetType2)( DetonationType& type ) = &Detonation::GetType;

   class_<Detonation, bases<Effect>, dtCore::RefPtr<Detonation> >("Detonation", init<osg::Node*, double, const osg::Vec3&, const std::string&, Transformable*>() )
      .def("GetPosition", GetPosition1, return_value_policy<copy_const_reference>())
      .def("GetPosition", GetPosition2, return_value_policy<copy_const_reference>())
      .def("GetType", GetType1, return_value_policy<copy_const_reference>())
      .def("GetType", GetType2)
      .def("GetParent", GetParent1, return_internal_reference<>())
      .def("GetParent", GetParent2, return_internal_reference<>())
      ;
}
