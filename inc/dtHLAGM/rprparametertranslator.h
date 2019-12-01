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

#ifndef DELTA_RPR_PARAMETER_TRANSLATOR
#define DELTA_RPR_PARAMETER_TRANSLATOR

#include <dtHLAGM/export.h>
#include <dtHLAGM/onetomanymapping.h>
#include <dtHLAGM/parametertranslator.h>

namespace dtUtil
{
   class Log;
   class Coordinates;
}

namespace dtCore
{
   class DataType;
}

namespace dtHLAGM
{
   class ObjectRuntimeMappingInfo;

   class DT_HLAGM_EXPORT RPRAttributeType : public AttributeType
   {
      DECLARE_ENUM(RPRAttributeType);

      public:

         enum SwitchEnum
         {
            SPATIAL_TYPE_ENUM,
            WORLD_COORDINATE_TYPE_ENUM,
            EULER_ANGLES_TYPE_ENUM,
            VELOCITY_VECTOR_TYPE_ENUM,
            ANGULAR_VELOCITY_VECTOR_TYPE_ENUM,
            UNSIGNED_INT_TYPE_ENUM,
            UNSIGNED_CHAR_TYPE_ENUM,
            UNSIGNED_SHORT_TYPE_ENUM,
            FLOAT_TYPE_ENUM,
            DOUBLE_TYPE_ENUM,
            ENTITY_TYPE_ENUM,
            ENTITY_IDENTIFIER_TYPE_ENUM,
            EVENT_IDENTIFIER_TYPE_ENUM,
            MARKING_TYPE_ENUM,
            MARKING_TYPE_32_ENUM,
            STRING_TYPE_ENUM,
            OCTET_TYPE_ENUM,
            ARTICULATED_PART_SINGLE_TYPE_ENUM,
            ARTICULATED_PART_TYPE_ENUM,
            RTI_OBJECT_ID_STRUCT_TYPE_ENUM,
            TIME_TAG_TYPE_ENUM,
            ENVIRONMENT_RECORD_LIST_TYPE_ENUM
         };

         /**
          * The RPR 2.0 spatial structure which provides the position, rotation, and all pertinent
          *  motion information to allow for deadreckoning.
          */
         static const RPRAttributeType SPATIAL_TYPE;

         /**
          * A vector of three floats.  This is a world location and will be converted
          * with the coordinate converter.
          */
         static const RPRAttributeType WORLD_COORDINATE_TYPE;

         ///Three angles used to define a 3D rotation.
         static const RPRAttributeType EULER_ANGLES_TYPE;

         /**
          * Three double values specifying the velocity vector in the coordinate system
          * of the entity.  This will be multiplied by the rotation offset.
          */
         static const RPRAttributeType VELOCITY_VECTOR_TYPE;

         /**
          * Three double values specifying the angular velocity vector (rotational acceleration)
          * in the coordinate system of the entity.  This will NOT be multiplied by the rotation offset.
          */
         static const RPRAttributeType ANGULAR_VELOCITY_VECTOR_TYPE;

         /// An unsigned integer.
         static const RPRAttributeType UNSIGNED_INT_TYPE;

         /// A unsigned character.
         static const RPRAttributeType UNSIGNED_CHAR_TYPE;

         /// An unsigned short integer.
         static const RPRAttributeType UNSIGNED_SHORT_TYPE;

         /// single precision floating point value.
         static const RPRAttributeType FLOAT_TYPE;

         /// double precision floating point value.
         static const RPRAttributeType DOUBLE_TYPE;

         /// struct holding a DIS entity enumeration.
         static const RPRAttributeType ENTITY_TYPE;

         /// An identifier uniquely marking an entity.
         static const RPRAttributeType ENTITY_IDENTIFIER_TYPE;

         /// An identifier uniquely marking an event.
         static const RPRAttributeType EVENT_IDENTIFIER_TYPE;

         /// Identifier for an entity marking used to hold both a marking type and the content of the marking.
         static const RPRAttributeType MARKING_TYPE;

         /// Identifier for an entity marking used to hold both a marking type and the content of the marking (32 bytes).
         static const RPRAttributeType MARKING_TYPE_32;

