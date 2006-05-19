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

#include <dtGame/messageparameter.h>

#include "dtHLAGM/onetoonemapping.h"
#include "dtHLAGM/rprparametertranslator.h"
#include "dtHLAGM/distypes.h"
#include "dtHLAGM/objectruntimemappinginfo.h"


namespace dtHLAGM 
{
   IMPLEMENT_ENUM(RPRAttributeType);
   const RPRAttributeType RPRAttributeType::WORLD_COORDINATE_TYPE("World Coordinate", 1, 24);
   const RPRAttributeType RPRAttributeType::EULER_ANGLES_TYPE("Euler Angle", 1, 12);
   const RPRAttributeType RPRAttributeType::VELOCITY_VECTOR_TYPE("Velocity Vector", 1, 12);
   const RPRAttributeType RPRAttributeType::UNSIGNED_INT_TYPE("Unsigned Int", 1, 4);
   const RPRAttributeType RPRAttributeType::UNSIGNED_CHAR_TYPE("Unsigned Char", 1, 1);
   const RPRAttributeType RPRAttributeType::UNSIGNED_SHORT_TYPE("Unsigned Short", 1, 2);
   const RPRAttributeType RPRAttributeType::DOUBLE("Double", 1, 8);
   const RPRAttributeType RPRAttributeType::ENTITY_TYPE("Entity", 1, 8);
   const RPRAttributeType RPRAttributeType::ENTITY_IDENTIFIER_TYPE("Entity Identifier", 1, 6);
   const RPRAttributeType RPRAttributeType::EVENT_IDENTIFIER_TYPE("Event Identifier", 1, 5);
   const RPRAttributeType RPRAttributeType::MARKING_TYPE("Marking", 1, 12);

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
   
   void RPRParameterTranslator::MapFromMessageParameters(char* buffer, size_t maxSize, 
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
            "Unable to map from HLA mapping %s because no parameter mappings are added to the mapping object.", 
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
            mapping.GetHLAName().c_str(), paramDef.GetGameName().c_str());
            
      parameter.WriteToLog(*mLogger);
      
      if (hlaType == RPRAttributeType::WORLD_COORDINATE_TYPE)
      {
         osg::Vec3 position;
         
         if (parameterDataType == dtDAL::DataType::VEC3)
         {
            position = static_cast<const dtGame::Vec3MessageParameter&>(parameter).GetValue();
         }

         osg::Vec3d outPos = mCoordinates.ConvertToRemoteTranslation(position);
         
         WorldCoordinate wc;
         wc.SetX(outPos.x());
         wc.SetY(outPos.y());
         wc.SetZ(outPos.z());

         if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
               "World coordinate has been set to %lf %lf %lf", wc.GetX(), wc.GetY(), wc.GetZ());
         
