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

#ifndef RTI1516eAMBASSADOR_H_
#define RTI1516eAMBASSADOR_H_

#include <dtHLAGM/rtiambassador.h>

namespace dtHLAGM
{

   class RTI1516eAmbassadorImpl;

   /*
    * Wraps the actual RTI ambassador features to hide differences in the various API specs.
    */
   class RTI1516eAmbassador: public RTIAmbassador
   {
   public:
      RTI1516eAmbassador();

      virtual void Tick();

      virtual void ConnectToRTI(RTIFederateAmbassador& federateCallback, const std::string& rtiSpecificConnectData);
      virtual bool CreateFederationExecution(const std::string& executionName, const std::vector<std::string>& fedFiles);
      virtual void JoinFederationExecution(const std::string& federateName, const std::string& executionName);
      virtual void ResignFederationExecution(const std::string& executionName = "");

      virtual dtCore::RefPtr<RTIObjectClassHandle> GetObjectClassForInstance(RTIObjectInstanceHandle& instanceHandle);
      virtual std::string GetObjectClassName(RTIObjectClassHandle& clsHandle);
      virtual dtCore::RefPtr<RTIObjectClassHandle> GetObjectClassHandle(const std::string& className);
      virtual dtCore::RefPtr<RTIAttributeHandle> GetAttributeHandle(const std::string& attrName, RTIObjectClassHandle& handle);
      virtual std::string GetAttributeName(RTIAttributeHandle& attrHandle, RTIObjectClassHandle& clsHandle);
      virtual void SubscribeObjectClassAttributes(RTIObjectClassHandle& handle, const RTIAttributeHandleSet& ahs, RTIRegion* region = NULL);
      virtual void PublishObjectClass(RTIObjectClassHandle& handle, const RTIAttributeHandleSet& ahs);
      virtual void UnsubscribeObjectClass(RTIObjectClassHandle& handle, RTIRegion* region = NULL);

      virtual std::string GetInteractionClassName(RTIInteractionClassHandle& intClsHandle);
      virtual dtCore::RefPtr<RTIInteractionClassHandle> GetInteractionClassHandle(const std::string& className);
      virtual dtCore::RefPtr<RTIParameterHandle> GetParameterHandle(const std::string& paramName, RTIInteractionClassHandle& handle);

      virtual void SubscribeInteractionClass(RTIInteractionClassHandle& handle, RTIRegion* region = NULL);
      virtual void PublishInteractionClass(RTIInteractionClassHandle& handle);
      virtual void UnsubscribeInteractionClass(RTIInteractionClassHandle& handle, RTIRegion* region = NULL);

      virtual void ReserveObjectInstanceName(const std::string& nameToReserve);
      virtual dtCore::RefPtr<RTIObjectInstanceHandle> RegisterObjectInstance(RTIObjectClassHandle& clsHandle, const std::string& stringName);
      virtual void DeleteObjectInstance(RTIObjectInstanceHandle& instanceHandleToDelete);

      virtual void UpdateAttributeValues(RTIObjectInstanceHandle& instanceToUpdate, RTIAttributeHandleValueMap& attrs, const std::string& tag);

      virtual void SendInteraction(RTIInteractionClassHandle& interationClass, const RTIParameterHandleValueMap& params, const std::string& tag);

      virtual dtCore::RefPtr<RTIRegion> CreateRegion(RTIDimensionHandleSet& dimensions);
      virtual void DeleteRegion(RTIRegion& region);
      virtual void SetRegionDimensions(RTIRegion& region, const RTIDimensionVector& regionDimensions);
      virtual void CommitRegionChanges(RTIRegion& region);

      virtual unsigned int GetNumDimensions(RTIRegion& region);

      virtual std::string GetDimensionName(RTIDimensionHandle& dimHandle);
      virtual dtCore::RefPtr<RTIDimensionHandle> GetDimensionHandle(const std::string& name);


   protected:
      virtual ~RTI1516eAmbassador();
   private:
      RTI1516eAmbassadorImpl* mImpl;
   };
}
#endif /* RTI1516eAMBASSADOR_H_ */
