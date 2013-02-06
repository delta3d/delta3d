/* -*-c++-*-
 * dtPhysics
 * Copyright 2007-2008, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#ifndef DTPHYSICS_GEOMETRY_H_
#define DTPHYSICS_GEOMETRY_H_

#include <osg/Referenced>
#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/primitivetype.h>

#include <dtCore/refptr.h>

class palGeometry;

namespace dtPhysics
{
   class GeometryImpl;

   struct DT_PHYSICS_EXPORT VertexData
   {
      VertexData();
      void DeleteData();
      void NullData();

      /**
       * creates a new cached vertex data object or returns an existing one based on the key
       * @return true if the data is new. that way the code will know if it should populate the data.
       */
      static bool GetOrCreateCachedData(const std::string& key, VertexData*& dataOut);

      static VertexData* FindCachedData(const std::string& key);

      static bool ClearCachedData(const std::string& key);

      static void ClearAllCachedData();

      Real* mVertices;
      unsigned* mIndices;
      size_t mNumVertices;
      size_t mNumIndices;

   };

   class DT_PHYSICS_EXPORT Geometry : public osg::Referenced
   {
   public:

      static const std::string NO_CACHE_KEY;

      static dtCore::RefPtr<Geometry> CreateBoxGeometry(const TransformType& worldxform, const VectorType wdh, Real mass);

      static dtCore::RefPtr<Geometry> CreateSphereGeometry(const TransformType& worldxform, Real radius, Real mass);

      static dtCore::RefPtr<Geometry> CreateCapsuleGeometry(const TransformType& worldxform, Real height, Real radius, Real Mass);

      static dtCore::RefPtr<Geometry> CreateConvexGeometry(const TransformType& worldxform, const osg::Node& nodeToParse, Real mass, const std::string& cacheKey = NO_CACHE_KEY);

      static dtCore::RefPtr<Geometry> CreateConvexGeometry(const TransformType& worldxform, VertexData& data, Real mass, bool assumePolytope = false);

      static dtCore::RefPtr<Geometry> CreateConcaveGeometry(const TransformType& worldxform, const osg::Node& nodeToParse, Real mass, const std::string& cacheKey = NO_CACHE_KEY);

      static dtCore::RefPtr<Geometry> CreateConcaveGeometry(const TransformType& worldxform, VertexData& data, Real mass);

      void GetTransform(TransformType& mat) const;

      PrimitiveType& GetPrimitiveType() const;

      palGeometry& GetPalGeometry();

      /// @return the mass of this geometry
      Real GetMass() const;
      /// Sets the mass of this geometry
      void SetMass(Real mass);

      /**
       * @return the collision margin / skin width around the shape.
       * If it returns -1.0, that means engine default or unsupported.
       */
      Real GetMargin() const;

      /**
       * Sets the collision margin / skin width around the shape.
       * Don't set this to 0.0, but -1.0 means engine default.
       * @return true if this setting is supported.
       */
      bool SetMargin(Real);

   protected:

      Geometry(PrimitiveType& primType);
      virtual ~Geometry();

   private:
      GeometryImpl* mImpl;
   };
}

#endif /* DTPHYSICS_GEOMETRY_H_ */
