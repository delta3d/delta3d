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
 * @author David Guthrie
 */

#include <osg/Endian>
#include <string.h>

#include <dtUtil/coordinates.h>
#include <dtUtil/log.h>

#include <dtCore/uniqueid.h>

#include <dtGame/messageparameter.h>

#include "dtHLAGM/onetoonemapping.h"
#include "dtHLAGM/rprparametertranslator.h"
#include "dtHLAGM/distypes.h"
#include "dtHLAGM/objectruntimemappinginfo.h"

namespace dtHLAGM
{
   IMPLEMENT_ENUM(RPRAttributeType);
   const RPRAttributeType RPRAttributeType::WORLD_COORDINATE_TYPE("WORLD_COORDINATE_TYPE", 1, 24);
   const RPRAttributeType RPRAttributeType::EULER_ANGLES_TYPE("EULER_ANGLES_TYPE", 1, 12);
   const RPRAttributeType RPRAttributeType::VELOCITY_VECTOR_TYPE("VELOCITY_VECTOR_TYPE", 1, 12);
   const RPRAttributeType RPRAttributeType::UNSIGNED_INT_TYPE("UNSIGNED_INT_TYPE", 1, 4);
   const RPRAttributeType RPRAttributeType::UNSIGNED_CHAR_TYPE("UNSIGNED_CHAR_TYPE", 1, 1);
   const RPRAttributeType RPRAttributeType::UNSIGNED_SHORT_TYPE("UNSIGNED_SHORT_TYPE", 1, 2);
   const RPRAttributeType RPRAttributeType::FLOAT_TYPE("FLOAT_TYPE", 1, 4);
   const RPRAttributeType RPRAttributeType::DOUBLE_TYPE("DOUBLE_TYPE", 1, 8);
   const RPRAttributeType RPRAttributeType::ENTITY_TYPE("ENTITY_TYPE", 1, 8);
   const RPRAttributeType RPRAttributeType::ENTITY_IDENTIFIER_TYPE("ENTITY_IDENTIFIER_TYPE", 1, 6);
   const RPRAttributeType RPRAttributeType::EVENT_IDENTIFIER_TYPE("EVENT_IDENTIFIER_TYPE", 1, 5);
   const RPRAttributeType RPRAttributeType::MARKING_TYPE("MARKING_TYPE", 1, 12);
   const RPRAttributeType RPRAttributeType::STRING_TYPE("STRING_TYPE", 1, 128);

   RPRParameterTranslator::RPRParameterTranslator(dtUtil::Coordinates& coordinates, ObjectRuntimeMappingInfo& runtimeMappings):
      mCoordinates(coordinates), mRuntimeMappings(runtimeMappings)
   {
      mLogger = &dtUtil::Log::GetInstance("parametertranslator.cpp");
   }

   RPRParameterTranslator::~RPRParameterTranslator()
   {
   }

   const AttributeType& RPRParameterTranslator::GetAttributeTypeForName(const std::string& name) const
   {
      dtUtil::Enumeration* enumVal = RPRAttributeType::GetValueForName(name);

      if (enumVal == NULL)
         return AttributeType::UNKNOWN;
      else
         return static_cast<const AttributeType&>(*enumVal);
   }

   bool RPRParameterTranslator::TranslatesAttributeType(const AttributeType& type) const
   {
      //I think doing a dynamic cast on the type to see if it's the subclass used by this class is probably
      //the fastest way to check.
      return dynamic_cast<const RPRAttributeType*>(&type) != NULL;
   }

   void RPRParameterTranslator::MapFromParamToWorldCoord(
      char* buffer, 
      const size_t maxSize, 
      const dtGame::MessageParameter& parameter, 
      const dtDAL::DataType& parameterDataType) const throw()
   {
      osg::Vec3 position;

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

      osg::Vec3d outPos = mCoordinates.ConvertToRemoteTranslation(position);

      WorldCoordinate wc;
      wc.SetX(outPos.x());
      wc.SetY(outPos.y());
      wc.SetZ(outPos.z());

      if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "World coordinate has been set to %lf %lf %lf",
                             wc.GetX(), wc.GetY(), wc.GetZ());

