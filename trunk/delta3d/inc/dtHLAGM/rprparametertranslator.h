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

#ifndef DELTA_RPR_PARAMETER_TRANSLATOR
#define DELTA_RPR_PARAMETER_TRANSLATOR

#include <dtHLAGM/export.h>
#include <dtHLAGM/parametertranslator.h>

namespace dtUtil 
{
   class Log;
   class Coordinates;
}

namespace dtGame 
{
   class MessageParameter;
}

namespace dtHLAGM
{
   class OneToManyMapping;
   class ObjectRuntimeMappingInfo;
   
   class DT_HLAGM_EXPORT RPRAttributeType : public AttributeType
   {
      DECLARE_ENUM(RPRAttributeType);

      public:
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

         ///An unsigned integer.
         static const RPRAttributeType UNSIGNED_INT_TYPE;

         ///A unsigned character.
         static const RPRAttributeType UNSIGNED_CHAR_TYPE;

         ///An unsigned short integer.
         static const RPRAttributeType UNSIGNED_SHORT_TYPE;

         ///double precision floating point value.
         static const RPRAttributeType DOUBLE;

         ///struct holding a DIS entity enumeration.
         static const RPRAttributeType ENTITY_TYPE;

         ///struct holding a DIS entity enumeration.
         static const RPRAttributeType ENTITY_IDENTIFIER_TYPE;

         ///An indentifier uniquely marking an event.
         static const RPRAttributeType EVENT_IDENTIFIER_TYPE;

         ///Indentifier for an entity marking used to hold both a marking type and the content of the marking.
         static const RPRAttributeType MARKING_TYPE;

      private:
         RPRAttributeType(const std::string& name, unsigned char id, size_t encodedLength):AttributeType(name, id, encodedLength)
         {
            AddInstance(this);
         }
         
         virtual ~RPRAttributeType() {}
   };
   
   class DT_HLAGM_EXPORT RPRParameterTranslator : public ParameterTranslator
   {
      public:
         RPRParameterTranslator(dtUtil::Coordinates& coordinates, ObjectRuntimeMappingInfo& runtimeMappings);

         virtual void MapToMessageParameters(const char* buffer, size_t size, std::vector<dtCore::RefPtr<dtGame::MessageParameter> >& parameters, const OneToManyMapping& mapping) const;

         virtual void MapFromMessageParameters(char* buffer, size_t maxSize, std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& parameters, const OneToManyMapping& mapping) const;

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
         void SetIntegerValue(long value, dtGame::MessageParameter& parameter, const OneToManyMapping& mapping, unsigned parameterDefIndex) const;
         long GetIntegerValue(const dtGame::MessageParameter& parameter, const OneToManyMapping& mapping, unsigned parameterDefIndex) const;

   };

}

#endif
