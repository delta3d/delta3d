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
#include "dtCore/shader.h"
#include "dtCore/shadermanager.h"
#include "dtCore/shaderparameter.h"
#include "dtCore/shadergroup.h"

#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>

#include <dtCore/globals.h>

#include <osg/Program>
#include <osg/Shader>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   Shader::Shader(const std::string &name) : mName(name)
   {
      Reset();
   }

   ///////////////////////////////////////////////////////////////////////////////
   Shader::~Shader()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Shader &Shader::operator=(const Shader &rhs)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////
   Shader::Shader(const Shader &rhs)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Shader::Reset()
   {
      mGLSLProgram = NULL;
      mVertexShader = NULL;
      mFragmentShader = NULL;
      mFragmentShaderFileName = "";
      mVertexShaderFileName = "";
      mParameters.clear();
      mParentGroup = NULL;
      mIsDirty = false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Shader::RemoveAllParameters()
   {
      mParameters.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Shader::AddParameter(ShaderParameter &newParam)
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::iterator itor =
         mParameters.find(newParam.GetName());

      if (itor != mParameters.end())
         EXCEPT(ShaderException::DUPLICATE_SHADER_PARAMETER_FOUND,
            "Shader parameters must have unique names.");

      mParameters.insert(std::make_pair(newParam.GetName(),&newParam));
      newParam.SetParentShader(this);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Shader::RemoveParameter(ShaderParameter &param)
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::iterator itor =
         mParameters.find(param.GetName());

      if (itor == mParameters.end())
      {
         LOG_WARNING("Could not remove shader parameter: " + param.GetName() +
            "  Parameter is not bound to shader: " + GetName());
      }
      else
      {
         itor->second->SetParentShader(NULL);
         mParameters.erase(itor);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Shader::RemoveParameter(const std::string &name)
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::iterator itor =
         mParameters.find(name);

      if (itor == mParameters.end())
      {
         LOG_WARNING("Could not remove shader parameter: " + name +
            "  Parameter is not bound to shader: " + GetName());
      }
      else
      {
         itor->second->SetParentShader(NULL);
         mParameters.erase(itor);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   const ShaderParameter *Shader::FindParameter(const std::string &name) const
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::const_iterator itor =
         mParameters.find(name);

      if (itor != mParameters.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter *Shader::FindParameter(const std::string &name)
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::iterator itor =
         mParameters.find(name);

      if (itor != mParameters.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Shader::GetParameterList(std::vector<dtCore::RefPtr<ShaderParameter> > &toFill) const
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::const_iterator itor;

      toFill.clear();
      for (itor=mParameters.begin(); itor!=mParameters.end(); ++itor)
         toFill.push_back(itor->second);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Shader::SetVertexShaderSource(const std::string &fileName)
   {
      std::string path = dtCore::FindFileInPathList(fileName);
      if (path.empty())
      {
         EXCEPT(ShaderException::SHADER_SOURCE_ERROR,"Could not find shader source: " +
            fileName + " in path list.");
      }

      mVertexShaderFileName = fileName;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Shader::SetFragmentShaderSource(const std::string &fileName)
   {
      std::string path = dtCore::FindFileInPathList(fileName);
      if (path.empty())
      {
         EXCEPT(ShaderException::SHADER_SOURCE_ERROR,"Could not find shader source: " +
            fileName + " in path list.");
      }

      mFragmentShaderFileName = fileName;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Shader::Update()
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::iterator itor;

      for (itor=mParameters.begin(); itor!=mParameters.end(); ++itor)
      {
         if (itor->second->IsDirty())
            itor->second->Update();
      }

      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Shader::SetDirty(bool flag)
   {
      mIsDirty = flag;
      if (mParentGroup != NULL)
         mParentGroup->SetDirty(flag);
   }

}
