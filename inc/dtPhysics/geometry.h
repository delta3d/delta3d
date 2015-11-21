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
#include <dtCore/refptr.h>
#include <dtCore/resourcedescriptor.h>
#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/primitivetype.h>
#include <dtPhysics/physicsreaderwriter.h>
#include <dtUtil/refstring.h>

#include <climits> // for unsigned INVALID_INDEX

class palGeometry;

namespace dtPhysics
{
   class GeometryImpl;

   typedef std::map<dtPhysics::MaterialIndex, dtUtil::RefString> MaterialNameTable;

   class DT_PHYSICS_EXPORT VertexData : public osg::Referenced
   {
   public:
      static const dtPhysics::MaterialIndex INVALID_INDEX = UINT_MAX;

      VertexData();

      VertexData& Swap(VertexData& readerData);

      VertexData& ConvertToPolytope();

      VertexData& Scale(const VectorType& scale);

      VertexData& Copy(VertexData& readerData);

      static const std::string NO_CACHE_KEY;

      /**
       * Converts an node to a triangle mesh and caches it.  It will simply pull the date back from the cache if it is exists.
       */
      static void GetOrCreateCachedDataForNode(dtCore::RefPtr<VertexData>& dataOut, const osg::Node* nodeToParse, const std::string& cacheKey, bool polytope);

      /**
       * creates a new cached vertex data object or returns an existing one based on the key
       * @return true if the data is new. that way the code will know if it should populate the data.
       */
      static bool GetOrCreateCachedData(dtCore::RefPtr<VertexData>& dataOut, const std::string& key);

      static dtCore::RefPtr<VertexData> FindCachedData(const std::string& key);

      static bool ClearCachedData(const std::string& key);

      static void ClearAllCachedData();

      void SetMaterialName(dtPhysics::MaterialIndex matIndex, const std::string& materialName);

      dtUtil::RefString GetMaterialName(dtPhysics::MaterialIndex matIndex) const;

      dtPhysics::MaterialIndex GetMaterialIndex(const std::string& materialName) const;

      int GetMaterialCount() const;

      const MaterialNameTable& GetMaterialTable() const;

      void SwapMaterialTable(VertexData& other);

      int ClearMaterialTable();

      dtPhysics::MaterialIndex GetFirstMaterialIndex() const;

      std::vector<VectorType> mVertices;
      std::vector<unsigned> mIndices;
      std::vector<unsigned> mMaterialFlags;
      VectorType mCurrentScale;
      dtCore::ResourceDescriptor mOutputFile;
   protected:
      ~VertexData();

      MaterialNameTable mMatNameTable;
   };

   typedef dtCore::RefPtr<VertexData> VertexDataPtr;

   /**
    * Physics Geometry.  These are added to physics objects as the collision geometry.
    */
   class DT_PHYSICS_EXPORT Geometry : public osg::Referenced
   {
   public:

      static dtCore::RefPtr<Geometry> CreateBoxGeometry(const TransformType& worldxform, const VectorType wdh, Real mass);

      static dtCore::RefPtr<Geometry> CreateSphereGeometry(const TransformType& worldxform, Real radius, Real mass);

      static dtCore::RefPtr<Geometry> CreateCapsuleGeometry(const TransformType& worldxform, Real height, Real radius, Real Mass);

      static dtCore::RefPtr<Geometry> CreateCylinderGeometry(const TransformType& worldxform, Real height, Real radius, Real Mass);

      static dtCore::RefPtr<Geometry> CreateConvexGeometry(const TransformType& worldxform, VertexData& data, Real mass, bool assumePolytope = false);

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

      /**
       * Returns the vertex data that was used to generate arbitrary geometry,
       * whether it be convex or concave.
       */
      const VertexData* GetVertexData() const;

   protected:

      Geometry(PrimitiveType& primType);
      virtual ~Geometry();

      // Subclasses can call this to set the internal implementation.
      void SetPalGeometry(palGeometry& geom);

   private:
      GeometryImpl* mImpl;
   };

   typedef dtCore::RefPtr<Geometry> GeometryPtr;
}

#endif /* DTPHYSICS_GEOMETRY_H_ */
