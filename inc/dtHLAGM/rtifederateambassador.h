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

#ifndef FEDERATE_AMBASSADOR_H_
#define FEDERATE_AMBASSADOR_H_

#include <dtHLAGM/export.h>
#include <osg/Referenced>

#include <dtHLAGM/rtihandle.h>
#include <dtHLAGM/rticontainers.h>

namespace dtHLAGM
{

   class DT_HLAGM_EXPORT RTIFederateAmbassador
   {
   public:
      RTIFederateAmbassador() {}
      virtual ~RTIFederateAmbassador() {}

      virtual void DiscoverObjectInstance(RTIObjectInstanceHandle& theObject,
                                          RTIObjectClassHandle& theObjectClassHandle,
                                          const std::string& objectName) = 0;
      /**
       * Invoked by the RTI ambassador to request that the federate provide
       * updated attribute values for the specified object.
       *
       * @param theObject the handle of the object of interest
       * @param theAttributes the set of attributes to update
       */
      virtual void ProvideAttributeValueUpdate(RTIObjectInstanceHandle& theObject,
                                               const RTIAttributeHandleSet& theAttributes) = 0;

      /**
       * Invoked by the RTI ambassador to notify the federate of updated object
       * attribute values.
       *
       * @note this is commented out because I didn't want to deal with wrapping the Fed time, which is not
       *       used anywhere in code.
       *
       * @param theObject the handle of the modified object
       * @param theAttributes the new attribute values
       * @param theTime the event timestamp
       * @param theTag the user-supplied tag associated with the event
       * @param theHandle the event retraction handle
       */
      /*virtual void ReflectAttributeValues(RTIObjectInstanceHandle theObject,
                                          const RTIAttributeHandleValueMap& theAttributes,
                                          const RTI::FedTime& theTime,
                                          const std::string& theTag,
                                          RTI::EventRetractionHandle theHandle) = 0;*/

      /**
       * Invoked by the RTI ambassador to notify the federate of updated object
       * attribute values.
       *
       * @param theObject the handle of the modified object
       * @param theAttributes the new attribute values
       * @param theTag the user-supplied tag associated with the event
       */
      virtual void ReflectAttributeValues(RTIObjectInstanceHandle& theObject,
                                          const RTIAttributeHandleValueMap& theAttributes,
                                          const std::string& theTag) = 0;


      /**
       * Invoked by the RTI ambassador to notify the federate of a deleted object
       * instance.
       *
       * @note commented out so I don't have to wrap fed time.
       *
       * @param theObject the handle of the removed object
       * @param theTime the event timestamp
       * @param theTag the user-supplied tag associated with the event
       * @param theHandle the event retraction handle
       */
      /*virtual void removeObjectInstance(RTIObjectInstanceHandle theObject,
                                        const RTI::FedTime& theTime,
                                        const std::string& theTag,
                                        RTI::EventRetractionHandle theHandle) = 0;*/

      /**
       * Invoked by the RTI ambassador to notify the federate of a removed object
       * instance.
       *
       * @param theObject the handle of the removed object
       * @param theTag the user-supplied tag associated with the event
       */
      virtual void RemoveObjectInstance(RTIObjectInstanceHandle& theObject, const std::string& theTag) = 0;

      /**
       * Invoked by the RTI ambassador to notify the federate of a received
       * interaction.
       *
       * @param theInteraction the handle of the received interaction
       * @param theParameters the parameters of the interaction
       * @param theTag the user-supplied tag associated with the event
       */
      virtual void ReceiveInteraction(RTIInteractionClassHandle&,
                                      const RTIParameterHandleValueMap& theParameters,
                                      const std::string& theTag) = 0;

      /// Called if the object name registration succeeds, meaning the object may be registered with the rti using the given name.
      virtual void ObjectInstanceNameReservationSucceeded (const std::string& theObjectInstanceName) = 0;

      /// Called if the object name registration succeeds, meaning the object may NOT be registered with the rti using the given name.
      virtual void ObjectInstanceNameReservationFailed (const std::string& theObjectInstanceName) = 0;


   private:
      // -----------------------------------------------------------------------
      //  Unimplemented constructors and operators
      // -----------------------------------------------------------------------
      RTIFederateAmbassador(const RTIFederateAmbassador&) {}
      RTIFederateAmbassador& operator=(const RTIFederateAmbassador&) { return *this; }
   };
}

#endif /* FEDERATE_AMBASSADOR_H_ */
