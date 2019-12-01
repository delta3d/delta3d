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
 * FOR A PARTI1516eCULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * David Guthrie
 */

#include "rti1516eambassador.h"
#include "rti1516ehandle.h"
#include "rti1516eutil.h"
#include "rti1516eregion.h"
#include <dtHLAGM/rtiexception.h>
#include <dtHLAGM/rtifederateambassador.h>
#include <dtHLAGM/ddmutil.h>

#include <RTI/RTI1516.h>
#include <RTI/NullFederateAmbassador.h>

#include <dtUtil/functor.h>

#include <sstream>

namespace dtHLAGM
{
   ///////////////////////////////////////
   class RegisterRTI1516e
   {
   public:
      RegisterRTI1516e()
      {
         RTIAmbassador::RegisterImplementation(RTIAmbassador::RTI1516e_IMPLEMENTATION,
                  RTIAmbassador::CreateFuncType(this, &RegisterRTI1516e::Create));
      }
      ~RegisterRTI1516e()
      {
         RTIAmbassador::UnregisterImplementation(RTIAmbassador::RTI1516e_IMPLEMENTATION);
      }

      dtCore::RefPtr<RTIAmbassador> Create()
      {
         DDMUtil::SetMaxExtent(ULONG_MAX);
         DDMUtil::SetMinExtent(0);
         return new RTI1516eAmbassador();
      }
   };

   static RegisterRTI1516e regRTI1516e;
   ///////////////////////////////////////

   class RTI1516eAmbassadorImpl : public rti1516e::NullFederateAmbassador
   {
   public:
      RTI1516eAmbassadorImpl()
      {
         try
         {
            rti1516e::RTIambassadorFactory factory;
            mAmbassador = factory.createRTIambassador();
         }
         catch (const rti1516e::Exception& ex)
         {
            RethrowRTI1516eException(ex);
         }
      }

      // Need the "throw()" because the base class has one.
      ~RTI1516eAmbassadorImpl() throw()
      {
      }
      virtual void discoverObjectInstance (
               rti1516e::ObjectInstanceHandle theObject,
               rti1516e::ObjectClassHandle theObjectClass,
               std::wstring const & theObjectInstanceName)
      throw (
               rti1516e::FederateInternalError)
      {
         dtCore::RefPtr<RTIObjectClassHandle> objClsHandle = mObjectClassHandleCache[theObjectClass];

         dtCore::RefPtr<RTIObjectInstanceHandle> objIntHandle =
                  WrapHandle<rti1516e::ObjectInstanceHandle, RTIObjectInstanceHandle, RTI1516eObjectInstanceHandle>(theObject, mObjectInstanceHandleCache);

         mFedAmbassador->DiscoverObjectInstance(*objIntHandle, *objClsHandle, WideToNarrow(theObjectInstanceName));

      }

      virtual void discoverObjectInstance (
               rti1516e::ObjectInstanceHandle theObject,
               rti1516e::ObjectClassHandle theObjectClass,
               std::wstring const & theObjectInstanceName,
               rti1516e::FederateHandle producingFederate)
      throw (
               rti1516e::FederateInternalError)
      {
         discoverObjectInstance(theObject, theObjectClass, theObjectInstanceName);
      }

      // 6.11
      virtual void reflectAttributeValues (
         rti1516e::ObjectInstanceHandle theObject,
         rti1516e::AttributeHandleValueMap const & theAttributeValues,
         rti1516e::VariableLengthData const & theUserSuppliedTag,
         rti1516e::OrderType sentOrder,
         rti1516e::TransportationType theType,
         rti1516e::SupplementalReflectInfo theReflectInfo)
         throw (
                  rti1516e::FederateInternalError)
      {
         dtCore::RefPtr<RTIObjectInstanceHandle> objIntHandle =
                  WrapHandle<rti1516e::ObjectInstanceHandle, RTIObjectInstanceHandle, RTI1516eObjectInstanceHandle>(theObject, mObjectInstanceHandleCache);

         RTIAttributeHandleValueMap attrMap;
         ConvertRTI1516eAHVMToAHVM(mAmbassador->getKnownObjectClassHandle(theObject), theAttributeValues, attrMap);

         std::string theTag;
         theTag.append((char*)(theUserSuppliedTag.data()), theUserSuppliedTag.size());

         mFedAmbassador->ReflectAttributeValues(*objIntHandle, attrMap, theTag);
      }

      virtual void reflectAttributeValues (
               rti1516e::ObjectInstanceHandle theObject,
               rti1516e::AttributeHandleValueMap const & theAttributeValues,
               rti1516e::VariableLengthData const & theUserSuppliedTag,
               rti1516e::OrderType sentOrder,
               rti1516e::TransportationType theType,
               rti1516e::LogicalTime const & theTime,
               rti1516e::OrderType receivedOrder,
               rti1516e::SupplementalReflectInfo theReflectInfo)
         throw (
                  rti1516e::FederateInternalError)
      {
         reflectAttributeValues (theObject, theAttributeValues, theUserSuppliedTag, sentOrder, theType, theReflectInfo);
      }

