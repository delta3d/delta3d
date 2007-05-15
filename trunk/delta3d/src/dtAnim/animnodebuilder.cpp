/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 03/28/2007
 */

#include <dtAnim/animnodebuilder.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/submesh.h>

#include <dtUtil/log.h>

#include <osg/Geode>

namespace dtAnim
{

AnimNodeBuilder::AnimNodeBuilder()
{
   SetCreate(CreateFunc(this, AnimNodeBuilder::CreateGeode));
}

AnimNodeBuilder::AnimNodeBuilder(const CreateFunc& pCreate)
: mCreateFunc(pCreate)
{
}


AnimNodeBuilder::~AnimNodeBuilder()
{
}

void AnimNodeBuilder::SetCreate(const CreateFunc& pCreate)
{
   mCreateFunc = pCreate;
}


dtCore::RefPtr<osg::Geode> AnimNodeBuilder::CreateGeode(Cal3DModelWrapper* pWrapper)
{
   return mCreateFunc(pWrapper);
}


dtCore::RefPtr<osg::Geode> AnimNodeBuilder::DefaultCreate(Cal3DModelWrapper* pWrapper)
{
   if(!pWrapper)
   {
      LOG_ERROR("Invalid parameter to CreateGeode.");
      return 0;
   }

   osg::Geode* geode = new osg::Geode();

   if(pWrapper->BeginRenderingQuery()) 
   {
      int meshCount = pWrapper->GetMeshCount();

      for(int meshId = 0; meshId < meshCount; meshId++) 
      {
         int submeshCount = pWrapper->GetSubmeshCount(meshId);

         for(int submeshId = 0; submeshId < submeshCount; submeshId++) 
         {
            dtAnim::SubMeshDrawable *submesh = new dtAnim::SubMeshDrawable(pWrapper, meshId, submeshId);
            geode->addDrawable(submesh);
         }
      }

      pWrapper->EndRenderingQuery();
   }

   pWrapper->Update(0);

   return geode;
}

}//namespace dtAnim
