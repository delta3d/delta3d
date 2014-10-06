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
 * FOR A PARTI13CULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * David Guthrie
 */

#include "rti13ambassador.h"
#include "rti13handle.h"
#include "rti13util.h"
#include "rti13region.h"
#include <dtHLAGM/rtiexception.h>
#include <dtHLAGM/rtifederateambassador.h>
#include <dtHLAGM/ddmutil.h>

#ifndef RTI_USES_STD_FSTREAM
#define RTI_USES_STD_FSTREAM
#endif
#include <RTI.hh>
#include <NullFederateAmbassador.hh>
#include <dtUtil/mswinmacros.h>

#include <sstream>

namespace dtHLAGM
{
   ///////////////////////////////////////
   class RegisterRTI13
   {
   public:
      RegisterRTI13()
      {
         RTIAmbassador::RegisterImplementation(RTIAmbassador::RTI13_IMPLEMENTATION,
                  RTIAmbassador::CreateFuncType(this, &RegisterRTI13::Create));
      }
      ~RegisterRTI13()
      {
         RTIAmbassador::UnregisterImplementation(RTIAmbassador::RTI13_IMPLEMENTATION);
      }

      dtCore::RefPtr<RTIAmbassador> Create()
      {
         DDMUtil::SetMaxExtent(MAX_EXTENT);
         DDMUtil::SetMinExtent(MIN_EXTENT);
         return new RTI13Ambassador();
      }
   };

   static RegisterRTI13 regRTI;
   ///////////////////////////////////////

   static const char* const HYPERSPACE="HyperSpace";

   class RTI13AmbassadorImpl : public NullFederateAmbassador
   {
   public:
      RTI13AmbassadorImpl()
      : mAmbassador(new RTI::RTIambassador())
      {

      }

      // Need the "throw()" because the base class has one.
      ~RTI13AmbassadorImpl() throw()
      {
         delete mAmbassador;
         mAmbassador = NULL;
      }

      virtual void discoverObjectInstance (
              RTI::ObjectHandle          theObject,      // supplied C1
              RTI::ObjectClassHandle     theObjectClass, // supplied C1
        const char*                      theObjectName)  // supplied C4
      throw (
               RTI::CouldNotDiscover,
               RTI::ObjectClassNotKnown,
               RTI::FederateInternalError)
      {
         dtCore::RefPtr<RTIObjectClassHandle> objClsHandle = mObjectClassHandleCache[theObjectClass];

         dtCore::RefPtr<RTIObjectInstanceHandle> objIntHandle =
                  WrapHandle<RTI::ObjectHandle, RTIObjectInstanceHandle, RTI13ObjectInstanceHandle>(theObject, mObjectInstanceHandleCache);

         mFedAmbassador->DiscoverObjectInstance(*objIntHandle, *objClsHandle, theObjectName);
      }

      virtual void reflectAttributeValues (
              RTI::ObjectHandle                 theObject,     // supplied C1
        const RTI::AttributeHandleValuePairSet& theAttributes, // supplied C4
        const RTI::FedTime&                     theTime,       // supplied C1
        const char                             *theTag,        // supplied C4
        RTI::EventRetractionHandle        theHandle)     // supplied C1
      throw (
               RTI::ObjectNotKnown,
               RTI::AttributeNotKnown,
               RTI::FederateOwnsAttributes,
               RTI::InvalidFederationTime,
               RTI::FederateInternalError)
      {
          reflectAttributeValues(theObject, theAttributes, theTag);
      }