         /// A variable length string.
         static const RPRAttributeType STRING_TYPE;

         /// A variable length block of bytes up to 65535 bytes.
         static const RPRAttributeType OCTET_TYPE;

         /// Type representing a single articulated part.
         static const RPRAttributeType ARTICULATED_PART_SINGLE_TYPE;

         /// A type for the articulation to be captured / sent
         static const RPRAttributeType ARTICULATED_PART_TYPE;

         /// RTI string ID type used to identify a single federation entity, usually in firing and detonation events.
         static const RPRAttributeType RTI_OBJECT_ID_STRUCT_TYPE;

         /// The time string passed to time tag
         static const RPRAttributeType TIME_TAG_TYPE;

         /// A list of environment process records.  Will be converted to a record count and a group property holding the parsed data.
         static const RPRAttributeType ENVIRONMENT_RECORD_LIST_TYPE;

         SwitchEnum GetEnumValue() const;

      private:
         RPRAttributeType(const std::string& name, unsigned char supportedParameters, size_t encodedLength, SwitchEnum enumValue);

         virtual ~RPRAttributeType() {}

         SwitchEnum mEnumValue;
   };

   class DT_HLAGM_EXPORT RPRParameterTranslator : public ParameterTranslator
   {
      public:
         RPRParameterTranslator(dtUtil::Coordinates& coordinates, ObjectRuntimeMappingInfo& runtimeMappings);

         virtual void MapToMessageParameters(const char* buffer, size_t size, std::vector<dtCore::RefPtr<dtGame::MessageParameter> >& parameters, const OneToManyMapping& mapping) const;

         virtual void MapFromMessageParameters(char* buffer, size_t& maxSize, std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& parameters, const OneToManyMapping& mapping) const;

         /**
          * @returns the attribute type this translator uses for this name.  It will return AttributeType::UNKNOWN if the name doesn't match anything.
          */
         virtual const AttributeType& GetAttributeTypeForName(const std::string& name) const;

         ///@return true if this translator supports the given type.
         virtual bool TranslatesAttributeType(const AttributeType& type) const;

      protected:
         dtUtil::Log* mLogger;
         dtUtil::Coordinates& mCoordinates;

         ObjectRuntimeMappingInfo& mRuntimeMappings;

         virtual ~RPRParameterTranslator();

         /**
          * Helper method to map between the hla and game value enum values.
          * @param value the hla or game value we need the mapping for.
          * @param paramDef the parameter definition that should hold the mapping.
          * @param returnGameValue true if "value" is the hla value an we want the game value. False for the opposite.
          * @return the mapped value, the default if no mapping is found, or empty string if the default fails.
          */
         const std::string GetEnumValue(
            const std::string& value,
            const OneToManyMapping::ParameterDefinition& paramDef,
            bool returnGameValue) const;

         void SetIntegerValue(unsigned value, dtGame::MessageParameter& parameter, const OneToManyMapping& mapping, unsigned parameterDefIndex) const;
         unsigned GetIntegerValue(const dtGame::MessageParameter& parameter, const OneToManyMapping& mapping, unsigned parameterDefIndex) const;

         osg::Vec3d CoordConvertPositionParameter(const dtGame::MessageParameter& parameter) const;
         osg::Vec3d CoordConvertRotationParameter(const dtGame::MessageParameter& parameter) const;
         osg::Vec3f CoordConvertVelocityParameter(const dtGame::MessageParameter& parameter) const;
         osg::Vec3f CoordConvertAngularVelocityParameter(const dtGame::MessageParameter& parameter) const;

         void CoordConvertWorldCoord(const WorldCoordinate& worldCoord, dtGame::MessageParameter& parameter) const;
         void CoordConvertOrientation(const EulerAngles& vec, dtGame::MessageParameter& parameter) const;
         void CoordConvertVelocityVector(const VelocityVector& vec, dtGame::MessageParameter& parameter) const;
         void CoordConvertAngularVelocityVector(const VelocityVector& vec, dtGame::MessageParameter& parameter) const;