      virtual void reflectAttributeValues (
               rti1516e::ObjectInstanceHandle theObject,
               rti1516e::AttributeHandleValueMap const & theAttributeValues,
               rti1516e::VariableLengthData const & theUserSuppliedTag,
               rti1516e::OrderType sentOrder,
               rti1516e::TransportationType theType,
               rti1516e::LogicalTime const & theTime,
               rti1516e::OrderType receivedOrder,
               rti1516e::MessageRetractionHandle theHandle,
               rti1516e::SupplementalReflectInfo theReflectInfo)
         throw (
                  rti1516e::FederateInternalError)
      {
         reflectAttributeValues (theObject, theAttributeValues, theUserSuppliedTag, sentOrder, theType, theReflectInfo);
      }


      // 6.13
      virtual void receiveInteraction (
               rti1516e::InteractionClassHandle theInteraction,
               rti1516e::ParameterHandleValueMap const & theParameterValues,
               rti1516e::VariableLengthData const & theUserSuppliedTag,
               rti1516e::OrderType sentOrder,
               rti1516e::TransportationType theType,
               rti1516e::SupplementalReceiveInfo theReceiveInfo)
      throw (
               rti1516e::FederateInternalError)
      {
         dtCore::RefPtr<RTIInteractionClassHandle> intClsHandle =
                  WrapHandle<rti1516e::InteractionClassHandle, RTIInteractionClassHandle, RTI1516eInteractionClassHandle>(theInteraction, mInteractionClassHandleCache);

         RTIParameterHandleValueMap paramMap;
         ConvertRTI1516ePHVMToPHVM(theInteraction, theParameterValues, paramMap);

         std::string theTag;
         theTag.append((char*)(theUserSuppliedTag.data()), theUserSuppliedTag.size());

         mFedAmbassador->ReceiveInteraction(*intClsHandle, paramMap, theTag);
      }

      virtual void receiveInteraction (
               rti1516e::InteractionClassHandle theInteraction,
               rti1516e::ParameterHandleValueMap const & theParameterValues,
               rti1516e::VariableLengthData const & theUserSuppliedTag,
               rti1516e::OrderType sentOrder,
               rti1516e::TransportationType theType,
               rti1516e::LogicalTime const & theTime,
               rti1516e::OrderType receivedOrder,
               rti1516e::SupplementalReceiveInfo theReceiveInfo)
      throw (
               rti1516e::FederateInternalError)
      {
         receiveInteraction (theInteraction, theParameterValues, theUserSuppliedTag, sentOrder, theType, theReceiveInfo);
      }

      virtual void receiveInteraction (
               rti1516e::InteractionClassHandle theInteraction,
               rti1516e::ParameterHandleValueMap const & theParameterValues,
               rti1516e::VariableLengthData const & theUserSuppliedTag,
               rti1516e::OrderType sentOrder,
               rti1516e::TransportationType theType,
               rti1516e::LogicalTime const & theTime,
               rti1516e::OrderType receivedOrder,
               rti1516e::MessageRetractionHandle theHandle,
               rti1516e::SupplementalReceiveInfo theReceiveInfo)
      throw (
               rti1516e::FederateInternalError)
      {
         receiveInteraction (theInteraction, theParameterValues, theUserSuppliedTag, sentOrder, theType, theReceiveInfo);
      }

      // 6.15
      virtual void removeObjectInstance (
         rti1516e::ObjectInstanceHandle theObject,
         rti1516e::VariableLengthData const & theUserSuppliedTag,
         rti1516e::OrderType sentOrder,
         rti1516e::SupplementalRemoveInfo theRemoveInfo)
         throw (
                  rti1516e::FederateInternalError)
      {
         dtCore::RefPtr<RTIObjectInstanceHandle> objIntHandle =
                  WrapHandle<rti1516e::ObjectInstanceHandle, RTIObjectInstanceHandle, RTI1516eObjectInstanceHandle>(theObject, mObjectInstanceHandleCache);

         std::string theTag;
         theTag.append((char*)(theUserSuppliedTag.data()), theUserSuppliedTag.size());

         mFedAmbassador->RemoveObjectInstance(*objIntHandle, theTag);
      }

      virtual void removeObjectInstance (
               rti1516e::ObjectInstanceHandle theObject,
               rti1516e::VariableLengthData const & theUserSuppliedTag,
               rti1516e::OrderType sentOrder,
               rti1516e::LogicalTime const & theTime,
               rti1516e::OrderType receivedOrder,
               rti1516e::SupplementalRemoveInfo theRemoveInfo)
         throw (
                  rti1516e::FederateInternalError)
      {
         removeObjectInstance (theObject, theUserSuppliedTag, sentOrder, theRemoveInfo);
      }

