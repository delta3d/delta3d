// distypebindings.cpp: DIS type binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtHLA/dis_types.h"

using namespace boost::python;
using namespace dtHLA;

void initDISTypeBindings()
{
   enum_<EntityKind>("EntityKind")
      .value("OtherKind", OtherKind)
      .value("PlatformKind", PlatformKind)
      .value("MunitionKind", MunitionKind)
      .value("LifeFormKind", LifeFormKind)
      .value("EnvironmentalKind", EnvironmentalKind)
      .value("CulturalFeatureKind", CulturalFeatureKind)
      .value("SupplyKind", SupplyKind)
      .value("RadioKind", RadioKind)
      .value("ExpendableKind", ExpendableKind)
      .value("SensorEmitterKind", SensorEmitterKind)
      .export_values();
      
   enum_<PlatformDomain>("PlatformDomain")
      .value("OtherPlatformDomain", OtherPlatformDomain)
      .value("LandPlatformDomain", LandPlatformDomain)
      .value("AirPlatformDomain", AirPlatformDomain)
      .value("SurfacePlatformDomain", SurfacePlatformDomain)
      .value("SubsurfacePlatformDomain", SubsurfacePlatformDomain)
      .value("SpacePlatformDomain", SpacePlatformDomain)
      .export_values();
   
   enum_<DamageState>("DamageState")
      .value("NoDamage", NoDamage)
      .value("SlightDamage", SlightDamage)
      .value("ModerateDamage", ModerateDamage)
      .value("Destroyed", Destroyed)
      .export_values();
      
   class_<EntityIdentifier>("EntityIdentifier", init<optional<unsigned short, unsigned short, unsigned short> >())
      .def("EncodedLength", &EntityIdentifier::EncodedLength)
      .def("Encode", &EntityIdentifier::Encode)
      .def("Decode", &EntityIdentifier::Decode)
      .def("SetSiteIdentifier", &EntityIdentifier::SetSiteIdentifier)
      .def("GetSiteIdentifier", &EntityIdentifier::GetSiteIdentifier)
      .def("SetApplicationIdentifier", &EntityIdentifier::SetApplicationIdentifier)
      .def("GetApplicationIdentifier", &EntityIdentifier::GetApplicationIdentifier)
      .def("SetEntityIdentifier", &EntityIdentifier::SetEntityIdentifier)
      .def("GetEntityIdentifier", &EntityIdentifier::GetEntityIdentifier);
      
   class_<EntityType>("EntityType", init<optional<unsigned char, unsigned char, unsigned short, unsigned char, unsigned char, unsigned char, unsigned char> >())
      .def(self < self)
      .def("RankMatch", &EntityType::RankMatch)
      .def("EncodedLength", &EntityType::EncodedLength)
      .def("Encode", &EntityType::Encode)
      .def("Decode", &EntityType::Decode)
      .def("SetKind", &EntityType::SetKind)
      .def("GetKind", &EntityType::GetKind)
      .def("SetDomain", &EntityType::SetDomain)
      .def("GetDomain", &EntityType::GetDomain)
      .def("SetCountry", &EntityType::SetCountry)
      .def("GetCountry", &EntityType::GetCountry)
      .def("SetCategory", &EntityType::SetCategory)
      .def("GetCategory", &EntityType::GetCategory)
      .def("SetSubcategory", &EntityType::SetSubcategory)
      .def("GetSubcategory", &EntityType::GetSubcategory)
      .def("SetSpecific", &EntityType::SetSpecific)
      .def("GetSpecific", &EntityType::GetSpecific)
      .def("SetExtra", &EntityType::SetExtra)
      .def("GetExtra", &EntityType::GetExtra);
      
   class_<EventIdentifier>("EventIdentifier", init<optional<unsigned short> >())
      .def("EncodedLength", &EventIdentifier::EncodedLength)
      .def("Encode", &EventIdentifier::Encode)
      .def("Decode", &EventIdentifier::Decode)
      .def("SetEventIdentifier", &EventIdentifier::SetEventIdentifier)
      .def("GetEventIdentifier", &EventIdentifier::GetEventIdentifier);
      
   class_<WorldCoordinate>("WorldCoordinate", init<optional<double, double, double> >())
      .def("EncodedLength", &WorldCoordinate::EncodedLength)
      .def("Encode", &WorldCoordinate::Encode)
      .def("Decode", &WorldCoordinate::Decode)
      .def("SetX", &WorldCoordinate::SetX)
      .def("GetX", &WorldCoordinate::GetX)
      .def("SetY", &WorldCoordinate::SetY)
      .def("GetY", &WorldCoordinate::GetY)
      .def("SetZ", &WorldCoordinate::SetZ)
      .def("GetZ", &WorldCoordinate::GetZ);
      
   class_<EulerAngles>("EulerAngles", init<optional<float, float, float> >())
      .def("EncodedLength", &EulerAngles::EncodedLength)
      .def("Encode", &EulerAngles::Encode)
      .def("Decode", &EulerAngles::Decode)
      .def("SetPsi", &EulerAngles::SetPsi)
      .def("GetPsi", &EulerAngles::GetPsi)
      .def("SetTheta", &EulerAngles::SetTheta)
      .def("GetTheta", &EulerAngles::GetTheta)
      .def("SetPhi", &EulerAngles::SetPhi)
      .def("GetPhi", &EulerAngles::GetPhi);
      
   class_<VelocityVector>("VelocityVector", init<optional<float, float, float> >())
      .def("EncodedLength", &VelocityVector::EncodedLength)
      .def("Encode", &VelocityVector::Encode)
      .def("Decode", &VelocityVector::Decode)
      .def("SetX", &VelocityVector::SetX)
      .def("GetX", &VelocityVector::GetX)
      .def("SetY", &VelocityVector::SetY)
      .def("GetY", &VelocityVector::GetY)
      .def("SetZ", &VelocityVector::SetZ)
      .def("GetZ", &VelocityVector::GetZ);
      
   enum_<ArticulatedTypeMetric>("ArticulatedTypeMetric")
      .value("PositionMetric", PositionMetric)
      .value("PositionRateMetric", PositionRateMetric)
      .value("ExtensionMetric", ExtensionMetric)
      .value("ExtensionRateMetric", ExtensionRateMetric)
      .value("XMetric", XMetric)
      .value("XRateMetric", XRateMetric)
      .value("YMetric", YMetric)
      .value("YRateMetric", YRateMetric)
      .value("ZMetric", ZMetric)
      .value("ZRateMetric", ZRateMetric)
      .value("AzimuthMetric", AzimuthMetric)
      .value("AzimuthRateMetric", AzimuthRateMetric)
      .value("ElevationMetric", ElevationMetric)
      .value("ElevationRateMetric", ElevationRateMetric)
      .value("RotationMetric", RotationMetric)
      .value("RotationRateMetric", RotationRateMetric)
      .export_values();
      
   class_<ArticulatedParts>("ArticulatedParts", init<optional<unsigned int, unsigned int, float> >())
      .def("EncodedLength", &ArticulatedParts::EncodedLength)
      .def("Encode", &ArticulatedParts::Encode)
      .def("Decode", &ArticulatedParts::Decode)
      .def("SetClass", &ArticulatedParts::SetClass)
      .def("GetClass", &ArticulatedParts::GetClass)
      .def("SetTypeMetric", &ArticulatedParts::SetTypeMetric)
      .def("GetTypeMetric", &ArticulatedParts::GetTypeMetric)
      .def("SetValue", &ArticulatedParts::SetValue)
      .def("GetValue", &ArticulatedParts::GetValue);
      
   class_<AttachedParts>("AttachedParts", init<optional<unsigned int> >())
      .def("EncodedLength", &AttachedParts::EncodedLength)
      .def("Encode", &AttachedParts::Encode)
      .def("Decode", &AttachedParts::Decode)
      .def("SetStation", &AttachedParts::SetStation)
      .def("GetStation", &AttachedParts::GetStation)
      .def("SetStoreType", &AttachedParts::SetStoreType)
      .def("GetStoreType", &AttachedParts::GetStoreType, return_internal_reference<>());
        
   enum_<ArticulatedParameterType>("ArticulatedParameterType")
      .value("ArticulatedPart", ArticulatedPart)
      .value("AttachedPart", AttachedPart)
      .export_values();
      
   class_<ParameterValue>("ParameterValue", init<optional<ArticulatedParameterType> >())
      .def("EncodedLength", &ParameterValue::EncodedLength)
      .def("Encode", &ParameterValue::Encode)
      .def("Decode", &ParameterValue::Decode)
      .def("SetArticulatedParameterType", &ParameterValue::SetArticulatedParameterType)
      .def("GetArticulatedParameterType", &ParameterValue::GetArticulatedParameterType)
      .def("SetArticulatedParts", &ParameterValue::SetArticulatedParts)
      .def("GetArticulatedParts", &ParameterValue::GetArticulatedParts, return_internal_reference<>())
      .def("SetAttachedParts", &ParameterValue::SetAttachedParts)
      .def("GetAttachedParts", &ParameterValue::GetAttachedParts, return_internal_reference<>());
      
   class_<ArticulatedParameter>("ArticulatedParameter", init<optional<unsigned char, unsigned short> >())
      .def("EncodedLength", &ArticulatedParameter::EncodedLength)
      .def("Encode", &ArticulatedParameter::Encode)
      .def("Decode", &ArticulatedParameter::Decode)
      .def("SetArticulatedParameterChange", &ArticulatedParameter::SetArticulatedParameterChange)
      .def("GetArticulatedParameterChange", &ArticulatedParameter::GetArticulatedParameterChange)
      .def("SetPartAttachedTo", &ArticulatedParameter::SetPartAttachedTo)
      .def("GetPartAttachedTo", &ArticulatedParameter::GetPartAttachedTo)
      .def("SetParameterValue", &ArticulatedParameter::SetParameterValue)
      .def("GetParameterValue", &ArticulatedParameter::GetParameterValue, return_internal_reference<>());
}