         /**
          * This expects to find the parameters in the order
          * Dead reckoning algorithm, frozen, world position, orientation, velocity, acceleration,
          * and angular velocity.  Frozen may be omitted
          */
         void MapFromParamsToSpatial(
            char* buffer,
            size_t& maxSize,
            std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& parameters,
            const OneToManyMapping& mapping) const;

         void MapFromParamToEnvProcessRecList(
            char* buffer,
            size_t& maxSize,
            const dtGame::MessageParameter& parameter,
            const OneToManyMapping::ParameterDefinition& paramDef) const;

         void MapFromParamToWorldCoord(
            char* buffer,
            const size_t maxSize,
            const dtGame::MessageParameter& parameter) const;

         void MapFromParamToEulerAngles(
            char* buffer,
            const size_t maxSize,
            const dtGame::MessageParameter& parameter) const;

         void MapFromParamToVelocityVector(
            char* buffer,
            const size_t maxSize,
            const dtGame::MessageParameter& parameter) const;

         void MapFromParamToAngularVelocityVector(
            char* buffer,
            const size_t maxSize,
            const dtGame::MessageParameter& parameter) const;

         void MapFromParamToEntityType(
            char* buffer,
            const size_t maxSize,
            const dtGame::MessageParameter& parameter,
            const OneToManyMapping& mapping,
            const OneToManyMapping::ParameterDefinition& paramDef) const;

         bool MapFromParamToArticulation(
            ArticulatedParameter& artParam,
            const dtGame::MessageParameter& parameter,
            const OneToManyMapping::ParameterDefinition& paramDef) const;

         void MapFromParamToArticulations(
            char* buffer,
            size_t& maxSize,
            const dtGame::MessageParameter& parameter,
            const OneToManyMapping::ParameterDefinition& paramDef) const;

         void MapFromStringParamToCharArray(
            char* buffer,
            size_t& maxSize,
            const std::string& parameterValue,
            const OneToManyMapping::ParameterDefinition& paramDef,
            const dtCore::DataType& parameterDataType,
            bool addNullTerminator = true) const;

         void MapFromSpatialToMessageParams(
            const char* buffer,
            const size_t maxSize,
            std::vector<dtCore::RefPtr<dtGame::MessageParameter> >& parameters,
            const OneToManyMapping& mapping) const;

         void MapFromEnvProcessRecListToMessageParams(
            const char* buffer,
            const size_t size,
            dtGame::MessageParameter& parameter) const;

         void MapFromWorldCoordToMessageParam(
            const char* buffer,
            const size_t size,
            dtGame::MessageParameter& parameter) const;

         void MapFromEulerAnglesToMessageParam(
            const char* buffer,
            const size_t size,
            dtGame::MessageParameter& parameter) const;

         void MapFromVelocityVectorToMessageParam(
            const char* buffer,
            const size_t size,
            dtGame::MessageParameter& parameter) const;

         void MapFromAngularVelocityVectorToMessageParam(
            const char* buffer,
            const size_t size,
            dtGame::MessageParameter& parameter) const;

         void MapFromCharArrayToStringParam(
            const char* buffer,
            const size_t size,
            dtGame::MessageParameter& parameter,
            const OneToManyMapping::ParameterDefinition& paramDef,
            bool stopAtNullTerminator = true) const;

         void MapFromArticulationToMessageParam(
            ArticulatedParameter artParam,
            dtGame::MessageParameter& parameter,
            const dtCore::DataType& parameterDataType,
            const OneToManyMapping::ParameterDefinition& paramDef) const;

         void MapFromArticulationsToMessageParam(
            const char* buffer,
            const size_t size,
            dtGame::MessageParameter& parameter,
            const dtCore::DataType& parameterDataType,
            const OneToManyMapping::ParameterDefinition& paramDef) const;

       private:

         /**Copy numChars of markingText into buffer. Add trailing \0's if required.
           *Preface buffer with '1' to denote the text is "ASCII". **/
         static void CopyMarkingTextToBuffer(const std::string &markingText,
                                             char *buffer,
                                             size_t numChars);

         ///Copy up to numChars of buffer into markingText.
         static void CopyBufferToMarkingText(const char *buffer,
                                             std::string &markingText,
                                             size_t numChars);
   };

}

#endif