      virtual void removeObjectInstance (
               rti1516e::ObjectInstanceHandle theObject,
               rti1516e::VariableLengthData const & theUserSuppliedTag,
               rti1516e::OrderType sentOrder,
               rti1516e::LogicalTime const & theTime,
               rti1516e::OrderType receivedOrder,
               rti1516e::MessageRetractionHandle theHandle,
               rti1516e::SupplementalRemoveInfo theRemoveInfo)
         throw (
                  rti1516e::FederateInternalError)
      {
         removeObjectInstance (theObject, theUserSuppliedTag, sentOrder, theRemoveInfo);
      }

      // 6.20
      virtual void provideAttributeValueUpdate (
               rti1516e::ObjectInstanceHandle theObject,
               rti1516e::AttributeHandleSet const & theAttributes,
               rti1516e::VariableLengthData const & theUserSuppliedTag)
         throw (
                  rti1516e::FederateInternalError)
      {
         dtCore::RefPtr<RTIObjectInstanceHandle> objIntHandle =
                  WrapHandle<rti1516e::ObjectInstanceHandle, RTIObjectInstanceHandle, RTI1516eObjectInstanceHandle>(theObject, mObjectInstanceHandleCache);

         RTIAttributeHandleSet attrSet;
         ConvertRTI1516eAHSToAHS(mAmbassador->getKnownObjectClassHandle(theObject), theAttributes, attrSet);
         mFedAmbassador->ProvideAttributeValueUpdate(*objIntHandle, attrSet);

      }

      virtual void objectInstanceNameReservationSucceeded (
         std::wstring const & theObjectInstanceName)
         throw (rti1516e::FederateInternalError)
      {
         mFedAmbassador->ObjectInstanceNameReservationSucceeded(WideToNarrow(theObjectInstanceName));
      }

      virtual void objectInstanceNameReservationFailed (
         std::wstring const & theObjectInstanceName)
         throw (rti1516e::FederateInternalError)
      {
         mFedAmbassador->ObjectInstanceNameReservationFailed(WideToNarrow(theObjectInstanceName));
      }

      dtCore::RefPtr<RTIAttributeHandle> WrapAttrHandle(const rti1516e::ObjectClassHandle& objClass, const rti1516e::AttributeHandle& handle)
      {
         AttrHandleCacheKey cachekey(handle, objClass);
         dtCore::RefPtr<RTIAttributeHandle> wrappedHandle = mAttrHandleCache[cachekey];
         if (!wrappedHandle.valid())
         {
            wrappedHandle = new RTI1516eAttributeHandle(handle);
            mAttrHandleCache[cachekey] = wrappedHandle;
         }
         return wrappedHandle;
      }

      dtCore::RefPtr<RTIAttributeHandle> WrapParamHandle(const rti1516e::InteractionClassHandle& intClass, const rti1516e::ParameterHandle& handle)
      {
         ParamHandleCacheKey cachekey(handle, intClass);
         dtCore::RefPtr<RTIParameterHandle> wrappedHandle = mParamHandleCache[cachekey];
         if (!wrappedHandle.valid())
         {
            wrappedHandle = new RTI1516eParameterHandle(handle);
            mParamHandleCache[cachekey] = wrappedHandle;
         }
         return wrappedHandle;
      }

      void ConvertRTI1516eAHSToAHS(rti1516e::ObjectClassHandle objClass, const rti1516e::AttributeHandleSet& rtiHandleSet, RTIAttributeHandleSet& dtHandleSetToFill)
      {
         rti1516e::AttributeHandleSet::const_iterator i, iend;
         i = rtiHandleSet.begin();
         iend = rtiHandleSet.end();
         for (; i != iend; ++i)
         {
            rti1516e::AttributeHandle attrHandle = *i;
            dtCore::RefPtr<RTIAttributeHandle> wrappedHandle = WrapAttrHandle(objClass, attrHandle);
            dtHandleSetToFill.insert(wrappedHandle);
         }
      }

      void ConvertRTI1516eAHVMToAHVM(rti1516e::ObjectClassHandle objClass, const rti1516e::AttributeHandleValueMap& rtiPairSet, RTIAttributeHandleValueMap& dtHandleValueMapToFill)
      {
         rti1516e::AttributeHandleValueMap::const_iterator i, iend;
         i = rtiPairSet.begin();
         iend = rtiPairSet.end();
         for (; i != iend; ++i)
         {
            RTIContainerValueData data;
            data.mData.append((const char*)(i->second.data()), i->second.size());
            //rti1516e::Region* reg = i->second
            //         rtiPairSet.getRegion(i);
            //if (reg != NULL)
            //{
            //   data.mRegion = new RTI1516eRegion(*reg);
            //}
            rti1516e::AttributeHandle attrHandle = i->first;
            dtCore::RefPtr<RTIAttributeHandle> wrappedHandle = WrapAttrHandle(objClass, attrHandle);
            dtHandleValueMapToFill.insert(std::make_pair(wrappedHandle, data));
         }
      }

