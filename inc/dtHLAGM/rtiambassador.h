/* -*-c++-*-
 * Delta3D
 * Copyright 2012, MASA Group Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * David Guthrie
 */

#ifndef RTIAMBASSADOR_H_
#define RTIAMBASSADOR_H_

#include <dtHLAGM/export.h>
#include <osg/Referenced>
#include <vector>

#include <dtHLAGM/rticontainers.h>
#include <dtHLAGM/rtiregion.h>
#include <dtHLAGM/rtifederateambassador.h>

#include <dtUtil/functor.h>

namespace dtHLAGM
{

   /*
    * Wraps the actual RTI ambassador features to hide differences in the various API specs.
    */
   class DT_HLAGM_EXPORT RTIAmbassador: public osg::Referenced
   {
   public:

      /// Use this constant with Create(...) to create the RTI13 Implementation
      static const std::string RTI13_IMPLEMENTATION;
      /// Use this constant with Create(...) to create the RTI1516e Implementation
      static const std::string RTI1516e_IMPLEMENTATION;

      /**
       * Creates an ambassador based on the implementation name supplied
       * @param implName The implementation library name to load.  In the form {prefix}dtHLAGM_{implName}.{os-library-extension}.
       *                 so rti13 would load libdtHLAGM_rti13.so on linux or dtHLAGM_rti13.dll on windows.
       *                 rti1516e is also provided with delta3d.
       * @return A new instance of the RTI Ambassador
       * @throw dtUtil::LibrarySharingManager::LibraryLoadingException if it could not load the needed library.
       */
      static dtCore::RefPtr<RTIAmbassador> Create(const std::string& implName = "rti13");

      typedef dtUtil::Functor<dtCore::RefPtr<RTIAmbassador>, TYPELIST_0()> CreateFuncType;
      /**
       * This allows registering a new RTI implementation.  The libraries loaded by Create are expected to call this on load.
       * @param implName the implementation name to register.
       */
      static void RegisterImplementation(const std::string& implName, CreateFuncType createFunctor);
      static void UnregisterImplementation(const std::string& implName);

      virtual void Tick() = 0;

      /// This needs to be called first
      virtual void ConnectToRTI(RTIFederateAmbassador& federateCallback, const std::string& rtiSpecificConnectData) = 0;

      /**
       * @param fedFiles older federations can only accept one fed file, but HLA 1516e federations almost always have many files.
       * @return true if the federation was created.  false if it already existed.
       * @throw RTIException if anything fails other than the federation already existing.
       */
      virtual bool CreateFederationExecution(const std::string& executionName, const std::vector<std::string>& fedFiles) = 0;

      virtual void JoinFederationExecution(const std::string& federateName, const std::string& executionName) = 0;

      /**
       * Resigns from the federation, but if you pass in an execution name, it will also try to destroy the federation.
       * @param executionName federation execution name.  Pass a non-empty name here if you want to destroy the federation execution after resigning.
       */
      virtual void ResignFederationExecution(const std::string& executionName = "") = 0;

      std::string GetObjectClassNameForInstance(RTIObjectInstanceHandle& instanceHandle)
      {
         dtCore::RefPtr<RTIObjectClassHandle> clsHandle = GetObjectClassForInstance(instanceHandle);
         return GetObjectClassName(*clsHandle);
      }

      virtual dtCore::RefPtr<RTIObjectClassHandle> GetObjectClassForInstance(RTIObjectInstanceHandle& instanceHandle) = 0;
      virtual std::string GetObjectClassName(RTIObjectClassHandle& clsHandle) = 0;
      virtual dtCore::RefPtr<RTIObjectClassHandle> GetObjectClassHandle(const std::string& className) = 0;
      virtual dtCore::RefPtr<RTIAttributeHandle> GetAttributeHandle(const std::string& attrName, RTIObjectClassHandle& handle) = 0;
      virtual std::string GetAttributeName(RTIAttributeHandle& attrHandle, RTIObjectClassHandle& clsHandle) = 0;

      virtual void SubscribeObjectClassAttributes(RTIObjectClassHandle& handle, const RTIAttributeHandleSet& ahs, RTIRegion* region = NULL) = 0;
      virtual void PublishObjectClass(RTIObjectClassHandle& handle, const RTIAttributeHandleSet& ahs) = 0;
      virtual void UnsubscribeObjectClass(RTIObjectClassHandle& handle, RTIRegion* region = NULL) = 0;

      virtual std::string GetInteractionClassName(RTIInteractionClassHandle& intClsHandle) = 0;
      virtual dtCore::RefPtr<RTIInteractionClassHandle> GetInteractionClassHandle(const std::string& className) = 0;
      virtual dtCore::RefPtr<RTIParameterHandle> GetParameterHandle(const std::string& paramName, RTIInteractionClassHandle& handle) = 0;

      virtual void SubscribeInteractionClass(RTIInteractionClassHandle& handle, RTIRegion* region = NULL) = 0;
      virtual void PublishInteractionClass(RTIInteractionClassHandle& handle) = 0;
      virtual void UnsubscribeInteractionClass(RTIInteractionClassHandle& handle, RTIRegion* region = NULL) = 0;

      virtual void ReserveObjectInstanceName(const std::string& nameToReserve) = 0;
      virtual dtCore::RefPtr<RTIObjectInstanceHandle> RegisterObjectInstance(RTIObjectClassHandle& clsHandle, const std::string& stringName) = 0;
      virtual void DeleteObjectInstance(RTIObjectInstanceHandle& instanceHandleToDelete) = 0;

      virtual void UpdateAttributeValues(RTIObjectInstanceHandle& instanceToUpdate, RTIAttributeHandleValueMap& attrs, const std::string& tag) = 0;
      virtual void SendInteraction(RTIInteractionClassHandle& interationClass, const RTIParameterHandleValueMap& params, const std::string& tag) = 0;

      virtual dtCore::RefPtr<RTIRegion> CreateRegion(RTIDimensionHandleSet& dimensions) = 0;
      virtual void DeleteRegion(RTIRegion& region) = 0;
      virtual void SetRegionDimensions(RTIRegion& region, const RTIDimensionVector& regionDimensions) = 0;
      virtual void CommitRegionChanges(RTIRegion& region) = 0;

      virtual unsigned int GetNumDimensions(RTIRegion& region) = 0;

      virtual std::string GetDimensionName(RTIDimensionHandle& dimHandle) = 0;
      virtual dtCore::RefPtr<RTIDimensionHandle> GetDimensionHandle(const std::string& name) = 0;

   protected:
      RTIAmbassador();
      virtual ~RTIAmbassador();
   };
}
#endif /* RTIAMBASSADOR_H_ */
