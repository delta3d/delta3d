/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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

#include <osg/Endian>
#include <string.h>

#include <dtUtil/coordinates.h>
#include <dtUtil/log.h>
#include <dtCore/uniqueid.h>

#include <dtHLAGM/onetomanymapping.h>
#include <dtHLAGM/rprparametertranslator.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/objectruntimemappinginfo.h>
#include <dtHLAGM/spatial.h>

#include <dtGame/deadreckoningcomponent.h>

namespace dtHLAGM
{
   IMPLEMENT_ENUM(RPRAttributeType);
   const RPRAttributeType RPRAttributeType::SPATIAL_TYPE("SPATIAL_TYPE", 6, 84);
   const RPRAttributeType RPRAttributeType::WORLD_COORDINATE_TYPE("WORLD_COORDINATE_TYPE", 1, 24);
   const RPRAttributeType RPRAttributeType::EULER_ANGLES_TYPE("EULER_ANGLES_TYPE", 1, 12);
   const RPRAttributeType RPRAttributeType::VELOCITY_VECTOR_TYPE("VELOCITY_VECTOR_TYPE", 1, 12);
   const RPRAttributeType RPRAttributeType::ANGULAR_VELOCITY_VECTOR_TYPE("ANGULAR_VELOCITY_VECTOR_TYPE", 1, 12);
   const RPRAttributeType RPRAttributeType::UNSIGNED_INT_TYPE("UNSIGNED_INT_TYPE", 1, 4);
   const RPRAttributeType RPRAttributeType::UNSIGNED_CHAR_TYPE("UNSIGNED_CHAR_TYPE", 1, 1);
   const RPRAttributeType RPRAttributeType::UNSIGNED_SHORT_TYPE("UNSIGNED_SHORT_TYPE", 1, 2);
   const RPRAttributeType RPRAttributeType::FLOAT_TYPE("FLOAT_TYPE", 1, 4);
   const RPRAttributeType RPRAttributeType::DOUBLE_TYPE("DOUBLE_TYPE", 1, 8);
   const RPRAttributeType RPRAttributeType::ENTITY_TYPE("ENTITY_TYPE", 1, 8);
   const RPRAttributeType RPRAttributeType::ENTITY_IDENTIFIER_TYPE("ENTITY_IDENTIFIER_TYPE", 1, 6);
   const RPRAttributeType RPRAttributeType::EVENT_IDENTIFIER_TYPE("EVENT_IDENTIFIER_TYPE", 1, 5);
   const RPRAttributeType RPRAttributeType::MARKING_TYPE("MARKING_TYPE", 1, 12);
   const RPRAttributeType RPRAttributeType::MARKING_TYPE_32("MARKING_TYPE_32", 1, 32);
   const RPRAttributeType RPRAttributeType::STRING_TYPE("STRING_TYPE", 1, 128);
   const RPRAttributeType RPRAttributeType::ARTICULATED_PART_TYPE("ARTICULATED_PART_TYPE", 1, 512);
   const RPRAttributeType RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE("RTI_OBJECT_ID_STRUCT_TYPE", 1, 128);
   const RPRAttributeType RPRAttributeType::TIME_TAG_TYPE("TIME_TAG_TYPE", 1, 17);

   /////////////////////////////////////////////////////////////////////////////
   RPRParameterTranslator::RPRParameterTranslator(dtUtil::Coordinates& coordinates, ObjectRuntimeMappingInfo& runtimeMappings):
      mCoordinates(coordinates), mRuntimeMappings(runtimeMappings)
   {
      mLogger = &dtUtil::Log::GetInstance("parametertranslator.cpp");
   }

   /////////////////////////////////////////////////////////////////////////////
   RPRParameterTranslator::~RPRParameterTranslator()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   const AttributeType& RPRParameterTranslator::GetAttributeTypeForName(const std::string& name) const
   {
      dtUtil::Enumeration* enumVal = RPRAttributeType::GetValueForName(name);

      if (enumVal == NULL)
         return AttributeType::UNKNOWN;
      else
         return static_cast<const AttributeType&>(*enumVal);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool RPRParameterTranslator::TranslatesAttributeType(const AttributeType& type) const
   {
      //I think doing a dynamic cast on the type to see if it's the subclass used by this class is probably
      //the fastest way to check.
      return dynamic_cast<const RPRAttributeType*>(&type) != NULL;
   }

   
   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromParamsToSpatial(
      char* buffer,
      size_t& maxSize,
      std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& parameters,
      const OneToManyMapping& mapping) const
   {
      //If we have parameters, we are also guaranteed to have paramdefs, so there is no need
      //to check
      if (parameters.size() < 7U)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                              "Not enough MessageParameters were supplied(" +
                              dtUtil::ToString(parameters.size()) + ") for a SPATIAL type(7).");
         return;
      }

      Spatial spatial;

      if (parameters[0].valid())
      {
         unsigned drCodeInt = GetIntegerValue(*parameters[0], mapping, 0);

         ///TODO verify the value of drCode is in range.
         char drCode = char(drCodeInt);
         dtUtil::Clamp(drCode, char(0), char(9));
         spatial.SetDeadReckoningAlgorithm(drCode);
      }

      //is frozen
      if (parameters[1].valid())
      {
         const dtGame::MessageParameter& frozenParam = *parameters[1];
         const dtDAL::DataType& dataType = mapping.GetParameterDefinitions()[1].GetGameType();

         if (dataType == dtDAL::DataType::BOOLEAN && frozenParam.GetDataType() == dataType)
         {
            spatial.SetFrozen(static_cast<const dtGame::BooleanMessageParameter&>(frozenParam).GetValue());
         }
         else
         {
            unsigned drCodeInt = GetIntegerValue(frozenParam, mapping, 1);
            spatial.SetFrozen(bool(drCodeInt));
         }
      }

      //world coordinate
      if (parameters[2].valid())
      {
         spatial.GetWorldCoordinate() = 
            CoordConvertPositionParameter(*parameters[2]);
      }
      
      if (parameters[3].valid())
      {
         spatial.GetOrientation() = osg::Vec3f(CoordConvertRotationParameter(*parameters[3]));
      }

      if (parameters[4].valid())
      {
         spatial.GetVelocity() = CoordConvertVelocityParameter(*parameters[4]);
      }

      if (parameters[5].valid())
      {
         spatial.GetAcceleration() = CoordConvertVelocityParameter(*parameters[5]);
      }

      if (parameters[6].valid())
      {
         spatial.GetAngularVelocity() = CoordConvertAngularVelocityParameter(*parameters[6]);
      }
      
