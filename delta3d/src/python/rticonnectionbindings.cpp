// rticonnectionbindings.cpp: RTIConnection binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "rticonnection.h"

using namespace boost::python;
using namespace dtCore;
using namespace dtHLA;

class DetonationListenerWrap : public DetonationListener
{
   public:
   
      DetonationListenerWrap(PyObject* self)
         : mSelf(self)
      {}
      
      virtual void DetonationOccurred(
         WorldCoordinate detonationLocation,
         EventIdentifier eventIdentifier,
         unsigned short fuseType,
         EntityType munitionType,
         unsigned short warheadType,
         unsigned char detonationResultCode,
         unsigned short quantityFired
      )
      {
         call_method<void>(
            mSelf, 
            "DetonationOccurred", 
            detonationLocation,
            eventIdentifier,
            fuseType,
            munitionType,
            warheadType,
            detonationResultCode,
            quantityFired
         );
      }
      
      void DefaultDetonationOccurred(
         WorldCoordinate detonationLocation,
         EventIdentifier eventIdentifier,
         unsigned short fuseType,
         EntityType munitionType,
         unsigned short warheadType,
         unsigned char detonationResultCode,
         unsigned short quantityFired
      )
      {
         DetonationListener::DetonationOccurred(
            detonationLocation,
            eventIdentifier,
            fuseType,
            munitionType,
            warheadType,
            detonationResultCode,
            quantityFired
         );
      }
      
   private:
      
      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(JFF_overloads, JoinFederationExecution, 0, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EGM_overloads, EnableGlobeMode, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EUM_overloads, EnableUTMMode, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(AETM1_overloads, AddEntityTypeMapping, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(AETM2_overloads, AddEntityTypeMapping, 3, 4)

void initRTIConnectionBindings()
{
   RTIConnection* (*RTIConnectionGI1)(int) = &RTIConnection::GetInstance;
   RTIConnection* (*RTIConnectionGI2)(std::string) = &RTIConnection::GetInstance;

   void (RTIConnection::*AddEntityTypeMapping1)(const EntityType&, std::string, std::string) = &RTIConnection::AddEntityTypeMapping;
   void (RTIConnection::*AddEntityTypeMapping2)(const EntityType&, std::string, const std::map<unsigned int, std::string>&, std::string) = 
      &RTIConnection::AddEntityTypeMapping;
   
   {
      scope RTIConnection_scope = class_<RTIConnection, bases<Base>, osg::ref_ptr<RTIConnection> >("RTIConnection", init<optional<std::string> >())
         .def("GetInstanceCount", &RTIConnection::GetInstanceCount)
         .staticmethod("GetInstanceCount")
         .def("GetInstance", RTIConnectionGI1, return_internal_reference<>())
         .def("GetInstance", RTIConnectionGI2, return_internal_reference<>())
         .staticmethod("GetInstance")
         .def("JoinFederationExecution", &RTIConnection::JoinFederationExecution, JFF_overloads())
         .def("LeaveFederationExecution", &RTIConnection::LeaveFederationExecution)
         .def("GetExecutionName", &RTIConnection::GetExecutionName)
         .def("SetScene", &RTIConnection::SetScene)
         .def("GetScene", &RTIConnection::GetScene, return_internal_reference<>())
         .def("SetEffectManager", &RTIConnection::SetEffectManager)
         .def("GetEffectManager", &RTIConnection::GetEffectManager, return_internal_reference<>())
         .def("EnableGlobeMode", &RTIConnection::EnableGlobeMode, EGM_overloads())
         .def("EnableUTMMode", &RTIConnection::EnableUTMMode, EUM_overloads())
         .def("GlobeModeEnabled", &RTIConnection::GlobeModeEnabled)
         .def("SetGlobeRadius", &RTIConnection::SetGlobeRadius)
         .def("GetGlobeRadius", &RTIConnection::GetGlobeRadius)
         .def("SetGeoOrigin", &RTIConnection::SetGeoOrigin)
         .def("SetOriginLocation", &RTIConnection::SetOriginLocation)
         .def("GetOriginLocation", &RTIConnection::GetOriginLocation)
         .def("SetOriginRotation", &RTIConnection::SetOriginRotation)
         .def("GetOriginRotation", &RTIConnection::GetOriginRotation)
         .def("SetSiteIdentifier", &RTIConnection::SetSiteIdentifier)
         .def("GetSiteIdentifier", &RTIConnection::GetSiteIdentifier)
         .def("SetApplicationIdentifier", &RTIConnection::SetApplicationIdentifier)
         .def("GetApplicationIdentifier", &RTIConnection::GetApplicationIdentifier)
         .def("RegisterMasterEntity", &RTIConnection::RegisterMasterEntity)
         .def("DeregisterMasterEntity", &RTIConnection::DeregisterMasterEntity)
         .def("GetMasterEntityCount", &RTIConnection::GetMasterEntityCount)
         .def("GetMasterEntity", &RTIConnection::GetMasterEntity, return_internal_reference<>())
         .def("GetGhostEntityCount", &RTIConnection::GetGhostEntityCount)
         .def("GetGhostEntity", &RTIConnection::GetGhostEntity, return_internal_reference<>())
         .def("AddEntityTypeMapping", AddEntityTypeMapping1, AETM1_overloads())
         .def("AddEntityTypeMapping", AddEntityTypeMapping2, AETM2_overloads())
         .def("RemoveEntityTypeMapping", &RTIConnection::RemoveEntityTypeMapping)
         .def("LoadEntityTypeMappings", &RTIConnection::LoadEntityTypeMappings)
         .def("SetGroundClampMode", &RTIConnection::SetGroundClampMode)
         .def("GetGroundClampMode", &RTIConnection::GetGroundClampMode)
         .def("AddDetonationListener", &RTIConnection::AddDetonationListener)
         .def("RemoveDetonationListener", &RTIConnection::RemoveDetonationListener);
         
      enum_<RTIConnection::GroundClampMode>("GroundClampMode")
         .value("NO_CLAMP", RTIConnection::NO_CLAMP)
         .value("CLAMP_ELEVATION", RTIConnection::CLAMP_ELEVATION)
         .value("CLAMP_ELEVATION_AND_ROTATION", RTIConnection::CLAMP_ELEVATION_AND_ROTATION)
         .export_values();
   }
   
   class_<DetonationListener, DetonationListenerWrap, boost::noncopyable>("DetonationListener")
      .def("DetonationOccurred", &DetonationListener::DetonationOccurred, &DetonationListenerWrap::DefaultDetonationOccurred);
}