      void ConvertRTI1516ePHVMToPHVM(rti1516e::InteractionClassHandle intClass, const rti1516e::ParameterHandleValueMap& rtiMap, RTIParameterHandleValueMap& dtHandleValueMapToFill)
      {
         //rti1516e::Region* reg13 = rtiPairSet.getRegion();
         //dtCore::RefPtr<RTIRegion> region = new RTI1516eRegion(*reg13);

         rti1516e::ParameterHandleValueMap::const_iterator i, iend;
         i = rtiMap.begin();
         iend = rtiMap.end();

         for (; i != iend; ++i)
         {
            RTIContainerValueData data;
            data.mData.append((const char*)(i->second.data()), i->second.size());

            //if (region.valid())
            //{
            //   data.mRegion = region;
            //}
            rti1516e::ParameterHandle paramHandle = i->first;
            dtCore::RefPtr<RTIParameterHandle> wrappedHandle = WrapParamHandle(intClass, paramHandle);
            dtHandleValueMapToFill.insert(std::make_pair(wrappedHandle, data));
         }
      }


      std::auto_ptr<rti1516e::RTIambassador> mAmbassador;
      RTIFederateAmbassador* mFedAmbassador;

      std::map<rti1516e::ObjectInstanceHandle, dtCore::RefPtr<RTIObjectInstanceHandle> > mObjectInstanceHandleCache;

      std::map<rti1516e::ObjectClassHandle, dtCore::RefPtr<RTIObjectClassHandle> > mObjectClassHandleCache;

      typedef std::pair<rti1516e::AttributeHandle, rti1516e::ObjectClassHandle > AttrHandleCacheKey;
      std::map<AttrHandleCacheKey, dtCore::RefPtr<RTIAttributeHandle> > mAttrHandleCache;

      std::map<rti1516e::InteractionClassHandle, dtCore::RefPtr<RTIObjectClassHandle> > mInteractionClassHandleCache;

      typedef std::pair<rti1516e::ParameterHandle, rti1516e::InteractionClassHandle > ParamHandleCacheKey;
      std::map<ParamHandleCacheKey, dtCore::RefPtr<RTIParameterHandle> > mParamHandleCache;

