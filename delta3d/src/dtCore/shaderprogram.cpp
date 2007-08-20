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
 * Matthew W. Campbell, Curtiss Murphy
 */
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shaderparameter.h>
#include <dtCore/shadergroup.h>

#include <dtUtil/exception.h>

#include <dtCore/globals.h>

#include <osg/Program>
#include <osg/Shader>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   ShaderProgram::ShaderProgram(const std::string &name) : mName(name)
   {
      Reset();
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderProgram::~ShaderProgram()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderProgram &ShaderProgram::operator=(const ShaderProgram &rhs)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderProgram::ShaderProgram(const ShaderProgram &rhs)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderProgram::Reset()
   {
      mGLSLProgram = NULL;
      mVertexCacheKey = "";
      mFragmentCacheKey = "";
      mFragmentShaderFileName.clear();
      mVertexShaderFileName.clear();
      mParameters.clear();
      mIsDirty = false;
   }
   ///////////////////////////////////////////////////////////////////////////////
   const std::string& ShaderProgram::GetVertexCacheKey()
   { 
      return mVertexCacheKey;
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   const std::string& ShaderProgram::GetFragmentCacheKey()
   { 
      return mFragmentCacheKey;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderProgram::RemoveAllParameters()
   {
      mParameters.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderProgram::AddParameter(ShaderParameter &newParam)
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::iterator itor =
         mParameters.find(newParam.GetName());

      if (itor != mParameters.end())
         throw dtUtil::Exception(ShaderException::DUPLICATE_SHADER_PARAMETER_FOUND,
            "Shader parameters must have unique names.", __FILE__, __LINE__);

      mParameters.insert(std::make_pair(newParam.GetName(),&newParam));
      newParam.SetParentShader(this);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderProgram::RemoveParameter(ShaderParameter &param)
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
   void ShaderProgram::RemoveParameter(const std::string &name)
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
   const ShaderParameter *ShaderProgram::FindParameter(const std::string &name) const
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::const_iterator itor =
         mParameters.find(name);

      if (itor != mParameters.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter *ShaderProgram::FindParameter(const std::string &name)
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::iterator itor =
         mParameters.find(name);

      if (itor != mParameters.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderProgram::GetParameterList(std::vector<dtCore::RefPtr<ShaderParameter> > &toFill) const
   {
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::const_iterator itor;

      toFill.clear();
      for (itor=mParameters.begin(); itor!=mParameters.end(); ++itor)
         toFill.push_back(itor->second);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderProgram::AddVertexShader(const std::string &fileName)
   {
      // Vertex source is now allowed to be empty - but, if a filename is set, the file needs to exist.
      if (!fileName.empty())
      {
         std::string path = dtCore::FindFileInPathList(fileName);
         if (path.empty())
         {
            throw dtUtil::Exception(ShaderException::SHADER_SOURCE_ERROR,"Could not find shader source: " +
               fileName + " in path list.", __FILE__, __LINE__);
         }
         else
         {
            mVertexShaderFileName.push_back(fileName);
            mVertexCacheKey += fileName;
         }
      }
   }


   ///////////////////////////////////////////////////////////////////////////////
   void ShaderProgram::AddFragmentShader(const std::string &fileName)
   {
      // Fragment source is now allowed to be empty - but, if a filename is set, the file needs to exist.
      if (!fileName.empty())
      {
         std::string path = dtCore::FindFileInPathList(fileName);
         if (path.empty())
         {
            throw dtUtil::Exception(ShaderException::SHADER_SOURCE_ERROR,"Could not find shader source: " +
               fileName + " in path list.", __FILE__, __LINE__);
         }
         else
         {
            mFragmentShaderFileName.push_back(fileName);
            mFragmentCacheKey += fileName;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderProgram::Update()
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
   void ShaderProgram::SetDirty(bool flag)
   {
      mIsDirty = flag;
      //if (mParentGroup != NULL)
      //   mParentGroup->SetDirty(flag);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::ShaderProgram *ShaderProgram::Clone() const
   {
      dtCore::ShaderProgram *newShader = new dtCore::ShaderProgram(GetName());

      // copy main values
      newShader->mVertexShaderFileName = GetVertexShaders();
      newShader->mFragmentShaderFileName = GetFragmentShaders();
      newShader->mGLSLProgram = mGLSLProgram;
      newShader->mVertexCacheKey = mVertexCacheKey;
      newShader->mFragmentCacheKey = mFragmentCacheKey;

      // copy all of the parameters. 
      std::map<std::string,dtCore::RefPtr<ShaderParameter> >::const_iterator paramItor;
      for (paramItor=mParameters.begin(); paramItor!=mParameters.end(); ++paramItor)
      {
         dtCore::ShaderParameter *newParam = paramItor->second->Clone();
         newParam->SetParentShader(newShader);

         newShader->mParameters.insert(std::make_pair(newParam->GetName(), newParam));
      }

      return newShader;
   }

}
