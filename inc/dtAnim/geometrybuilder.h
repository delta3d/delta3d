/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2014, Caper Holdings LLC
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
 * Bradley Anderegg
 */

#ifndef __DELTA_GEOMETRYBUILDER_H__
#define __DELTA_GEOMETRYBUILDER_H__

#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <dtUtil/functor.h>

#include <osg/Referenced>
#include <osg/State>
#include <osg/Node> // needed for the bounding sphere callback

#include <cal3d/global.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geode;
   class Program;
}
/// @endcond

namespace dtCore
{
   class ShaderProgram;
}

class CalHardwareModel;


namespace dtAnim
{
   class Cal3DModelWrapper;
   class Cal3DModelData;
   class Array;

   class DT_ANIM_EXPORT GeometryBuilder: public osg::Referenced
   {
   public:
      
      class DT_ANIM_EXPORT MeshCacheData
      {
      public:
         typedef std::pair<int, int> MeshSubMeshIdPair;

         MeshCacheData();
         ~MeshCacheData();


         MeshSubMeshIdPair mId;
         std::string mName;       
         dtCore::RefPtr<osg::Geometry> mGeometry;
      };


      class DT_ANIM_EXPORT GeometryCache
      {
         public:
            typedef std::multimap<std::string, MeshCacheData> GeometryMap;

            GeometryCache();
            ~GeometryCache();

            osg::ref_ptr<osg::Node> GetOrCreateModel(Cal3DModelWrapper* pWrapper);

         private:
            osg::ref_ptr<osg::Geometry> CreateMeshSubMesh(CalHardwareModel* hardwareModel, Cal3DModelWrapper* pWrapper, int meshId, int subMeshId, int vertexCount, int faceCount, int boneCount, int baseIndex, int startIndex);
            osg::ref_ptr<osg::Geometry> CopySubmeshGeometry(Cal3DModelWrapper* pWrapper, CalHardwareModel* hardwareModel, GeometryBuilder::MeshCacheData& mcd);

            void SetUpMaterial(osg::Geometry* geom, CalHardwareModel* hardwareModel, Cal3DModelWrapper* pWrapper, int meshId, int subMeshId);

            GeometryMap mLoadedModels;
      };

      
      GeometryBuilder();
      
            
      virtual dtCore::RefPtr<osg::Node> CreateGeometry(osg::RenderInfo* renderInfo, Cal3DModelWrapper* pWrapper);
      

   protected:
      virtual ~GeometryBuilder();
      GeometryBuilder(const GeometryBuilder&);
      GeometryBuilder& operator=(const GeometryBuilder&);

      dtCore::ShaderProgram* LoadShaders(Cal3DModelData& modelData, osg::Node& geode) const;

   private:

      GeometryCache mGeometries;
   };

} // namespace dtAnim

#endif // __DELTA_GEOMETRYBUILDER_H__