      virtual void reflectAttributeValues (
               RTI::ObjectHandle                 theObject,     // supplied C1
        const RTI::AttributeHandleValuePairSet& theAttributes, // supplied C4
        const char                             *theTag)        // supplied C4
      throw (
               RTI::ObjectNotKnown,
               RTI::AttributeNotKnown,
               RTI::FederateOwnsAttributes,
               RTI::FederateInternalError)
      {
         dtCore::RefPtr<RTIObjectInstanceHandle> objIntHandle =
                  WrapHandle<RTI::ObjectHandle, RTIObjectInstanceHandle, RTI13ObjectInstanceHandle>(theObject, mObjectInstanceHandleCache);

         RTIAttributeHandleValueMap attrMap;
         ConvertRTI13AHVSToAHVM(mAmbassador->getObjectClass(theObject), theAttributes, attrMap);

         mFedAmbassador->ReflectAttributeValues(*objIntHandle, attrMap, theTag);
      }

      // 4.6
      virtual void receiveInteraction (
               RTI::InteractionClassHandle       theInteraction, // supplied C1
        const RTI::ParameterHandleValuePairSet& theParameters,  // supplied C4
        const RTI::FedTime&                     theTime,        // supplied C4
        const char*                             theTag,         // supplied C4
        RTI::EventRetractionHandle        theHandle)      // supplied C1
      throw (
               RTI::InteractionClassNotKnown,
               RTI::InteractionParameterNotKnown,
               RTI::InvalidFederationTime,
               RTI::FederateInternalError)
      {
         receiveInteraction(theInteraction, theParameters, theTag);
      }

      virtual void receiveInteraction (
               RTI::InteractionClassHandle       theInteraction, // supplied C1
        const RTI::ParameterHandleValuePairSet& theParameters,  // supplied C4
        const char*                             theTag)         // supplied C4
      throw (
               RTI::InteractionClassNotKnown,
               RTI::InteractionParameterNotKnown,
               RTI::FederateInternalError)
      {
         dtCore::RefPtr<RTIInteractionClassHandle> intClsHandle =
                  WrapHandle<RTI::InteractionClassHandle, RTIInteractionClassHandle, RTI13InteractionClassHandle>(theInteraction, mInteractionClassHandleCache);

         RTIParameterHandleValueMap paramMap;
         ConvertRTI13PHVSToPHVM(theInteraction, theParameters, paramMap);

         mFedAmbassador->ReceiveInteraction(*intClsHandle, paramMap, theTag);
      }

      virtual void removeObjectInstance (
               RTI::ObjectHandle          theObject, // supplied C1
        const RTI::FedTime&              theTime,   // supplied C4
        const char                      *theTag,    // supplied C4
        RTI::EventRetractionHandle theHandle) // supplied C1
      throw (
               RTI::ObjectNotKnown,
               RTI::InvalidFederationTime,
               RTI::FederateInternalError)
      {
         removeObjectInstance(theObject, theTag);
      }

      virtual void removeObjectInstance (
               RTI::ObjectHandle          theObject, // supplied C1
        const char                      *theTag)    // supplied C4
      throw (
               RTI::ObjectNotKnown,
               RTI::FederateInternalError)
      {
         dtCore::RefPtr<RTIObjectInstanceHandle> objIntHandle =
                  WrapHandle<RTI::ObjectHandle, RTIObjectInstanceHandle, RTI13ObjectInstanceHandle>(theObject, mObjectInstanceHandleCache);

         mFedAmbassador->RemoveObjectInstance(*objIntHandle, theTag);
      }

      virtual void provideAttributeValueUpdate (
               RTI::ObjectHandle        theObject,     // supplied C1
        const RTI::AttributeHandleSet& theAttributes) // supplied C4
      throw (
               RTI::ObjectNotKnown,
               RTI::AttributeNotKnown,
               RTI::AttributeNotOwned,
               RTI::FederateInternalError)
      {
         dtCore::RefPtr<RTIObjectInstanceHandle> objIntHandle =
                  WrapHandle<RTI::ObjectHandle, RTIObjectInstanceHandle, RTI13ObjectInstanceHandle>(theObject, mObjectInstanceHandleCache);

         RTIAttributeHandleSet attrSet;
         ConvertRTI13AHSToAHS(mAmbassador->getObjectClass(theObject), theAttributes, attrSet);
         mFedAmbassador->ProvideAttributeValueUpdate(*objIntHandle, attrSet);
      }

