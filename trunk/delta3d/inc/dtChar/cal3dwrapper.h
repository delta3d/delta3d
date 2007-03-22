/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 MOVES Institute
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
 * Erik Johnson 03/20/2007
 */

#ifndef __DELTA_CAL3DWRAPPER_H__
#define __DELTA_CAL3DWRAPPER_H__

#include <dtChar/export.h>
#include <cal3d/model.h>
#include <cal3d/coremodel.h>
#include <cal3d/renderer.h>
#include <cal3d/mixer.h>
#include <osg/Referenced>

namespace dtChar
{
   /**
    * Wraps the Cal3D API
    */
   class DT_CHAR_EXPORT Cal3DWrapper : public osg::Referenced
   {
      public:
   
         Cal3DWrapper( CalModel *model );
         virtual ~Cal3DWrapper();

         void SetCalModel( CalModel *model );


         bool AttachMesh( int meshID ) { return mCalModel->attachMesh(meshID); }
         bool DetachMesh( int meshID ) { return mCalModel->detachMesh(meshID); }

         void SetMaterialSet(int materialSetID) { mCalModel->setMaterialSet(materialSetID); }
         void SetLODLevel(float level) { mCalModel->setLodLevel(level); }

         void Update(float deltaTime) {mCalModel->update(deltaTime); }

         bool BeginRenderingQuery() { return mRenderer->beginRendering(); } 
         void EndRenderingQuery()   { mRenderer->endRendering(); }

         bool SelectMeshSubmesh(int meshID, int submeshID) {return mRenderer->selectMeshSubmesh(meshID, submeshID);}

         int GetVertexCount() { return mRenderer->getVertexCount(); }
         int GetFaceCount()   { return mRenderer->getFaceCount(); }
         int GetMapCount()    { return mRenderer->getMapCount(); }
         int GetMeshCount()                 { return mRenderer->getMeshCount(); }
         int GetSubmeshCount(int submeshID) { return mRenderer->getSubmeshCount(submeshID); }

         int GetCoreMeshCount() { return mCalModel->getCoreModel()->getCoreMeshCount(); }
         int GetCoreAnimationCount() {return mCalModel->getCoreModel()->getCoreAnimationCount(); }
         int GetCoreMaterialCount() {return mCalModel->getCoreModel()->getCoreMaterialCount();}
         CalCoreMaterial* GetCoreMaterial(int matID) {return mCalModel->getCoreModel()->getCoreMaterial(matID); }

         int GetFaces(int *faces)          { return mRenderer->getFaces( (CalIndex*)faces ); }
         int GetNormals(float *normals, int stride=0) { return mRenderer->getNormals(normals, stride); }
         int GetTextureCoords(int mapID, float *coords, int stride=0) {return mRenderer->getTextureCoordinates(mapID, coords, stride);}
         int GetVertices(float *vertBuffer, int stride=0) {return mRenderer->getVertices(vertBuffer, stride);}

         void GetAmbientColor( unsigned char *colorBuffer ) {mRenderer->getAmbientColor(colorBuffer);}
         void GetDiffuseColor( unsigned char *colorBuffer ) {mRenderer->getDiffuseColor(colorBuffer);}
         void GetSpecularColor( unsigned char *colorBuffer ) {mRenderer->getSpecularColor(colorBuffer);}
         float GetShininess() { return mRenderer->getShininess(); }
         void* GetMapUserData(int mapID) { return mRenderer->getMapUserData(mapID); }

         bool BlendCycle(int id, float weight, float delay) {return mMixer->blendCycle(id, weight, delay);}
         bool ClearCycle(int id, float delay)               {return mMixer->clearCycle(id, delay);}
         bool ExecuteAction(int id, float delayIn, float delayOut,
                            float weightTgt=1.f, bool autoLock=false) {return mMixer->executeAction(id, delayIn, delayOut, weightTgt, autoLock);}

         bool RemoveAction(int id)                          {return mMixer->removeAction(id);}

   
      private:
         CalModel    *mCalModel;
         CalRenderer *mRenderer;
         CalMixer    *mMixer;
   
   };
}//namespace dtChar

#endif // __DELTA_CAL3DWRAPPER_H__
