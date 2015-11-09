/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2009
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
* Erik Johnson
*/

#include <dtAnim/lodcullcallback.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/modeldatabase.h>
#include <dtAnim/submesh.h>
#include <dtUtil/mathdefines.h>

////////////////////////////////////////////////////////////////////////////////
dtAnim::LODCullCallback::LODCullCallback(Cal3DModelWrapper& wrapper, int meshID):
  mWrapper(&wrapper)
, mMeshID(meshID)
{
}

////////////////////////////////////////////////////////////////////////////////////////
bool dtAnim::LODCullCallback::cull(osg::NodeVisitor* nv, osg::Drawable* drawable, osg::RenderInfo* renderInfo) const
{
   dtAnim::MeshInterface* mesh = mWrapper->GetMeshByIndex(mMeshID);
   if (mesh == NULL || !mesh->IsVisible())
   {
      return true;
   }

   dtAnim::BaseModelData* modelData = mWrapper->GetModelData();
   const osg::Node* parent = nv->getNodePath().back();
   if (parent != NULL && modelData != NULL)
   {
      const float distance = nv->getDistanceFromEyePoint(parent->getBound().center(), true);

      // disappear once the max distance is reached
      if (distance > modelData->GetLODOptions().GetMaxVisibleDistance())
      {
         return true;
      }

      //calculate and set the LOD level on the software mode
      SubmeshDrawable* submeshDraw = dynamic_cast<SubmeshDrawable*>(drawable);
      if (submeshDraw != NULL)
      {
         const float start = modelData->GetLODOptions().GetStartDistance();
         const float end   = modelData->GetLODOptions().GetEndDistance();
         const float slope = 1.0f / (end - start);

         float lod = 1.0f - (slope*(distance - start));
         dtUtil::Clamp(lod, 0.0f, 1.0f);
         if (!osg::isNaN(lod))
         {
            //copy user data to the state.
            //dtCore::RefPtr<SubmeshUserData> userData = new SubmeshUserData;
            //userData->mLOD = lod;
            //renderInfo->setUserData(userData.get());

            submeshDraw->SetCurrentLOD(lod);
            //std::cout << "Setting LOD to " << lod << std::endl;
         }

      }
   }

   return osg::Geometry::CullCallback::cull(nv, drawable, renderInfo);
}
