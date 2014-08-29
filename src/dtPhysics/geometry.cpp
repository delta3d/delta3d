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

#include <dtPhysics/geometry.h>
#include <pal/pal.h>
#include <pal/palFactory.h>
#include <dtPhysics/palutil.h>
#include <dtPhysics/trianglerecorder.h>
#include <dtPhysics/convexhull.h>
#include <dtUtil/exception.h>

namespace dtPhysics
{
   const std::string Geometry::NO_CACHE_KEY("");

   class MeshCache
   {
   public:
      typedef std::map<std::string, VertexData> MeshCacheContainerType;
      ~MeshCache()
      {
         DeleteAll();
      }

      void DeleteAll()
      {
         MeshCacheContainerType::iterator i, iend;
         i = mMeshCacheMap.begin();
         iend = mMeshCacheMap.end();
         for (; i != iend; ++i)
         {
            i->second.DeleteData();
         }
         mMeshCacheMap.clear();
      }

      MeshCacheContainerType mMeshCacheMap;
   };

   static MeshCache gMeshCache;

   /////////////////////////////////////////////////
   VertexData::VertexData()
   {
      NullData();
   }

   /////////////////////////////////////////////////
   void VertexData::DeleteData()
   {
      delete[] mVertices;
      delete[] mIndices;
      NullData();
   }

   /////////////////////////////////////////////////
   void VertexData::NullData()
   {
      mNumIndices = 0U;
      mNumVertices = 0U;
      mIndices = NULL;
      mVertices = NULL;
   }

   ////////////////////////////////////////////////////////////
   bool VertexData::GetOrCreateCachedData(const std::string& key, VertexData*& dataOut)
   {
      bool result = false;
      dataOut = FindCachedData(key);
      if (dataOut == NULL)
      {
         VertexData meshData;
         std::pair<MeshCache::MeshCacheContainerType::iterator, bool> insertResult = gMeshCache.mMeshCacheMap.insert(std::make_pair(key, meshData));
         if (insertResult.second)
         {
            VertexData& vertData = insertResult.first->second;
            // Return the created mesh data object so the external code can populated it.
            dataOut = &vertData;
            result = true;
         }
         else
         {
            throw dtUtil::Exception(std::string("Attempted to create a cached data object that already exists even though the find failed: ") + key,
                     __FILE__, __LINE__);
         }
      }
      else
      {
         result = false;
      }
      return result;
   }