      std::map<rti1516e::DimensionHandle, dtCore::RefPtr<RTIDimensionHandle> > mDimensionHandleCache;
   };



RTI1516eAmbassador::RTI1516eAmbassador()
: mImpl(new RTI1516eAmbassadorImpl)
{
}

RTI1516eAmbassador::~RTI1516eAmbassador()
{
   delete mImpl;
}

void RTI1516eAmbassador::Tick()
{
   try
   {
      mImpl->mAmbassador->evokeMultipleCallbacks(0.001, 0.01);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
}

dtCore::RefPtr<RTIObjectClassHandle> RTI1516eAmbassador::GetObjectClassForInstance(RTIObjectInstanceHandle& instanceHandle)
{
   dtCore::RefPtr<RTIObjectClassHandle> result;
   try
   {
      rti1516e::ObjectClassHandle tmpHandle = mImpl->mAmbassador->getKnownObjectClassHandle(static_cast<RTI1516eObjectInstanceHandle&>(instanceHandle).GetRTI1516eHandle());

      result = WrapHandle<rti1516e::ObjectClassHandle, RTIObjectClassHandle, RTI1516eObjectClassHandle>(tmpHandle, mImpl->mObjectClassHandleCache);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
   return result;
}

std::string RTI1516eAmbassador::GetObjectClassName(RTIObjectClassHandle& clsHandle)
{
   std::string result;
   try
   {
      result = WideToNarrow(mImpl->mAmbassador->getObjectClassName(static_cast<RTI1516eObjectClassHandle&>(clsHandle).GetRTI1516eHandle()));
      //Some RTIs return this prefix, so this chops it off to make it more consistent.
      static const std::string OBJECT_ROOT("HLAobjectRoot.");

      if (result.length() > OBJECT_ROOT.length() &&
               result.substr(0, OBJECT_ROOT.length()) == OBJECT_ROOT)
      {
         result = result.substr(OBJECT_ROOT.size());
      }
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
   return result;
}

dtCore::RefPtr<RTIObjectClassHandle> RTI1516eAmbassador::GetObjectClassHandle(const std::string& className)
{
   dtCore::RefPtr<RTIObjectClassHandle> result;

   try
   {
      // Store and cache all the results so we can always use the same object for the handle.
      rti1516e::ObjectClassHandle clsH = mImpl->mAmbassador->getObjectClassHandle(NarrowToWide(className));
      result = WrapHandle<rti1516e::ObjectClassHandle, RTIObjectClassHandle, RTI1516eObjectClassHandle>(clsH, mImpl->mObjectClassHandleCache);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

   return result;
}

dtCore::RefPtr<RTIAttributeHandle> RTI1516eAmbassador::GetAttributeHandle(const std::string& attrName, RTIObjectClassHandle& handle)
{
   dtCore::RefPtr<RTIAttributeHandle> result;

   try
   {
      // Store and cache all the results so we can always use the same object for the handle.
      rti1516e::ObjectClassHandle clsH = static_cast<RTI1516eObjectClassHandle&>(handle).GetRTI1516eHandle();
      rti1516e::AttributeHandle attrH = mImpl->mAmbassador->getAttributeHandle(clsH, NarrowToWide(attrName));

      result = mImpl->WrapAttrHandle(clsH, attrH);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

   return result;

}

std::string RTI1516eAmbassador::GetAttributeName(RTIAttributeHandle& attrHandle, RTIObjectClassHandle& clsHandle)
{
   std::string result;
   try
   {
      result = WideToNarrow(mImpl->mAmbassador->getAttributeName(
               static_cast<RTI1516eObjectClassHandle&>(clsHandle).GetRTI1516eHandle(),
               static_cast<RTI1516eAttributeHandle&>(attrHandle).GetRTI1516eHandle()
               ));
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
   return result;
}

void RTI1516eAmbassador::SubscribeObjectClassAttributes(RTIObjectClassHandle& handle, const RTIAttributeHandleSet& ahs, RTIRegion* region)
{
   rti1516e::AttributeHandleSet ahsRTI1516e;
   ConvertAHStoRTI1516eAHS(ahs, ahsRTI1516e);
   rti1516e::ObjectClassHandle handleRTI1516e = static_cast<RTI1516eObjectClassHandle*>(&handle)->GetRTI1516eHandle();

   try
   {
      if (region == NULL)
      {
         mImpl->mAmbassador->subscribeObjectClassAttributes(handleRTI1516e, ahsRTI1516e);
      }
      else
      {
         rti1516e::RegionHandle& regionHandle = static_cast<RTI1516eRegion*>(region)->GetRTI1516eRegion();

         rti1516e::AttributeHandleSetRegionHandleSetPairVector ahsRsVRTI1516e;
         rti1516e::RegionHandleSet regHS1516e;
         regHS1516e.insert(regionHandle);

         ahsRsVRTI1516e.push_back(std::make_pair(ahsRTI1516e, regHS1516e));

         mImpl->mAmbassador->subscribeObjectClassAttributesWithRegions(handleRTI1516e, ahsRsVRTI1516e);
      }
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

}

void RTI1516eAmbassador::PublishObjectClass(RTIObjectClassHandle& handle, const RTIAttributeHandleSet& ahs)
{
   rti1516e::AttributeHandleSet ahsRTI1516e;
   ConvertAHStoRTI1516eAHS(ahs, ahsRTI1516e);
   rti1516e::ObjectClassHandle handleRTI1516e = static_cast<RTI1516eObjectClassHandle*>(&handle)->GetRTI1516eHandle();
   try
   {
         mImpl->mAmbassador->publishObjectClassAttributes(handleRTI1516e, ahsRTI1516e);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

}

void RTI1516eAmbassador::UnsubscribeObjectClass(RTIObjectClassHandle& handle, RTIRegion* region)
{
   rti1516e::ObjectClassHandle handleRTI1516e = static_cast<RTI1516eObjectClassHandle*>(&handle)->GetRTI1516eHandle();

   try
   {
      if (region == NULL)
      {
         mImpl->mAmbassador->unsubscribeObjectClass(handleRTI1516e);
      }
      else
      {
         // TODO deal with attributes and regions differently so this can happen. regions are assigned per
         // attribute, so it's hard to unsubscribe to a region for a class unless you know which attributes.
//         rti1516e::RegionHandle& regionHandle = static_cast<RTI1516eRegion*>(region)->GetRTI1516eRegion();
//
//         rti1516e::AttributeHandleSetRegionHandleSetPairVector ahsRsVRTI1516e;
//         rti1516e::RegionHandleSet regHS1516e;
//         regHS1516e.insert(regionHandle);
//
//         ahsRsVRTI1516e.push_back(std::make_pair(ahsRTI1516e, regHS1516e));
//         mImpl->mAmbassador->unsubscribeObjectClassWithRegion(handleRTI1516e, ahsRsVRTI1516e);
      }
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

}

std::string RTI1516eAmbassador::GetInteractionClassName(RTIInteractionClassHandle& intClsHandle)
{
   std::string result;
   try
   {
      result = WideToNarrow(mImpl->mAmbassador->getInteractionClassName(static_cast<RTI1516eInteractionClassHandle&>(intClsHandle).GetRTI1516eHandle()));

      // strip off the InteractionRoot prefix because some RTIs return that as part of the string.
      static const std::string INTERACTION_ROOT("HLAinteractionRoot.");
      if (result.size() > INTERACTION_ROOT.size() &&
               result.substr(0, INTERACTION_ROOT.size()) == INTERACTION_ROOT)
      {
         result = result.substr(INTERACTION_ROOT.size());
      }
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
   return result;

}

dtCore::RefPtr<RTIInteractionClassHandle> RTI1516eAmbassador::GetInteractionClassHandle(const std::string& className)
{
   dtCore::RefPtr<RTIInteractionClassHandle> result;

   try
   {
      rti1516e::InteractionClassHandle intrH = mImpl->mAmbassador->getInteractionClassHandle(NarrowToWide(className));

      result = WrapHandle<rti1516e::InteractionClassHandle, RTIInteractionClassHandle, RTI1516eInteractionClassHandle>(intrH, mImpl->mInteractionClassHandleCache);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

   return result;

}

dtCore::RefPtr<RTIParameterHandle> RTI1516eAmbassador::GetParameterHandle(const std::string& paramName, RTIInteractionClassHandle& handle)
{
   dtCore::RefPtr<RTIParameterHandle> result;


   if (result == NULL)
   {
      try
      {
         // We want to always return the same handle instance, so we cache them.
         rti1516e::InteractionClassHandle intrH = static_cast<RTI1516eInteractionClassHandle&>(handle).GetRTI1516eHandle();
         rti1516e::ParameterHandle paramH = mImpl->mAmbassador->getParameterHandle(intrH, NarrowToWide(paramName));

         result = mImpl->WrapParamHandle(intrH, paramH);
      }
      catch (const rti1516e::Exception& ex)
      {
         RethrowRTI1516eException(ex);
      }
   }

   return result;

}

void RTI1516eAmbassador::SubscribeInteractionClass(RTIInteractionClassHandle& handle, RTIRegion* region)
{
   rti1516e::InteractionClassHandle handleRTI1516e = static_cast<RTI1516eInteractionClassHandle*>(&handle)->GetRTI1516eHandle();

   try
   {
      if (region == NULL)
      {
         mImpl->mAmbassador->subscribeInteractionClass(handleRTI1516e);
      }
      else
      {
         rti1516e::RegionHandleSet rhs;
         rhs.insert(static_cast<RTI1516eRegion*>(region)->GetRTI1516eRegion());
         mImpl->mAmbassador->subscribeInteractionClassWithRegions(handleRTI1516e, rhs);
      }
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

}

void RTI1516eAmbassador::PublishInteractionClass(RTIInteractionClassHandle& handle)
{
   rti1516e::InteractionClassHandle handleRTI1516e = static_cast<RTI1516eInteractionClassHandle*>(&handle)->GetRTI1516eHandle();

   try
   {
      mImpl->mAmbassador->publishInteractionClass(handleRTI1516e);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

}

void RTI1516eAmbassador::UnsubscribeInteractionClass(RTIInteractionClassHandle& handle, RTIRegion* region)
{
   rti1516e::InteractionClassHandle handleRTI1516e = static_cast<RTI1516eInteractionClassHandle*>(&handle)->GetRTI1516eHandle();

   try
   {
      if (region == NULL)
      {
         mImpl->mAmbassador->unsubscribeInteractionClass(handleRTI1516e);
      }
      else
      {
         rti1516e::RegionHandleSet rhs;
         rhs.insert(static_cast<RTI1516eRegion*>(region)->GetRTI1516eRegion());
         mImpl->mAmbassador->unsubscribeInteractionClassWithRegions(handleRTI1516e, rhs);
      }
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
}

void RTI1516eAmbassador::ConnectToRTI(RTIFederateAmbassador& federateCallback, const std::string& rtiSpecificConnectData)
{
   try
   {
      mImpl->mFedAmbassador = &federateCallback;
      mImpl->mAmbassador->connect(*mImpl,rti1516e::HLA_EVOKED, NarrowToWide(rtiSpecificConnectData));
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
}

bool RTI1516eAmbassador::CreateFederationExecution(const std::string& executionName, const std::vector<std::string>& fedFiles)
{
   if (fedFiles.empty())
   {
      throw RTIException("The fedfiles parameter must have at least one entry.  It is empty.", __FILE__, __LINE__);
   }
   try
   {
      std::vector<std::wstring> fedFilesWide;
      dtUtil::Functor<std::wstring, TYPELIST_1(const std::string&)> transformFunc(&dtHLAGM::NarrowToWide);
      fedFilesWide.resize(fedFiles.size(), std::wstring());
      std::transform(fedFiles.begin(), fedFiles.end(), fedFilesWide.begin(), transformFunc);
      mImpl->mAmbassador->createFederationExecution(NarrowToWide(executionName), fedFilesWide);
      //some other rti's want the data in more of this format.
      //mRTIAmbassador->createFederationExecution(osgDB::getStrippedName(fedFile).c_str(), ".fed");
   }
   catch(const rti1516e::FederationExecutionAlreadyExists&)
   {
      return false;
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
   return true;
}

void RTI1516eAmbassador::JoinFederationExecution(const std::string& federateName, const std::string& executionName)
{
   try
   {
      mImpl->mAmbassador->joinFederationExecution(NarrowToWide(federateName), NarrowToWide(executionName));
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
}

void RTI1516eAmbassador::ResignFederationExecution(const std::string& executionName)
{
   try
   {
      mImpl->mAmbassador->resignFederationExecution(rti1516e::CANCEL_THEN_DELETE_THEN_DIVEST);
      mImpl->mAmbassador->disconnect();
   }
   catch(rti1516e::RTIinternalError& ex)
   {
      RethrowRTI1516eException(ex);
   }

   if (!executionName.empty())
   {
      try
      {
         mImpl->mAmbassador->destroyFederationExecution(NarrowToWide(executionName.c_str()));
      }
      catch(rti1516e::FederatesCurrentlyJoined& ex)
      {
         //std::wostringstream ss;
         //ss << ex;
         LOG_INFO("Could not destroy the federation execution because other federates are still connected: " + WideToNarrow(ex.what()));
      }
      catch(rti1516e::FederationExecutionDoesNotExist& ex)
      {
         //std::wostringstream ss;
         //ss << ex;
         LOG_DEBUG("Could not destroy the federation execution because it does not exist: " + WideToNarrow(ex.what()));
      }
      catch(rti1516e::Exception& ex)
      {
         RethrowRTI1516eException(ex);
      }
   }
}

void RTI1516eAmbassador::DeleteObjectInstance(RTIObjectInstanceHandle& instanceHandleToDelete)
{
   try
   {
      rti1516e::VariableLengthData vld(NULL, 0);
      mImpl->mAmbassador->deleteObjectInstance(static_cast<RTI1516eObjectInstanceHandle*>(&instanceHandleToDelete)->GetRTI1516eHandle(), vld);
   }
   // This should not happen unless RTI is broken some how.
   catch( rti1516e::Exception& ex )
   {
      RethrowRTI1516eException(ex);
   }

}

void RTI1516eAmbassador::ReserveObjectInstanceName(const std::string& nameToReserve)
{
   try
   {
      mImpl->mAmbassador->reserveObjectInstanceName(NarrowToWide(nameToReserve));
   }
   catch (rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
}


dtCore::RefPtr<RTIObjectInstanceHandle> RTI1516eAmbassador::RegisterObjectInstance(RTIObjectClassHandle& clsHandle, const std::string& stringName)
{
   dtCore::RefPtr<RTIObjectInstanceHandle> result;
   try
   {
      //Pass ClassHandle to registerObjectInstance
      rti1516e::ObjectInstanceHandle objectHandle = mImpl->mAmbassador->registerObjectInstance(static_cast<RTI1516eObjectClassHandle*>(&clsHandle)->GetRTI1516eHandle(), NarrowToWide(stringName));
      result = WrapHandle<rti1516e::ObjectInstanceHandle, RTIObjectInstanceHandle, RTI1516eObjectInstanceHandle>(objectHandle, mImpl->mObjectInstanceHandleCache);
   }
   catch (rti1516e::ObjectInstanceNameInUse& ex)
   {
      RethrowRTI1516eException(ex);
   }
   catch (rti1516e::ObjectInstanceNameNotReserved& ex)
   {
      RethrowRTI1516eException(ex);
   }
   catch (rti1516e::ObjectClassNotPublished& ex)
   {
      RethrowRTI1516eException(ex);
   }
   catch (rti1516e::ObjectClassNotDefined& ex)
   {
      RethrowRTI1516eException(ex);
   }
   catch (rti1516e::SaveInProgress& ex)
   {
      RethrowRTI1516eException(ex);
   }
   catch (rti1516e::RestoreInProgress& ex)
   {
      RethrowRTI1516eException(ex);
   }
   catch (rti1516e::FederateNotExecutionMember& ex)
   {
      RethrowRTI1516eException(ex);
   }
   catch (rti1516e::NotConnected& ex)
   {
      RethrowRTI1516eException(ex);
   }
   catch (rti1516e::RTIinternalError& ex)
   {
      RethrowRTI1516eException(ex);
   }
   catch (rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

   return result;
}

void RTI1516eAmbassador::UpdateAttributeValues(RTIObjectInstanceHandle& instanceToUpdate, RTIAttributeHandleValueMap& attrs, const std::string& tag)
{
   rti1516e::AttributeHandleValueMap rtiAttrs;
   ConvertAHVMToRTI1516eAHVM(attrs, rtiAttrs);
   try
   {
      rti1516e::VariableLengthData vld(tag.c_str(), tag.length());
      mImpl->mAmbassador->updateAttributeValues(
               static_cast<RTI1516eObjectInstanceHandle*>(&instanceToUpdate)->GetRTI1516eHandle(),
               rtiAttrs,
               vld);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

}

void RTI1516eAmbassador::SendInteraction(RTIInteractionClassHandle& interationClass, const RTIParameterHandleValueMap& params, const std::string& tag)
{
   rti1516e::ParameterHandleValueMap rtiParams;
   ConvertPHVMToRTI1516ePHVM(params, rtiParams);
   try
   {
      rti1516e::VariableLengthData vld(tag.c_str(), tag.length());
      mImpl->mAmbassador->sendInteraction(
               static_cast<RTI1516eInteractionClassHandle*>(&interationClass)->GetRTI1516eHandle(),
               rtiParams,
               vld);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

}

std::string RTI1516eAmbassador::GetDimensionName(RTIDimensionHandle& dimHandle)
{
   std::string result;
   try
   {
      result = WideToNarrow(mImpl->mAmbassador->getDimensionName(static_cast<RTI1516eDimensionHandle&>(dimHandle).GetRTI1516eHandle()));
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
   return result;
}

dtCore::RefPtr<RTIDimensionHandle> RTI1516eAmbassador::GetDimensionHandle(const std::string& name)
{
   dtCore::RefPtr<RTIDimensionHandle> result;

   try
   {
      rti1516e::DimensionHandle dimH = mImpl->mAmbassador->getDimensionHandle(NarrowToWide(name));
      // We want to always return the same handle instance, so we cache them.

      result = WrapHandle<rti1516e::DimensionHandle, RTIDimensionHandle, RTI1516eDimensionHandle>(dimH, mImpl->mDimensionHandleCache);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

   return result;
}

dtCore::RefPtr<RTIRegion> RTI1516eAmbassador::CreateRegion(RTIDimensionHandleSet& dimensions)
{
   dtCore::RefPtr<RTIRegion> result;
   try
   {
      rti1516e::DimensionHandleSet dhs;
      RTIDimensionHandleSet::const_iterator i, iend;
      i = dimensions.begin();
      iend = dimensions.end();
      for (; i != iend; ++i)
      {
         dhs.insert(static_cast<const RTI1516eDimensionHandle*>(i->get())->GetRTI1516eHandle());
      }

      rti1516e::RegionHandle region = mImpl->mAmbassador->createRegion(dhs);
      result = new RTI1516eRegion(region);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }

   return result;
}

void RTI1516eAmbassador::DeleteRegion(RTIRegion& region)
{
   try
   {
      mImpl->mAmbassador->deleteRegion(static_cast<RTI1516eRegion&>(region).GetRTI1516eRegion());
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
}

void RTI1516eAmbassador::SetRegionDimensions(RTIRegion& region, const RTIDimensionVector& regionDimensions)
{
   for (unsigned i = 0; i < regionDimensions.size(); ++i)
   {
      rti1516e::DimensionHandle rtiDim = static_cast<RTI1516eDimensionHandle&>(*regionDimensions[i].mDimHandle).GetRTI1516eHandle();
      rti1516e::RegionHandle& rtiRegion = static_cast<RTI1516eRegion&>(region).GetRTI1516eRegion();
      rti1516e::RangeBounds bounds(regionDimensions[i].mMin, regionDimensions[i].mMax);
      mImpl->mAmbassador->setRangeBounds(rtiRegion, rtiDim, bounds);
   }
}

void RTI1516eAmbassador::CommitRegionChanges(RTIRegion& region)
{
   try
   {
      rti1516e::RegionHandleSet regHS;
      regHS.insert(static_cast<RTI1516eRegion&>(region).GetRTI1516eRegion());
      mImpl->mAmbassador->commitRegionModifications(regHS);
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
}

unsigned int RTI1516eAmbassador::GetNumDimensions(RTIRegion& region)
{
   unsigned int result = 0;
   try
   {
      result = mImpl->mAmbassador->getDimensionHandleSet(static_cast<RTI1516eRegion&>(region).GetRTI1516eRegion()).size();
   }
   catch (const rti1516e::Exception& ex)
   {
      RethrowRTI1516eException(ex);
   }
   return result;
}

}
