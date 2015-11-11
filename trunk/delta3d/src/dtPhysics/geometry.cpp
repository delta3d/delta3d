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
#include <dtUtil/mathdefines.h>

namespace dtPhysics
{
   const std::string VertexData::NO_CACHE_KEY("");

   class MeshCache
   {
   public:
      typedef std::map<std::string, dtCore::RefPtr<VertexData> > MeshCacheContainerType;
      ~MeshCache()
      {
         DeleteAll();
      }

      void DeleteAll()
      {
         mMeshCacheMap.clear();
      }

      MeshCacheContainerType mMeshCacheMap;
   };

   static MeshCache gMeshCache;

   /////////////////////////////////////////////////
   VertexData::VertexData()
      : mCurrentScale(Real(1.0), Real(1.0), Real(1.0))
   {}

   /////////////////////////////////////////////////
   VertexData::~VertexData()
   {}

   /////////////////////////////////////////////////
   VertexData& VertexData::Swap(VertexData& readerData)
   {
      mIndices.swap(readerData.mIndices);
      mVertices.swap(readerData.mVertices);
      mMaterialFlags.swap(readerData.mMaterialFlags);
      mMatNameTable.swap(readerData.mMatNameTable);

      return *this;
   }

   /////////////////////////////////////////////////
   VertexData& VertexData::ConvertToPolytope()
   {
      ConvexHull hull(*this, 100);

      hull.GetVertexData(*this);
      return *this;
   }

   /////////////////////////////////////////////////
   VertexData& VertexData::Scale(const VectorType& scale)
   {
      // If the new scale equals the current, do nothing.
      if (dtUtil::Equivalent(scale, mCurrentScale))
      {
         return *this;
      }
      VectorType rescale = VectorType(scale.x()*mCurrentScale.x(), scale.y()*mCurrentScale.y(), scale.z()*mCurrentScale.z());

      std::vector<VectorType>::iterator i, iend;
      i = mVertices.begin();
      iend = mVertices.end();
      for (; i != iend; ++i)
      {
         VectorType& v = *i;
         v = VectorType(v.x()*rescale.x(), v.y()*rescale.y(), v.z()*rescale.z());
      }
      mCurrentScale = scale;
      return *this;
   }


   /////////////////////////////////////////////////
   VertexData& VertexData::Copy(VertexData& readerData)
   {
      mIndices = readerData.mIndices;
      mVertices = readerData.mVertices;
      mMaterialFlags = readerData.mMaterialFlags;
      mMatNameTable = readerData.mMatNameTable;

      return *this;
   }


   /////////////////////////////////////////////////
   void VertexData::GetOrCreateCachedDataForNode(dtCore::RefPtr<VertexData>& dataOut, const osg::Node* nodeToParse, const std::string& cacheKey, bool polytope)
   {
      bool newData = false;
      if (cacheKey != NO_CACHE_KEY)
      {
         newData = VertexData::GetOrCreateCachedData(dataOut, cacheKey);
      }
      else
      {
         dataOut = new VertexData;
         newData = true;
      }

      if (newData)
      {
         TriangleRecorder tr;
         tr.Record(*nodeToParse);

         // Using the last one is fine because we aren't splitting the data.
         if (tr.mData.back()->mVertices.empty() || tr.mData.back()->mIndices.empty())
         {
            throw dtUtil::Exception("Unable to build Vertex data object, no vertex data was found when traversing the osg Node.", __FILE__, __LINE__);
         }
         dataOut->Swap(*tr.mData.back());
         if (polytope)
         {
            dataOut->ConvertToPolytope();
         }
      }
   }