         if (maxSize >= wc.EncodedLength())
            wc.Encode(buffer);
         else
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, 
               "Not enough space was allocated in the buffer to convert", wc.GetX(), wc.GetY(), wc.GetZ());
      }
      else if (hlaType == RPRAttributeType::ENTITY_TYPE)
      {
         EntityType entityType;

         std::string valueString = static_cast<const dtGame::EnumMessageParameter&>(parameter).GetValue();

//         strcpy(encodedEntityType, valueString.c_str());
//
//         entityType.SetKind(encodedEntityType[0]);
//         entityType.SetDomain(encodedEntityType[1]);
//         entityType.SetCountry(*(unsigned short *)(&encodedEntityType[2]));
//         entityType.SetCategory(encodedEntityType[4]);
//         entityType.SetSubcategory(encodedEntityType[5]);
//         entityType.SetSpecific(encodedEntityType[6]);
//         entityType.SetExtra(encodedEntityType[7]);
//
//         entityType.Encode(encodedEntityType);
      }
      else if (hlaType == RPRAttributeType::EULER_ANGLES_TYPE)
      {
         osg::Vec3 result; 
         if (parameterDataType == dtDAL::DataType::VEC3)
         {
            osg::Vec3 angle = static_cast<const dtGame::Vec3MessageParameter&>(parameter).GetValue();
            osg::Vec3 angleReordered(angle[1], angle[2], angle[0]);
            result = mCoordinates.ConvertToRemoteRotation(angleReordered);            
         }
         else if (parameterDataType == dtDAL::DataType::VEC3D)
         {
            osg::Vec3d angle = static_cast<const dtGame::Vec3dMessageParameter&>(parameter).GetValue();
            osg::Vec3 angleReordered(angle[1], angle[2], angle[0]);
            result = mCoordinates.ConvertToRemoteRotation(angleReordered);            
         }

         EulerAngles eulerAngles;
         eulerAngles.SetPsi(result[0]);
         eulerAngles.SetTheta(result[1]);
         eulerAngles.SetPhi(result[2]);

         eulerAngles.Encode(buffer);

      }
      else if (hlaType == RPRAttributeType::VELOCITY_VECTOR_TYPE)
      {
         osg::Vec3 result;
         if (parameterDataType == dtDAL::DataType::VEC3)
         {

            result = mCoordinates.GetOriginRotationMatrixInverse().preMult(
               static_cast<const dtGame::Vec3MessageParameter&>(parameter).GetValue());

         }
         // USED FOR ANGULAR VELOCITY, ACCELERATION VECTOR, AND VELOCITY VECTOR
         VelocityVector velocityVector;
         velocityVector.SetX(result.x());
         velocityVector.SetY(result.y());
         velocityVector.SetZ(result.z());
         velocityVector.Encode(buffer);
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
      else if (hlaType == RPRAttributeType::EVENT_IDENTIFIER_TYPE)
      {
         EventIdentifier eventIdentifier;
         eventIdentifier.Encode(buffer);
         //if (
      }
      else if (hlaType == RPRAttributeType::ENTITY_TYPE)
      {
         EntityType entityType;
         entityType.Encode(buffer);
      }
      else if (hlaType == RPRAttributeType::MARKING_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::STRING)
         {
            std::string markingText;
            markingText = static_cast<const dtGame::StringMessageParameter&>(parameter).GetValue();
            
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
         WorldCoordinate wc;
         wc.Decode(buffer);
         
         if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
            "World coordinate has been set to %lf %lf %lf", wc.GetX(), wc.GetY(), wc.GetZ());

         osg::Vec3d inPos(wc.GetX(), wc.GetY(), wc.GetZ());
         osg::Vec3 position = mCoordinates.ConvertToLocalTranslation(inPos);
         
         if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
            "The world coordinate was converted a local coordinate %lf %lf %lf", 
               position[0], position[1], position[2]);

         if (parameterDataType == dtDAL::DataType::VEC3)
         {
            static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(position);
            parameter.WriteToLog(*mLogger);
         }
      }
      else if (hlaType == RPRAttributeType::EULER_ANGLES_TYPE)
      {
         EulerAngles eulerAngles;

         eulerAngles.Decode(buffer);

         //std::cout << eulerAngles.GetPsi() << " " << eulerAngles.GetTheta() << " " << eulerAngles.GetPhi() << " " << std::endl;
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
      else if(hlaType == RPRAttributeType::VELOCITY_VECTOR_TYPE)
      {
         // USED FOR ANGULAR VELOCITY, ACCELERATION VECTOR, AND VELOCITY VECTOR
         VelocityVector velocityVector;
         velocityVector.Decode(buffer);

         if (parameterDataType == dtDAL::DataType::VEC3)
         {
            osg::Vec3 thisVector;

            thisVector[0] = velocityVector.GetX();
            thisVector[1] = velocityVector.GetY();
            thisVector[2] = velocityVector.GetZ();

            thisVector =  mCoordinates.GetOriginRotationMatrix().preMult(thisVector);

            static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(thisVector);
            parameter.WriteToLog(*mLogger);
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
         //if (
      }
      else if (hlaType == RPRAttributeType::ENTITY_TYPE)
      {
         EntityType entityType;
         entityType.Decode(buffer);
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
            parameter.WriteToLog(*mLogger);
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
            
            parameter.WriteToLog(*mLogger);
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
         std::string sValue;
         if (paramDef.GetGameEnumerationValue(int(value), sValue))
         {
            static_cast<dtGame::EnumMessageParameter&>(parameter).SetValue(sValue);
            parameter.WriteToLog(*mLogger);
         }
         else
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "No mapping was specified for value \"%d\" for an enumeration on mapping of \"%s\" to \"%s\". "
                  "Using default value \"%s\".", (int)value, paramDef.GetGameName().c_str(),
                  mapping.GetHLAName().c_str(), paramDef.GetDefaultValue().c_str());

            static_cast<dtGame::EnumMessageParameter&>(parameter).SetValue(paramDef.GetDefaultValue());
         }
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
         std::string sValue = static_cast<const dtGame::EnumMessageParameter&>(parameter).GetValue();
         int value;
         if (paramDef.GetHLAEnumerationValue(sValue, value))
         {
            return long(value);
         }
         else
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "No mapping was specified for value \"%d\" for an enumeration on mapping of \"%s\" to \"%s\". "
                  "Using default value \"%s\".", (int)value, paramDef.GetGameName().c_str(), 
                  mapping.GetHLAName().c_str(), paramDef.GetDefaultValue().c_str());

            if (paramDef.GetHLAEnumerationValue(paramDef.GetDefaultValue(), value))
               return long(value);
            else
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "No mapping to HLA was specified for the default value for an enumeration on mapping of \"%s\" to \"%s\". "
                  "The default value is \"%s\".", (int)value, paramDef.GetGameName().c_str(), 
                  mapping.GetHLAName().c_str(), paramDef.GetDefaultValue().c_str());
               return 0;
            }
         }
      }
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
         "Unhandled conversion for an enumeration on mapping of \"%s\" to \"%s\". "
         "The datatypes should have been logged above.", 
         paramDef.GetGameName().c_str(), mapping.GetHLAName().c_str());

      return 0;
   }


}

