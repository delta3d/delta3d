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
 * Matthew W. Campbell
 */

#ifndef DELTA_DATATYPES
#define DELTA_DATATYPES

#include <dtUtil/enumeration.h>
#include <dtCore/export.h>


namespace dtCore
{
   /**
    * The DataType class is an enumeration representing the different datatypes
    * that are supported by the Delta3D engine.
    */
   class DT_CORE_EXPORT DataType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(DataType);
      public:
         enum DataTypeId
         {
            UNKNOWN_ID = 0, CHAR_ID, UCHAR_ID, FLOAT_ID, DOUBLE_ID, INT_ID, UINT_ID, LONGINT_ID,
            ULONGINT_ID, SHORTINT_ID, USHORTINT_ID, STRING_ID, BOOLEAN_ID,
            VEC4_ID, VEC3_ID, VEC2_ID, VEC4F_ID, VEC3F_ID, VEC2F_ID, VEC4D_ID, VEC3D_ID, VEC2D_ID,
            RGBCOLOR_ID, RGBACOLOR_ID, GROUP_ID, ACTOR_ID, ENUMERATION_ID, GAMEEVENT_ID,
            STATICMESH_ID, VOLUME_ID, TEXTURE_ID, TERRAIN_ID, SOUND_ID, PARTICLESYSTEM_ID,
            SKELETAL_MESH_ID, ARRAY_ID, CONTAINER_ID, CONTAINER_SELECTOR_ID, PROPERTY_CONTAINER_ID, PREFAB_ID,
            SHADER_ID, DIRECTOR_ID, BIT_MASK_ID
         };

         /**
          * Data type used to represent an unknown type.  For example, the GenericActorProperty
          * returns identifies its datatype as unknown.  It does this because the
          * GenericActorProperty can be any type.
          * @see GenericActorProperty
          */
         static DataType UNKNOWN;

         /**
          * Standard 8-bit signed char value.
          */
         static DataType CHAR;

         /**
          * Standard 8-bit unsigned char value.
          */
         static DataType UCHAR;

         /**
          * Standard 32-bit floating point value.
          */
         static DataType FLOAT;

         /**
          * Standard 64-bit precision decimal number.
          */
         static DataType DOUBLE;

         /**
          * Standard 32-bit signed integer value.
          */
         static DataType INT;

         /**
          * Standard 32-bit unsigned integer value.
          */
         static DataType UINT;

         /**
          * Similar to the long datatype.
          */
         static DataType LONGINT;

         /**
          * Similar to the unsigned long datatype.
          */
         static DataType ULONGINT;

         /**
          * Similar to the short datatype.
          */
         static DataType SHORTINT;

         /**
          * Similar to the unsigned short datatype.
          */
         static DataType USHORTINT;

         /**
          * String data type.
          */
         static DataType STRING;

         /**
          * Boolean data type.
          */
         static DataType BOOLEAN;

         /**
          * A vector of 4 values.
          */
         static DataType& VEC4;

         /**
          * A vector of 3 values.
          */
         static DataType& VEC3;

         /**
          * A vector of 2 values.
          */
         static DataType& VEC2;

         /**
          * A vector of 4 values.
          */
         static DataType VEC4F;

         /**
          * A vector of 3 values.
          */
         static DataType VEC3F;

         /**
          * A vector of 2 values.
          */
         static DataType VEC2F;

         /**
          * A vector of 4 values.
          */
         static DataType VEC4D;

         /**
          * A vector of 3 values.
          */
         static DataType VEC3D;

         /**
          * A vector of 2 values.
          */
         static DataType VEC2D;

         /**
          * A color data type with 3 values, Red, Green, and Blue.
          */
         static DataType RGBCOLOR;

         /**
          * A color data type with 4 values, Red, Green, Blue, and Alpha.
          */
         static DataType RGBACOLOR;

         /**
          * A group
          */
         static DataType GROUP;

         /**
          * An Array
          */
         static DataType ARRAY;

         /**
          * A Container.
          */
         static DataType CONTAINER;

         /**
          *	A Container selector.
          */
         static DataType CONTAINER_SELECTOR;

         /**
          * A list of property containers
          */
         static DataType PROPERTY_CONTAINER;

         /**
          * An actor
          */
         static DataType ACTOR;

         /**
          * Datatype which represents an enumeration.
          * @see dtCore::Enumeration
          */
         static DataType ENUMERATION;

         /**
          * Game event data type.
          */
         static DataType GAME_EVENT;

         /**
          * A static mesh resource
          */
         static DataType STATIC_MESH;

         /**
          * A volume of voxels, point, particles, etc.
          */
         static DataType VOLUME;

         /**
          * A Skeletal Mesh
          */
         static DataType SKELETAL_MESH;

         /**
          * A texture resource
          */
         static DataType TEXTURE;

         /**
          * Geometry, textures, etc that together should be used as a terrain.
          */
         static DataType TERRAIN;

         /**
          * A Sound resource
          */
         static DataType SOUND;

         /**
          * A Particle System resource
          */
         static DataType PARTICLE_SYSTEM;

         /**
         * A Prefab resource.
         */
         static DataType PREFAB;

         /**
          *  A pixel shader definition
          */
         static DataType SHADER;

         /**
         * A Director Graph resource.
         */
         static DataType DIRECTOR;

         /**
          * A bit mask
          */
         static DataType BIT_MASK;

         /**
          * @return true if this datatype represents a resource
          */
         bool IsResource() const { return mResource; };

         /**
          * Gets a pretty name that can be used in the UI.
          * @return The pretty name
          */
         const std::string& GetDisplayName() { return mDisplayName; }

         /**
          * Gets the id associated with this datatype
          */
         unsigned char GetTypeId() const { return mId; }

         /**
          * Sets the id of a datatype
          */
         void SetTypeId(unsigned char newId) { mId = newId; }

      protected:
         virtual int Compare(const std::string& nameString) const;

         /**
          * Private constructor which registers a new DataType enumeration
          * with the static list of available DataType enumerations.
          */
          DataType(const std::string& name, const std::string& displayName, bool resource = false, unsigned char id = 0, const std::string& alias = "__NONE__");
       private:

          bool mResource;
          std::string mAltName, mDisplayName;
          unsigned char mId;

   };
}

#endif // DELTA_DATATYPES
