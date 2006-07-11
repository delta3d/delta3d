/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
#include "dtCore/shadergroup.h"
#include "dtCore/shadermanager.h"
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <osg/Shader>
#include <osg/Program>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   ShaderGroup::ShaderGroup(const std::string &name) : mName(name),
      mDefaultShader(NULL)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderGroup::~ShaderGroup()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderGroup::AddShader(Shader &shader, bool isDefault)
   {
      std::map<std::string,dtCore::RefPtr<Shader> >::iterator itor =
         mShaders.find(shader.GetName());

      if (itor != mShaders.end())
         EXCEPT(ShaderException::SHADER_SOURCE_ERROR,"Shader names must be unique.");

      mShaders.insert(std::make_pair(shader.GetName(),&shader));
      shader.SetParentGroup(this);
      if (isDefault)
         mDefaultShader = &shader;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderGroup::RemoveShader(Shader &shader)
   {
      std::map<std::string,dtCore::RefPtr<Shader> >::iterator itor =
         mShaders.find(shader.GetName());

      if (itor == mShaders.end())
      {
         LOG_WARNING("Could not remove shader: " + shader.GetName() + " Shader "
            "not found in this group's list.");
      }
      else
      {
         //Hang on to a reference for a second.
         itor->second->SetParentGroup(NULL);
         dtCore::RefPtr<Shader> tempRef = &shader;
         mShaders.erase(itor);
         if (tempRef == mDefaultShader)
         {
            if (!mShaders.empty())
               mDefaultShader = mShaders.begin()->second;
            else
               mDefaultShader = NULL;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderGroup::RemoveShader(const std::string &shaderName)
   {
      std::map<std::string,dtCore::RefPtr<Shader> >::iterator itor =
         mShaders.find(shaderName);
      RemoveShader(*(itor->second));
   }

   ///////////////////////////////////////////////////////////////////////////////
   Shader *ShaderGroup::FindShader(const std::string &name)
   {
      std::map<std::string,dtCore::RefPtr<Shader> >::iterator itor =
         mShaders.find(name);

      if (itor != mShaders.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const Shader *ShaderGroup::FindShader(const std::string &name) const
   {
      std::map<std::string,dtCore::RefPtr<Shader> >::const_iterator itor =
         mShaders.find(name);

      if (itor != mShaders.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderGroup::GetAllShaders(std::vector<dtCore::RefPtr<Shader> > &toFill)
   {
      toFill.clear();

      if (mShaders.empty())
         return;

      std::map<std::string,dtCore::RefPtr<Shader> >::iterator itor;
      toFill.reserve(mShaders.size());
      for (itor=mShaders.begin(); itor!=mShaders.end(); ++itor)
         toFill.push_back(itor->second);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderGroup::Update()
   {
      std::map<std::string,dtCore::RefPtr<Shader> >::iterator itor;
      for (itor=mShaders.begin(); itor!= mShaders.end(); ++itor)
      {
         if (itor->second->IsDirty())
            itor->second->Update();
      }

      SetDirty(false);
   }

}