      dtCore::RefPtr<RTIAttributeHandle> WrapAttrHandle(const RTI::ObjectClassHandle& objClass, const RTI::AttributeHandle& handle)
      {
         AttrHandleCacheKey cachekey(handle, objClass);
         dtCore::RefPtr<RTIAttributeHandle> wrappedHandle = mAttrHandleCache[cachekey];
         if (!wrappedHandle.valid())
         {
            wrappedHandle = new RTI13AttributeHandle(handle);
            mAttrHandleCache[cachekey] = wrappedHandle;
         }
         return wrappedHandle;
      }

      dtCore::RefPtr<RTIAttributeHandle> WrapParamHandle(const RTI::InteractionClassHandle& intClass, const RTI::ParameterHandle& handle)
      {
         ParamHandleCacheKey cachekey(handle, intClass);
         dtCore::RefPtr<RTIParameterHandle> wrappedHandle = mParamHandleCache[cachekey];
         if (!wrappedHandle.valid())
         {
            wrappedHandle = new RTI13ParameterHandle(handle);
            mParamHandleCache[cachekey] = wrappedHandle;
         }
         return wrappedHandle;
      }

      void ConvertRTI13AHSToAHS(RTI::ObjectClassHandle objClass, const RTI::AttributeHandleSet& rtiHandleSet, RTIAttributeHandleSet& dtHandleSetToFill)
      {
         for (unsigned i = 0; i < rtiHandleSet.size(); ++i)
         {
            RTI::AttributeHandle attrHandle = rtiHandleSet.getHandle(i);
            dtCore::RefPtr<RTIAttributeHandle> wrappedHandle = WrapAttrHandle(objClass, attrHandle);
            dtHandleSetToFill.insert(wrappedHandle);
         }
      }

      void ConvertRTI13AHVSToAHVM(RTI::ObjectClassHandle objClass, const RTI::AttributeHandleValuePairSet& rtiPairSet, RTIAttributeHandleValueMap& dtHandleValueMapToFill)
      {
         for (unsigned i = 0; i < rtiPairSet.size(); ++i)
         {
            RTIContainerValueData data;
            // just need this variable to make the getValuePointer call happy.
            RTI::ULong lengthHolder = 0;
            data.mData.append(rtiPairSet.getValuePointer(i, lengthHolder), rtiPairSet.getValueLength(i));
#ifdef CERTI_WORKAROUND
            RTI::Region* reg = NULL;
#else
            RTI::Region* reg = rtiPairSet.getRegion(i);
#endif
            if (reg != NULL)
            {
               data.mRegion = new RTI13Region(*reg);
            }
            RTI::AttributeHandle attrHandle = rtiPairSet.getHandle(i);
            dtCore::RefPtr<RTIAttributeHandle> wrappedHandle = WrapAttrHandle(objClass, attrHandle);
            dtHandleValueMapToFill.insert(std::make_pair(wrappedHandle, data));
         }
      }

      void ConvertRTI13PHVSToPHVM(RTI::InteractionClassHandle intClass, const RTI::ParameterHandleValuePairSet& rtiPairSet, RTIParameterHandleValueMap& dtHandleValueMapToFill)
      {
#ifdef CERTI_WORKAROUND
         RTI::Region* reg13 = NULL;
#else
         RTI::Region* reg13 = rtiPairSet.getRegion();
#endif
         dtCore::RefPtr<RTIRegion> region = new RTI13Region(*reg13);

         for (unsigned i = 0; i < rtiPairSet.size(); ++i)
         {
            //rtiPairSet.
            RTIContainerValueData data;
            // just need this variable to make the getValuePointer call happy.
            RTI::ULong lengthHolder = 0;
            data.mData.append(rtiPairSet.getValuePointer(i, lengthHolder), rtiPairSet.getValueLength(i));
            if (region.valid())
            {
               data.mRegion = region;
            }
            RTI::ParameterHandle paramHandle = rtiPairSet.getHandle(i);
            dtCore::RefPtr<RTIParameterHandle> wrappedHandle = WrapParamHandle(intClass, paramHandle);
            dtHandleValueMapToFill.insert(std::make_pair(wrappedHandle, data));
         }
      }