   ////////////////////////////////////////////////////////////
   VertexData* VertexData::FindCachedData(const std::string& key)
   {
      MeshCache::MeshCacheContainerType::iterator found = gMeshCache.mMeshCacheMap.find(key);
      if (found != gMeshCache.mMeshCacheMap.end())
      {
         return &found->second;
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////
   bool VertexData::ClearCachedData(const std::string& key)
   {
      MeshCache::MeshCacheContainerType::iterator found = gMeshCache.mMeshCacheMap.find(key);
      if (found != gMeshCache.mMeshCacheMap.end())
      {
         found->second.DeleteData();
         gMeshCache.mMeshCacheMap.erase(found);
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////
   void VertexData::ClearAllCachedData()
   {
      gMeshCache.DeleteAll();
   }

   /////////////////////////////////////////////////
   class GeometryImpl
   {
   public:
      GeometryImpl()
      : mGeom(NULL)
      , mPrimitiveType(&PrimitiveType::SPHERE)
      {

      }

      ~GeometryImpl()
      {
         //If the internal geom is still connected to a pal body, we can't delete it yet.
         // This parental delete check is no longer necessary.
//         if (mGeom != NULL && mGeom->GetBaseBody() == NULL)
//         {
            delete mGeom;
            mGeom = NULL;
//         }
      }

      palGeometry* mGeom;
      TransformType mOffsetTransform;
      PrimitiveType* mPrimitiveType;
   };

   /////////////////////////////////////
   Geometry::Geometry(PrimitiveType& primType)
   : mImpl(new GeometryImpl)
   {
      mImpl->mPrimitiveType = &primType;
   }

   /////////////////////////////////////
   Geometry::~Geometry()
   {
      delete mImpl;
      mImpl = NULL;
   }

   /////////////////////////////////////
   dtCore::RefPtr<Geometry> Geometry::CreateBoxGeometry(const TransformType& worldxform,
            const VectorType wdh, Real mass)
   {
      dtCore::RefPtr<Geometry> geometry = new Geometry(PrimitiveType::BOX);
      palBoxGeometry* boxGeom = palFactory::GetInstance()->CreateBoxGeometry();
      geometry->mImpl->mGeom = boxGeom;

      MatrixType osgMat;
      worldxform.Get(osgMat);
      palMatrix4x4 palMat;
      TransformToPalMatrix(palMat, osgMat);

      //init the geom
      boxGeom->Init(palMat, wdh.x(), wdh.z(),  wdh.y(), mass);
      return geometry;
   }

   /////////////////////////////////////
   dtCore::RefPtr<Geometry> Geometry::CreateSphereGeometry(const TransformType& worldxform,
            Real radius, Real mass)
   {
      dtCore::RefPtr<Geometry> geometry = new Geometry(PrimitiveType::SPHERE);

      palSphereGeometry* sphereGeom = palFactory::GetInstance()->CreateSphereGeometry();

      geometry->mImpl->mGeom = sphereGeom;

      MatrixType osgMat;
      worldxform.Get(osgMat);
      palMatrix4x4 palMat;
      TransformToPalMatrix(palMat, osgMat);

      //init the geom
      sphereGeom->Init(palMat, radius, mass);
      return geometry;
   }

   /////////////////////////////////////
   dtCore::RefPtr<Geometry> Geometry::CreateCapsuleGeometry(const TransformType& worldxform,
            Real height, Real radius, Real mass)
   {
      dtCore::RefPtr<Geometry> geometry = new Geometry(PrimitiveType::CYLINDER);

      palCapsuleGeometry* capGeom = palFactory::GetInstance()->CreateCapsuleGeometry();

      geometry->mImpl->mGeom = capGeom;

      MatrixType osgMat;
      worldxform.Get(osgMat);
      palMatrix4x4 palMat;
      TransformToPalMatrix(palMat, osgMat);

      //init the geom
      capGeom->Init(palMat, radius, height, mass);
      return geometry;
   }

   /////////////////////////////////////
   dtCore::RefPtr<Geometry> Geometry::CreateConvexGeometry(const TransformType& worldxform,
            const osg::Node& nodeToParse, Real mass, const std::string& cacheKey)
   {
      VertexData* data = NULL;
      VertexData tmpData;
      bool newData = false;
      if (cacheKey != NO_CACHE_KEY)
      {
         newData = VertexData::GetOrCreateCachedData(cacheKey, data);
      }
      else
      {
         data = &tmpData;
         newData = true;
      }

      if (newData)
      {
         TriangleRecorder tr;
         tr.Record(nodeToParse);

         if (tr.mData.mVertices->empty() || tr.mData.mFaces->empty())
         {
            throw dtUtil::Exception("Unable to create convex mesh, no vertex data was found when traversing the osg Node.", __FILE__, __LINE__);
         }

         VertexData triangleData;
         triangleData.mNumIndices = tr.mData.mFaces->size();
         triangleData.mNumVertices = tr.mData.mVertices->size();
         triangleData.mIndices = &tr.mData.mFaces->front();
         triangleData.mVertices = reinterpret_cast<Real*>(&tr.mData.mVertices->front());

         ConvexHull hull(triangleData, 100);

         *data = hull.ReleaseNewVertexData();
      }

      dtCore::RefPtr<Geometry> geom = CreateConvexGeometry(worldxform, *data, mass, true);
      if (cacheKey == NO_CACHE_KEY)
      {
         data->DeleteData();
      }
      return geom;
   }

   /////////////////////////////////////
   dtCore::RefPtr<Geometry> Geometry::CreateConvexGeometry(const TransformType& worldxform, VertexData& data, Real mass, bool assumePolytope)
   {
      dtCore::RefPtr<Geometry> geometry = new Geometry(PrimitiveType::CONVEX_HULL);

      palConvexGeometry* convGeom = palFactory::GetInstance()->CreateConvexGeometry();

      geometry->mImpl->mGeom = convGeom;

      MatrixType osgMat;
      worldxform.Get(osgMat);
      palMatrix4x4 palMat;
      TransformToPalMatrix(palMat, osgMat);

      if (!assumePolytope)
      {
         ConvexHull hull(data, 100);

         VertexData newData = hull.ReleaseNewVertexData();

         //init the geom
         convGeom->Init(palMat, newData.mVertices, newData.mNumVertices, reinterpret_cast<int*>(newData.mIndices), newData.mNumIndices, mass);
         // Must do this manually since the vertex data object doesn't know if it owns its data.
         newData.DeleteData();
      }
      else
      {
         convGeom->Init(palMat, data.mVertices, data.mNumVertices, reinterpret_cast<int*>(data.mIndices), data.mNumIndices, mass);
      }
      return geometry;
   }

   /////////////////////////////////////
   dtCore::RefPtr<Geometry> Geometry::CreateConcaveGeometry(const TransformType& worldxform,
            const osg::Node& nodeToParse, Real mass, const std::string& cacheKey)
   {
      TriangleRecorder tr;
      tr.Record(nodeToParse);
      if (tr.mData.mVertices->empty() || tr.mData.mFaces->empty())
      {
         throw dtUtil::Exception("Unable to create concave mesh, no vertex data was found when traversing the osg Node.", __FILE__, __LINE__);
      }
      VertexData data;
      data.mNumIndices = tr.mData.mFaces->size();
      data.mNumVertices = tr.mData.mVertices->size();
      // TODO must copy the data in order to cache it.
      data.mIndices = &tr.mData.mFaces->front();
      data.mVertices = reinterpret_cast<Real*>(&tr.mData.mVertices->front());
      return CreateConcaveGeometry(worldxform, data, mass);
   }

   /////////////////////////////////////
   dtCore::RefPtr<Geometry> Geometry::CreateConcaveGeometry(const TransformType& worldxform, VertexData& data, Real mass)
   {
      dtCore::RefPtr<Geometry> geometry = new Geometry(PrimitiveType::TRIANGLE_MESH);

      palConcaveGeometry* convGeom = palFactory::GetInstance()->CreateConcaveGeometry();
      if(convGeom == NULL)
      {
         LOG_ERROR("Failed to create triangle mesh/concave geometry.");
      }

      geometry->mImpl->mGeom = convGeom;

      MatrixType osgMat;
      worldxform.Get(osgMat);
      palMatrix4x4 palMat;
      TransformToPalMatrix(palMat, osgMat);

      //init the geom
      convGeom->Init(palMat, data.mVertices, data.mNumVertices, reinterpret_cast<int*>(data.mIndices), data.mNumIndices, mass);
      return geometry;
   }

   /////////////////////////////////////
   void Geometry::GetTransform(TransformType& mat) const
   {
      palMatrix4x4 palMat = mImpl->mGeom->GetLocationMatrix();
      PalMatrixToTransform(mat, palMat);
   }

   /////////////////////////////////////
   PrimitiveType& Geometry::GetPrimitiveType() const
   {
      return *mImpl->mPrimitiveType;
   }

   /////////////////////////////////////
   palGeometry& Geometry::GetPalGeometry()
   {
      return *mImpl->mGeom;
   }

   /////////////////////////////////////
   Real Geometry::GetMargin() const
   {
      return Real(mImpl->mGeom->GetMargin());
   }

   /////////////////////////////////////
   bool Geometry::SetMargin(Real margin)
   {
      return mImpl->mGeom->SetMargin(Float(margin));
   }

   /////////////////////////////////////
   Real Geometry::GetMass() const
   {
      return Real(mImpl->mGeom->GetMass());
   }

   /////////////////////////////////////
   void Geometry::SetMass(Real mass)
   {
      mImpl->mGeom->SetMass(Float(mass));
   }

}