         osg::Vec3 testReversePos = mCoordinates.ConvertToLocalTranslation(outPos);

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

   void RPRParameterTranslator::MapFromParamToEulerAngles(
      char* buffer, 
      const size_t maxSize, 
      const dtGame::MessageParameter& parameter, 
      const dtDAL::DataType& parameterDataType) const throw()
   {
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
                             "The incoming parameter \"%s\" is not of a supported type \"%s\" for conversion to \"%s\"",
                             parameter.GetName().c_str(), parameterDataType.GetName().c_str(),
                             RPRAttributeType::EULER_ANGLES_TYPE.GetName().c_str());
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The rotation is %f %f %f",
                             angleReordered.x(), angleReordered.y(), angleReordered.z());
      }


      result = mCoordinates.ConvertToRemoteRotation(angleReordered);


      EulerAngles eulerAngles;
      eulerAngles.SetPsi(result[0]);
      eulerAngles.SetTheta(result[1]);
      eulerAngles.SetPhi(result[2]);

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "Euler Angles have been set to %f %f %f",
                             eulerAngles.GetPsi() , eulerAngles.GetTheta(), eulerAngles.GetPhi());

         osg::Vec3 reversed = mCoordinates.ConvertToLocalRotation(
               eulerAngles.GetPsi(), eulerAngles.GetTheta(), eulerAngles.GetPhi());

         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The reversed rotation would be have been set to %f %f %f",
                             reversed.x(), reversed.y(), reversed.z());
      }

      eulerAngles.Encode(buffer);
   }

   void RPRParameterTranslator::MapFromParamToVelocityVector(
      char* buffer, 
      const size_t maxSize, 
      const dtGame::MessageParameter& parameter, 
      const dtDAL::DataType& parameterDataType) const throw()
   {
      osg::Vec3 result;
      if (parameterDataType == dtDAL::DataType::VEC3)
      {
         result = mCoordinates.GetOriginRotationMatrixInverse().preMult(
            static_cast<const dtGame::Vec3MessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::VEC3F)
      {
         const osg::Vec3f& temp = static_cast<const dtGame::Vec3fMessageParameter&>(parameter).GetValue();
         osg::Vec3 preResult(temp.x(), temp.y(), temp.z());
         
         result = mCoordinates.GetOriginRotationMatrixInverse().preMult(preResult);
      } 
      else 
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The incoming parameter \"%s\" is not of a supported type \"%s\" for conversion to \"%s\"",
                             parameter.GetName().c_str(), parameterDataType.GetName().c_str(),
                             RPRAttributeType::VELOCITY_VECTOR_TYPE.GetName().c_str());
      }
      // USED FOR ANGULAR VELOCITY, ACCELERATION VECTOR, AND VELOCITY VECTOR
      VelocityVector velocityVector;
      velocityVector.SetX(result.x());
      velocityVector.SetY(result.y());
      velocityVector.SetZ(result.z());
      velocityVector.Encode(buffer);
   }
   
   void RPRParameterTranslator::MapFromParamToEntityType(
      char* buffer,
      const size_t maxSize,
      const dtGame::MessageParameter& parameter,
      const OneToManyMapping& mapping,
      const OneToManyMapping::ParameterDefinition& paramDef) const throw()
   {
      if (parameter.GetDataType() != dtDAL::DataType::ENUMERATION)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "The incoming parameter \"%s\" is not of a supported type \"%s\" for conversion to \"%s\"."
                             "Only enumeration parameters are supported.",
                             parameter.GetName().c_str(), parameter.GetDataType().GetName().c_str(),
                             RPRAttributeType::VELOCITY_VECTOR_TYPE.GetName().c_str());
         return;
      } 
      
      EntityType entityType;

      const std::string& msgParamValue = static_cast<const dtGame::EnumMessageParameter&>(parameter).GetValue();
      
      const std::string& valueAsString = GetEnumValue(msgParamValue, paramDef, false);

      std::istringstream iss;
      iss.str(valueAsString);
      iss >> entityType;
      entityType.Encode(buffer);
   }

   void RPRParameterTranslator::MapFromMessageParameters(char* buffer, size_t& maxSize,
      std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& parameters, const OneToManyMapping& mapping) const
   {
      const AttributeType& hlaType = mapping.GetHLAType();

      if (parameters.size() == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unable to map from Game to HLA mapping %s because no message parameters were passed into the mapping method.",
            mapping.GetHLAName().c_str());

         return;
      }

      if (mapping.GetParameterDefinitions().size() == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
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

      if (hlaType == RPRAttributeType::WORLD_COORDINATE_TYPE)
      {
         MapFromParamToWorldCoord(buffer, maxSize, parameter, parameterDataType);
      }
      else if (hlaType == RPRAttributeType::ENTITY_TYPE)
      {
         MapFromParamToEntityType(buffer, maxSize, parameter, mapping, paramDef);
      }
      else if (hlaType == RPRAttributeType::EULER_ANGLES_TYPE)
      {
         MapFromParamToEulerAngles(buffer, maxSize, parameter, parameterDataType);
      }
      else if (hlaType == RPRAttributeType::VELOCITY_VECTOR_TYPE)
      {
         MapFromParamToVelocityVector(buffer, maxSize, parameter, parameterDataType);
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
         eventIdentifier.Encode(buffer);
      }
      else if (hlaType == RPRAttributeType::ENTITY_TYPE)
      {
         EntityType entityType;
         entityType.Encode(buffer);
      }
      else if (hlaType == RPRAttributeType::STRING_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::STRING ||
             parameterDataType == dtDAL::DataType::ENUMERATION)
         {
            const std::string& parameterValue = static_cast<const dtGame::StringMessageParameter&>(parameter).GetValue();

            std::string value;
            if (parameterDataType == dtDAL::DataType::ENUMERATION)
               value = GetEnumValue(parameterValue, paramDef, false);
            else
               value = parameterValue;

            for (unsigned i = 0; i < RPRAttributeType::STRING_TYPE.GetEncodedLength(); ++i)
            {
               if (i < value.size())
                  buffer[i] = value[i];
               else
                  //zero anything after the string value.
                  buffer[i] = '\0';
            }
            //change the size of this parameter to match the actual string length.
            maxSize = value.size() + 1;
            
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Mapped parameter to a string value.  The result with size \"%u\" is \"%s\".",
                  maxSize, buffer);
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

            //1 is ASCII
            buffer[0] = 1;
            for (unsigned i = 1; i < RPRAttributeType::MARKING_TYPE.GetEncodedLength(); ++i)
            {
               if (i <= markingText.size())
                  buffer[i] = markingText[i-1];
               else
                  buffer[i] = '\0';
            }

         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map from Game Type \"%s\" to HLA type \"%s\"",
               parameterDataType.GetName().c_str(),
               RPRAttributeType::MARKING_TYPE.GetName().c_str());
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

   }

   void RPRParameterTranslator::MapFromWorldCoordToMessageParam(
      const char* buffer, 
      const size_t size,
      dtGame::MessageParameter& parameter,
      const dtDAL::DataType& parameterDataType ) const throw()
   {
      WorldCoordinate wc;
      wc.Decode(buffer);

      if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
         "World coordinate has been decoded to %lf %lf %lf", wc.GetX(), wc.GetY(), wc.GetZ());

      osg::Vec3d inPos(wc.GetX(), wc.GetY(), wc.GetZ());
      osg::Vec3 position = mCoordinates.ConvertToLocalTranslation(inPos);

      if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
         "The world coordinate was converted a local coordinate %lf %lf %lf",
            position[0], position[1], position[2]);

      if (parameterDataType == dtDAL::DataType::VEC3)
      {
         static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(position);
      }
      else if (parameterDataType == dtDAL::DataType::VEC3F)
      {
         static_cast<dtGame::Vec3fMessageParameter&>(parameter).SetValue(
            osg::Vec3f(position.x(), position.y(), position.z()));
      }
   }

   void RPRParameterTranslator::MapFromEulerAnglesToMessageParam(
      const char* buffer, 
      const size_t size,
      dtGame::MessageParameter& parameter,
      const dtDAL::DataType& parameterDataType ) const throw()
   {
      EulerAngles eulerAngles;

      eulerAngles.Decode(buffer);

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "The incoming euler angles are %lf %lf %lf",
            eulerAngles.GetPsi(), eulerAngles.GetTheta(), eulerAngles.GetPhi());
            
      osg::Vec3 result = mCoordinates.ConvertToLocalRotation(eulerAngles.GetPsi(), eulerAngles.GetTheta(), eulerAngles.GetPhi());

      if (parameterDataType == dtDAL::DataType::VEC3)
      {
         //convert to x,y,z
         osg::Vec3 thisEulerAngle(result[1], result[2], result[0]);

         static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(thisEulerAngle);
         parameter.WriteToLog(*mLogger);
      }
      else if (parameterDataType == dtDAL::DataType::VEC3D)
      {
         //convert to x,y,z
         osg::Vec3d thisEulerAngle(result[1], result[2], result[0]);

         static_cast<dtGame::Vec3dMessageParameter&>(parameter).SetValue(thisEulerAngle);
         parameter.WriteToLog(*mLogger);
      }
   }
   
   void RPRParameterTranslator::MapFromVelocityVectorToMessageParam(
      const char* buffer, 
      const size_t size,
      dtGame::MessageParameter& parameter,
      const dtDAL::DataType& parameterDataType ) const throw()
   {
      // USED FOR ANGULAR VELOCITY, ACCELERATION VECTOR, AND VELOCITY VECTOR
      VelocityVector velocityVector;
      velocityVector.Decode(buffer);

      osg::Vec3 thisVector;

      thisVector[0] = velocityVector.GetX();
      thisVector[1] = velocityVector.GetY();
      thisVector[2] = velocityVector.GetZ();

      thisVector =  mCoordinates.GetOriginRotationMatrix().preMult(thisVector);

      if (parameterDataType == dtDAL::DataType::VEC3)
      {
         static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(thisVector);
      }
      else if (parameterDataType == dtDAL::DataType::VEC3F)
      {
         static_cast<dtGame::Vec3fMessageParameter&>(parameter).SetValue(
            osg::Vec3f(thisVector.x(), thisVector.y(), thisVector.z()));
      }
   }
   
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
   
   void RPRParameterTranslator::MapToMessageParameters(const char* buffer, size_t size,
      std::vector<dtCore::RefPtr<dtGame::MessageParameter> >& parameters, const OneToManyMapping& mapping) const
   {
      const AttributeType& hlaType = mapping.GetHLAType();

      if (parameters.size() == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unable to map from HLA mapping %s because no message parameters were passed into the mapping method.",
            mapping.GetHLAName().c_str());
         return;
      }

      if (mapping.GetParameterDefinitions().size() == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
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

      if (hlaType == RPRAttributeType::WORLD_COORDINATE_TYPE)
      {
         MapFromWorldCoordToMessageParam( buffer, size, parameter, parameterDataType );
      }
      else if (hlaType == RPRAttributeType::EULER_ANGLES_TYPE)
      {
         MapFromEulerAnglesToMessageParam( buffer, size, parameter, parameterDataType );
      }
      else if (hlaType == RPRAttributeType::VELOCITY_VECTOR_TYPE)
      {
         MapFromVelocityVectorToMessageParam( buffer, size, parameter, parameterDataType );
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

         SetIntegerValue((long)value, parameter, mapping, 0);

      }
      else if (hlaType == RPRAttributeType::UNSIGNED_SHORT_TYPE)
      {
         unsigned short value = *(unsigned short*)(&buffer[0]);

         if (osg::getCpuByteOrder() == osg::LittleEndian)
           osg::swapBytes((char*)(&value), sizeof(unsigned short));

         SetIntegerValue((long)value, parameter, mapping, 0);
      }
      else if (hlaType == RPRAttributeType::UNSIGNED_CHAR_TYPE)
      {
         unsigned char value = *(unsigned char*)(&buffer[0]);
         SetIntegerValue((long)value, parameter, mapping, 0);
      }
      else if (hlaType == RPRAttributeType::EVENT_IDENTIFIER_TYPE)
      {
         EventIdentifier eventIdentifier;
         eventIdentifier.Decode(buffer);
      }
      else if (hlaType == RPRAttributeType::ENTITY_TYPE)
      {
         EntityType entityType;
         entityType.Decode(buffer);
         
         if (parameterDataType == dtDAL::DataType::ENUMERATION)
         {
            std::string mappedValue;
            std::ostringstream stringValue;
            
            //this current code only allows for exact matches and a default.
            stringValue << entityType;
            
            mappedValue = GetEnumValue(stringValue.str(), paramDef, true);

            static_cast<dtGame::EnumMessageParameter&>(parameter).SetValue(mappedValue);            
         }
      }
      else if (hlaType == RPRAttributeType::STRING_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::STRING ||
             parameterDataType == dtDAL::DataType::ENUMERATION)
         {
            std::string value;
            for (unsigned i = 0; i < size; ++i)
            {
               char c = buffer[i];
               if (c == '\0')
                  break;
               value.append(1, c);
            }
            if (parameterDataType == dtDAL::DataType::ENUMERATION)
               value = GetEnumValue(value, paramDef, true);
            
            static_cast<dtGame::StringMessageParameter&>(parameter).SetValue(value);
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
            //unsigned char markingEnum = *(unsigned char*)(&buffer[0]);
            std::string markingText;
            //1 is ASCII
            //if (markingEnum == 1)
            //{
            for (unsigned i = 1; i < RPRAttributeType::MARKING_TYPE.GetEncodedLength(); ++i)
            {
               char c = buffer[i];
               if (c == '\0')
                  break;
               markingText.append(1, c);
            }
            //}
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

   void RPRParameterTranslator::SetIntegerValue(long value, dtGame::MessageParameter& parameter, const OneToManyMapping& mapping, unsigned parameterDefIndex) const
   {
      const dtDAL::DataType& parameterDataType = parameter.GetDataType();
      const OneToManyMapping::ParameterDefinition& paramDef = mapping.GetParameterDefinitions()[parameterDefIndex];

      //std::cout << mapping.GetHLAName() << " " << value << std::endl;

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
      else if (parameterDataType == dtDAL::DataType::ENUMERATION)
      {
         std::string mappedValue;
         std::ostringstream stringValue;
         
         stringValue << int(value);
         
         mappedValue = GetEnumValue(stringValue.str(), paramDef, true);
         
         static_cast<dtGame::EnumMessageParameter&>(parameter).SetValue(mappedValue);
         parameter.WriteToLog(*mLogger);
      }
   }

   long RPRParameterTranslator::GetIntegerValue(const dtGame::MessageParameter& parameter, const OneToManyMapping& mapping, unsigned parameterDefIndex) const
   {
      const dtDAL::DataType& parameterDataType = parameter.GetDataType();
      const OneToManyMapping::ParameterDefinition& paramDef = mapping.GetParameterDefinitions()[parameterDefIndex];

      //std::cout << mapping.GetHLAName() << " " << value << std::endl;
      parameter.WriteToLog(*mLogger);

      if (parameterDataType == dtDAL::DataType::UINT)
      {
         return long(static_cast<const dtGame::UnsignedIntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::ULONGINT)
      {
         return long(static_cast<const dtGame::UnsignedLongIntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::USHORTINT)
      {
         return long(static_cast<const dtGame::UnsignedShortIntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::INT)
      {
         return long(static_cast<const dtGame::IntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::LONGINT)
      {
         return static_cast<const dtGame::LongIntMessageParameter&>(parameter).GetValue();
      }
      else if (parameterDataType == dtDAL::DataType::SHORTINT)
      {
         return long(static_cast<const dtGame::ShortIntMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::BOOLEAN)
      {
         return long(static_cast<const dtGame::BooleanMessageParameter&>(parameter).GetValue());
      }
      else if (parameterDataType == dtDAL::DataType::ENUMERATION)
      {
         const std::string& msgParamValue = static_cast<const dtGame::EnumMessageParameter&>(parameter).GetValue();
         long value = 0L;
         std::string valueAsString;
         
         valueAsString = GetEnumValue(msgParamValue, paramDef, false);
         
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


}