      RTI::RTIambassador* mAmbassador;
      RTIFederateAmbassador* mFedAmbassador;

      std::map<RTI::ObjectHandle, dtCore::RefPtr<RTIObjectInstanceHandle> > mObjectInstanceHandleCache;

      std::map<RTI::ObjectClassHandle, dtCore::RefPtr<RTIObjectClassHandle> > mObjectClassHandleCache;

      typedef std::pair<RTI::AttributeHandle, RTI::ObjectClassHandle > AttrHandleCacheKey;
      std::map<AttrHandleCacheKey, dtCore::RefPtr<RTIAttributeHandle> > mAttrHandleCache;

      std::map<RTI::InteractionClassHandle, dtCore::RefPtr<RTIObjectClassHandle> > mInteractionClassHandleCache;

      typedef std::pair<RTI::ParameterHandle, RTI::InteractionClassHandle > ParamHandleCacheKey;
      std::map<ParamHandleCacheKey, dtCore::RefPtr<RTIParameterHandle> > mParamHandleCache;

      std::map<RTI::DimensionHandle, dtCore::RefPtr<RTIDimensionHandle> > mDimensionHandleCache;
   };



RTI13Ambassador::RTI13Ambassador()
: mImpl(new RTI13AmbassadorImpl)
{
}

RTI13Ambassador::~RTI13Ambassador()
{
   delete mImpl;
}

void RTI13Ambassador::Tick()
{
   try
   {
      mImpl->mAmbassador->tick();
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
}

dtCore::RefPtr<RTIObjectClassHandle> RTI13Ambassador::GetObjectClassForInstance(RTIObjectInstanceHandle& instanceHandle)
{
   dtCore::RefPtr<RTIObjectClassHandle> result;
   try
   {
      RTI::ObjectClassHandle tmpHandle = mImpl->mAmbassador->getObjectClass(static_cast<RTI13ObjectInstanceHandle&>(instanceHandle).GetRTI13Handle());

      result = WrapHandle<RTI::ObjectClassHandle, RTIObjectClassHandle, RTI13ObjectClassHandle>(tmpHandle, mImpl->mObjectClassHandleCache);
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
   return result;
}

std::string RTI13Ambassador::GetObjectClassName(RTIObjectClassHandle& clsHandle)
{
   std::string result;
   try
   {
      result = mImpl->mAmbassador->getObjectClassName(static_cast<RTI13ObjectClassHandle&>(clsHandle).GetRTI13Handle());

      //Some RTIs return this prefix, so this chops it off to make it more consistent.
      static const std::string OBJECT_ROOT("ObjectRoot.");

      if (result.length() > OBJECT_ROOT.length() &&
               result.substr(0, OBJECT_ROOT.length()) == OBJECT_ROOT)
      {
         result = result.substr(OBJECT_ROOT.size());
      }
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
   return result;
}

dtCore::RefPtr<RTIObjectClassHandle> RTI13Ambassador::GetObjectClassHandle(const std::string& className)
{
   dtCore::RefPtr<RTIObjectClassHandle> result;

   try
   {
      // Store and cache all the results so we can always use the same object for the handle.
      RTI::ObjectClassHandle clsH = mImpl->mAmbassador->getObjectClassHandle(className.c_str());
      result = WrapHandle<RTI::ObjectClassHandle, RTIObjectClassHandle, RTI13ObjectClassHandle>(clsH, mImpl->mObjectClassHandleCache);
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

   return result;
}

dtCore::RefPtr<RTIAttributeHandle> RTI13Ambassador::GetAttributeHandle(const std::string& attrName, RTIObjectClassHandle& handle)
{
   dtCore::RefPtr<RTIAttributeHandle> result;

   try
   {
      // Store and cache all the results so we can always use the same object for the handle.
      RTI::ObjectClassHandle clsH = static_cast<RTI13ObjectClassHandle&>(handle).GetRTI13Handle();
      RTI::AttributeHandle attrH = mImpl->mAmbassador->getAttributeHandle(attrName.c_str(), clsH);

      result = mImpl->WrapAttrHandle(clsH, attrH);
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

   return result;

}

std::string RTI13Ambassador::GetAttributeName(RTIAttributeHandle& attrHandle, RTIObjectClassHandle& clsHandle)
{
   std::string result;
   try
   {
      result = mImpl->mAmbassador->getAttributeName(
               static_cast<RTI13AttributeHandle&>(attrHandle).GetRTI13Handle(),
               static_cast<RTI13ObjectClassHandle&>(clsHandle).GetRTI13Handle());
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
   return result;
}

void RTI13Ambassador::SubscribeObjectClassAttributes(RTIObjectClassHandle& handle, const RTIAttributeHandleSet& ahs, RTIRegion* region)
{
   RTI::AttributeHandleSet* ahsRTI13 = ConvertAHStoRTI13AHS(ahs);
   RTI::ObjectClassHandle handleRTI13 = static_cast<RTI13ObjectClassHandle*>(&handle)->GetRTI13Handle();

   try
   {
      if (region == NULL)
      {
         mImpl->mAmbassador->subscribeObjectClassAttributes(handleRTI13, *ahsRTI13);
      }
      else
      {
         RTI::Region& region13 = static_cast<RTI13Region*>(region)->GetRTI13Region();
         mImpl->mAmbassador->subscribeObjectClassAttributesWithRegion(handleRTI13, region13, *ahsRTI13);
      }
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

   delete ahsRTI13;

}

void RTI13Ambassador::PublishObjectClass(RTIObjectClassHandle& handle, const RTIAttributeHandleSet& ahs)
{
   RTI::AttributeHandleSet* ahsRTI13 = ConvertAHStoRTI13AHS(ahs);
   RTI::ObjectClassHandle handleRTI13 = static_cast<RTI13ObjectClassHandle*>(&handle)->GetRTI13Handle();
   try
   {
         mImpl->mAmbassador->publishObjectClass(handleRTI13, *ahsRTI13);
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

   delete ahsRTI13;

}

void RTI13Ambassador::UnsubscribeObjectClass(RTIObjectClassHandle& handle, RTIRegion* region)
{
   RTI::ObjectClassHandle handleRTI13 = static_cast<RTI13ObjectClassHandle*>(&handle)->GetRTI13Handle();

   try
   {
      if (region == NULL)
      {
         mImpl->mAmbassador->unsubscribeObjectClass(handleRTI13);
      }
      else
      {
         RTI::Region& region13 = static_cast<RTI13Region*>(region)->GetRTI13Region();
         mImpl->mAmbassador->unsubscribeObjectClassWithRegion(handleRTI13, region13);
      }
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

}

std::string RTI13Ambassador::GetInteractionClassName(RTIInteractionClassHandle& intClsHandle)
{
   std::string result;
   try
   {
      result = mImpl->mAmbassador->getInteractionClassName(static_cast<RTI13InteractionClassHandle&>(intClsHandle).GetRTI13Handle());

      // strip off the InteractionRoot prefix because some rtis return that as part of the string.
      static const std::string INTERACTION_ROOT("InteractionRoot.");
      if (result.size() > INTERACTION_ROOT.size() &&
               result.substr(0, INTERACTION_ROOT.size()) == INTERACTION_ROOT)
      {
         result = result.substr(INTERACTION_ROOT.size());
      }
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
   return result;

}

dtCore::RefPtr<RTIInteractionClassHandle> RTI13Ambassador::GetInteractionClassHandle(const std::string& className)
{
   dtCore::RefPtr<RTIInteractionClassHandle> result;

   try
   {
      RTI::InteractionClassHandle intrH = mImpl->mAmbassador->getInteractionClassHandle(className.c_str());

      result = WrapHandle<RTI::InteractionClassHandle, RTIInteractionClassHandle, RTI13InteractionClassHandle>(intrH, mImpl->mInteractionClassHandleCache);
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

   return result;

}

dtCore::RefPtr<RTIParameterHandle> RTI13Ambassador::GetParameterHandle(const std::string& paramName, RTIInteractionClassHandle& handle)
{
   dtCore::RefPtr<RTIParameterHandle> result;


   if (result == NULL)
   {
      try
      {
         // We want to always return the same handle instance, so we cache them.
         RTI::InteractionClassHandle intrH = static_cast<RTI13InteractionClassHandle&>(handle).GetRTI13Handle();
         RTI::ParameterHandle paramH = mImpl->mAmbassador->getParameterHandle(paramName.c_str(), intrH);

         result = mImpl->WrapParamHandle(intrH, paramH);
      }
      catch (const RTI::Exception& ex)
      {
         RethrowRTIException(ex);
      }
   }

   return result;

}

void RTI13Ambassador::SubscribeInteractionClass(RTIInteractionClassHandle& handle, RTIRegion* region)
{
   RTI::InteractionClassHandle handleRTI13 = static_cast<RTI13InteractionClassHandle*>(&handle)->GetRTI13Handle();

   try
   {
      if (region == NULL)
      {
         mImpl->mAmbassador->subscribeInteractionClass(handleRTI13);
      }
      else
      {
         RTI::Region& region13 = static_cast<RTI13Region*>(region)->GetRTI13Region();
         mImpl->mAmbassador->subscribeInteractionClassWithRegion(handleRTI13, region13);
      }
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

}

void RTI13Ambassador::PublishInteractionClass(RTIInteractionClassHandle& handle)
{
   RTI::InteractionClassHandle handleRTI13 = static_cast<RTI13InteractionClassHandle*>(&handle)->GetRTI13Handle();

   try
   {
      mImpl->mAmbassador->publishInteractionClass(handleRTI13);
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

}

void RTI13Ambassador::UnsubscribeInteractionClass(RTIInteractionClassHandle& handle, RTIRegion* region)
{
   RTI::InteractionClassHandle handleRTI13 = static_cast<RTI13InteractionClassHandle*>(&handle)->GetRTI13Handle();

   try
   {
      if (region == NULL)
      {
         mImpl->mAmbassador->unsubscribeInteractionClass(handleRTI13);
      }
      else
      {
         RTI::Region& region13 = static_cast<RTI13Region*>(region)->GetRTI13Region();
         mImpl->mAmbassador->unsubscribeInteractionClassWithRegion(handleRTI13, region13);
      }
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
}

void RTI13Ambassador::ConnectToRTI(RTIFederateAmbassador& federateCallback, const std::string& /*rtiSpecificConnectData*/)
{
   mImpl->mFedAmbassador = &federateCallback;
}

bool RTI13Ambassador::CreateFederationExecution(const std::string& executionName, const std::vector<std::string>& fedFiles)
{
   if (fedFiles.empty())
   {
      throw RTIException("The fedfiles parameter must have at least one entry.  It is empty.", __FILE__, __LINE__);
   }
   try
   {
      mImpl->mAmbassador->createFederationExecution(executionName.c_str(), fedFiles.front().c_str());
      //some other rti's want the data in more of this format.
      //mRTIAmbassador->createFederationExecution(osgDB::getStrippedName(fedFile).c_str(), ".fed");
   }
   catch(const RTI::FederationExecutionAlreadyExists&)
   {
      return false;
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
   return true;
}

void RTI13Ambassador::JoinFederationExecution(const std::string& federateName, const std::string& executionName)
{
   try
   {
      mImpl->mAmbassador->joinFederationExecution(federateName.c_str(), executionName.c_str(), mImpl);
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
}

void RTI13Ambassador::ResignFederationExecution(const std::string& executionName)
{
   try
   {
      mImpl->mAmbassador->resignFederationExecution(RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
   }
   catch(RTI::RTIinternalError& ex)
   {
      RethrowRTIException(ex);
   }

   if (!executionName.empty())
   {
      try
      {
         mImpl->mAmbassador->destroyFederationExecution(executionName.c_str());
      }
      catch(RTI::FederatesCurrentlyJoined& ex)
      {
         std::ostringstream ss;
         //workaround for a strange namespace issue
         ::operator<<(ss, ex);
         LOG_INFO("Could not destroy the federation execution because other federates are still connected: " + ss.str());
      }
      catch(RTI::FederationExecutionDoesNotExist& ex)
      {
         std::ostringstream ss;
         //workaround for a strange namespace issue
         ::operator<<(ss, ex);
         LOG_DEBUG("Could not destroy the federation execution because it does not exist: " + ss.str());
      }
      catch(RTI::Exception& ex)
      {
         RethrowRTIException(ex);
      }
   }
}

void RTI13Ambassador::DeleteObjectInstance(RTIObjectInstanceHandle& instanceHandleToDelete)
{
   try
   {
      mImpl->mAmbassador->deleteObjectInstance(static_cast<RTI13ObjectInstanceHandle*>(&instanceHandleToDelete)->GetRTI13Handle(), "");
   }
   // This should not happen unless RTI is broken some how.
   catch( RTI::Exception& ex )
   {
      RethrowRTIException(ex);
   }

}

void RTI13Ambassador::ReserveObjectInstanceName(const std::string& nameToReserve)
{
   // This is not required in 13, so we just call succeeded.
   mImpl->mFedAmbassador->ObjectInstanceNameReservationSucceeded(nameToReserve);
}

dtCore::RefPtr<RTIObjectInstanceHandle> RTI13Ambassador::RegisterObjectInstance(RTIObjectClassHandle& clsHandle, const std::string& stringName)
{
   dtCore::RefPtr<RTIObjectInstanceHandle> result;
   try
   {
      //Pass ClassHandle to registerObjectInstance
      RTI::ObjectHandle objectHandle = mImpl->mAmbassador->registerObjectInstance(static_cast<RTI13ObjectClassHandle*>(&clsHandle)->GetRTI13Handle(), stringName.c_str());
      result = WrapHandle<RTI::ObjectHandle, RTIObjectInstanceHandle, RTI13ObjectInstanceHandle>(objectHandle, mImpl->mObjectInstanceHandleCache);
   }
   catch (RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

   return result;
}

void RTI13Ambassador::UpdateAttributeValues(RTIObjectInstanceHandle& instanceToUpdate, RTIAttributeHandleValueMap& attrs, const std::string& tag)
{
   RTI::AttributeHandleValuePairSet* rtiAttrs = ConvertAHVMToRTI13AHVS(attrs);
   try
   {
      mImpl->mAmbassador->updateAttributeValues(
               static_cast<RTI13ObjectInstanceHandle*>(&instanceToUpdate)->GetRTI13Handle(),
               *rtiAttrs,
               tag.c_str());
   }
   catch (const RTI::Exception& ex)
   {
      delete rtiAttrs;
      rtiAttrs = NULL;
      RethrowRTIException(ex);
   }
   delete rtiAttrs;
   rtiAttrs = NULL;

}

void RTI13Ambassador::SendInteraction(RTIInteractionClassHandle& interationClass, const RTIParameterHandleValueMap& params, const std::string& tag)
{
   RTI::ParameterHandleValuePairSet* rtiParams = ConvertPHVMToRTI13PHVS(params);
   try
   {
      mImpl->mAmbassador->sendInteraction(
               static_cast<RTI13InteractionClassHandle*>(&interationClass)->GetRTI13Handle(),
               *rtiParams,
               tag.c_str());
   }
   catch (const RTI::Exception& ex)
   {
      delete rtiParams;
      rtiParams = NULL;
      RethrowRTIException(ex);
   }
   delete rtiParams;
   rtiParams = NULL;

}

std::string RTI13Ambassador::GetDimensionName(RTIDimensionHandle& dimHandle)
{
   std::string result;
   try
   {
      RTI::SpaceHandle spaceHandle =  mImpl->mAmbassador->getRoutingSpaceHandle(HYPERSPACE);
      result = mImpl->mAmbassador->getDimensionName(static_cast<RTI13DimensionHandle&>(dimHandle).GetRTI13Handle(), spaceHandle);
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
   return result;
}

dtCore::RefPtr<RTIDimensionHandle> RTI13Ambassador::GetDimensionHandle(const std::string& name)
{
   dtCore::RefPtr<RTIDimensionHandle> result;

   try
   {
      RTI::SpaceHandle spaceHandle =  mImpl->mAmbassador->getRoutingSpaceHandle(HYPERSPACE);
      // Store and cache all the results so we can always use the same object for the handle.
      RTI::DimensionHandle dimH = mImpl->mAmbassador->getDimensionHandle(name.c_str(), spaceHandle);
      // We want to always return the same handle instance, so we cache them.

      result = WrapHandle<RTI::DimensionHandle, RTIDimensionHandle, RTI13DimensionHandle>(dimH, mImpl->mDimensionHandleCache);
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

   return result;
}

dtCore::RefPtr<RTIRegion> RTI13Ambassador::CreateRegion(RTIDimensionHandleSet& dimensions)
{
   dtCore::RefPtr<RTIRegion> result;
   try
   {
      RTI::SpaceHandle spaceHandle =  mImpl->mAmbassador->getRoutingSpaceHandle(HYPERSPACE);
      RTI::Region* region = mImpl->mAmbassador->createRegion(spaceHandle, dimensions.size());
      result = new RTI13Region(*region);
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }

   return result;
}

void RTI13Ambassador::DeleteRegion(RTIRegion& region)
{
   try
   {
      mImpl->mAmbassador->deleteRegion(&static_cast<RTI13Region&>(region).GetRTI13Region());
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
}

void RTI13Ambassador::SetRegionDimensions(RTIRegion& region, const RTIDimensionVector& regionDimensions)
{
   for (unsigned i = 0; i < regionDimensions.size(); ++i)
   {
      RTI::DimensionHandle rtiHandle = static_cast<RTI13DimensionHandle&>(*regionDimensions[i].mDimHandle).GetRTI13Handle();
      RTI::Region& rtiRegion = static_cast<RTI13Region&>(region).GetRTI13Region();
      rtiRegion.setRangeLowerBound(i, rtiHandle, regionDimensions[i].mMin);
      rtiRegion.setRangeUpperBound(i, rtiHandle, regionDimensions[i].mMax);
   }
}

void RTI13Ambassador::CommitRegionChanges(RTIRegion& region)
{
   try
   {
      mImpl->mAmbassador->notifyAboutRegionModification(static_cast<RTI13Region&>(region).GetRTI13Region());
   }
   catch (const RTI::Exception& ex)
   {
      RethrowRTIException(ex);
   }
}

unsigned int RTI13Ambassador::GetNumDimensions(RTIRegion& region)
{
   return static_cast<RTI13Region&>(region).GetRTI13Region().getNumberOfExtents();
}

}
