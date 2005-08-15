/*
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
 * @author Matthew W. Campbell
*/
#ifndef DELTA_DATATYPES
#define DELTA_DATATYPES

#include <dtCore/export.h>
#include <dtUtil/enumeration.h>

namespace dtDAL
{
    /**
     * The DataType class is an enumeration representing the different datatypes
     * that are supported by the Delta3D engine.
     */
    class DT_EXPORT DataType : public dtUtil::Enumeration
    {
        DECLARE_ENUM(DataType);
    public:
        /**
         * Data type used to represent an unknown type.  For example, the GenericActorProperty
         * returns identifies its datatype as unknown.  It does this because the
         * GenericActorProperty can be any type.
         * @see GenericActorProperty
         */
        static DataType UNKNOWN;

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
         * Similar to the INT datatype.
         */
        static DataType LONGINT;

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
        static DataType VEC4;

        /**
         * A vector of 3 values.
         */
        static DataType VEC3;

        /**
         * A vector of 2 values.
         */
        static DataType VEC2;

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
         * An actor
         */
        static DataType ACTOR;

        /**
         * Datatype which represents an enumeration.
         * @see dtDAL::Enumeration
         */
        static DataType ENUMERATION;

        /**
         * A static mesh resource
         */
        static DataType STATIC_MESH;

        /**
         * A texture resource
         */
        static DataType TEXTURE;

        /**
         * A replicant body character
         */
        static DataType CHARACTER;

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
         * @return true if this datatype represents a resource
         */
        bool IsResource() const { return mResource; };

        /**
         * Gets a pretty name that can be used in the UI.
         * @return The pretty name
         */
        const std::string &GetDisplayName() { return mDisplayName; }

    private:
        /**
         * Private constructor which registers a new DataType enumeration
         * with the static list of available DataType enumerations.
         */
        DataType(const std::string &name, const std::string &displayName, bool resource = false) : dtUtil::Enumeration(name) {
            AddInstance(this);
            mResource = resource;
            mDisplayName = displayName;
        }

        bool mResource;
        std::string mDisplayName;
    };
}

#endif //DELTA_DATATYPES