      maxSize = spatial.Encode(buffer, maxSize);
      if (maxSize == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "The parameter data could not be encoded into a \"%s\" struct.",
                             RPRAttributeType::SPATIAL_TYPE.GetName().c_str());
         return;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3d RPRParameterTranslator::CoordConvertPositionParameter(const dtGame::MessageParameter& parameter) const
   {
      osg::Vec3 position;

      const dtDAL::DataType& parameterDataType = parameter.GetDataType();

      if (parameterDataType == dtDAL::DataType::VEC3)
      {
         position = static_cast<const dtGame::Vec3MessageParameter&>(parameter).GetValue();
      }
      else if (parameterDataType == dtDAL::DataType::VEC3F)
      {
         osg::Vec3f posTemp = static_cast<const dtGame::Vec3fMessageParameter&>(parameter).GetValue();
         position.x() = posTemp.x();
         position.y() = posTemp.y();
         position.z() = posTemp.z();
      }
      else if (parameterDataType == dtDAL::DataType::VEC3D)
      {
         osg::Vec3d posTemp = static_cast<const dtGame::Vec3dMessageParameter&>(parameter).GetValue();
         //We're loosing precision here if a Vec3 is not compiled as a vec3d, but the
         //coordinate converter doesn't support Vec3d directly.
         position.x() = posTemp.x();
         position.y() = posTemp.y();
         position.z() = posTemp.z();
      }
      else
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The incoming parameter \"%s\" is not of a supported type \"%s\" for conversion to \"%s\"",
                             parameter.GetName().c_str(), parameterDataType.GetName().c_str(),
                             RPRAttributeType::WORLD_COORDINATE_TYPE.GetName().c_str());
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The position is %f %f %f",
                             position.x(), position.y(), position.z());
      }

      return mCoordinates.ConvertToRemoteTranslation(position);
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromParamToWorldCoord(
      char* buffer, 
      const size_t maxSize, 
      const dtGame::MessageParameter& parameter) const
   {
      WorldCoordinate wc = CoordConvertPositionParameter(parameter);

      if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "World coordinate has been set to %lf %lf %lf",
                             wc.GetX(), wc.GetY(), wc.GetZ());

         osg::Vec3 testReversePos = mCoordinates.ConvertToLocalTranslation(wc);

         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The reverse coordinate would be %f %f %f",
                             testReversePos.x(), testReversePos.y(), testReversePos.z());
      }

      if (maxSize >= wc.EncodedLength())
         wc.Encode(buffer);
      else
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Not enough space was allocated in the buffer to convert", wc.GetX(), wc.GetY(), wc.GetZ());
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3d RPRParameterTranslator::CoordConvertRotationParameter(
            const dtGame::MessageParameter& parameter) const
   {
      const dtDAL::DataType& parameterDataType = parameter.GetDataType();

      osg::Vec3 result;
      osg::Vec3 angleReordered;
      if (parameterDataType == dtDAL::DataType::VEC3)
      {
         osg::Vec3 angle = static_cast<const dtGame::Vec3MessageParameter&>(parameter).GetValue();
         angleReordered.set(angle[2], angle[0], angle[1]);
      }
      else if (parameterDataType == dtDAL::DataType::VEC3F)
      {
         osg::Vec3f angle = static_cast<const dtGame::Vec3fMessageParameter&>(parameter).GetValue();
         angleReordered.set(angle[2], angle[0], angle[1]);
      }
      else if (parameterDataType == dtDAL::DataType::VEC3D)
      {
         osg::Vec3d angle = static_cast<const dtGame::Vec3dMessageParameter&>(parameter).GetValue();
         angleReordered.set(angle[2], angle[0], angle[1]);
      }
      else
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The incoming parameter \"%s\" is not of a supported type "
                             "\"%s\" for conversion to \"%s\"",
                             parameter.GetName().c_str(), parameterDataType.GetName().c_str(),
                             RPRAttributeType::EULER_ANGLES_TYPE.GetName().c_str());
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The rotation is %f %f %f",
                             angleReordered.x(), angleReordered.y(), angleReordered.z());
      }

      return mCoordinates.ConvertToRemoteRotation(angleReordered);
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromParamToEulerAngles(
      char* buffer, 
      const size_t maxSize, 
      const dtGame::MessageParameter& parameter) const
   {
      EulerAngles eulerAngles = osg::Vec3f(CoordConvertRotationParameter(parameter));
      
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "Euler Angles have been set to %f %f %f",
                             eulerAngles.GetPsi() , eulerAngles.GetTheta(), eulerAngles.GetPhi());

         osg::Vec3 reversed = mCoordinates.ConvertToLocalRotation(eulerAngles);

         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The reversed rotation would be have been set to %f %f %f",
                             reversed.x(), reversed.y(), reversed.z());
      }

      eulerAngles.Encode(buffer);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3f RPRParameterTranslator::CoordConvertVelocityParameter(
            const dtGame::MessageParameter& parameter) const
   {
      const dtDAL::DataType& parameterDataType = parameter.GetDataType();
      osg::Vec3 result;

      if (parameterDataType == dtDAL::DataType::VEC3)
      {
         const osg::Vec3& preResult = static_cast<const dtGame::Vec3MessageParameter&>(parameter).GetValue();
         result = mCoordinates.GetOriginRotationMatrixInverse().preMult(preResult);
      }
      else if (parameterDataType == dtDAL::DataType::VEC3F)
      {
         const osg::Vec3f& preResult = static_cast<const dtGame::Vec3fMessageParameter&>(parameter).GetValue();
         result = mCoordinates.GetOriginRotationMatrixInverse().preMult(preResult);
      } 
      else 
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The incoming parameter \"%s\" is not of a supported type \"%s\" for conversion to \"%s\"",
                             parameter.GetName().c_str(), parameterDataType.GetName().c_str(),
                             RPRAttributeType::VELOCITY_VECTOR_TYPE.GetName().c_str());
      }
      return osg::Vec3f(result);
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromParamToVelocityVector(
      char* buffer, 
      const size_t maxSize, 
      const dtGame::MessageParameter& parameter) const
   {
      VelocityVector velocityVector = CoordConvertVelocityParameter(parameter);
      velocityVector.Encode(buffer);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3f RPRParameterTranslator::CoordConvertAngularVelocityParameter(
            const dtGame::MessageParameter& parameter) const
   {
      const dtDAL::DataType& parameterDataType = parameter.GetDataType();
      osg::Vec3f result;
      if (parameterDataType == dtDAL::DataType::VEC3)
      {
         result = osg::Vec3f(static_cast<const dtGame::Vec3MessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::VEC3F)
      {
         result = static_cast<const dtGame::Vec3fMessageParameter&>(parameter).GetValue();
      } 
      else 
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The incoming parameter \"%s\" is not of a supported type \"%s\" for conversion to \"%s\"",
                             parameter.GetName().c_str(), parameterDataType.GetName().c_str(),
                             RPRAttributeType::ANGULAR_VELOCITY_VECTOR_TYPE.GetName().c_str());
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromParamToAngularVelocityVector(
      char* buffer,
      const size_t maxSize,
      const dtGame::MessageParameter& parameter) const
   {
      
      // USED FOR ANGULAR VELOCITY, ACCELERATION VECTOR, AND VELOCITY VECTOR
      VelocityVector velocityVector = CoordConvertAngularVelocityParameter(parameter);
      velocityVector.Encode(buffer);
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromParamToEntityType(
      char* buffer,
      const size_t maxSize,
      const dtGame::MessageParameter& parameter,
      const OneToManyMapping& mapping,
      const OneToManyMapping::ParameterDefinition& paramDef) const
   {
      std::stringstream valueAsString;

      if( parameter.GetDataType() == dtDAL::DataType::ENUMERATION )
      {
         const std::string& msgParamValue
            = static_cast<const dtGame::EnumMessageParameter&>(parameter).GetValue();

         valueAsString << (GetEnumValue(msgParamValue, paramDef, false).c_str());
      }
      else if( parameter.GetDataType() == dtDAL::DataType::STRING )
      {
         valueAsString << (static_cast<const dtGame::StringMessageParameter&>
            (parameter).GetValue().c_str());
      }
      else
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The incoming parameter \"%s\" is not of a supported type \"%s\" for conversion to \"%s\"."
                             "Only enumeration parameters are supported.",
                             parameter.GetName().c_str(), parameter.GetDataType().GetName().c_str(),
                             RPRAttributeType::VELOCITY_VECTOR_TYPE.GetName().c_str());
         return;
      } 

      EntityType entityType;
      valueAsString >> entityType;
      entityType.Encode(buffer);
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromStringParamToCharArray(
         char* buffer,
         size_t& maxSize,
         const dtGame::StringMessageParameter& parameter,
         const OneToManyMapping::ParameterDefinition& paramDef,
         const dtDAL::DataType& parameterDataType) const
   {
      const std::string& parameterValue = parameter.GetValue();

      std::string value;
      if (parameterDataType == dtDAL::DataType::ENUMERATION)
         value = GetEnumValue(parameterValue, paramDef, false);
      else
         value = parameterValue;

      //change the size of this parameter to match the actual string length.
      maxSize = value.size() + 1;

      for (unsigned i = 0; i < maxSize; ++i)
      {
         if (i < value.size())
         {
            buffer[i] = value[i];
         }
         else
         {
            //zero anything after the string value.
            buffer[i] = '\0';
         }
      }
      
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Mapped parameter to a string value.  The result with size \"%u\" is \"%s\".",
            maxSize, buffer);
   
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromMessageParameters(char* buffer, size_t& maxSize,
      std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& parameters, const OneToManyMapping& mapping) const
   {
      const AttributeType& hlaType = mapping.GetHLAType();

      if (parameters.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
            "Unable to map from Game to HLA mapping %s because no message parameters were passed into the mapping method.",
            mapping.GetHLAName().c_str());

         return;
      }

      if (mapping.GetParameterDefinitions().empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
            "Unable to map to HLA mapping %s because no parameter mappings are added to the mapping object.",
            mapping.GetHLAName().c_str());
         return;
      }

      //All the current mappings use only one parameter.
      const dtGame::MessageParameter& parameter = *parameters[0];
      const dtDAL::DataType& parameterDataType  = parameter.GetDataType();
      const OneToManyMapping::ParameterDefinition& paramDef = mapping.GetParameterDefinitions()[0];

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Mapping values from game mapping %s to HLA mapping %s.",
            paramDef.GetGameName().c_str(), mapping.GetHLAName().c_str());

      parameter.WriteToLog(*mLogger);

      if (parameterDataType != paramDef.GetGameType())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Warning, the Message Parameter DataType for \"%s\" is \"%s\", but the mapping configuration says it should be \"%s\"",
            parameter.GetName().c_str(), parameterDataType.GetName().c_str(), 
            paramDef.GetGameType().GetName().c_str());
      }

      if (hlaType == RPRAttributeType::SPATIAL_TYPE)
      {
         MapFromParamsToSpatial(buffer, maxSize, parameters, mapping);
      }
      else if (hlaType == RPRAttributeType::WORLD_COORDINATE_TYPE)
      {
         MapFromParamToWorldCoord(buffer, maxSize, parameter);
      }
      else if (hlaType == RPRAttributeType::ENTITY_TYPE)
      {
         MapFromParamToEntityType(buffer, maxSize, parameter, mapping, paramDef);
      }
      else if (hlaType == RPRAttributeType::EULER_ANGLES_TYPE)
      {
         MapFromParamToEulerAngles(buffer, maxSize, parameter);
      }
      else if (hlaType == RPRAttributeType::VELOCITY_VECTOR_TYPE)
      {
         MapFromParamToVelocityVector(buffer, maxSize, parameter);
      }
      else if (hlaType == RPRAttributeType::ANGULAR_VELOCITY_VECTOR_TYPE)
      {
         MapFromParamToAngularVelocityVector(buffer, maxSize, parameter);
      }
      else if (hlaType == RPRAttributeType::UNSIGNED_INT_TYPE)
      {
         unsigned value = (unsigned)GetIntegerValue(parameter, mapping, 0);

         if (osg::getCpuByteOrder() == osg::LittleEndian)
            osg::swapBytes((char*)(&value), sizeof(unsigned int));

         *(unsigned int*)(&buffer[0]) = value;
      }
      else if (hlaType == RPRAttributeType::UNSIGNED_SHORT_TYPE)
      {
         unsigned short value = (unsigned short)GetIntegerValue(parameter, mapping, 0);

         if (osg::getCpuByteOrder() == osg::LittleEndian)
            osg::swapBytes((char*)(&value), sizeof(unsigned short));

         *(unsigned short*)(&buffer[0]) = value;
      }
      else if (hlaType == RPRAttributeType::UNSIGNED_CHAR_TYPE)
      {
         unsigned char value = (unsigned char)GetIntegerValue(parameter, mapping, 0);

         *(unsigned char*)(&buffer[0]) = value;
      }
      else if (hlaType == RPRAttributeType::DOUBLE_TYPE)
      {
         double* value = (double*)(&buffer[0]);

         if (parameter.GetDataType() == dtDAL::DataType::DOUBLE)
         {
            *value = static_cast<const dtGame::DoubleMessageParameter&>(parameter).GetValue();
         }
         else if (parameter.GetDataType() == dtDAL::DataType::FLOAT)
         {
            *value = double(static_cast<const dtGame::FloatMessageParameter&>(parameter).GetValue());
         }

         if (osg::getCpuByteOrder() == osg::LittleEndian)
            osg::swapBytes((char*)(value), sizeof(double));
      }
      else if (hlaType == RPRAttributeType::FLOAT_TYPE)
      {
         float* value = (float*)(&buffer[0]);

         if (parameter.GetDataType() == dtDAL::DataType::DOUBLE)
         {
            *value = float(static_cast<const dtGame::DoubleMessageParameter&>(parameter).GetValue());
         }
         else if (parameter.GetDataType() == dtDAL::DataType::FLOAT)
         {
            *value = static_cast<const dtGame::FloatMessageParameter&>(parameter).GetValue();
         }

         if (osg::getCpuByteOrder() == osg::LittleEndian)
            osg::swapBytes((char*)(value), sizeof(float));
      }
      else if (hlaType == RPRAttributeType::EVENT_IDENTIFIER_TYPE)
      {
         EventIdentifier eventIdentifier;
         eventIdentifier.SetEventIdentifier(static_cast<const dtGame::UnsignedShortIntMessageParameter&>(parameter).GetValue());
         eventIdentifier.Encode(buffer);
      }
      else if (hlaType == RPRAttributeType::STRING_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::STRING ||
             parameterDataType == dtDAL::DataType::ENUMERATION)
         {
            MapFromStringParamToCharArray(buffer, maxSize, 
                  static_cast<const dtGame::StringMessageParameter&>(parameter), 
                  paramDef, parameterDataType);
         }
         else if (parameterDataType == dtDAL::DataType::ACTOR)
         {
            const dtCore::UniqueId& value = static_cast<const dtGame::ActorMessageParameter&>(parameter).GetValue();

            const std::string& stringValue = value.ToString();
            for (unsigned i = 0; i < RPRAttributeType::STRING_TYPE.GetEncodedLength(); ++i)
            {
               if (i < stringValue.size())
                  buffer[i] = stringValue[i];
               else
                  //zero anything after the string value.
                  buffer[i] = '\0';
            }
            //change the size of this parameter to match the actual string length.
            maxSize = stringValue.size() + 1;
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map from Game Type \"%s\" to HLA type \"%s\"",
               parameterDataType.GetName().c_str(),
               RPRAttributeType::STRING_TYPE.GetName().c_str());
         }
      }
      else if (hlaType == RPRAttributeType::MARKING_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::STRING)
         {
            const std::string& markingText = static_cast<const dtGame::StringMessageParameter&>(parameter).GetValue();

            CopyMarkingTextToBuffer(markingText, buffer, RPRAttributeType::MARKING_TYPE.GetEncodedLength());
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map from Game Type \"%s\" to HLA type \"%s\"",
               parameterDataType.GetName().c_str(),
               RPRAttributeType::MARKING_TYPE.GetName().c_str());
         }
      }
      else if (hlaType == RPRAttributeType::MARKING_TYPE_32)
      {
         if (parameterDataType == dtDAL::DataType::STRING)
         {
            const std::string& markingText = static_cast<const dtGame::StringMessageParameter&>(parameter).GetValue();

            CopyMarkingTextToBuffer(markingText, buffer, RPRAttributeType::MARKING_TYPE_32.GetEncodedLength());
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map from Game Type \"%s\" to HLA type \"%s\"",
               parameterDataType.GetName().c_str(),
               RPRAttributeType::MARKING_TYPE_32.GetName().c_str());
         }
      }
      else if (hlaType == RPRAttributeType::ENTITY_IDENTIFIER_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::ACTOR)
         {
            dtCore::UniqueId aid = static_cast<const dtGame::ActorMessageParameter&>(parameter).GetValue();

            const EntityIdentifier* eid = mRuntimeMappings.GetEntityId(aid);
            if (eid != NULL)
               eid->Encode(buffer);
            else
               //clear it.
               memset((void*)buffer, 0, maxSize);
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map from Game Type \"%s\" to HLA type \"%s\"",
               parameterDataType.GetName().c_str(),
               RPRAttributeType::MARKING_TYPE.GetName().c_str());
         }
      }
      else if (hlaType == RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::ACTOR)
         {
            const dtCore::UniqueId& value = static_cast<const dtGame::ActorMessageParameter&>(parameter).GetValue();

            const std::string* rtiId = mRuntimeMappings.GetRTIId(value);
            if( rtiId == NULL || rtiId->empty())
            {
               maxSize = 0;
               return;
//               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
//                  "No RTI string ID was mapped to ActorID \"%s\"",
//                  value.ToString().c_str());
            }

            const std::string& stringValue = *rtiId;
            for (unsigned i = 0; i < RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE.GetEncodedLength(); ++i)
            {
               if (i < stringValue.size())
                  buffer[i] = stringValue[i];
               else
                  //zero anything after the string value.
                  buffer[i] = '\0';
            }
            //change the size of this parameter to match the actual string length.
            maxSize = stringValue.size() + 1;
         }
         // enumeration doesn't really make sense for an ID, but the method it calls supports
         // enumerations, so there is no reason to limit it.
         else if (parameterDataType == dtDAL::DataType::STRING ||
               parameterDataType == dtDAL::DataType::ENUMERATION)
         {
            MapFromStringParamToCharArray(buffer, maxSize, 
                  static_cast<const dtGame::StringMessageParameter&>(parameter),
                  paramDef, parameterDataType);
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map from Game Type \"%s\" to HLA type \"%s\"",
               parameterDataType.GetName().c_str(),
               RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE.GetName().c_str());
         }
      }
      else if( hlaType == RPRAttributeType::ARTICULATED_PART_TYPE )
      {
         MapFromParamToArticulations( buffer, maxSize, parameter, paramDef );
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::CoordConvertWorldCoord(const WorldCoordinate& worldCoord,
            dtGame::MessageParameter& parameter) const
   {
      osg::Vec3 position = mCoordinates.ConvertToLocalTranslation(worldCoord);

      if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
         "The world coordinate was converted a local coordinate %lf %lf %lf",
            position[0], position[1], position[2]);

      if (parameter.GetDataType() == dtDAL::DataType::VEC3)
      {
         static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(position);
      }
      else if (parameter.GetDataType() == dtDAL::DataType::VEC3F)
      {
         static_cast<dtGame::Vec3fMessageParameter&>(parameter).SetValue(
            osg::Vec3f(position.x(), position.y(), position.z()));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::CoordConvertOrientation(const EulerAngles& eulerAngles,
            dtGame::MessageParameter& parameter) const
   {
      osg::Vec3 result = mCoordinates.ConvertToLocalRotation(eulerAngles.GetPsi(), eulerAngles.GetTheta(), eulerAngles.GetPhi());

      if (parameter.GetDataType() == dtDAL::DataType::VEC3)
      {
         //convert to x,y,z
         osg::Vec3 thisEulerAngle(result[1], result[2], result[0]);

         static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(thisEulerAngle);
         parameter.WriteToLog(*mLogger);
      }
      else if (parameter.GetDataType() == dtDAL::DataType::VEC3D)
      {
         //convert to x,y,z
         osg::Vec3d thisEulerAngle(result[1], result[2], result[0]);

         static_cast<dtGame::Vec3dMessageParameter&>(parameter).SetValue(thisEulerAngle);
         parameter.WriteToLog(*mLogger);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::CoordConvertVelocityVector(const VelocityVector& velocityVector,
            dtGame::MessageParameter& parameter) const
   {
      osg::Vec3 thisVector;

      thisVector =  mCoordinates.GetOriginRotationMatrix().preMult(velocityVector);

      if (mCoordinates.GetLocalCoordinateType()== dtUtil::LocalCoordinateType::GLOBE)
      {
         if (mCoordinates.GetIncomingCoordinateType() == dtUtil::IncomingCoordinateType::GEOCENTRIC)
         {
            thisVector[0] = (thisVector[0]/dtUtil::semiMajorAxis)*mCoordinates.GetGlobeRadius();
            thisVector[1] = (thisVector[1]/dtUtil::semiMajorAxis)*mCoordinates.GetGlobeRadius();
            thisVector[2] = (thisVector[2]/dtUtil::semiMajorAxis)*mCoordinates.GetGlobeRadius();
         }
         else
         {
            LOGN_ERROR("coordinates.cpp", "With local coordinates in globe mode, only GEOCENTRIC coordinates types are supported.");
         }
      }
      if (parameter.GetDataType() == dtDAL::DataType::VEC3)
      {
         static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(thisVector);
      }
      else if (parameter.GetDataType() == dtDAL::DataType::VEC3F)
      {
         static_cast<dtGame::Vec3fMessageParameter&>(parameter).SetValue(
            osg::Vec3f(thisVector.x(), thisVector.y(), thisVector.z()));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::CoordConvertAngularVelocityVector(const VelocityVector& velocityVector,
            dtGame::MessageParameter& parameter) const
   {
      if (parameter.GetDataType() == dtDAL::DataType::VEC3)
      {
         static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(velocityVector);
      }
      else if (parameter.GetDataType() == dtDAL::DataType::VEC3F)
      {
         static_cast<dtGame::Vec3fMessageParameter&>(parameter).SetValue(
            osg::Vec3f(velocityVector));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromSpatialToMessageParams(
      const char* buffer,
      const size_t maxSize,
      std::vector<dtCore::RefPtr<dtGame::MessageParameter> >& parameters,
      const OneToManyMapping& mapping) const
   {
      //If we have parameters, we are also guaranteed to have paramdefs, so there is no need
      //to check
      if (parameters.size() < 7U)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                             "Not enough MessageParameters were supplied(" +
                             dtUtil::ToString(parameters.size()) + ") for a SPATIAL type(7).");
         parameters.clear();
         return;
      }

      Spatial spatial;
      spatial.Decode(buffer, maxSize);

      if (maxSize == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "The parameter data could not be encoded into a \"%s\" struct.",
                             RPRAttributeType::SPATIAL_TYPE.GetName().c_str());
         return;
      }

      if (parameters[0].valid())
      {
         char drCode = spatial.GetDeadReckoningAlgorithm();
         SetIntegerValue(unsigned(drCode), *parameters[0], mapping, 0);
      }

      //is frozen
      if (parameters[1].valid())
      {
         dtGame::MessageParameter& frozenParam = *parameters[1];
         const dtDAL::DataType& dataType = mapping.GetParameterDefinitions()[1].GetGameType();

         if (dataType == dtDAL::DataType::BOOLEAN && frozenParam.GetDataType() == dataType)
         {
            static_cast<dtGame::BooleanMessageParameter&>(frozenParam).SetValue(spatial.IsFrozen());
         }
         else
         {
            unsigned frozenInt = spatial.IsFrozen() ? 1U: 0U;
            SetIntegerValue(frozenInt, frozenParam, mapping, 1);
         }
      }

      //world coordinate
      if (parameters[2].valid())
      {
         CoordConvertWorldCoord(spatial.GetWorldCoordinate(), *parameters[2]);
      }

      if (parameters[3].valid())
      {
         CoordConvertOrientation(spatial.GetOrientation(), *parameters[3]);
      }

      if (parameters[4].valid())
      {
         if (spatial.HasVelocity())
            CoordConvertVelocityVector(spatial.GetVelocity(), *parameters[4]);
         else
            parameters[4] = NULL;
      }

      if (parameters[5].valid())
      {
         if (spatial.HasAcceleration())
            CoordConvertVelocityVector(spatial.GetAcceleration(), *parameters[5]);
         else
            parameters[5] = NULL;
      }

      if (parameters[6].valid())
      {
         if (spatial.HasAngularVelocity())
            CoordConvertAngularVelocityVector(spatial.GetAngularVelocity(), *parameters[6]);
         else
            parameters[6] = NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromWorldCoordToMessageParam(
      const char* buffer, 
      const size_t size,
      dtGame::MessageParameter& parameter) const
   {
      WorldCoordinate wc;
      wc.Decode(buffer);

      if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
         "World coordinate has been decoded to %lf %lf %lf", wc.GetX(), wc.GetY(), wc.GetZ());

      CoordConvertWorldCoord(wc, parameter);
      
   }

   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromEulerAnglesToMessageParam(
      const char* buffer, 
      const size_t size,
      dtGame::MessageParameter& parameter) const
   {
      EulerAngles eulerAngles;

      eulerAngles.Decode(buffer);

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "The incoming euler angles are %lf %lf %lf",
            eulerAngles.GetPsi(), eulerAngles.GetTheta(), eulerAngles.GetPhi());
            
      CoordConvertOrientation(eulerAngles, parameter);
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromVelocityVectorToMessageParam(
      const char* buffer, 
      const size_t size,
      dtGame::MessageParameter& parameter) const
   {
      // USED FOR ACCELERATION VECTOR, AND VELOCITY VECTOR
      VelocityVector velocityVector;
      velocityVector.Decode(buffer);

      CoordConvertVelocityVector(velocityVector, parameter);
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromAngularVelocityVectorToMessageParam(
      const char* buffer, 
      const size_t size,
      dtGame::MessageParameter& parameter) const
   {
      // Angular ACCELERATION VECTOR
      VelocityVector velocityVector;
      velocityVector.Decode(buffer);

      CoordConvertAngularVelocityVector(velocityVector, parameter);
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string RPRParameterTranslator::GetEnumValue(const std::string& value, 
      const OneToManyMapping::ParameterDefinition& paramDef, bool returnGameValue) const
   {
      std::string mappedValue;
      bool found = false;

      if (returnGameValue)
         found = paramDef.GetGameEnumerationValue(value, mappedValue);
      else
         found = paramDef.GetHLAEnumerationValue(value, mappedValue);

      if (found)
      {
         return mappedValue;
      }
      else
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "No mapping was specified for value \"%s\" for an enumeration on mapping for game parameter \"%s\". "
               "Using game default value \"%s\".", 
               value.c_str(), 
               paramDef.GetGameName().c_str(), 
               paramDef.GetDefaultValue().c_str());
         }

         if (returnGameValue)
         {
             return paramDef.GetDefaultValue();
         }
         else if (paramDef.GetHLAEnumerationValue(paramDef.GetDefaultValue(), mappedValue))
         {
            return mappedValue;
         }
      
         //so we want the HLA Value, but no value was mapped to the game default value.
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "No mapping to HLA was specified for the default value for an enumeration on mapping for game parameter \"%s\". "
            "That is, the game default value is \"%s\", but no HLA value is mapped to that default", 
            paramDef.GetGameName().c_str(), 
            paramDef.GetDefaultValue().c_str());
         
         return "";
      }
   }

   //////////////////////////c///////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromCharArrayToStringParam(
         const char* buffer, const size_t size, 
         dtGame::StringMessageParameter& parameter,
         const OneToManyMapping::ParameterDefinition& paramDef) const
   {
      std::string value;
      for (unsigned i = 0; i < size; ++i)
      {
         char c = buffer[i];
         if (c == '\0')
            break;
         value.append(1, c);
      }
      
      if (parameter.GetDataType() == dtDAL::DataType::ENUMERATION)
         value = GetEnumValue(value, paramDef, true);
      
      parameter.SetValue(value);
   }

   /////////////////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromParamToArticulations(
      char* buffer, 
      size_t& maxSize, 
      const dtGame::MessageParameter& parameter,
      const OneToManyMapping::ParameterDefinition& paramDef) const
   {
      // Retain the maximum buffer size and set the current size (maxSize)
      // to 0 so it can be incremented and returned. This out parameter
      // is the number of bytes written to the buffer.
      const size_t bufferMaxSize = maxSize;
      maxSize = 0;

      unsigned bufferOffset = 0;
      const dtGame::GroupMessageParameter* gParams = (dynamic_cast<const dtGame::GroupMessageParameter*>(&parameter));
      const dtGame::GroupMessageParameter* curGroupParam = NULL;
      std::vector<unsigned> parentClassIds;
      std::vector<ArticulatedParameter> articulatedParams;

      std::vector<const dtGame::MessageParameter*> groupParamsList;
      gParams->GetParameters( groupParamsList );
      unsigned paramCount = groupParamsList.size();

      for( unsigned i = 0; i < paramCount; ++i )
      {
         // Reference the sub group that could be attach parts or articulate parts.
         curGroupParam = static_cast<const dtGame::GroupMessageParameter*> (groupParamsList[i]);

         if( curGroupParam == NULL )
         { 
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Outgoing articulation parameter does not have a valid grouping of sub parameters." );
            continue;
         }

         // Get the name of the sub group that will determine the group type.
         const std::string& groupName = curGroupParam->GetName();

         // Determine if name is not long enough for comparison.
         if( groupName.size() < 2 )
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Outgoing articulation parameter does not have a long enough name for comparison." );
            continue;
         }

         // Capture message parameters in a new articulated parameter to be 
         // written to the buffer.
         ArticulatedParameter curArtParam;

         // Get the ArticulatedParameterChange value
         const dtGame::UnsignedIntMessageParameter* changeParam
            = static_cast<const dtGame::UnsignedIntMessageParameter*> 
            (curGroupParam->GetParameter("Change"));

         if( changeParam != NULL )
         {
            curArtParam.SetArticulatedParameterChange( changeParam->GetValue() );
         }

         // Ensure that the class id list will be the same length as the final
         // articulation parameters output list.
         parentClassIds.push_back( 0 );

         ParameterValue& curParamValue = curArtParam.GetParameterValue();

         // Fill in Attached Parts
         if( groupName[1] == 't' ) // "AttachedPartMessageParam"
         {
            curParamValue.SetArticulatedParameterType( AttachedPart ); // 1

            AttachedParts& attachParts = curParamValue.GetAttachedParts();

            // Get the station
            const dtGame::UnsignedIntMessageParameter* stationParam
               = static_cast<const dtGame::UnsignedIntMessageParameter*> 
               (curGroupParam->GetParameter("Station"));

            if( stationParam != NULL )
            {
               attachParts.SetStation( stationParam->GetValue() );
            }

            // Get the dis info
            const dtGame::EnumMessageParameter* disParam 
               = static_cast<const dtGame::EnumMessageParameter*>
               (curGroupParam->GetParameter("DISInfo")); // Enum Param
            
            if( disParam != NULL )
            {
               const std::string& value = GetEnumValue( disParam->GetValue(), paramDef, false );
               std::vector<std::string> tokens;
               dtUtil::StringTokenizer<dtUtil::IsSpace>::tokenize( tokens, value );

               EntityType dis;
               switch( tokens.size() )
               {
               case 7: dis.SetExtra( (unsigned char) dtUtil::ToUnsignedInt(tokens[6]) );
               case 6: dis.SetSpecific( (unsigned char) dtUtil::ToUnsignedInt(tokens[5]) );
               case 5: dis.SetSubcategory( (unsigned char) dtUtil::ToUnsignedInt(tokens[4]) );
               case 4: dis.SetCategory( (unsigned char) dtUtil::ToUnsignedInt(tokens[3]) );
               case 3: dis.SetCountry( (unsigned short) dtUtil::ToUnsignedInt(tokens[2]) );
               case 2: dis.SetDomain( (unsigned char) dtUtil::ToUnsignedInt(tokens[1]) );
               case 1: dis.SetKind( (unsigned char) dtUtil::ToUnsignedInt(tokens[0]) );
               default:
                  break;
               }

               attachParts.SetStoreType( dis );
            }



            // Get the attach parent; this will be a class id (unsigned int when mapped from the contained name)
            const dtGame::StringMessageParameter* attachToParam
               = static_cast<const dtGame::EnumMessageParameter*>
               (curGroupParam->GetParameter("OurParent"));

            if( attachToParam != NULL )
            {
               // Map from the parent name to a class id.
               const std::string& classValue = GetEnumValue( attachToParam->GetValue(), paramDef, false );

               // Add the class id to a list so that it can be
               // referenced by and assigned as an index relative
               // to the articulation parameters array.
               parentClassIds[i] = dtUtil::ToUnsignedInt( classValue );
            }
         }
         // Fill in Articulated Parts
         else if( groupName[1] == 'r' ) // "ArticulatedPartMessageParam"
         {
            curParamValue.SetArticulatedParameterType( ArticulatedPart ); // 0

            // Get a reference to the parts object to be filled
            ArticulatedParts& artParts = curParamValue.GetArticulatedParts();

            // Prepare loop variables
            const dtDAL::NamedParameter* curNamedParam = NULL;
            std::vector<const dtDAL::NamedParameter*> params;
            curGroupParam->GetParameters( params );
            std::vector<const dtDAL::NamedParameter*>::iterator paramIter = params.begin();

            // Collect the articulated value. The loop avoids the overhead of
            // searching over several names possible for the float parameter.
            // There should only be three parameters in this group, so the loop
            // will be very short and the name of the parameter will be
            // accessed faster.
            for( ; paramIter != params.end(); ++paramIter )
            {
               curNamedParam = *paramIter;

               if( curNamedParam == NULL ) { continue; }

               // Get the Class
               if( curNamedParam->GetName() == "OurName" )
               {
                  const dtGame::StringMessageParameter* classParam 
                     = static_cast<const dtGame::StringMessageParameter*> (curNamedParam); // Enum Param

                  // Capture the class value
                  if( classParam != NULL )
                  {
                     const std::string& classValue = GetEnumValue( classParam->GetValue(), paramDef, false );
                     artParts.SetClass( dtUtil::ToUnsignedInt( classValue ) );
                  }
               }
               else if( curNamedParam->GetName() == "OurParent" )
               {
                  // Get the attach parent; this will be a class id (unsigned int when mapped from the contained name)
                  const dtGame::StringMessageParameter* attachToParam
                     = static_cast<const dtGame::StringMessageParameter*> (curNamedParam);

                  if( attachToParam != NULL )
                  {
                     // Map from the parent name to a class id.
                     const std::string& classValue = GetEnumValue( attachToParam->GetValue(), paramDef, false );

                     // Add the class id to a list so that it can be
                     // referenced by and assigned as an index relative
                     // to the articulation parameters array.
                     parentClassIds[i] = dtUtil::ToUnsignedInt( classValue );
                  }
               }
               // Get the value
               else if( curNamedParam->GetName() != "Change" ) // Change is the only other parameter (already captured)
               {
                  const dtGame::FloatMessageParameter* floatParam 
                     = dynamic_cast<const dtGame::FloatMessageParameter*> (curNamedParam);

                  // Capture the float value
                  if( floatParam != NULL )
                  {
                     artParts.SetValue( floatParam->GetValue() );

                     const std::string& paramName = curNamedParam->GetName();

                     // Capture the metric type via the name of the parameter.
                     // For now, this linear comparison is unavoidable.
                     //(Enumeration (Enumerator "Position")         (Representation 1))
                     if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_POSITION )
                     { artParts.SetTypeMetric( 1 ); }

                     //(Enumeration (Enumerator "PositionRate")     (Representation 2))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_POSITIONRATE )
                     { artParts.SetTypeMetric( 2 ); }
                     
                     //(Enumeration (Enumerator "Extension")        (Representation 3))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_EXTENSION )
                     { artParts.SetTypeMetric( 3 ); }
                     
                     //(Enumeration (Enumerator "ExtensionRate")    (Representation 4))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_EXTENSIONRATE )
                     { artParts.SetTypeMetric( 4 ); }
                     
                     //(Enumeration (Enumerator "X")                (Representation 5))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_X )
                     { artParts.SetTypeMetric( 5 ); }
                     
                     //(Enumeration (Enumerator "XRate")            (Representation 6))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_XRATE )
                     { artParts.SetTypeMetric( 6 ); }
                     
                     //(Enumeration (Enumerator "Y")                (Representation 7))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_Y )
                     { artParts.SetTypeMetric( 7 ); }
                     
                     //(Enumeration (Enumerator "YRate")            (Representation 8))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_YRATE )
                     { artParts.SetTypeMetric( 8 ); }
                     
                     //(Enumeration (Enumerator "Z")                (Representation 9))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_Z )
                     { artParts.SetTypeMetric( 9 ); }
                     
                     //(Enumeration (Enumerator "ZRate")            (Representation 10))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ZRATE )
                     { artParts.SetTypeMetric( 10 ); }
                     
                     //(Enumeration (Enumerator "Azimuth")          (Representation 11))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_AZIMUTH )
                     { artParts.SetTypeMetric( 11 ); }
                     
                     //(Enumeration (Enumerator "AzimuthRate")      (Representation 12))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_AZIMUTHRATE )
                     { artParts.SetTypeMetric( 12 ); }
                     
                     //(Enumeration (Enumerator "Elevation")        (Representation 13))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ELEVATION )
                     { artParts.SetTypeMetric( 13 ); }
                     
                     //(Enumeration (Enumerator "ElevationRate")    (Representation 14))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ELEVATIONRATE )
                     { artParts.SetTypeMetric( 14 ); }
                     
                     //(Enumeration (Enumerator "Rotation")         (Representation 15))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ROTATION )
                     { artParts.SetTypeMetric( 15 ); }
                     
                     //(Enumeration (Enumerator "RotationRate")     (Representation 16))
                     else if( paramName == dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ROTATIONRATE )
                     { artParts.SetTypeMetric( 16 ); }
                  }
                  
               }
            }
         }

         // Store the newly collected values of the current articulated parameter.
         articulatedParams.push_back( curArtParam );
      }

      
      // Go over each parameter, referencing its attach parent by index
      // and then write it to the buffer.
      ArticulatedParameter* curArtParam = NULL;
      paramCount = articulatedParams.size();
      for( unsigned i = 0; i < paramCount; ++i )
      {
         curArtParam = &articulatedParams[i];

         // Ensure another articulate parameter will fit the buffer
         if( bufferOffset + curArtParam->EncodedLength() > bufferMaxSize )
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "HLA buffer is not large enough to write all outgoing articulation parameters. %i of %i articulations have been written to the buffer.",
               i, paramCount );
            break;
         }

         // Assign the index of the articulate parameter that 
         // is the parent of this articulate parameter (matched on class id).
         for( unsigned j = 0; j < paramCount; ++j )
         {
            if( articulatedParams[j].GetParameterValue().GetArticulatedParameterType() == ArticulatedPart
               && articulatedParams[j].GetParameterValue().GetArticulatedParts().GetClass() == parentClassIds[i] )
            {
               curArtParam->SetPartAttachedTo( j );
               break;
            }
         }

         // Write the articulation to the buffer
         curArtParam->Encode( &buffer[bufferOffset] );
         bufferOffset += curArtParam->EncodedLength();
         maxSize += curArtParam->EncodedLength();
      }
   }

   
   /////////////////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapToMessageParameters(const char* buffer, size_t size,
      std::vector<dtCore::RefPtr<dtGame::MessageParameter> >& parameters, const OneToManyMapping& mapping) const
   {
      const AttributeType& hlaType = mapping.GetHLAType();

      if (parameters.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
            "Unable to map from HLA mapping %s because no message parameters were passed into the mapping method.",
            mapping.GetHLAName().c_str());
         return;
      }

      if (mapping.GetParameterDefinitions().empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
            "Unable to map from HLA mapping %s because no parameter mappings are added to the mapping object.",
            mapping.GetHLAName().c_str());
         return;
      }

      //All the current mappings use only one parameter.
      dtGame::MessageParameter& parameter = *parameters[0];
      const OneToManyMapping::ParameterDefinition& paramDef = mapping.GetParameterDefinitions()[0];

      const dtDAL::DataType& parameterDataType = parameter.GetDataType();

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Mapping values from HLA mapping %s to game mapping %s",
            mapping.GetHLAName().c_str(), paramDef.GetGameName().c_str());

      if (hlaType == RPRAttributeType::SPATIAL_TYPE)
      {
         MapFromSpatialToMessageParams( buffer, size, parameters, mapping );
      }
      else if (hlaType == RPRAttributeType::WORLD_COORDINATE_TYPE)
      {
         MapFromWorldCoordToMessageParam( buffer, size, parameter );
      }
      else if (hlaType == RPRAttributeType::EULER_ANGLES_TYPE)
      {
         MapFromEulerAnglesToMessageParam( buffer, size, parameter );
      }
	   else if (hlaType == RPRAttributeType::ANGULAR_VELOCITY_VECTOR_TYPE)
      {
         MapFromAngularVelocityVectorToMessageParam( buffer, size, parameter );
      }
      else if (hlaType == RPRAttributeType::VELOCITY_VECTOR_TYPE)
      {
         MapFromVelocityVectorToMessageParam( buffer, size, parameter );
      }
      else if (hlaType == RPRAttributeType::DOUBLE_TYPE)
      {
         double value = *(double*)(&buffer[0]);

         if (osg::getCpuByteOrder() == osg::LittleEndian)
         {
            osg::swapBytes((char*)(&value), sizeof(double));
         }

         if (parameterDataType == dtDAL::DataType::DOUBLE)
         {
            static_cast<dtGame::DoubleMessageParameter&>(parameter).SetValue(value);
         }
         else if (parameterDataType == dtDAL::DataType::FLOAT)
         {
            static_cast<dtGame::FloatMessageParameter&>(parameter).SetValue(float(value));
         }

      }
      else if (hlaType == RPRAttributeType::FLOAT_TYPE)
      {
         float value = *(float*)(&buffer[0]);

         if (osg::getCpuByteOrder() == osg::LittleEndian)
         {
            osg::swapBytes((char*)(&value), sizeof(float));
         }

         if (parameterDataType == dtDAL::DataType::DOUBLE)
         {
            static_cast<dtGame::DoubleMessageParameter&>(parameter).SetValue(double(value));
         }
         else if (parameterDataType == dtDAL::DataType::FLOAT)
         {
            static_cast<dtGame::FloatMessageParameter&>(parameter).SetValue(value);
         }
      }
      else if (hlaType == RPRAttributeType::UNSIGNED_INT_TYPE)
      {
         unsigned int value = *(unsigned int*)(&buffer[0]);

         if (osg::getCpuByteOrder() == osg::LittleEndian)
         {
            osg::swapBytes((char*)(&value), sizeof(unsigned int));
         }

         SetIntegerValue(unsigned (value), parameter, mapping, 0);
      }
      else if (hlaType == RPRAttributeType::UNSIGNED_SHORT_TYPE)
      {
         unsigned short value = *(unsigned short*)(&buffer[0]);

         if (osg::getCpuByteOrder() == osg::LittleEndian)
           osg::swapBytes((char*)(&value), sizeof(unsigned short));

         SetIntegerValue(unsigned (value), parameter, mapping, 0);
      }
      else if (hlaType == RPRAttributeType::UNSIGNED_CHAR_TYPE)
      {
         unsigned char value = *(unsigned char*)(&buffer[0]);
         SetIntegerValue(unsigned (value), parameter, mapping, 0);
      }
      else if (hlaType == RPRAttributeType::EVENT_IDENTIFIER_TYPE)
      {
         EventIdentifier eventIdentifier;
         eventIdentifier.Decode(buffer);
      }
      else if (hlaType == RPRAttributeType::ENTITY_TYPE)
      {
         // Convert to Entity Type from either an Enum or a String.
         if( parameterDataType == dtDAL::DataType::ENUMERATION
            || parameterDataType == dtDAL::DataType::STRING )
         {
            // Since this is a valid type, convert the buffer to
            // an Entity Type object.
            EntityType entityType;
            entityType.Decode(buffer);

            std::ostringstream stringValue;
            //this current code only allows for exact matches and a default.
            stringValue << entityType;

            // Write the value to the property
            if( parameterDataType == dtDAL::DataType::ENUMERATION )
            {
               std::string mappedValue = GetEnumValue(stringValue.str(), paramDef, true);
               static_cast<dtGame::EnumMessageParameter&>(parameter).SetValue(mappedValue);  
            }
            else // STRING
            {
               static_cast<dtGame::StringMessageParameter&>(parameter).SetValue(stringValue.str());  
            }
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map HLA type \"%s\" to \"%s\"",
               RPRAttributeType::ENTITY_TYPE.GetName().c_str(),
               parameterDataType.GetName().c_str());
         }
      }
      else if (hlaType == RPRAttributeType::STRING_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::STRING ||
             parameterDataType == dtDAL::DataType::ENUMERATION)
         {
            MapFromCharArrayToStringParam(buffer, size, 
                  static_cast<dtGame::StringMessageParameter&>(parameter),
                  paramDef);
         }
         else if (parameterDataType == dtDAL::DataType::ACTOR)
         {
            std::string value;
            for (unsigned i = 0; i < size; ++i)
            {
               char c = buffer[i];
               if (c == '\0')
                  break;
               value.append(1, c);
            }
            //}
            static_cast<dtGame::ActorMessageParameter&>(parameter).SetValue(dtCore::UniqueId(value));
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map HLA type \"%s\" to \"%s\"",
               RPRAttributeType::STRING_TYPE.GetName().c_str(),
               parameterDataType.GetName().c_str());
         }
      }
      else if (hlaType == RPRAttributeType::MARKING_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::STRING)
         {
            std::string markingText;
            CopyBufferToMarkingText(buffer, markingText, RPRAttributeType::MARKING_TYPE.GetEncodedLength());

            static_cast<dtGame::StringMessageParameter&>(parameter).SetValue(markingText);
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map HLA type \"%s\" to \"%s\"",
               RPRAttributeType::MARKING_TYPE.GetName().c_str(),
               parameterDataType.GetName().c_str());
         }
      }
      else if (hlaType == RPRAttributeType::MARKING_TYPE_32)
      {
         if (parameterDataType == dtDAL::DataType::STRING)
         {
            std::string markingText;
            CopyBufferToMarkingText(buffer, markingText, RPRAttributeType::MARKING_TYPE_32.GetEncodedLength());

            static_cast<dtGame::StringMessageParameter&>(parameter).SetValue(markingText);
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map HLA type \"%s\" to \"%s\"",
               RPRAttributeType::MARKING_TYPE_32.GetName().c_str(),
               parameterDataType.GetName().c_str());
         }
      }
      else if (hlaType == RPRAttributeType::ENTITY_IDENTIFIER_TYPE)
      {
         EntityIdentifier eid;
         eid.Decode(buffer);
         if (parameterDataType == dtDAL::DataType::ACTOR)
         {
            const dtCore::UniqueId* oid = mRuntimeMappings.GetId(eid);
            if (oid != NULL)
               static_cast<dtGame::ActorMessageParameter&>(parameter).SetValue(*oid);
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map HLA type \"%s\" to \"%s\"",
               RPRAttributeType::ENTITY_IDENTIFIER_TYPE.GetName().c_str(),
               parameterDataType.GetName().c_str());
         }
      }
      else if (hlaType == RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::ACTOR)
         {
            std::string value;
            for (unsigned i = 0; i < size; ++i)
            {
               char c = buffer[i];
               if (c == '\0')
                  break;
               value.append(1, c);
            }

            // Get the actor id mapped to the RTI id
            const dtCore::UniqueId* actorId = mRuntimeMappings.GetIdByRTIId(value);

            // Set the actor id value
            static_cast<dtGame::ActorMessageParameter&>(parameter)
               .SetValue( actorId != NULL ? *actorId : dtCore::UniqueId(""));
         }
         else if (parameterDataType == dtDAL::DataType::STRING ||
               parameterDataType == dtDAL::DataType::ENUMERATION)
         {
            MapFromCharArrayToStringParam(buffer, size, 
                  static_cast<dtGame::StringMessageParameter&>(parameter), paramDef);
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map HLA type \"%s\" to \"%s\"",
               RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE.GetName().c_str(),
               parameterDataType.GetName().c_str());
         }
      }
      else if (hlaType == RPRAttributeType::ARTICULATED_PART_TYPE)
      {
         MapFromArticulationsToMessageParam(buffer, size, parameter, parameterDataType, paramDef);
      }
      else
      {
         //This could only happen if someone defined a new Attribute type but didn't write
         //code to handle it.
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Unhandled attribute type \"%s\"",
                             hlaType.GetName().c_str());
      }
      parameter.WriteToLog(*mLogger);
   }

   /////////////////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::SetIntegerValue(unsigned value, dtGame::MessageParameter& parameter, 
      const OneToManyMapping& mapping, unsigned parameterDefIndex) const
   {
      const dtDAL::DataType& parameterDataType = parameter.GetDataType();
      const OneToManyMapping::ParameterDefinition& paramDef = mapping.GetParameterDefinitions()[parameterDefIndex];

      parameter.WriteToLog(*mLogger);
      if (parameterDataType == dtDAL::DataType::UINT)
      {
         static_cast<dtGame::UnsignedIntMessageParameter&>(parameter).SetValue(unsigned(value));
      }
      else if (parameterDataType == dtDAL::DataType::ULONGINT)
      {
         static_cast<dtGame::UnsignedLongIntMessageParameter&>(parameter).SetValue((unsigned long)(value));
      }
      else if (parameterDataType == dtDAL::DataType::USHORTINT)
      {
         static_cast<dtGame::UnsignedShortIntMessageParameter&>(parameter).SetValue((unsigned short)(value));
      }
      else if (parameterDataType == dtDAL::DataType::UCHAR)
      {
         static_cast<dtGame::UnsignedCharMessageParameter&>(parameter).SetValue((unsigned char)(value));
      }
      else if (parameterDataType == dtDAL::DataType::INT)
      {
         static_cast<dtGame::IntMessageParameter&>(parameter).SetValue(int(value));
      }
      else if (parameterDataType == dtDAL::DataType::LONGINT)
      {
         static_cast<dtGame::LongIntMessageParameter&>(parameter).SetValue(long(value));
      }
      else if (parameterDataType == dtDAL::DataType::SHORTINT)
      {
         static_cast<dtGame::ShortIntMessageParameter&>(parameter).SetValue(short(value));
      }
      else if (parameterDataType == dtDAL::DataType::BOOLEAN)
      {
         static_cast<dtGame::BooleanMessageParameter&>(parameter).SetValue(value != 0);
      }
      else if (parameterDataType == dtDAL::DataType::ENUMERATION
         || parameterDataType == dtDAL::DataType::STRING
         || parameterDataType.IsResource() )
      {
         std::string mappedValue;
         std::ostringstream stringValue;
         
         stringValue << unsigned(value);
         
         mappedValue = GetEnumValue(stringValue.str(), paramDef, true);

         parameter.FromString(mappedValue);
         parameter.WriteToLog(*mLogger);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////////////
   unsigned RPRParameterTranslator::GetIntegerValue(const dtGame::MessageParameter& parameter, 
      const OneToManyMapping& mapping, unsigned parameterDefIndex) const
   {
      const dtDAL::DataType& parameterDataType = parameter.GetDataType();
      const OneToManyMapping::ParameterDefinition& paramDef = mapping.GetParameterDefinitions()[parameterDefIndex];

      parameter.WriteToLog(*mLogger);

      if (parameterDataType == dtDAL::DataType::UINT)
      {
         return unsigned(static_cast<const dtGame::UnsignedIntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::ULONGINT)
      {
         return unsigned(static_cast<const dtGame::UnsignedLongIntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::USHORTINT)
      {
         return unsigned(static_cast<const dtGame::UnsignedShortIntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::UCHAR)
      {
         return unsigned(static_cast<const dtGame::UnsignedCharMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::INT)
      {
         return unsigned(static_cast<const dtGame::IntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::LONGINT)
      {
         return unsigned(static_cast<const dtGame::LongIntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::SHORTINT)
      {
         return unsigned(static_cast<const dtGame::ShortIntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::BOOLEAN)
      {
         return unsigned(static_cast<const dtGame::BooleanMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::ENUMERATION
         || parameterDataType == dtDAL::DataType::STRING
         || parameterDataType.IsResource() )
      {
         std::string valueAsString;
         const std::string& msgParamValue = parameter.ToString();
         valueAsString = GetEnumValue(msgParamValue, paramDef, false);

         unsigned value = 0;
         std::istringstream iss;
         iss.str(valueAsString);
         iss >> value;
         return value;
      }
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
         "Unhandled conversion for an enumeration on mapping of \"%s\" to \"%s\". "
         "The datatypes should have been logged above.",
         paramDef.GetGameName().c_str(), mapping.GetHLAName().c_str());

      return 0L;
   }



   
   /////////////////////////////////////////////////////////////////////////////////////////
   void RPRParameterTranslator::MapFromArticulationsToMessageParam(
      const char* buffer, 
      const size_t size,
      dtGame::MessageParameter& parameter,
      const dtDAL::DataType& parameterDataType,
      const OneToManyMapping::ParameterDefinition& paramDef) const
   {
      //Create articulated part
      std::vector<ArticulatedParameter> artParams;
      ArticulatedParameter artParam;
      artParam.Decode(buffer);
      artParams.push_back(artParam);
      int amount = (int)(size / artParam.EncodedLength());

      /////////////////////////////////////////////////////////////////////////////////////////
      // ERROR CHECKING TO MAKE SURE SIZE IS CORRECT
      if(size % artParam.EncodedLength() != 0)
      {
         // Log error they sent us bad data 
         if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Received inaccurate data from hla in the articulated parts area, size is %d",
            (int)size);
      }
      else if(amount == 0)
      {
         if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Received inaccurate data from hla in the articulated parts area, amount is 0");
      }

      /////////////////////////////////////////////////////////////////////////////////////////
      // Add all the data onto the vector
      for(int i = 1; i < amount; i++)
      {
         ArticulatedParameter tempparam;
         const char* tempString =  &buffer[i * artParam.EncodedLength()];
         tempparam.Decode(tempString);
         artParams.push_back(tempparam);
      }

      /////////////////////////////////////////////////////////////////////////////////////////
      // Error Check values
      std::vector<ArticulatedParameter>::iterator paramsIter;
      const ParameterValue* curParamValue = NULL;
      for(paramsIter = artParams.begin(); paramsIter != artParams.end(); ++paramsIter)
      {
         curParamValue = &(*paramsIter).GetParameterValue();
         // attached part check
         if((int)curParamValue->GetArticulatedParameterType() == 1)
         {
            if(curParamValue->GetAttachedParts().GetStation() == 0)
            {
               if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Received inaccurate data from hla in the articulated parts area, Station ID is = %d", 
                  (unsigned int)(*paramsIter).GetParameterValue().GetAttachedParts().GetStation());
            }
         }
         else // articulation part check
         {
            if((unsigned int)curParamValue->GetArticulatedParts().GetClass() == 0)
            {
               if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Received inaccurate data from hla in the articulated parts area, Unknown Class type = %d", 
                  (unsigned int)curParamValue->GetArticulatedParts().GetClass());
            }
            if((unsigned int)curParamValue->GetArticulatedParts().GetTypeMetric() == 0 
               || (unsigned int)curParamValue->GetArticulatedParts().GetTypeMetric() > 16)
            {
               if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Received inaccurate data from hla in the articulated parts area, Unknown Metric type = %d", 
                  (unsigned int)curParamValue->GetArticulatedParts().GetTypeMetric());
            }
         }
      }

      /////////////////////////////////////////////////////////////////////////////////////////
      // fill in all info to a group message parameter
      dtGame::GroupMessageParameter* gParams = (dynamic_cast<dtGame::GroupMessageParameter*>(&parameter));
      int j = 0;
      int k = 0;
      for(paramsIter = artParams.begin(); paramsIter != artParams.end(); ++paramsIter)
      {
         curParamValue = &(*paramsIter).GetParameterValue();
         char buffer[64];
         if((int)curParamValue->GetArticulatedParameterType() == 1)
         {
            sprintf(buffer, "AttachedPartMessageParam%d", k);
            k++;

            dtCore::RefPtr<dtGame::GroupMessageParameter> newGroupParam = new dtGame::GroupMessageParameter(buffer);

            // fill in with all messages we need to send for this data
            newGroupParam->AddParameter( *new dtGame::UnsignedIntMessageParameter(
               "Station", (unsigned int)curParamValue->GetAttachedParts().GetStation()) );

            std::ostringstream disStream;
            disStream << (curParamValue->GetAttachedParts().GetStoreType());
            std::string finalEnumString = GetEnumValue(disStream.str(), paramDef, true);
            newGroupParam->AddParameter( *new dtGame::EnumMessageParameter("DISInfo", finalEnumString.c_str()) );

            // we're done add to big group
            gParams->AddParameter( *newGroupParam );
         }
         else
         {
            sprintf(buffer, "ArticulatedPartMessageParam%d", j);
            j++;

            dtCore::RefPtr<dtGame::GroupMessageParameter> newGroupParam = new dtGame::GroupMessageParameter(buffer);

            // fill in with all messages we need to send for this data
            std::ostringstream classEnumValue;
            classEnumValue << (unsigned int)curParamValue->GetArticulatedParts().GetClass();
            std::string dofName = GetEnumValue(classEnumValue.str(), paramDef, true);

            // WHat dof are we, which one we moving.
            newGroupParam->AddParameter( *new dtGame::StringMessageParameter("OurName", dofName.c_str()) );

            // what is our parent that we are attached to.
            std::vector<ArticulatedParameter>::iterator paramsIterTwo;
            int l = 0;
            std::string parentDofName = "";
            for(paramsIterTwo = artParams.begin(); paramsIterTwo != artParams.end(); ++paramsIterTwo)
            {
               if((unsigned short)(*paramsIter).GetPartAttachedTo() == 0)
               {
                  // change to read from enum???
                  newGroupParam->AddParameter( *new dtGame::StringMessageParameter("OurParent", "dof_chasis") );
                  break;
               }
               else if((unsigned short)(*paramsIter).GetPartAttachedTo() == l)
               {
                  std::ostringstream parentClassEnumValue;
                  parentClassEnumValue << (unsigned int)curParamValue->GetArticulatedParts().GetClass();
                  parentDofName = GetEnumValue(parentClassEnumValue.str(), paramDef, true);
                  newGroupParam->AddParameter( *new dtGame::StringMessageParameter("OurParent", parentDofName.c_str()) );
                  break;
               }
               l++;
            }

            float value = (float)curParamValue->GetArticulatedParts().GetValue();

            switch((unsigned int)curParamValue->GetArticulatedParts().GetTypeMetric())
            {
               //(Enumeration (Enumerator "Position")         (Representation 1))
            case 1:
               {
                  newGroupParam->AddParameter( 
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_POSITION, value) );
               }
               break;

               //(Enumeration (Enumerator "PositionRate")     (Representation 2))
            case 2:
               {
                  newGroupParam->AddParameter( 
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_POSITIONRATE, value) );
               }
               break;

               //(Enumeration (Enumerator "Extension")        (Representation 3))
            case 3:
               {
                  newGroupParam->AddParameter( 
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_EXTENSION, value) );
               }
               break;

               //(Enumeration (Enumerator "ExtensionRate")    (Representation 4))
            case 4:
               {
                  newGroupParam->AddParameter( 
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_EXTENSIONRATE, value) );
               }
               break;

               //(Enumeration (Enumerator "X")                (Representation 5))
            case 5:
               {
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter(  dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_X, value) );
               }
               break;

               //(Enumeration (Enumerator "XRate")            (Representation 6))
            case 6:
               {
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_XRATE, value) );
               }
               break;

               //(Enumeration (Enumerator "Y")                (Representation 7))
            case 7:
               {
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_Y, value) );
               }
               break;

               //(Enumeration (Enumerator "YRate")            (Representation 8))
            case 8:
               {
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_YRATE, value) );
               }
               break;

               //(Enumeration (Enumerator "Z")                (Representation 9))
            case 9:
               {
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_Z, value) );
               }
               break;

               //(Enumeration (Enumerator "ZRate")            (Representation 10))
            case 10:
               {
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ZRATE, value) );
               }
               break;

               //(Enumeration (Enumerator "Azimuth")          (Representation 11))
            case 11:
               {
                  float tempValue = -value;
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_AZIMUTH, tempValue) );
               }
               break;

               //(Enumeration (Enumerator "AzimuthRate")      (Representation 12))
            case 12:
               {
                  float tempValue = -value;
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_AZIMUTHRATE, tempValue) );
               }
               break;

               //(Enumeration (Enumerator "Elevation")        (Representation 13))
            case 13:
               {
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ELEVATION, value) );
               }
               break;

               //(Enumeration (Enumerator "ElevationRate")    (Representation 14))
            case 14:
               {
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ELEVATIONRATE, value) );
               }
               break;

               //(Enumeration (Enumerator "Rotation")         (Representation 15))
            case 15:
               {
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ROTATION, value) );
               }
               break;

               //(Enumeration (Enumerator "RotationRate")     (Representation 16))
            case 16:
               {
                  newGroupParam->AddParameter(
                     *new dtGame::FloatMessageParameter( dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ROTATIONRATE, value) );
               }
               break;

            default: // log error
               {
                  // Logged above in error check there.
               }
               break;
            }

            // we're done add to big group
            gParams->AddParameter( *newGroupParam );
         }          
      }
   }

   //static
   void RPRParameterTranslator::CopyMarkingTextToBuffer( const std::string &markingText, 
                                                         char *buffer, size_t numChars )
   {
      //1 is ASCII
      buffer[0] = 1;
      for (unsigned int i = 1; i < numChars; ++i)
      {
         if (i <= markingText.size())
            buffer[i] = markingText[i-1];
         else
            buffer[i] = '\0';
      }
   }

   //static
   void RPRParameterTranslator::CopyBufferToMarkingText( const char *buffer, 
                                                         std::string &markingText,
                                                         size_t numChars )
   {
      //skip the 0'th character (assuming its a '1' which denotes "ascii")
      for (unsigned int i = 1; i < numChars; ++i)
      {
         char c = buffer[i];
         if (c == '\0')
            break;
         markingText.append(1, c);
      }
   }
}