   ////////////////////////////////////////////////////////////
   bool VertexData::GetOrCreateCachedData(dtCore::RefPtr<VertexData>& dataOut, const std::string& key)
   {
      bool result = false;
      dataOut = FindCachedData(key);
      if (!dataOut.valid())
      {
         std::pair<MeshCache::MeshCacheContainerType::iterator, bool> insertResult = gMeshCache.mMeshCacheMap.insert(std::make_pair(key, new VertexData));
         if (insertResult.second)
         {
            dataOut = insertResult.first->second;
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
   dtCore::RefPtr<VertexData> VertexData::FindCachedData(const std::string& key)
   {
      MeshCache::MeshCacheContainerType::iterator found = gMeshCache.mMeshCacheMap.find(key);
      if (found != gMeshCache.mMeshCacheMap.end())
      {
         return found->second;
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////
   bool VertexData::ClearCachedData(const std::string& key)
   {
      MeshCache::MeshCacheContainerType::iterator found = gMeshCache.mMeshCacheMap.find(key);
      if (found != gMeshCache.mMeshCacheMap.end())
      {
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

   ////////////////////////////////////////////////////////////
   void VertexData::SetMaterialName(dtPhysics::MaterialIndex matIndex, const std::string& materialName)
   {
      mMatNameTable[matIndex] = materialName;
   }

   ////////////////////////////////////////////////////////////
   dtUtil::RefString VertexData::GetMaterialName(dtPhysics::MaterialIndex matIndex) const
   {
      dtUtil::RefString matName;

      MaterialNameTable::const_iterator foundIter = mMatNameTable.find(matIndex);

      if (foundIter != mMatNameTable.end())
      {
         matName = foundIter->second;
      }

      return matName;
   }

   ////////////////////////////////////////////////////////////
   dtPhysics::MaterialIndex VertexData::GetMaterialIndex(const std::string& materialName) const
   {
      dtPhysics::MaterialIndex matIndex = INVALID_INDEX;

      dtUtil::RefString curName;
      MaterialNameTable::const_iterator curIter = mMatNameTable.begin();
      MaterialNameTable::const_iterator endIter = mMatNameTable.end();
      for (; curIter != endIter; ++curIter)
      {
         curName = curIter->second;

         if (curName == materialName)
         {
            matIndex = curIter->first;
         }
      }

      return matIndex;
   }

   /////////////////////////////////////////////////
   int VertexData::GetMaterialCount() const
   {
      return (int) mMatNameTable.size();
   }

   /////////////////////////////////////////////////
   const MaterialNameTable& VertexData::GetMaterialTable() const
   {
      return mMatNameTable;
   }

   /////////////////////////////////////////////////
   void VertexData::SwapMaterialTable(VertexData& other)
   {
      mMatNameTable.swap(other.mMatNameTable);
   }

   /////////////////////////////////////////////////
   int VertexData::ClearMaterialTable()
   {
      int count = (int)mMatNameTable.size();

      mMatNameTable.clear();

      return count;
   }

   /////////////////////////////////////////////////
   dtPhysics::MaterialIndex VertexData::GetFirstMaterialIndex() const
   {
      dtPhysics::MaterialIndex matIndex = INVALID_INDEX;

      if ( ! mMatNameTable.empty())
      {
         matIndex = mMatNameTable.begin()->first;
      }

      return matIndex;
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
      dtCore::ObserverPtr<dtPhysics::VertexData> mVertData;
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
      dtCore::RefPtr<Geometry> geometry = new Geometry(PrimitiveType::CAPSULE);

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
   dtCore::RefPtr<Geometry> Geometry::CreateCylinderGeometry(const TransformType& worldxform,
            Real height, Real radius, Real mass)
   {
      dtCore::RefPtr<Geometry> geometry = new Geometry(PrimitiveType::CYLINDER);

      palCylinderGeometry* cyGeom = palFactory::GetInstance()->CreateCylinderGeometry();

      geometry->mImpl->mGeom = cyGeom;

      MatrixType osgMat;
      worldxform.Get(osgMat);
      palMatrix4x4 palMat;
      TransformToPalMatrix(palMat, osgMat);

      //init the geom
      cyGeom->Init(palMat, radius, height, mass);
      return geometry;
   }

   /////////////////////////////////////
   dtCore::RefPtr<Geometry> Geometry::CreateConvexGeometry(const TransformType& worldxform, VertexData& data, Real mass, bool assumePolytope)
   {
      dtCore::RefPtr<Geometry> geometry = new Geometry(PrimitiveType::CONVEX_HULL);

      palConvexGeometry* convGeom = palFactory::GetInstance()->CreateConvexGeometry();
      if(convGeom == NULL)
      {
         LOG_ERROR("Failed to create convex geometry.");
         return NULL;
      }

      geometry->mImpl->mGeom = convGeom;

      MatrixType osgMat;
      worldxform.Get(osgMat);
      palMatrix4x4 palMat;
      TransformToPalMatrix(palMat, osgMat);

      if (!assumePolytope)
      {
         ConvexHull hull(data, 100);

         dtCore::RefPtr<VertexData> newData = new VertexData;
         hull.GetVertexData(*newData);

         //init the geom
         convGeom->Init(palMat, reinterpret_cast<Real*>(&newData->mVertices.front()), newData->mVertices.size(), reinterpret_cast<int*>(&newData->mIndices.front()), newData->mIndices.size(), mass);

         geometry->mImpl->mVertData = newData.get();
      }
      else
      {
         convGeom->Init(palMat, reinterpret_cast<Real*>(&data.mVertices.front()), data.mVertices.size(), reinterpret_cast<int*>(&data.mIndices.front()), data.mIndices.size(), mass);
         
         geometry->mImpl->mVertData = &data;
      }
      return geometry;
   }

   /////////////////////////////////////
   dtCore::RefPtr<Geometry> Geometry::CreateConcaveGeometry(const TransformType& worldxform, VertexData& data, Real mass)
   {
      dtCore::RefPtr<Geometry> geometry = new Geometry(PrimitiveType::TRIANGLE_MESH);

      palConcaveGeometry* convGeom = palFactory::GetInstance()->CreateConcaveGeometry();
      if(convGeom == NULL)
      {
         LOG_ERROR("Failed to create triangle mesh/concave geometry.");
         return NULL;
      }

      geometry->mImpl->mGeom = convGeom;
      geometry->mImpl->mVertData = &data;

      MatrixType osgMat;
      worldxform.Get(osgMat);
      palMatrix4x4 palMat;
      TransformToPalMatrix(palMat, osgMat);

      //init the geom
      convGeom->Init(palMat, reinterpret_cast<Real*>(&data.mVertices.front()), data.mVertices.size(), reinterpret_cast<int*>(&data.mIndices.front()), data.mIndices.size(), mass);
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
   void Geometry::SetPalGeometry(palGeometry& geom)
   {
      mImpl->mGeom = &geom;
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

   /////////////////////////////////////
   const VertexData* Geometry::GetVertexData() const
   {
      return mImpl->mVertData.get();
   }

}
